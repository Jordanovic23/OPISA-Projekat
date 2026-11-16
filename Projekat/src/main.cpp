#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenessAnalysis.h"
#include "InterferenceGraph.h"
#include "CodeGenerator.h"

#include <iostream>
#include <exception>

using namespace std;

void main()
{
	try
	{
		std::string fileName = ".\\..\\examples\\simple.mavn";
		bool retVal = false;

		LexicalAnalysis lex;
		
		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");
		
		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		/*
		* Pokretanje leksicke analize
		*/

		SyntaxAnalysis syntax(lex);

		if (syntax.Do())
		{
			cout << "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("\nException! Syntax analysis failed!\n");
		}

		/*
		*	Kreiranje objekta klase LivenessAnalysis
		*	Poziv analize zivotnog veka
		*/
		LivenessAnalysis liveness(syntax.getInstructions(),syntax.getLabels());

		if (liveness.doAnalysis())
		{
			cout << "Liveness analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("\nException! Liveness analysis failed!\n");
		}

		InterferenceGraph interferenceGraph(syntax.getVariables(),syntax.getInstructions());

		if (interferenceGraph.doInterferenceGraph()) {
			cout << "Interference graph created successfully!" << endl;
		}
		else {
			throw runtime_error("\nException! Interference graph failed to create!\n");
		}
		CodeGenerator generator(
			syntax.getVariables(),
			syntax.getInstructions(),
			syntax.getLabels());

		if (generator.generateCode("output.s"))
		{
			cout << "Code generation finished successfully!" << endl;
		}
		else
		{
			cout << "Code generation failed!" << endl;
		}

	}
	catch (runtime_error e)
	{
		cout << e.what() << endl;
	}
}