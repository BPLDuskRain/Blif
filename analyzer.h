#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>
# include <array>
# include <stack>
# include <set>
# include <unordered_map>
# include <algorithm>

# include "MyTree.h"

constexpr auto GateTypeNums = 3;

constexpr auto UNIQUE = 1;
constexpr auto WEIGHT = 0;
constexpr auto INF = -1;

using wireType = std::string;

class Analyzer {
private:
	enum GateType {
		UNKNOWN, NOT = 1, AND = 2, OR = 3
	};
	struct Gate {
		wireType suc;
		std::vector<wireType> pres;
		GateType gateType = UNKNOWN;
		int cycle = 0;
		bool scheduled = false;
	};

	std::vector<std::string> tmpLines;
	std::unordered_map<std::string, std::vector<std::string>> tmpExps;
	char flag;
	std::unordered_map<wireType, Gate> tmpGates;
	std::vector<Gate*> huArray;

	void clear();
	bool inOutput(const wireType&)const;
	bool inInput(const wireType&)const;
	bool inMiddle(const wireType&)const;
	Gate expBreak(std::vector<std::string>* exp);

	int getGateCycle_ASAP(const Gate&);
	int getGatesCycle();
	void setGateCycle_ALAP(Gate&, int);
	int getMaxCycle(const int flag);
	void getHuArray();
	void Hu(int, int flag = UNIQUE);
	//bool preInSet(const std::set<Gate*>&, const Gate*);
	bool presAreScheduled(const Gate*);
	void cycleConfirmReset();
	void MLRCS(std::array<int, GateTypeNums>);
	bool srcEnough(const int)const;
	bool srcsEnough(const std::vector<int>&)const;
	std::vector<Gate*>::iterator elementRemove(Gate*);
	void MRLCS_Basic(int, std::array<int, GateTypeNums>& , const std::array<bool, GateTypeNums>&);
	void MRLCS(int, std::array<int, GateTypeNums>&, const std::array<bool, GateTypeNums>&);
	void setGateCycle_MRLCS(Gate&, int);
	int getGateCycle_MRLCS(const Gate&);
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
	void toMidForm_elementary();//只能处理单一门
	void toMidForm();
	void cycleConfirm_ASAP();
	void cycleConfirm_ALAP();
	void cycleConfirm_Hu(int, int flag = UNIQUE);
	int cycleConfirm_MLRCS(int);//废弃
	int cycleConfirm_MLRCS(std::array<int, GateTypeNums>);
	std::array<int, 3> cycleConfirm_MRLCS(int);
	void writeMidForm(int flag = UNIQUE);
};