#include "InterferenceGraph.h"
#include <iostream>
#include <iomanip>

using namespace std;

/*
* Konstruktor klase.
* Cuvamo reference na liste promenljivih i instrukcija.
*/

InterferenceGraph::InterferenceGraph(Variables& var, Instructions& instr) :variables(var), instructions(instr) {

}

/*
* Glavna metoda za formiranje grafa smetnji.
*/

bool InterferenceGraph::doInterferenceGraph() {
	initializeMatrix();
	buildGraph();
	printMatrix();

	calculateDegree();
	printDegree();

	if (!simplifyGraph()) {
		return false;
	}
	printSimplificationStack();

	if (!allocateRegisters())
	{
		cout << "Greska: nije moguce dodeliti registre!" << endl;
		return false;
	}

	printAllocation();
	
	return true;
}


/*
* Inicijalizacija matrice smetnji.
* Velicina matrice jednaka je broju promenljivih, 
* matrica se reazlizuje kao vektor vektora.
*/

void InterferenceGraph::initializeMatrix() {

	regVariables.clear();
	for (Variable* var : variables) {
		if (var->getTypeVariable() == Variable::REG_VAR) {
			regVariables.push_back(var);
		}
	}

	int size_matrix = regVariables.size();
	interferenceMatrix.clear();
	interferenceMatrix.resize(size_matrix);
	for (int i = 0;i < size_matrix;i++) {
		interferenceMatrix[i].resize(size_matrix, 0);
	}
}

/*
* Formiramo graf smetnji.
* Pravilo po kom radi:
* 1) Ukoliko nije MOVE sva ka promenljiva DEF
* je u smetnji sa OUT
* 2) Nemamo MOVE tako da ne obradjujemo
*/

void InterferenceGraph::buildGraph() {
	for (Instruction* instr : instructions) {
		for (Variable* def : instr->getDefInstruction()) {
			for (Variable* out : instr->getOutInstruction()) {
				if (def != out) {
					int red = getRegVariableIndex(def);
					int kolona = getRegVariableIndex(out);
					if (red == -1 || kolona == -1) {
						continue;
					}

					// moraju se promeniti obe
					interferenceMatrix[red][kolona] = 1;
					interferenceMatrix[kolona][red] = 1;
				}
			}
		}
	}
}

// Ispis matrice smetnji

void InterferenceGraph::printMatrix()
{
	cout << "\n===== MATRICA SMETNJI =====" << endl;

	cout << setw(6) << "";

	for (Variable* var : regVariables)
	{
		cout << setw(4) << var->getNameVariable();
	}
	cout << endl;

	int i = 0;

	for (Variable* rowVar : regVariables)
	{
		cout << setw(6) << rowVar->getNameVariable();

		for (int j = 0; j < interferenceMatrix[i].size(); j++)
		{
			cout << setw(4) << interferenceMatrix[i][j];
		}

		cout << endl;
		i++;
	}

	cout << "===========================\n" << endl;
}

/*
*	Funckija za vracanje pozicije reg variable
*	U suprotnom vraca -1 odnosno da nije nasla reg variablu
*/
int InterferenceGraph::getRegVariableIndex(Variable* var)
{
	int index = 0;

	for (Variable* regVar : regVariables)
	{
		if (regVar == var)
		{
			return index;
		}

		index++;
	}

	return -1;
}

/*
*	Prolazi kroz matricu smetnji.
*	Ukoliko pronadje 1 povecava broj suseda.
*	Na kraju svakoj promenljivoj postavlja broj suseda.
*/
void InterferenceGraph::calculateDegree() {
	int i = 0;
	for (Variable* var : regVariables) {
		int poc_degree = 0;
		for (int j = 0;j < interferenceMatrix[i].size();j++) {
			if (interferenceMatrix[i][j] == 1) {
				poc_degree++;
			}
		}
		var->setDegreeVariable(poc_degree);
		i++;
	}
}

/*
* Pomocna metoda za ispis stepena svakog cvora.
*/

void InterferenceGraph::printDegree()
{
	cout << "\n===== DEGREE ISPIS =====" << endl;

	for (Variable* var : regVariables)
	{
		cout << var->getNameVariable()
			<< " -> "
			<< var->getDegreeVariable()
			<< endl;
	}

	cout << "========================\n" << endl;
}

