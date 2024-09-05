# include "analyzer.h"

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
				//symbols
				size_t andNum;
				size_t orNum;
				std::vector<int> notPos;
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
				orNum = -1;
				while (true) {
					it++;
					if (it->at(0) == '.') {
						it--;
						break;
					}

					size_t pos = it->find('0');
					if (pos != std::string::npos) {
						notPos.push_back(pos);
					}
					orNum++;
				}
				andNum = names.size() - 1 - orNum;

				//��ϱ��ʽ
				std::ostringstream oss;
				oss << result << " = ";

				for (int i = 0; i < names.size(); i++) {
					if (i < notPos.size()) {
						if (notPos[i] == i) {
							oss << '!';
						}
					}

					oss << names[i];

					if (andNum != 0) {
						oss << " & ";
						andNum--;
					}
					else if (orNum != 0) {
						oss << " | ";
						orNum--;
					}
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
	int size = inputs.size();
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

bool Analyzer::reOutput(std::string name) {
	for (std::string p : outputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::reInput(std::string name) {
	for (std::string p : inputs) {
		if (p == name) {
			return true;
		}
	}
	return false;
}

bool Analyzer::reMiddle(std::string name) {
	for (std::string p : middles) {
		if (p == name) {
			return true;
		}
	}
	return false;
}