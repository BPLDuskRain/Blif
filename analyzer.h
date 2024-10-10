#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <array>
# include <set>
# include <map>
# include <algorithm>

constexpr auto UNIQUE = 1;
constexpr auto WEIGHT = 0;
constexpr auto INFINITE = -1;

using wireType = std::string;

class Analyzer {
private:
	enum GateType{
		UNKNOWN, NOT = 1, AND = 2, OR = 3
	};
	struct Gate {
		std::vector<wireType> pres;
		GateType gateType = UNKNOWN;
		int cycle = -1;
		wireType suc;
		bool scheduled = false;
	};

	std::vector<std::string> tmpLines;
	char flag; 
	std::map<std::string, Gate> tmpGates;
	std::vector<Gate*> huArray;

	void clear();
	bool reOutput(const wireType&);
	bool reInput(const wireType&);
	bool reMiddle(const wireType&);
	int getGateCycle_ASAP(const Gate&);
	int getGatesCycle();
	void setGateCycle_ALAP(Gate&, int);
	int getMaxCycle(int flag);
	void getHuArray();
	void Hu(int, int flag = UNIQUE);
	//bool preInSet(const std::set<Gate*>&, const Gate*);
	bool presAreScheduled(const Gate*);
	void cycleConfirmReset();
public:
	std::string model;
	std::vector<wireType> inputs;
	std::vector<wireType> outputs;
	std::string clock;
	std::vector<wireType> middles;
	std::vector<std::string> expressions;

	bool readBlif(std::string filename);
	void analyze();
	void writeV(std::string filename);
	void toMidForm();
	void cycleConfirm_ASAP();
	void cycleConfirm_ALAP();
	void cycleConfirm_Hu(int, int flag = UNIQUE);
	int cycleConfirm_MLRCS(int);
	int cycleConfirm_MRLCS(int);
	void writeMidForm(int flag = UNIQUE);
};