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
				//symbols
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

					for (int namePos = 0, linePos = 0; namePos < names.size() || linePos < it->length(); namePos++, linePos++) {
						if (it->at(linePos) == ' ') {
							linePos++;
							break;
						}

						if (it->at(linePos) != '-') {
							if (it->at(linePos) == '0') {
								tmpExp.push_back("!");
							}
							tmpExp.push_back(names[namePos]);
							tmpExp.push_back(" & ");
						}
						/*std::cout << names[namePos] << " " << it->at(linePos) << std::endl;*/
					}
					tmpExp.pop_back();
					tmpExp.push_back(" | ");
				}
				tmpExp.pop_back();

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