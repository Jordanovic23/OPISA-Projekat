#pragma once

#include "Types.h"


/**
 * This class represents one variable from program code.
 */
class Variable
{
public:
	enum VariableType
	{
		MEM_VAR,
		REG_VAR,
		NO_TYPE
	};

	Variable() : m_type(NO_TYPE), m_name(""), m_position(-1), m_assignment(no_assign),m_degree(0),m_value(0) {}
	Variable(std::string name, int pos) : m_type(NO_TYPE), m_name(name), m_position(pos), m_assignment(no_assign),m_degree(0),m_value(0) {}

	//	pravimo gettere i settere za Variablu

	VariableType getTypeVariable() const {
		return m_type;
	}
	void setTypeVariable(VariableType v) {
		m_type = v;
	}
	std::string getNameVariable() const {
		return m_name;
	}
	void setNameVariable(std::string name) {
		m_name = name;
	}
	int getPositionVariable() const {
		return m_position;
	}
	void setPositionVariable(int pos) {
		m_position = pos;
	}
	Regs getRegVariable() const {
		return m_assignment;
	}
	void setRegVariable(Regs regi) {
		m_assignment = regi;
	}
	int getDegreeVariable() const {
		return m_degree;
	}
	void setDegreeVariable(int deg) {
		m_degree = deg;
	}
	int getValueVariable() const {
		return m_value;
	}
	void setValueVariable(int val) {
		m_value = val;
	}
	

private:
	VariableType m_type;
	std::string m_name;
	int m_position;
	Regs m_assignment;
	
	/*
	*	Dodajemo polje vezano za
	*	vrednost mem. promenljive
	*/
	int m_value;

	// Dodajemo polje vezano za broj suseda.
	int m_degree;
};


/**
 * This type represents list of variables from program code.
 */
typedef std::list<Variable*> Variables;

/**
 * This class represents one instruction in program code.
 */
class Instruction
{
public:
	// izmenili smo konstruktore dodajuci m_num
	Instruction () : m_position(0), m_type(I_NO_TYPE),m_num(0) {}
	Instruction (int pos, InstructionType type, Variables& dst, Variables& src,int mn) :
		m_position(pos), m_type(type), m_dst(dst), m_src(src),m_num(mn) {}

	// Getteri i setteri za Instruction

	int getPositionInstruction() const {
		return m_position;
	}
	void setPositionInstruction(int pos) {
		m_position = pos;
	}
	InstructionType getTypeInstruction() const {
		return m_type;
	}
	void setTypeInstruction(InstructionType ins) {
		m_type = ins;
	}
	Variables& getDstInstruction()  {
		return m_dst;
	}
	Variables& getSrcInstruction() {
		return m_src;
	}
	std::string getLabelInstruction() const {
		return m_label;
	}
	void setLabelInstruction(std::string name) {
		m_label = name;
	}
	int getNumInstruction() const {
		return m_num;
	}
	void setNumInstruction(int br) {
		m_num = br;
	}
	Variables& getUseInstruction() {
		return m_use;
	}

	Variables& getDefInstruction() {
		return m_def;
	}

	Variables& getInInstruction() {
		return m_in;
	}

	Variables& getOutInstruction() {
		return m_out;
	}

	std::list<Instruction*>& getSuccInstruction() {
		return m_succ;
	}

	std::list<Instruction*>& getPredInstruction() {
		return m_pred;
	}


private:
	int m_position;
	InstructionType m_type;
	
	Variables m_dst;
	Variables m_src;
	
	//	zbog naredbi skoka uvodimo sledece polje
	std::string m_label;
	//	zbog koriscenja konsanti uvodimo
	int m_num;

	Variables m_use;
	Variables m_def;
	Variables m_in;
	Variables m_out;
	std::list<Instruction*> m_succ;
	std::list<Instruction*> m_pred;
};


/**
 * This type represents list of instructions from program code.
 */
typedef std::list<Instruction*> Instructions;


/**
 * Dodajemo klasu kako bismo pamtili informacije o labelama, trebaju nam samo njeno ime i pozicija
 */
class Label
{
public:
	Label(std::string name, int pos)
		: m_name(name), m_position(pos)
	{
	}

	std::string getNameLabel() const {
		return m_name;
	}

	int getPositionLabel() const {
		return m_position;
	}

private:
	std::string m_name;
	int m_position;
};

/**
 * Samo lista pokazivac labela
 */
typedef std::list<Label*> Labels;
