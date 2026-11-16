#include <iostream>
#include <iomanip>

#include "SyntaxAnalysis.h"
#include <stdexcept>
#include <string>

using namespace std;

/*
*	Implementacija kontruktora sintaksne analize
*	On prima referencu za leksicku analizu, zatim gresku stavlja na false
*	Postavlja iterator na pocetak liste tokena
*/

// Naknadno se u konstruktoru inicijaliziju polja vezanih za variable i instrukcije

SyntaxAnalysis::SyntaxAnalysis(LexicalAnalysis& lex)
	: lexicalAnalysis(lex), errorFound(false), tokenIterator(lexicalAnalysis.getTokenList().begin()),
	variablePosition(0), instructionPosition(0)
{
}

/*
*	Implementacija metode do koja pokrece sintaksnu analizu
*	Pozivamo prvi neterminalni znak i prvi token
*
*/

bool SyntaxAnalysis::Do() {

	currentToken = getNextToken();
	Q();
	if (!errorFound) {
		printIR();
	}
	return !errorFound;

}

/*
*	Metoda koja ispisuje gresku
*	Osim toga i informacije o tokenu
*/

void SyntaxAnalysis::printSyntaxError(Token token) {

	cout << "Sintaksna greska token: " << token.getValue() << " neocekivan" << endl;
}

//	Ispisuje informacije o tokenu

void SyntaxAnalysis::printTokenInfo(Token token) {
	token.printTokenInfo();
}


/*
*	Proverava da li je token zeljenog tipa
*	Ako nije baca gresku
*/

void SyntaxAnalysis::eat(TokenType t) {
	if (!errorFound) {
		if (currentToken.getType() == t) {
			if (currentToken.getType() != T_END_OF_FILE) {
				currentToken = getNextToken();
			}
		}
		else {
			printSyntaxError(currentToken);
			errorFound = true;
		}
	}

}

//	Metoda za dobavljanje tokena ako nije dosao do kraja

Token SyntaxAnalysis::getNextToken() {
	if (tokenIterator == lexicalAnalysis.getTokenList().end()) {
		throw runtime_error("End of input file reached");
	}
	return *tokenIterator++;
}

//Sledi implementacija neterminalnih znakova


void SyntaxAnalysis::Q() {

	//Q → S ; L
	if (errorFound) return;
	S();
	if (errorFound) return;
	eat(T_SEMI_COL);
	if (errorFound) return;
	L();
}

