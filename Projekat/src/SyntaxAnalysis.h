#pragma once

#include "Token.h"
#include "LexicalAnalysis.h"
#include "IR.h"
#include <string>
#include <cctype>

class SyntaxAnalysis {
public:

	/**
	*	Konstruktor koji sluzi kako bismo mogli koristiti listu tokena
	*	prima se po referenci i takodje uz pomoc njega uspostavljamo kontakt sa leksickom analizom 
	*/

	SyntaxAnalysis(LexicalAnalysis& lex);

	//	Metoda koja nam proverava ispravnost sintaksne analize

	bool Do();

	//	Getteri
	Variables& getVariables();
	Instructions& getInstructions();
	Labels& getLabels();

private:

	//	Ispis greske do koje je doslo zbog sintaksne greske

	void printSyntaxError(Token token);

	//	Pomocna metoda za ispis informacija o tokenu

	void printTokenInfo(Token token);

	/*
	*	Metoda kojoj je prosledjen token odnosno proverava njegov tip
	*	Ako je ispravan prelazi na sledeci ako ne vraca gresku
	*/

	void eat(TokenType);

	// Metoda koja prosledjuje naredni token

	Token getNextToken();

	/*
	*	Naredno oznacavamo neterminalne karaktere
	*	To su u nasem slucaju Q, S, L i E
	*/

	void Q();
	void S();
	void L();
	void E();

	/*
	*	Referenca koja pokazuje na objekat same leksicke analize
	*	Jednostavno prolazimo kroz sve ono sto je leksicki analizator obradio
	*/

	LexicalAnalysis& lexicalAnalysis;

	//	Oznaka da li je nadjena sintaksna greska

	bool errorFound;

	// Iterator na listu tokena dobijenih leksickom analizom
	
	TokenList::iterator tokenIterator;

	//	Token koji se trenutno obradjuje

	Token currentToken;

	// Naknadno dodajemo polja vezana za varijable,instrukcije i labele

	Variables variables;
	Instructions instructions;
	Labels labels;

	int variablePosition;
	int instructionPosition;


	// findVariable() vraca pokazivac na variablu sa datim imenom ako postoji u listi

	Variable* findVariable(std::string name);

	// slicno sa labelama
	Label* findLabel(std::string name);

	// da li data labela postoji
	bool containsLabel(std::string labelName);

	// pomocna funkcija za ispis
	void printIR();

	// za proveru funkcija memorijskih i registarskih promeniljivih
	bool isStringDigit(std::string value);
	bool isStringAlphaNum(std::string value);

};