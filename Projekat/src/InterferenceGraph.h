#pragma once

#include "IR.h"
#include <vector>
#include <stack>

/*
* Klasa koja predstavlja graf smetnji.
* Graf se formira na osnovu rezultata analize zivotnog veka.
*/

class InterferenceGraph
{
public:

	/*
	* Konstruktor prima liste promenljivih i instrukcija.
	*/

	InterferenceGraph(Variables& variables, Instructions& instructions);

	/*
	* Pokretanje formiranja grafa smetnji.
	*/

	bool doInterferenceGraph();

private:

	Variables& variables;
	Instructions& instructions;
	Variables regVariables;

	/*
	* Matrica smetnji.
	* 0 znaci da smetnja ne postoji,
	* 1 znaci da smetnja postoji.
	*/

	std::vector<std::vector<int>> interferenceMatrix;

	void initializeMatrix();
	void buildGraph();
	void printMatrix();

	// kako bismo imali index variable koja je registarskog tipa
	// to ranije nisam obradio 
	int getRegVariableIndex(Variable* var);

	// Izracunavanje broja suseda kao i ispis.
	void calculateDegree();
	void printDegree();

	// Dolazi faza uproscavanja pa pripremamo stek
	std::stack<Variable*> simplificationStack;

	// Sluzi za uproscvanja grafa kao i ispis
	bool simplifyGraph();
	void printSimplificationStack();

	// Dodeljivanje odnosno alokacija registara
	bool allocateRegisters();
	int getAvailableRegister(Variable* var);
	void printAllocation();
};