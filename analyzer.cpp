# include "Analyzer.h"

bool Analyzer::reOutput(const wireType& name) {
	for (std::string p : outputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::reInput(const wireType& name) {
	for (std::string p : inputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::reMiddle(const wireType& name) {
	for (std::string p : middles) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

void Analyzer::clear() {
	tmpLines.clear();
	tmpGates.clear();
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
		if (it->at(0) == '.') {
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
					if (reOutput(name) || reInput(name) || reMiddle(name)) continue;
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
}

void Analyzer::writeV(std::string filename) {
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

void Analyzer::toMidForm() {
	for (auto it = tmpLines.begin(); it != tmpLines.end(); ++it) {
		if (it->at(0) == '.') {
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
}

void Analyzer::cycleConfirm_ASAP() {
	//遍历tmpGates全部确定cycle
	for (auto pairP = tmpGates.begin(); pairP != tmpGates.end(); ++pairP) {
		pairP->second.cycle = getGateCycle_ASAP(pairP->second);
	}
}

int Analyzer::getGateCycle_ASAP(const Gate& gate) {
	int max = -1;

	for (auto pre : gate.pres) {
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
	for (auto pre : gate.pres) {
		//递归确定gate前驱的cycle（input不需要cycle）
		if (!reInput(pre)) {
			setGateCycle_ALAP(tmpGates[pre], cy - 1);
		}
	}
}

void Analyzer::cycleConfirm_Hu(int limit) {
	//先使用ALAP得到基础cycle，再按cycle排序塞进Gate数组huArray
	cycleConfirm_ALAP();
	std::vector<Gate*> huArray = std::vector<Gate*>();
	for (auto gateP = tmpGates.begin(); gateP != tmpGates.end(); ++gateP) {
		huArray.push_back(&(gateP->second));
	}
	std::sort(huArray.begin(), huArray.end(), [](const Gate* a, const Gate* b) -> bool {
		return a->cycle < b->cycle;
	});

	int lmt = limit;
	int cycle = 0;
	std::set<Gate*> set = std::set<Gate*>();//正在执行的节点
	for (auto index = huArray.begin(); index < huArray.end();) {
		//资源足够
		if (lmt > 0) {
			//未被执行
			if (!(*index)->scheduled) {
				if (presAreScheduled(*index)) {
					//前驱全部准备，可执行
					(*index)->cycle = cycle;
					set.insert(*index);
					--lmt;
					++index;
				}
				else {
					//前驱未准备好
					++index;
					//若集合到末尾，重新遍历
					if (index == huArray.end()) {
						index = huArray.begin();
						continue;
					}
				}
			}
			//已被执行直接跳过
			else ++index;
		}
		else {
			//资源不足加轮次
			for (auto setP : set) {
				setP->scheduled = true;
			}
			set.clear();
			++cycle;
			lmt = limit;
		}
	}
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
	for (auto pre : gate->pres) {
		//非inputs
		if (tmpGates.find(pre) != tmpGates.end()) {
			if (!tmpGates[pre].scheduled) return false;
		}
	}
	return true;
}

void Analyzer::cycleConfirm_MLRCS() {
	for (auto pair = tmpGates.begin(); pair != tmpGates.end(); ++pair) {
		//pair->second.cycle = ? ;
	}
}

void Analyzer::cycleConfirm_MRLCS() {
	for (auto pair = tmpGates.begin(); pair != tmpGates.end(); ++pair) {
		//pair->second.cycle = ? ;
	}
}

void Analyzer::writeMidForm() {
	std::cout << "PIs :";
	for (int i = 0; i < inputs.size(); i++) {
		std::cout << inputs[i];
		if (i + 1 < inputs.size()) {
			std::cout << ", ";
		}
		else {
			std::cout << "  ";
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

	int max = 0;
	for (auto pair : tmpGates) {
		if (pair.second.cycle + 1> max) {
			max = pair.second.cycle + 1;
		}
	}
	std::cout << "Total " << max << " Cycles\n";
	std::vector<std::array<std::vector<wireType>, 3>> cycles;
	for (int i = 0; i < max; i++) {
		std::array<std::vector<wireType>, 3> gateNums;
		for (auto p : tmpGates) {
			if (p.second.cycle == i) {
				gateNums[p.second.gateType - 1].push_back(p.second.suc);
			}
		}
		cycles.push_back(gateNums);
	}
	for (int i = 0; i < max; i++) {
		std::cout << "Cycle " << i << ':';
		for (int j = 0; j < 3; j++) {
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