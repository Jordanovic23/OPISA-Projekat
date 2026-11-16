#pragma once

#include "IR.h"

/*
* Klasa za analizu zivotnog veka promenljivih.
* Kao ulaz prima listu instrukcija formiranu tokom sintaksne analize.
*/

class LivenessAnalysis
{
public:

	/*
	* Konstruktor prima referencu na listu instrukcija.
	*/

	LivenessAnalysis(Instructions& instructions,Labels& labels);

	/*
	* Pokretanje analize.
	* Za pocetak ce samo formirati USE i DEF skupove.
	*/

	bool doAnalysis();

private:

	/*
	* Referenca na listu instrukcija (labela) nad kojima se vrsi analiza.
	*/

	Instructions& instructions;
	Labels& labels;

	/*
	* Formiranje USE i DEF skupova za svaku instrukciju.
	*/

	void makeUseDef();

	// Pomocna metoda za proveru da li su use i def pravilno popunjeni
	void printUseDef();

	// Formiranje liste prethodnika i sledbenika te potom ispis
	void makeSuccPred();
	void printSuccPred();


	// Pomocna funkcija koja vraca pokazivac na instrukciju na osnovu pozicije
	Instruction* findInstructionByPosition(int position);

	/*
	* Pomocna metoda za pronalazak instrukcije
	* na koju pokazuje prosledjena labela.
	*/

	Instruction* findInstructionForLabel(std::string labelName);


	/*
	* Metode za izracunavanje in i out skupova uz pomoc algoritma
	* Pomocna metoda koja sluzi za ispis
	*/
	void calculateInOut();
	void printInOut();

	// Pomocna metoda koja nam proverava da li se promenljiva nalazi u listi
	bool variableExistsInList(Variable* var, Variables& vars);

};