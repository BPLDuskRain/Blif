#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <array>

using wireType = std::string;

class Analyzer {
private:
	enum GateType{
		UNKNOWN, AND, OR, NOT
	};
	struct Gate {
		std::vector<wireType> pres;
		GateType gateType = UNKNOWN;
		int cycle;
		wireType suc;
	};

	std::vector<std::string> tmpLines;
	char flag; 
	std::vector<Gate> tmpGates;

	bool reOutput(const wireType&);
	bool reInput(const wireType&);
	bool reMiddle(const wireType&);
	void cycleConfirm(Gate&);
	bool allInInputs(const Gate&);
	int maxCircle(const Gate&);
public:
	std::string model;
	std::vector<wireType> inputs;
	std::vector<wireType> outputs;
	std::string clock;
	std::vector<wireType> middles;
	std::vector<std::string> expressions;

	void readBlif(std::string filename);
	void analyze();
	void writeV(std::string filename);
	void toMidForm();
	void writeMidForm();
};