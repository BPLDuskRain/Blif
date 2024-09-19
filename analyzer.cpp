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

void Analyzer::readBlif(std::string filename) {
	std::ifstream infilestream(filename);

	if (infilestream.is_open()) {
		std::string line;
		while (getline(infilestream, line)) {
			tmpLines.push_back(line);
		}
		infilestream.close();
	}
	return;
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
				it++;
				if (it->find('-') != std::string::npos) {
					gate.gateType = OR;
				}
				else if (it->size() == 3) {
					gate.gateType = NOT;
				}
				else {
					gate.gateType = AND;
				}

				//确定轮次
				cycleConfirm(gate);

				tmpGates.push_back(gate);
			}
		}
	}
}

void Analyzer::cycleConfirm(Gate& gate) {
	if (allInInputs(gate)) {
		gate.cycle = 0;
	}
	else {
		gate.cycle = maxCircle(gate) + 1;
	}
}

bool Analyzer::allInInputs(const Gate& gate) {
	for (auto pre : gate.pres) {
		if (!reInput(pre)) {
			return false;
		}
	}
	return true;
}

int Analyzer::maxCircle(const Gate& gate) {
	int max = 0;
	for (auto pre : gate.pres) {
		for (auto tmpGate : tmpGates) {
			if (pre == tmpGate.suc) {
				if (tmpGate.cycle > max) {
					max = tmpGate.cycle;
				}
			}
		}
	}
	return max;
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
	for (auto p : tmpGates) {
		if (p.cycle + 1 > max) {
			max = p.cycle + 1;
		}
	}
	std::cout << "Total " << max << " Cycles\n";
	std::vector<std::array<std::vector<wireType>, 3>> cycles;
	for (int i = 0; i < max; i++) {
		std::array<std::vector<wireType>, 3> gateNums;
		for (auto p : tmpGates) {
			if (p.cycle == i) {
				gateNums[p.gateType - 1].push_back(p.suc);
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