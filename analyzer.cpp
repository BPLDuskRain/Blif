# include "Analyzer.h"

bool Analyzer::inOutput(const wireType& name) const {
	for (std::string p : outputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::inInput(const wireType& name) const {
	for (std::string p : inputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::inMiddle(const wireType& name) const {
	for (std::string p : middles) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

void Analyzer::clear() {
	tmpLines.clear();
	tmpExps.clear();
	tmpGates.clear();
	huArray.clear();
	inputs.clear();
	outputs.clear();
	middles.clear();
	expressions.clear();
}

bool Analyzer::readBlif(std::string filename) {
	clear();
	std::ifstream infilestream(filename);

	if (infilestream.is_open()) {
		std::string line;
		while (getline(infilestream, line)) {
			tmpLines.push_back(line);
		}
		infilestream.close();
		return true;
	}
	return false;
}

void Analyzer::analyze() {
	for (auto it = tmpLines.begin(); it != tmpLines.end(); it++) {
		if (it->at(0) != '.') return;
		size_t divider = it->find(' ');
		if (divider == std::string::npos) {
			std::string substring = it->substr(1);
			if (substring == "end") return;
			std::cout << "Delimiter not found!\n";
			return;
		}
		std::string substring = it->substr(1, divider - 1);
		//找model
		if (substring == "model") {
			model = it->substr(divider + 1);
		}
		//找inputs
		else if (substring == "inputs") {
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string input;
			while (std::getline(tmpStream, input, ' ')) {
				inputs.push_back(input);
			}
		}
		//找outputs
		else if (substring == "outputs") {
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string output;
			while (std::getline(tmpStream, output, ' ')) {
				outputs.push_back(output);
			}
		}
		//解析names
		else if (substring == "names") {
			Gate gate;
			std::string result;
			//names
			std::vector<std::string> names;
			//read temp strings
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string name;
			while (std::getline(tmpStream, name, ' ')) {
				//记录变量名字
				names.push_back(name);
				//补充中间变量
				if (inOutput(name) || inInput(name) || inMiddle(name)) continue;
				middles.push_back(name);
			}
			result = name;
			names.pop_back();

			//读入真值表
			std::vector<std::string> tmpExp;
			while (true) {
				it++;
				if (it->at(0) == '.') {
					it--;
					break;
				}

				for (int namePos = 0, linePos = 0; namePos < names.size() || linePos < it->size(); namePos++, linePos++) {
					//确定是析取还是合取
					size_t space = it->find(" ");
					if (space != std::string::npos) {
						flag = it->at(space + 1);
					}
					//对于直接赋值语句
					else if (it->size() == 1) {
						tmpExp.push_back(*it);
						goto Assign;
					}

					//读到空格停止
					if (it->at(linePos) == ' ') {
						linePos++;
						break;
					}
					//根据flag判定非门
					if (it->at(linePos) != '-') {
						if ((it->at(linePos) == '0' && flag == '1') || (it->at(linePos) == '1' && flag == '0')) {
							tmpExp.push_back("!");
						}
						//同行均用与门
						tmpExp.push_back(names[namePos]);
						tmpExp.push_back(" & ");
					}
				}
				//行间用或门
				tmpExp.pop_back();
				tmpExp.push_back(" | ");
			}
			tmpExp.pop_back();
			tmpExps.insert({ result, tmpExp });
		Assign:

			//组合表达式
			std::ostringstream oss;
			oss << result << " = ";

			for (int expPos = 0; expPos < tmpExp.size(); expPos++) {
				oss << tmpExp[expPos];
			}
			expressions.push_back(oss.str());
		}
		else std::cerr << "Invalid name\n";
	}
}

void Analyzer::writeV(std::string filename) const {
	std::ofstream outfilestream(filename);

	//module行
	outfilestream << "module " << model
		<< "(clk, rst, ";
	size_t size = inputs.size();
	for (std::string p : outputs) {
		outfilestream << p << ", ";
	}
	for (std::string p : inputs) {
		outfilestream << p;
		size--;
		if (size != 0) {
			outfilestream << ", ";
		}
		else outfilestream << ");\n";
	}

	//第一input
	outfilestream << "input clk, rst;\n";

	//outputs
	for (std::string p : outputs) {
		outfilestream << "output " << p << ";\n";
	}

	//inputs
	for (std::string p : inputs) {
		outfilestream << "input " << p << ";\n";
	}

	outfilestream << std::endl;

	//wire
	for (std::string p : inputs) {
		outfilestream << "wire " << p << ";\n";
	}
	for (std::string p : outputs) {
		outfilestream << "wire " << p << ";\n";
	}
	for (std::string p : middles) {
		outfilestream << "wire " << p << ";\n";
	}

	outfilestream << std::endl;

	//assign
	for (std::string p : expressions) {
		outfilestream << "assign " << p << ";\n";
	}

	outfilestream << std::endl << "endmodule";

	outfilestream.close();
}

void Analyzer::toMidForm_elementary() {
	tmpGates.clear();
	for (auto it = tmpLines.begin(); it != tmpLines.end(); ++it) {
		if (it->at(0) != '.') continue;
		size_t divider = it->find(' ');
		if (divider == std::string::npos) {
			std::string substring = it->substr(1);
			if (substring == "end") return;
			std::cout << "Delimiter not found!\n";
			return;
		}
		std::string substring = it->substr(1, divider - 1);
		//找inputs
		if (substring == "inputs") {
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string input;
			while (std::getline(tmpStream, input, ' ')) {
				inputs.push_back(input);

			}
		}
		//找outputs
		else if (substring == "outputs") {
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string output;
			while (std::getline(tmpStream, output, ' ')) {
				outputs.push_back(output);
			}
		}
		//解析names
		else if (substring == "names") {
			Gate gate;
			//read temp strings
			std::istringstream tmpStream(it->substr(divider + 1));
			std::string name;
			while (std::getline(tmpStream, name, ' ')) {
				gate.pres.push_back(name);
			}
			gate.pres.pop_back();
			gate.suc = name;

			//读入真值表
			//只能处理单一门！！！
			++it;
			if (it->find('-') != std::string::npos) {
				gate.gateType = OR;
			}
			else if (it->size() == 3) {
				gate.gateType = NOT;
			}
			else {
				gate.gateType = AND;
			}
			gate.cycle = 0;
			tmpGates.insert({ name, gate });
		}
	}
}

void Analyzer::toMidForm() {
	analyze();
	for (auto& exp : tmpExps) {
		MyTree myTree;
		for (auto& ch : exp.second) {
			myTree.add(ch);
		}
		Gate gate = expBreak(myTree.post_order_toString());
		gate.suc = exp.first;
		tmpGates.insert({ gate.suc, gate });
	}
}

Analyzer::Gate Analyzer::expBreak(std::vector<std::string>* exp) {
	std::stack<std::string> bufs;
	const std::string tmp = "_tmp";
	int count = 0;
	int flag = UNKNOWN;//用于判断上一个符号是否和本次一致，即是否可级联
	for (auto ch_ptr = exp->begin(); ch_ptr < exp->end(); ++ch_ptr) {
		//非门（一元）则取末缓冲区形成表达式
		if (*ch_ptr == "!") {
			Gate gate;
			gate.pres.push_back(bufs.top()); bufs.pop();
			gate.gateType = NOT;
			//最后一个式子直接返回，suc交给外部处理
			if (ch_ptr + 1 == exp->end()) return gate;
			gate.suc = tmp + std::to_string(count++);
			tmpGates.insert({ gate.suc, gate });
			//生成中间值填充末缓冲区
			bufs.push(gate.suc);
			continue;
		}
		//与或门（二元）则取两缓冲区形成表达式
		if (*ch_ptr == " | " || *ch_ptr == " & ") {
			Gate gate;
			if (*ch_ptr == " | ") gate.gateType = OR;
			if (*ch_ptr == " & ") gate.gateType = AND;
			//可级联时
			if (gate.gateType == flag) {
				//从tmpGates中取出上一个gate并保存
				gate = tmpGates[bufs.top()];
				tmpGates.erase(bufs.top());
				//tmp出栈
				bufs.pop();
				//下方数据进gate，出栈
				gate.pres.push_back(bufs.top()); bufs.pop();

				//若为最后一个式子直接返回，suc交给外部处理
				if (ch_ptr + 1 == exp->end()) return gate;

				tmpGates.insert({ gate.suc, gate });
			}
			//不可级联
			else {
				//弹出两个操作数
				gate.pres.push_back(bufs.top()); bufs.pop();
				gate.pres.push_back(bufs.top()); bufs.pop();

				//若为最后一个式子直接返回，suc交给外部处理
				if (ch_ptr + 1 == exp->end()) return gate;

				gate.suc = tmp + std::to_string(count++);

				tmpGates.insert({ gate.suc, gate });
			}
			//生成中间值填充末缓冲区;
			bufs.push(gate.suc);
			flag = gate.gateType;
			continue;
		}
		//填充缓冲区
		bufs.push(*ch_ptr);
		flag = UNKNOWN;
	}
}

void Analyzer::cycleConfirm_ASAP() {
	//遍历tmpGates全部确定cycle
	for (auto pairP = tmpGates.begin(); pairP != tmpGates.end(); ++pairP) {
		pairP->second.cycle = getGateCycle_ASAP(pairP->second);
	}
}

int Analyzer::getGateCycle_ASAP(const Gate& gate) {
	int max = -1;

	for (auto& pre : gate.pres) {
		//若节点前驱为inputs不在tmpGates中，会直接跳过，返回0
		if (tmpGates.find(pre) != tmpGates.end()) {
			//递归确定前驱cycle，取最大值，+1作为自己的cycle
			int tmp = getGateCycle_ASAP(tmpGates[pre]);
			if (tmp > max) {
				max = tmp;
			}
		}
	}
	return max + 1;
}

void Analyzer::cycleConfirm_ALAP() {
	//先全部赋值为最大值
	int maxCycle = getGatesCycle() - 1;
	for (auto gateP = tmpGates.begin(); gateP != tmpGates.end(); ++gateP) {
		gateP->second.cycle = maxCycle;
	}

	//通过递归函数setGateCycle溯源赋值，因此只需要遍历输出
	for (auto outputP = outputs.begin(); outputP < outputs.end(); ++outputP) {
		setGateCycle_ALAP(tmpGates[*outputP], maxCycle);
	}
}

int Analyzer::getGatesCycle() {
	//确定全流程最大cycle
	int max = 0;
	for (auto p = outputs.begin(); p < outputs.end(); ++p) {
		int tmp = getGateCycle_ASAP(tmpGates[*p]);
		if (tmp > max) {
			max = tmp;
		}
	}
	return max + 1;
}

void Analyzer::setGateCycle_ALAP(Gate& gate, int cy) {
	//利用cy控制本gate的cycle，对于相同前驱节点，会在多次遍历中自动找到最小值
	if (cy < gate.cycle) {
		gate.cycle = cy;
	}
	for (auto& pre : gate.pres) {
		//递归确定gate前驱的cycle（input不需要cycle）
		if (!inInput(pre)) {
			setGateCycle_ALAP(tmpGates[pre], cy - 1);
		}
	}
}

int Analyzer::getMaxCycle(const int flag) {
	int max = 0;
	switch (flag) {
	case UNIQUE:
		for (auto& pair : tmpGates) {
			if (pair.second.cycle + 1 > max) {
				max = pair.second.cycle + 1;
			}
		}
		break;
	case WEIGHT:
		for (auto& pair : tmpGates) {
			if (pair.second.cycle + pair.second.gateType > max) {
				max = pair.second.cycle + pair.second.gateType;
			}
		}
		break;
	}
	return max;
}

void Analyzer::getHuArray() {
	//先使用ALAP得到基础cycle，再按cycle排序塞进Gate数组huArray
	cycleConfirm_ALAP();
	huArray.clear();
	for (auto gateP = tmpGates.begin(); gateP != tmpGates.end(); ++gateP) {
		huArray.push_back(&(gateP->second));
	}
	std::sort(huArray.begin(), huArray.end(), [](const Gate* a, const Gate* b) -> bool {
		return a->cycle < b->cycle;
	});
}

void Analyzer::Hu(int limit, int flag) {
	//Hu算法主体，需要先获取huArray
	int lmt = limit;
	int cycle = 0;
	std::set<Gate*> set = std::set<Gate*>();//正在执行的节点
	for (auto index = huArray.begin(); index < huArray.end();) {
		//资源足够
		if (lmt > 0) {
			//未被执行
			if (!(*index)->scheduled) {
				//不在执行且前驱全部准备，可执行
				if (set.find(*index) == set.end() && presAreScheduled(*index)) {
					(*index)->cycle = cycle;
					set.insert(*index);
					--lmt;
					++index;
				}
				else {
					//前驱未准备好或已经在执行
					++index;
					//若集合到末尾，进入下一轮，重新遍历
					if (index == huArray.end()) {
						index = huArray.begin();
						goto NotEnough;
					}
				}
			}
			//已被执行直接跳过
			else ++index;
		}
		//资源不足加轮次
		else {
			NotEnough: 
			switch (flag) {
			case UNIQUE:
				for (auto setP = set.begin(); setP != set.end(); ++setP) {
					if ((*setP)->cycle == cycle) {
						(*setP)->scheduled = true;
					}
				}
				set.clear();
				break;
			case WEIGHT:
				for (auto setP = set.begin(); setP != set.end();) {
					if ((*setP)->cycle + (*setP)->gateType - 1 == cycle) {
						(*setP)->scheduled = true;
						setP = set.erase(setP);
					}
					else {
						++setP;
					}
				}
				break;
			}
			++cycle;
			lmt = limit - set.size();
			index = huArray.begin();
		}
	}
}

void Analyzer::cycleConfirmReset() {
	for (auto& gatePair : tmpGates) {
		gatePair.second.cycle = 0;
		gatePair.second.scheduled = false;
	}
}

void Analyzer::cycleConfirm_Hu(int limit, int flag) {
	cycleConfirmReset();
	getHuArray();
	Hu(limit, flag);
}

//bool Analyzer::preInSet(const std::set<Gate*>& set, const Gate* gate) {
//	for (auto pre : gate->pres) {
//		if (set.find(&tmpGates[pre]) != set.end()) {
//			return true;
//		}
//	}
//	return false;
//}

bool Analyzer::presAreScheduled(const Gate* gate) {
	for (auto& pre : gate->pres) {
		//非inputs
		if (tmpGates.find(pre) != tmpGates.end()) {
			if (!tmpGates[pre].scheduled) return false;
		}
	}
	return true;
}

int Analyzer::cycleConfirm_MLRCS(int limit) {
	//使用有权重的Hu算法，并返回轮次
	cycleConfirm_Hu(limit, WEIGHT);
	return getMaxCycle(WEIGHT);
}

int Analyzer::cycleConfirm_MLRCS(std::array<int, GateTypeNums> limits, int flag) {
	cycleConfirmReset();
	getHuArray();
	MLRCS(limits, flag);
	return getMaxCycle(flag);
}

void Analyzer::MLRCS(std::array<int, GateTypeNums> limits, int flag) {
	//MLRCS算法主体，需要先获取huArray
	std::vector<int> lmts(limits.begin(), limits.end());
	int cycle = 0;
	std::set<Gate*> set = std::set<Gate*>();//正在执行的节点
	for (auto index = huArray.begin(); !huArray.empty();) {
		//资源足够
		if (srcsEnough(lmts)) {
			//不在执行且前驱全部准备，可执行
			if (set.find(*index) == set.end() && presAreScheduled(*index)) {
				//按门结算资源
				switch ((*index)->gateType) {
				case AND:
					if (srcEnough(lmts[0])) {
						(*index)->cycle = cycle;
						set.insert(*index);
						if(lmts[0] != INF) --lmts[0];
					}
					break;
				case OR:
					if (srcEnough(lmts[1])) {
						(*index)->cycle = cycle;
						set.insert(*index);
						if (lmts[1] != INF) --lmts[1];
					}
					break;
				case NOT:
					if (srcEnough(lmts[2])) {
						(*index)->cycle = cycle;
						set.insert(*index);
						if (lmts[2] != INF) --lmts[2];
					}
					break;
				}
				++index;
				if (index == huArray.end()) {
					index = huArray.begin();
					goto NotEnough;
				}
			}
			else {
				//前驱未准备好或已经在执行
				++index;
				//若集合到末尾，进入下一轮，重新遍历
				if (index == huArray.end()) {
					index = huArray.begin();
					goto NotEnough;
				}
			}
		}
		//资源不足加轮次
		else {
		NotEnough:
			//遍历set，如果执行完毕将其踢出huArrry和set，归还资源
			switch (flag) {
			case UNIQUE:
				for (auto setP = set.begin(); setP != set.end();) {
					(*setP)->scheduled = true;
					switch ((*setP)->gateType) {
					case AND: if (lmts[0] != INF) ++lmts[0]; break;
					case OR: if (lmts[1] != INF) ++lmts[1]; break;
					case NOT: if (lmts[2] != INF) ++lmts[2]; break;
					}
					index = elementRemove((*setP));
					setP = set.erase(setP);
				}
				break;
			case WEIGHT:
				for (auto setP = set.begin(); setP != set.end();) {
					if ((*setP)->cycle + (*setP)->gateType - 1 == cycle) {
						(*setP)->scheduled = true;
						switch ((*setP)->gateType) {
						case AND: if (lmts[0] != INF) ++lmts[0]; break;
						case OR: if (lmts[1] != INF) ++lmts[1]; break;
						case NOT: if (lmts[2] != INF) ++lmts[2]; break;
						}
						index = elementRemove((*setP));
						setP = set.erase(setP);
					}
					else {
						++setP;
					}
				}
				break;
			}
			++cycle;
			index = huArray.begin();
		}
	}
}

bool Analyzer::srcEnough(const int lmt) const {
	return lmt > 0 || lmt == INF;
}

bool Analyzer::srcsEnough(const std::vector<int>& lmts) const {
	for (auto& lmt : lmts) {
		if (srcEnough(lmt)) return true;
	}
	return false;
}

std::vector<Analyzer::Gate*>::iterator Analyzer::elementRemove(Gate* gate) {
	auto it = std::find(huArray.begin(), huArray.end(), gate);
	if (it != huArray.end()) {
		return huArray.erase(it);
	}
	return huArray.end();
}

//int Analyzer::cycleConfirm_MRLCS(int limit) {
//	//判断无限资源所需轮次
//	cycleConfirmReset();
//	getHuArray();
//	const int minCycle = getGatesCycle();
//	//无限资源下轮次大于要求则不能完成
//	if (minCycle > limit) {
//		return -1;
//	}
//
//	int cycle = 0;
//	int res = 1;
//	//资源由小到大，轮次则由大到小
//	//轮次正好小于等于限制，即为最小资源
//	while (true) {
//		cycleConfirmReset();
//		Hu(res, WEIGHT);
//		cycle = getMaxCycle(WEIGHT);
//		if (cycle <= limit) {
//			break;
//		}
//		++res;
//	}
//	return res;
//}

std::array<int, GateTypeNums> Analyzer::cycleConfirm_MRLCS(int limit) {
	//判断无限资源所需轮次
	const int minCycle = cycleConfirm_MLRCS({INF, INF, INF}, WEIGHT);
	//无限资源下轮次大于要求则不能完成
	if (minCycle > limit) {
		return { INF, INF, INF };
	}
	//遍历所有门保存数量
	std::array<int, GateTypeNums> gateNum = {0, 0, 0};
	for (auto& gatePair : tmpGates) {
		switch (gatePair.second.gateType) {
		case AND: ++gateNum[0]; break;
		case OR : ++gateNum[1]; break;
		case NOT: ++gateNum[2]; break;
		}
	}
	//排除不需要的门
	std::array<bool, GateTypeNums> needGate = { true, true, true };
	for (int i = 0; i < gateNum.size(); ++i) {
		if (gateNum[i] == 0) needGate[i] = false;
	}
	//如果不需要某种门，则其资源是0
	std::array<int, GateTypeNums> resourseNum = {0, 0, 0};
	for (int i = 0; i < resourseNum.size(); ++i) {
		if (needGate[i]) resourseNum[i] = 1;
	}

	//为需要的门置1试探
	int cycle = cycleConfirm_MLRCS(resourseNum, WEIGHT);
	if (cycle <= limit) return resourseNum;

	MRLCS(limit, resourseNum, needGate);
	return resourseNum;
}

//MRLCS：暴力解法，存在死循环情况弃用
void Analyzer::MRLCS_Basic(int limit, std::array<int, GateTypeNums>& resourseNum, const std::array<bool, GateTypeNums>& needGate) {
	int cycle = cycleConfirm_MLRCS(resourseNum, WEIGHT);
	int index = 0;
	//资源由小到大，轮次则由大到小
	//轮次正好小于等于限制，即为最小资源
	while (true) {
		//尝试增加资源
		if (needGate[index]) ++resourseNum[index];
		int tmp = cycleConfirm_MLRCS(resourseNum, WEIGHT);
		//增加资源有效，覆写cycle
		if (tmp < cycle) cycle = tmp;
		//无效，撤回增加资源
		else --resourseNum[index];
		//索引自增（循环）
		index = ++index % resourseNum.size();
		if (cycle <= limit) return;
	}
}

void Analyzer::MRLCS(int limit, std::array<int, GateTypeNums>& resourseNum, const std::array<bool, GateTypeNums>& needGate) {
	std::vector<std::array<int, GateTypeNums>> needNum;
	for (int i = 0; i < limit; ++i) needNum.push_back({});
	//同ALAP
	for (auto& gatePair : tmpGates) {
		gatePair.second.cycle = limit;
	}
	for (const auto& output : outputs) {
		setGateCycle_MRLCS(tmpGates[output], limit);
	}
	//最早回合
	std::vector<Gate> tmpGatesArray;
	for (const auto& gatePair : tmpGates) {
		int earliestStartCycle = getGateCycle_MRLCS(gatePair.second);
		if (earliestStartCycle == gatePair.second.cycle) continue;
		tmpGatesArray.push_back(gatePair.second);
	}
	//按最终cycle降序排列
	std::sort(tmpGatesArray.begin(), tmpGatesArray.end(), [](const Gate a, const Gate b) -> bool {
		return a.cycle > b.cycle;
	});
	for (auto& gate : tmpGatesArray) {
		gate.cycle = getGateCycle_MRLCS(gate);
	}
	//填充需求数组
	for (auto& gatePair : tmpGates) {
		switch (gatePair.second.gateType) {
		case AND: for (int i = 0; i < AND; ++i) ++needNum[gatePair.second.cycle + i][0]; break;
		case OR : for (int i = 0; i < OR ; ++i) ++needNum[gatePair.second.cycle + i][1]; break;
		case NOT: for (int i = 0; i < NOT; ++i) ++needNum[gatePair.second.cycle + i][2]; break;
		}
	}
	//除去浮动数
	for (auto& gate : tmpGatesArray) {
		switch (gate.gateType) {
		case AND: for (int i = 0; i < AND; ++i) --needNum[tmpGates[gate.suc].cycle + i][0]; break;
		case OR : for (int i = 0; i < OR ; ++i) --needNum[tmpGates[gate.suc].cycle + i][1]; break;
		case NOT: for (int i = 0; i < NOT; ++i) --needNum[tmpGates[gate.suc].cycle + i][2]; break;
		}
	}
	//初步确定资源
	for (auto& cycleGates : needNum) {
		for (int i = 0; i < GateTypeNums; ++i) {
			if (cycleGates[i] > resourseNum[i]) resourseNum[i] = cycleGates[i];
		}
	}
	//根据浮动数尝试增添
	while (true) {
		auto gate = tmpGatesArray.back();
		int earliestStartCycle = gate.cycle;
		int latestStartcycle = tmpGates[gate.suc].cycle;
		//遍历浮动数可能的选择
		for (int i = earliestStartCycle; i < latestStartcycle; ++i) {
			//若可先做，则：置cycle、needNum矩阵修改、弹出数组，跳出
			switch (gate.gateType) {
			case AND: 
				if (needNum[i][0] + 1 <= resourseNum[0]) { 
					tmpGates[gate.suc].cycle = i;
					for (int j = 0; j < AND; ++j) ++needNum[i + j][0];
					tmpGatesArray.pop_back();
					goto OUTER;
				}
				else break;
			case OR:  
				if (needNum[i][1] + 1 <= resourseNum[1]) { 
					tmpGates[gate.suc].cycle = i; 
					for (int j = 0; j < OR ; ++j) ++needNum[i + j][1];
					tmpGatesArray.pop_back(); 
					goto OUTER; 
				}
				   else break;
			case NOT: 
				if (needNum[i][2] + 1 <= resourseNum[2]) {
					tmpGates[gate.suc].cycle = i;
					for (int j = 0; j < NOT; ++j) ++needNum[i + j][2];
					tmpGatesArray.pop_back();
					goto OUTER;
				}
				else break;
			}
		}
		//不可则加资源
		switch (gate.gateType) {
		case AND: ++resourseNum[0]; break;
		case OR : ++resourseNum[1]; break;
		case NOT: ++resourseNum[2]; break;
		}
		OUTER:
		if (tmpGatesArray.size() == 0) break;
	}
}

void Analyzer::setGateCycle_MRLCS(Gate& gate, int limit) {
	int latestStartCycle = limit - gate.gateType;
	if (latestStartCycle < gate.cycle) {
		gate.cycle = latestStartCycle;
	}
	for (const auto& pre : gate.pres) {
		//递归确定gate前驱的cycle（input不需要cycle）
		if (!inInput(pre)) {
			setGateCycle_MRLCS(tmpGates[pre], latestStartCycle);
		}
	}
}

int Analyzer::getGateCycle_MRLCS(const Gate& gate) {
	int max = 0;

	for (auto& pre : gate.pres) {
		//若节点前驱为inputs不在tmpGates中，会直接跳过，返回0
		if (tmpGates.find(pre) != tmpGates.end()) {
			//递归确定前驱cycle，取最大值，+gateType作为自己的cycle
			int tmp = getGateCycle_MRLCS(tmpGates[pre]) + tmpGates[pre].gateType;
			if (tmp > max) {
				max = tmp;
			}
		}
	}
	return max;
}

void Analyzer::writeMidForm(int flag) {
	std::cout << "Input :";
	for (int i = 0; i < inputs.size(); i++) {
		std::cout << inputs[i];
		if (i + 1 < inputs.size()) {
			std::cout << ", ";
		}
		else {
			std::cout << ' ';
		}
	}
	std::cout << "Output :";
	for (int i = 0; i < outputs.size(); i++) {
		std::cout << outputs[i];
		if (i + 1 < outputs.size()) {
			std::cout << ", ";
		}
		else {
			std::cout << " \n";
		}
	}

	int max = getMaxCycle(flag);
	std::cout << "Total " << max << " Cycles\n";
	std::vector<std::array<std::vector<wireType>, GateTypeNums>> cycles;
	for (int i = 0; i < max; i++) {
		std::array<std::vector<wireType>, GateTypeNums> gateNums;
		switch (flag) {
		case UNIQUE:
			for (auto& p : tmpGates) {
				if (p.second.cycle == i) {
					switch (p.second.gateType) {
					case AND:
						gateNums[0].push_back(p.second.suc);
						break;
					case OR:
						gateNums[1].push_back(p.second.suc);
						break;
					case NOT:
						gateNums[2].push_back(p.second.suc);
						break;
					default:
						std::cerr << "UNKNOWN type" << std::endl;
						break;
					}
				}
			}
			break;
		case WEIGHT:
			for (auto& p : tmpGates) {
				if (p.second.cycle <= i && p.second.cycle + p.second.gateType - 1 >= i) {
					switch (p.second.gateType) {
					case AND:
						gateNums[0].push_back(p.second.suc);
						break;
					case OR:
						gateNums[1].push_back(p.second.suc);
						break;
					case NOT:
						gateNums[2].push_back(p.second.suc);
						break;
					default:
						std::cerr << "UNKNOWN type" << std::endl;
						break;
					}
				}
			}
			break;
		}
		cycles.push_back(gateNums);
	}
	for (int i = 0; i < max; i++) {
		std::cout << "Cycle " << i << ':';
		for (int j = 0; j < GateTypeNums; j++) {
			std::cout << "{ ";
			for (wireType p : cycles[i][j]) {
				std::cout << p << ' ';
			}
			std::cout << '}';
			if (j < 2) {
				std::cout << ',';
			}
			else {
				std::cout << std::endl;
			}
		}
	}
}