#include "LivenessAnalysis.h"
#include <iostream>
using namespace std;


/*
* Konstruktor klase.
* Cuvamo referencu na listu instrukcija.
*/

LivenessAnalysis::LivenessAnalysis(Instructions & instr,Labels & lab)
	: instructions(instr),labels(lab)
{
}

/*
* Glavna metoda za pokretanje analize.
*/

bool LivenessAnalysis::doAnalysis()
{
	makeUseDef();
	makeSuccPred();

	printUseDef();
	printSuccPred();
	calculateInOut();

	return true;
}

/*
* Formiranje USE i DEF skupova.
*
* USE predstavlja skup promenljivih koje instrukcija koristi, isto sto i source
* DEF predstavlja skup promenljivih koje instrukcija definise, isto sto i destionation
*
*/

void LivenessAnalysis::makeUseDef()
{
	for (Instruction* instr : instructions) {
		for (Variable* src : instr->getSrcInstruction()) {
			if (src != nullptr &&  src->getTypeVariable() == Variable::REG_VAR) {
				instr->getUseInstruction().push_back(src);
			}
		}
		for (Variable* dst : instr->getDstInstruction()) {
			if (dst != nullptr && dst->getTypeVariable() == Variable::REG_VAR) {
				instr->getDefInstruction().push_back(dst);
			}
		}
	}
}

// Pomocna metoda za proveru ispisa

void LivenessAnalysis::printUseDef()
{
	cout << "\n===== USE / DEF ISPIS =====" << endl;

	for (Instruction* instr : instructions)
	{
		cout << "Instrukcija " << instr->getPositionInstruction() << endl;

		cout << "USE: ";
		for (Variable* var : instr->getUseInstruction())
		{
			cout << var->getNameVariable() << " ";
		}

		cout << "\nDEF: ";
		for (Variable* var : instr->getDefInstruction())
		{
			cout << var->getNameVariable() << " ";
		}

		cout << "\n------------------------" << endl;
	}

	cout << "==========================\n" << endl;
}

// Implementacija metode za povratak instrukcije na osnovu pozicije
Instruction* LivenessAnalysis::findInstructionByPosition(int position) {
	for (Instruction* instr : instructions) {
		if (instr->getPositionInstruction() == position) {
			return instr;
		}
	}
	return nullptr;
}


/*
* Formiranje SUCC i PRED skupova.
*
* Za svaku instrukciju odredjujemo njene naslednike
* i prethodnike u grafu toka upravljanja.
*
* B instrukcija ima samo jednog naslednika
* i to instrukciju na koju pokazuje labela.
*
* BEQ i BLTZ imaju dva naslednika:
* sledecu instrukciju i instrukciju na koju pokazuje labela.
*
* Sve ostale instrukcije imaju samo jednog naslednika,
* a to je sledeca instrukcija u programu.
*/

void LivenessAnalysis::makeSuccPred()
{
	for (Instruction* instr : instructions)
	{
		int nextPosition = instr->getPositionInstruction() + 1;
		Instruction* nextInstr = findInstructionByPosition(nextPosition);

		if (instr->getTypeInstruction() == I_B) {
			Instruction* labelInstruction = findInstructionForLabel(instr->getLabelInstruction());
			if (labelInstruction != nullptr) {
				instr->getSuccInstruction().push_back(labelInstruction);
				labelInstruction->getPredInstruction().push_back(instr);
			}
		}
		else {
			if (nextInstr != nullptr)
			{
				instr->getSuccInstruction().push_back(nextInstr);
				nextInstr->getPredInstruction().push_back(instr);
			}
			if (instr->getTypeInstruction() == I_BEQ || instr->getTypeInstruction() == I_BLTZ) {
				Instruction* labelInstr = findInstructionForLabel(instr->getLabelInstruction());
				// imamo dodatan uslov da bismo izbegli duplikate
				if (labelInstr != nullptr && labelInstr != nextInstr) {
					instr->getSuccInstruction().push_back(labelInstr);
					labelInstr->getPredInstruction().push_back(instr);
				}
			}
		}
		
	}
}

