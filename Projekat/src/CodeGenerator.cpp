#include "CodeGenerator.h"
#include <fstream>

using namespace std;

CodeGenerator::CodeGenerator(Variables& vars, Instructions& instr, Labels& lab)
	: variables(vars), instructions(instr), labels(lab)
{
}

string CodeGenerator::getRegisterName(Variable* var)
{
	switch (var->getRegVariable())
	{
	case t0:
		return "$t0";
	case t1:
		return "$t1";
	case t2:
		return "$t2";
	case t3:
		return "$t3";
	default:
		return "$zero";
	}
}

bool CodeGenerator::generateCode(string outputFileName)
{
	ofstream out(outputFileName);

	if (!out.is_open())
	{
		return false;
	}
	string functionName = labels.front()->getNameLabel();
	out << ".globl " << functionName << endl << endl;

	out << ".data" << endl;

	for (Variable* var : variables)
	{
		if (var->getTypeVariable() == Variable::MEM_VAR)
		{
			out << var->getNameVariable()
				<< ":\t\t.word "
				<< var->getValueVariable()
				<< endl;
		}
	}

	out << endl;
	out << ".text" << endl;
	out << functionName << ":" << endl;


	for (Instruction* instr : instructions)
	{
		writeLabelsForPosition(out, instr->getPositionInstruction());

		out << "        " << left << setw(8);
		switch (instr->getTypeInstruction())
		{
		case I_ADD:
			out << "add"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().back()) << endl;
			break;

		case I_ADDI:
			out << "addi"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< instr->getNumInstruction() << endl;
			break;

		case I_SUB:
			out << "sub"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().back()) << endl;
			break;

		case I_MUL:
			out << "mul"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().back()) << endl;
			break;

		case I_LA:
			out << "la"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< instr->getSrcInstruction().front()->getNameVariable() << endl;
			break;

		case I_LI:
			out << "li"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< instr->getNumInstruction() << endl;
			break;

		case I_LW:
			out << "lw"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< instr->getNumInstruction()
				<< "(" << getRegisterName(instr->getSrcInstruction().front()) << ")" << endl;
			break;

		case I_SW:
			out << "sw"
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< instr->getNumInstruction()
				<< "(" << getRegisterName(instr->getSrcInstruction().back()) << ")" << endl;
			break;

		case I_B:
			out << "b"
				<< instr->getLabelInstruction() << endl;
			break;

		case I_BEQ:
			out << "beq"
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().back()) << ", "
				<< instr->getLabelInstruction() << endl;
			break;

		case I_BLTZ:
			out << "bltz"
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< instr->getLabelInstruction() << endl;
			break;

		case I_OR:
			out << "or"
				<< getRegisterName(instr->getDstInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().front()) << ", "
				<< getRegisterName(instr->getSrcInstruction().back()) << endl;
			break;

		case I_NOP:
			out << "nop" << endl;
			break;

		default:
			break;
		}
		
	}


	out.close();
	return true;
}

void CodeGenerator::writeLabelsForPosition(ofstream& out, int position)
{
	bool first = true;

	for (Label* label : labels)
	{
		if (first)
		{
			first = false;
			continue;
		}

		if (label->getPositionLabel() == position)
		{
			out << label->getNameLabel() << ":" << endl;
		}
	}

	
}