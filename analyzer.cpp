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
			//��model
			if (substring == "model") {
				model = it->substr(divider + 1);
			}
			//��inputs
			else if (substring == "inputs") {
				std::istringstream tmpStream(it->substr(divider + 1));
				std::string input;
				while (std::getline(tmpStream, input, ' ')) {
					inputs.push_back(input);
				}
			}
			//��outputs
			else if (substring == "outputs") {
				std::istringstream tmpStream(it->substr(divider + 1));
				std::string output;
				while (std::getline(tmpStream, output, ' ')) {
					outputs.push_back(output);
				}
			}
			//����names
			else if (substring == "names") {
				std::string result;
				//names
				std::vector<std::string> names;
				//read temp strings
				std::istringstream tmpStream(it->substr(divider + 1));
				std::string name;
				while (std::getline(tmpStream, name, ' ')) {
					//��¼��������
					names.push_back(name);
					//�����м����
					if (reOutput(name) || reInput(name) || reMiddle(name)) continue;
					middles.push_back(name);
				}
				result = name;
				names.pop_back();

				//������ֵ��
				std::vector<std::string> tmpExp;
				while (true) {
					it++;
					if (it->at(0) == '.') {
						it--;
						break;
					}

					for (int namePos = 0, linePos = 0; namePos < names.size() || linePos < it->size(); namePos++, linePos++) {
						//ȷ������ȡ���Ǻ�ȡ
						size_t space = it->find(" ");
						if (space != std::string::npos) {
							flag = it->at(space + 1);
						}
						//����ֱ�Ӹ�ֵ���
						else if (it->size() == 1) {
							tmpExp.push_back(*it);
							goto Assign;
						}

						//�����ո�ֹͣ
						if (it->at(linePos) == ' ') {
							linePos++;
							break;
						}
						//����flag�ж�����
						if (it->at(linePos) != '-') {
							if ((it->at(linePos) == '0' && flag == '1') || (it->at(linePos) == '1' && flag == '0')) {
								
							}
							//ͬ�о�������
							tmpExp.push_back(names[namePos]);
							tmpExp.push_back(" & ");
						}
					}
					//�м��û���
					tmpExp.pop_back();
					tmpExp.push_back(" | ");
				}
				tmpExp.pop_back();
			Assign:

				//��ϱ��ʽ
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

	//module��
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

	//��һinput
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
			//��inputs
			if (substring == "inputs") {
				std::istringstream tmpStream(it->substr(divider + 1));
				std::string input;
				while (std::getline(tmpStream, input, ' ')) {
					inputs.push_back(input);

				}
			}
			//��outputs
			else if (substring == "outputs") {
				std::istringstream tmpStream(it->substr(divider + 1));
				std::string output;
				while (std::getline(tmpStream, output, ' ')) {
					outputs.push_back(output);
				}
			}
			//����names
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

				//������ֵ��
				//ֻ�ܴ���һ�ţ�����
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
	//����tmpGatesȫ��ȷ��cycle
	for (auto pairP = tmpGates.begin(); pairP != tmpGates.end(); ++pairP) {
		pairP->second.cycle = getGateCycle_ASAP(pairP->second);
	}
}

int Analyzer::getGateCycle_ASAP(const Gate& gate) {
	int max = -1;

	for (auto pre : gate.pres) {
		//���ڵ�ǰ��Ϊinputs����tmpGates�У���ֱ������������0
		if (tmpGates.find(pre) != tmpGates.end()) {
			//�ݹ�ȷ��ǰ��cycle��ȡ���ֵ��+1��Ϊ�Լ���cycle
			int tmp = getGateCycle_ASAP(tmpGates[pre]);
			if (tmp > max) {
				max = tmp;
			}
		}
	}
	return max + 1;
}

void Analyzer::cycleConfirm_ALAP() {
	//��ȫ����ֵΪ���ֵ
	int maxCycle = getGatesCycle() - 1;
	for (auto gateP = tmpGates.begin(); gateP != tmpGates.end(); ++gateP) {
		gateP->second.cycle = maxCycle;
	}

	//ͨ���ݹ麯��setGateCycle��Դ��ֵ�����ֻ��Ҫ�������
	for (auto outputP = outputs.begin(); outputP < outputs.end(); ++outputP) {
		setGateCycle_ALAP(tmpGates[*outputP], maxCycle);
	}
}

int Analyzer::getGatesCycle() {
	//ȷ��ȫ�������cycle
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
	//����cy���Ʊ�gate��cycle��������ͬǰ���ڵ㣬���ڶ�α������Զ��ҵ���Сֵ
	if (cy < gate.cycle) {
		gate.cycle = cy;
	}
	for (auto pre : gate.pres) {
		//�ݹ�ȷ��gateǰ����cycle��input����Ҫcycle��
		if (!reInput(pre)) {
			setGateCycle_ALAP(tmpGates[pre], cy - 1);
		}
	}
}

void Analyzer::cycleConfirm_Hu(int limit) {
	//��ʹ��ALAP�õ�����cycle���ٰ�cycle��������Gate����huArray
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
	std::set<Gate*> set = std::set<Gate*>();//����ִ�еĽڵ�
	for (auto index = huArray.begin(); index < huArray.end();) {
		//��Դ�㹻
		if (lmt > 0) {
			//δ��ִ��
			if (!(*index)->scheduled) {
				if (presAreScheduled(*index)) {
					//ǰ��ȫ��׼������ִ��
					(*index)->cycle = cycle;
					set.insert(*index);
					--lmt;
					++index;
				}
				else {
					//ǰ��δ׼����
					++index;
					//�����ϵ�ĩβ�����±���
					if (index == huArray.end()) {
						index = huArray.begin();
						continue;
					}
				}
			}
			//�ѱ�ִ��ֱ������
			else ++index;
		}
		else {
			//��Դ������ִ�
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
		//��inputs
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