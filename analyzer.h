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

using wireType = std::string;

class Analyzer {
private:
	enum GateType{
		UNKNOWN, NOT = 1, AND = 2, OR = 3
	};
	struct Gate {wireType suc;
		std::vector<wireType> pres;
		GateType gateType = UNKNOWN;
		int cycle = -1;
		
		bool scheduled = false;
	};

	std::vector<std::string> tmpLines;
	char flag; 
	std::map<wireType, Gate> tmpGates;
	std::vector<Gate*> huArray;

	void clear();
	bool reOutput(const wireType&)const;
	bool reInput(const wireType&)const;
	bool reMiddle(const wireType&)const;

	int getGateCycle_ASAP(const Gate&);
	int getGatesCycle();
	void setGateCycle_ALAP(Gate&, int);
	int getMaxCycle(int flag);
	void getHuArray();
	void Hu(int, int flag = UNIQUE);
	//bool preInSet(const std::set<Gate*>&, const Gate*);
	bool presAreScheduled(const Gate*);
	void cycleConfirmReset();
	void MLRCS(std::vector<int>);
	bool srcEnough(const std::vector<int>&)const;
	std::vector<Gate*>::iterator elementRemove(Gate*);
public:
	std::string model;
	std::vector<wireType> inputs;
	std::vector<wireType> outputs;
	std::string clock;
	std::vector<wireType> middles;
	std::vector<std::string> expressions;

	bool readBlif(std::string filename);
	void analyze();
	void writeV(std::string filename)const;
	void toMidForm();
	void cycleConfirm_ASAP();
	void cycleConfirm_ALAP();
	void cycleConfirm_Hu(int, int flag = UNIQUE);
	int cycleConfirm_MLRCS(int);//·ÏÆú
	int cycleConfirm_MLRCS(std::vector<int>);
	int cycleConfirm_MRLCS(int);
	void writeMidForm(int flag = UNIQUE);
};