void SyntaxAnalysis::S() {

	/*
	*S -> _mem mid num
	*S -> _reg rid
	*S -> _func id
	*S -> id: E
	*S -> E
	*/

	/*
	* Naknadno pamtimo informacije o variablama
	* Prvo uzmemo njihovo ime zatim proverimo da li su validne
	* Nakon toga kreirama pokazivac sa informacijama o imenu i poziciji
	* Uvecamo poziciju
	* Zatim dodelimo tip variable i dodamo u listu
	*/

	switch (currentToken.getType()) {
	
	case T_MEM:
	{
		eat(T_MEM);
		std::string name = currentToken.getValue();
		if (name[0] != 'm' || !isStringDigit(name.substr(1)))
		{
			cout << "Greska: neispravno ime memorijske promenljive!" << endl;
			errorFound = true;
			return;
		}
		eat(T_M_ID);
		int value = stoi(currentToken.getValue());
		eat(T_NUM);
		Variable* var = new Variable(name, variablePosition);
		variablePosition++;
		var->setValueVariable(value);
		var->setTypeVariable(Variable::MEM_VAR);
		variables.push_back(var);
		break;
	}
	
	case T_REG:
	{
		eat(T_REG);
		std::string name = currentToken.getValue();
		if (name[0] != 'r' || !isStringDigit(name.substr(1)))
		{
			cout << "Greska: neispravno ime registarske promenljive!" << endl;
			errorFound = true;
			return;
		}
		eat(T_R_ID);
		Variable* var = new Variable(name, variablePosition);
		variablePosition++;
		var->setTypeVariable(Variable::REG_VAR);
		variables.push_back(var);
		break;
	}

	case T_FUNC:
	{
		eat(T_FUNC);
		std::string funcName = currentToken.getValue();

		if (!isalpha(funcName[0]) || !isStringAlphaNum(funcName.substr(1)))
		{
			cout << "Greska: neispravno ime funkcije!" << endl;
			errorFound = true;
			return;
		}

		if (containsLabel(funcName))
		{
			cout << "Greska: funkcija vec postoji!" << endl;
			errorFound = true;
			return;
		}

		Label* funcLabel = new Label(funcName, instructionPosition);
		labels.push_back(funcLabel);

		eat(T_ID);
		break;
	}
	
	case T_ID:
	{
		std::string label_name = currentToken.getValue();
		eat(T_ID);
		eat(T_COL);
		if (containsLabel(label_name)) {
			printSyntaxError(currentToken);
			errorFound = true;
			return;
		}
		Label* label = new Label(label_name, instructionPosition);
		labels.push_back(label);
		E();
		break;
	}

	default:
		E();
		break;
	}
}
void SyntaxAnalysis::L() {
	if (errorFound) return;
	if (currentToken.getType() == T_END_OF_FILE) {
		eat(T_END_OF_FILE);
	}
	else {
		Q();
	}
}
void SyntaxAnalysis::E() {
	/*
	* E → add rid, rid, rid
	* E → addi rid, rid, num
	* E → sub rid, rid, rid
	* E → la rid, mid
	* E → lw rid, num(rid)
	* E → li rid, num
	* E → sw rid, num(rid)
	* E → b id
	* E → bltz rid, id
	* E → nop
	* E → mul rid, rid, rid
	* E → or rid, rid, rid
	* E → beq rid, rid, id
	* poslednje tri su dodate
	*/

	/*
	* Nakadna implementacija eata dovodi do punjenja liste instrukcija
	* Prvo uzimamo imena variabli, te uz pomoc findVariable dobijamo pokazivac na njih
	* Nakon toga ih grupisemo u instrukciju i dodajemo u listu
	*/

	switch (currentToken.getType()) {


	case T_ADD:
	{
		eat(T_ADD);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		Variable* dst = findVariable(dst_name);
		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition);
		instructionPosition++;
		instr->setTypeInstruction(I_ADD);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);

		instructions.push_back(instr);


		break;
	}

	case T_ADDI:
	{
		eat(T_ADDI);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		int num = stoi(currentToken.getValue());
		eat(T_NUM);
		Variable* dst = findVariable(dst_name);
		Variable* src = findVariable(src_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_ADDI);
		instr->setNumInstruction(num);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src);
		instructions.push_back(instr);
		break;
	}

	case T_SUB: {
		eat(T_SUB);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		Variable* dst = findVariable(dst_name);
		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition);
		instructionPosition++;
		instr->setTypeInstruction(I_SUB);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);

		instructions.push_back(instr);


		break;
	}
		

	case T_LA:
	{
		eat(T_LA);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src_name = currentToken.getValue();
		eat(T_M_ID);

		Variable* dst = findVariable(dst_name);
		Variable* src = findVariable(src_name);

		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_LA);
		instr->getSrcInstruction().push_back(src);
		
		instr->getDstInstruction().push_back(dst);

		instructions.push_back(instr);
		break;
	}

	case T_LW:
	{
		eat(T_LW);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		int num = stoi(currentToken.getValue());
		eat(T_NUM);
		eat(T_L_PARENT);
		std::string src_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_R_PARENT);

		Variable* dst = findVariable(dst_name);
		Variable* src = findVariable(src_name);

		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_LW);
		instr->setNumInstruction(num);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src);

		instructions.push_back(instr);
		break;
	}

	case T_LI:
	{
		eat(T_LI);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		int num = stoi(currentToken.getValue());
		eat(T_NUM);

		Variable* dst = findVariable(dst_name);

		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_LI);
		instr->setNumInstruction(num);
		instr->getDstInstruction().push_back(dst);

		instructions.push_back(instr);
		break;
	}

	case T_SW:
	{
		eat(T_SW);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		int num = stoi(currentToken.getValue());
		eat(T_NUM);
		eat(T_L_PARENT);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_R_PARENT);

		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);

		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_SW);
		instr->setNumInstruction(num);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);

		instructions.push_back(instr);
		break;
	}

	case T_B:
	{
		eat(T_B);
		std::string label_name = currentToken.getValue();
		eat(T_ID);
		Instruction* instr = new Instruction();
		instr->setLabelInstruction(label_name);
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_B);
		instructions.push_back(instr);
		break;
	}

	case T_BLTZ:
	{
		eat(T_BLTZ);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string label_name = currentToken.getValue();
		eat(T_ID);
		Variable* src1 = findVariable(src1_name);
		Instruction* instr = new Instruction();
		instr->setLabelInstruction(label_name);
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_BLTZ);
		instr->getSrcInstruction().push_back(src1);
		instructions.push_back(instr);
		break;
	}

	case T_NOP:
	{
		eat(T_NOP);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setTypeInstruction(I_NOP);
		instructions.push_back(instr);
		break;
	}

	case T_MUL:
	{
		eat(T_MUL);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		Variable* dst = findVariable(dst_name);
		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition);
		instructionPosition++;
		instr->setTypeInstruction(I_MUL);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);

		instructions.push_back(instr);

		break;
	}

	case T_OR:
	{
		eat(T_OR);
		std::string dst_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		Variable* dst = findVariable(dst_name);
		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition);
		instructionPosition++;
		instr->setTypeInstruction(I_OR);
		instr->getDstInstruction().push_back(dst);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);
		instructions.push_back(instr);
		break;
	}
	
	case T_BEQ:
	{
		eat(T_BEQ);
		std::string src1_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string src2_name = currentToken.getValue();
		eat(T_R_ID);
		eat(T_COMMA);
		std::string label_name = currentToken.getValue();
		eat(T_ID);
		Variable* src1 = findVariable(src1_name);
		Variable* src2 = findVariable(src2_name);
		Instruction* instr = new Instruction();
		instr->setPositionInstruction(instructionPosition++);
		instr->setLabelInstruction(label_name);
		instr->setTypeInstruction(I_BEQ);
		instr->getSrcInstruction().push_back(src1);
		instr->getSrcInstruction().push_back(src2);
		instructions.push_back(instr);

		break;
	}
	default:
		printSyntaxError(currentToken);
		errorFound = true;
		break;


	}


}