//	 pomocna metoda za ispis
void LivenessAnalysis::printSuccPred()
{
	cout << "\n===== SUCC / PRED ISPIS =====" << endl;

	for (Instruction* instr : instructions)
	{
		cout << "Instrukcija " << instr->getPositionInstruction() << endl;

		cout << "SUCC: ";
		for (Instruction* succ : instr->getSuccInstruction())
		{
			cout << succ->getPositionInstruction() << " ";
		}

		cout << "\nPRED: ";
		for (Instruction* pred : instr->getPredInstruction())
		{
			cout << pred->getPositionInstruction() << " ";
		}

		cout << "\n------------------------" << endl;
	}

	cout << "===========================\n" << endl;
}


/*
* Pronalazi instrukciju na koju pokazuje labela.
*
* Prvo se prolazi kroz listu labela i trazi
* labela odgovarajuceg imena.
*
* Nakon toga se preko pozicije labele pronalazi
* instrukcija kojoj ta labela pripada.
*/


Instruction* LivenessAnalysis::findInstructionForLabel(std::string label_name) {
	for (Label* lab : labels) {
		if (lab->getNameLabel() == label_name) {
			return findInstructionByPosition(lab->getPositionLabel());
		}
	}
	return nullptr;
}


/*
* Proverava da li se prosledjena promenljiva
* vec nalazi u zadatoj listi promenljivih.
*/

bool LivenessAnalysis::variableExistsInList(Variable* var, Variables& vars)
{
	for (Variable* currentVar : vars)
	{
		if (currentVar == var)
		{
			return true;
		}
	}

	return false;
}

/*
* Racunanje IN i OUT skupova.
*
* Algoritam se ponavlja sve dok se
* IN i OUT skupovi ne prestanu menjati.
*/

void LivenessAnalysis::calculateInOut()
{
	bool changed;
	int iteration = 1;

	do
	{
		changed = false;

		// prolazak kroz instrukcije od poslednje ka prvoj kako je naznaceno u algoritmu
		cout << "\n===== ITERACIJA " << iteration << " =====" << endl;

		for (auto it = instructions.rbegin();
			it != instructions.rend();
			++it)
		{
			Instruction* instr = *it;

			// cuvanje starih vrednosti	radi poredjenja na kraju

			Variables oldIn = instr->getInInstruction();
			Variables oldOut = instr->getOutInstruction();

			// ovde racunamo OUT

			//prvo cistimo kako se ne bi pomesalo od prethodnika
			instr->getOutInstruction().clear();
			for (Instruction* succ : instr->getSuccInstruction()) {
				for (Variable* var : succ->getInInstruction()) {
					if (!variableExistsInList(var, instr->getOutInstruction())) {
						instr->getOutInstruction().push_back(var);
					}
				}
			}
			

			// ovde racunamo IN

			//	 opet se prvo cisti stari in skup
			instr->getInInstruction().clear();
			for (Variable* use : instr->getUseInstruction()) {
				if (!variableExistsInList(use, instr->getInInstruction())) {
					instr->getInInstruction().push_back(use);
				}
			}
			for (Variable* out : instr->getOutInstruction()) {
				if (!variableExistsInList(out, instr->getDefInstruction())) {
					if (!variableExistsInList(out, instr->getInInstruction())) {
						instr->getInInstruction().push_back(out);
					}
				}
			}

			// provera da li je doslo do promene cime bi doslo do kraja

			if (oldIn != instr->getInInstruction() ||
				oldOut != instr->getOutInstruction())
			{
				changed = true;
			}
		}
		printInOut();
		iteration++;

	} while (changed);
}

// metoda za proveru IN i OUT skupova
void LivenessAnalysis::printInOut()
{
	cout << "\n===== IN / OUT ISPIS =====" << endl;

	for (Instruction* instr : instructions)
	{
		cout << "Instrukcija " << instr->getPositionInstruction() << endl;

		cout << "IN: ";
		for (Variable* var : instr->getInInstruction())
		{
			cout << var->getNameVariable() << " ";
		}

		cout << "\nOUT: ";
		for (Variable* var : instr->getOutInstruction())
		{
			cout << var->getNameVariable() << " ";
		}

		cout << "\n------------------------" << endl;
	}

	cout << "==========================\n" << endl;
}