/*
* Faza uproscavanja grafa.
*/
bool InterferenceGraph::simplifyGraph()
{
	const int K = 4;

	vector<bool> removed(regVariables.size(), false);
	int removedCount = 0;

	while (removedCount < regVariables.size())
	{
		int selectedIndex = -1;
		int maxDegree = -1;

		int i = 0;
		for (Variable* var : regVariables)
		{
			if (!removed[i] &&
				var->getDegreeVariable() < K &&
				var->getDegreeVariable() >= maxDegree)
			{
				maxDegree = var->getDegreeVariable();
				selectedIndex = i;
			}

			i++;
		}

		if (selectedIndex == -1)
		{
			cout << endl << "Greska: graf ne moze da se uprosti, potrebno je prelivanje!" << endl << endl;
			return false;
		}

		Variable* selectedVariable = nullptr;

		int index = 0;
		for (Variable* var : regVariables)
		{
			if (index == selectedIndex)
			{
				selectedVariable = var;
				break;
			}

			index++;
		}

		simplificationStack.push(selectedVariable);
		cout << "Uklanjam: "
			<< selectedVariable->getNameVariable()
			<< " (degree = "
			<< selectedVariable->getDegreeVariable()
			<< ")" << endl;
		removed[selectedIndex] = true;
		removedCount++;

		for (int j = 0; j < interferenceMatrix[selectedIndex].size(); j++)
		{
			if (interferenceMatrix[selectedIndex][j] == 1 && !removed[j])
			{
				int k = 0;

				for (Variable* neighbour : regVariables)
				{
					if (k == j)
					{
						neighbour->setDegreeVariable(
							neighbour->getDegreeVariable() - 1);
						break;
					}

					k++;
				}
			}
		}
	}
	return true;
}
	
/*
*	Ispis steka uproscenog grafa.
*/
void InterferenceGraph::printSimplificationStack()
{
	cout << "\n===== SIMPLIFICATION STACK =====" << endl;

	std::stack<Variable*> temp = simplificationStack;

	while (!temp.empty())
	{
		cout << temp.top()->getNameVariable() << endl;
		temp.pop();
	}

	cout << "================================\n" << endl;
}

/*
*	Metoda dobavljanja slobodnog registra.
*	Uzima neku promenljivu i gleda koji registar moze da koristi.
*	To radi uz pomoc canUse koji postaje false kada petlja
*	naidje na promenljivu koja je razlicita od var, postoji
*	smetnja izmedju njih i koristi isti registar.
*	Na taj nacin se analogno ponovi za 4 registra i ukoliko
*	nije moguce alocirati registar vraca se -1.
*/
int InterferenceGraph::getAvailableRegister(Variable* var)
{
	const int K = 4;

	int varIndex = getRegVariableIndex(var);

	for (int reg = 1; reg <= K; reg++)
	{
		bool canUse = true;

		int i = 0;
		for (Variable* other : regVariables)
		{
			if (other != var &&
				interferenceMatrix[varIndex][i] == 1 &&
				other->getRegVariable() == (Regs)reg)
			{
				canUse = false;
				break;
			}

			i++;
		}

		if (canUse)
		{
			return reg;
		}
	}

	return -1;
}

/*
*	Alociranje registara uzimanjem promenljivih sa vrha steka.
*	Ukoliko je nemoguce pronaci registar za zeljenu promenljivu,
*	vraca se false u suprotnom postavljamo castovanjem
*/
bool InterferenceGraph::allocateRegisters()
{
	while (!simplificationStack.empty())
	{
		Variable* var = simplificationStack.top();
		simplificationStack.pop();

		int reg = getAvailableRegister(var);

		if (reg == -1)
		{
			return false;
		}

		var->setRegVariable((Regs)reg);
	}

	return true;
}

/*
*	Ispis alokacije registara
*	Moglo je i sa t0,t1.. radi bolje citljivosti
*/
void InterferenceGraph::printAllocation()
{
	cout << "\n===== ALOKACIJA REGISTARA =====" << endl;

	for (Variable* var : regVariables)
	{
		cout << var->getNameVariable() << " -> ";

		switch (var->getRegVariable())
		{
		case 1:
			cout << "$t0";
			break;

		case 2:
			cout << "$t1";
			break;

		case 3:
			cout << "$t2";
			break;

		case 4:
			cout << "$t3";
			break;

		default:
			cout << "NO_REG";
			break;
		}

		cout << endl;
	}

	cout << "================================\n" << endl;
}