//	Implementacija findVariable
Variable* SyntaxAnalysis::findVariable(std::string name) {
	for (Variable* var : variables) {
		if (var->getNameVariable() == name) {
			return var;
		}
	}
	return nullptr;
}

void SyntaxAnalysis::printIR()
{
	cout << "\n===== IR ISPIS =====" << endl;

	cout << "Broj promenljivih: " << variables.size() << endl;
	cout << "Broj instrukcija: " << instructions.size() << endl;

	cout << "\nPromenljive:" << endl;
	for (Variable* var : variables)
	{
		cout << var->getPositionVariable() << " ";
		cout << var->getNameVariable() << " ";

		if (var->getTypeVariable() == Variable::REG_VAR)
			cout << "REG_VAR";
		else if (var->getTypeVariable() == Variable::MEM_VAR)
			cout << "MEM_VAR";
		else
			cout << "NO_TYPE";

		cout << endl;
	}

	cout << "\nInstrukcije:" << endl;
	for (Instruction* instr : instructions)
	{
		cout << instr->getPositionInstruction() << " ";

		switch (instr->getTypeInstruction())
		{
		case I_ADD:
			cout << "ADD";
			break;
		case I_SUB:
			cout << "SUB";
			break;
		case I_MUL:
			cout << "MUL";
			break;
		case I_OR:
			cout << "OR";
			break;
		case I_BEQ:
			cout << "BEQ";
			break;
		case I_ADDI: 
			cout << "ADDI"; 
			break;
		case I_LA: 
			cout << "LA"; 
			break;
		case I_LI: 
			cout << "LI"; 
			break;
		case I_LW: 
			cout << "LW"; 
			break;
		case I_SW: 
			cout << "SW"; 
			break;
		case I_B: 
			cout << "B"; 
			break;
		case I_BLTZ: 
			cout << "BLTZ";
			break;
		case I_NOP: cout << "NOP";
			break;
		default:
			cout << "OTHER";
			break;
		}

		cout << endl;
	}

	cout << "====================\n" << endl;
}

Variables& SyntaxAnalysis::getVariables()
{
	return variables;
}

Instructions& SyntaxAnalysis::getInstructions()
{
	return instructions;
}

// informacije o labelama
bool SyntaxAnalysis::containsLabel(std::string labelName)
{
	for (Label* lbl : labels)
	{
		if (lbl->getNameLabel() == labelName)
		{
			return true;
		}
	}

	return false;
}
Label* SyntaxAnalysis::findLabel(std::string name)
{
	for (Label* label : labels)
	{
		if (label->getNameLabel() == name)
		{
			return label;
		}
	}

	return nullptr;
}

bool SyntaxAnalysis::isStringDigit(std::string value)
{
	for (char c : value)
	{
		if (!isdigit(c))
		{
			return false;
		}
	}

	return true;
}

bool SyntaxAnalysis::isStringAlphaNum(std::string value)
{
	for (char c : value)
	{
		if (!isalnum(c))
		{
			return false;
		}
	}

	return true;
}
//	getter za labele
Labels& SyntaxAnalysis::getLabels()
{
	return labels;
}