#pragma once

#include "IR.h"
#include <string>
#include <iomanip>
#include <fstream>

class CodeGenerator
{
public:
	// Konstruktor
	CodeGenerator(Variables& variables, Instructions& instructions, Labels& labels);

	// Metoda koja poziva generisanje MIPS koda
	bool generateCode(std::string outputFileName);

private:
	Variables& variables;
	Instructions& instructions;
	Labels& labels;

	// Sluzi za vracanja registara u formatu stringa.
	std::string getRegisterName(Variable* var);

	/*
	*	Pozicija labele je predstavljena uz 
	*	pomoc prve instrukcije ispod nje,
	*	stoga na osnovu pozicije f-je
	*	ispisujemo labelu
	*/
	void writeLabelsForPosition(std::ofstream& out, int position);
};
