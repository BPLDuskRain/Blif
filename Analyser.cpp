# include "Analyser.h"


int Analyser::Name::inParameters(std::string para) {
	for (int i = 0; i < parameters.size(); i++) {
		if (parameters[i] == para) {
			return i;
		}
	}
	return NOTFOUND;
}

void Analyser::readBlif(std::string filename) {
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

void Analyser::analyse() {
	clock = false;
	for (std::string i : tmpLines) {
		if (i == "endmodule") break;
		if (i == "") continue;
		size_t divider = i.find(' ');
		if (divider == std::string::npos) {
			std::cerr << "Wrong format\n";
		}
		std::string title = i.substr(0, divider);

		if (title == "module") {
			size_t left = i.find('(');
			module = i.substr(divider + 1, left - divider - 1);
			std::string paras(i.substr(left + 1));
			paraAnalyse(paras);
		}
		else if (title == "input") {
			if (clock == true) {
				size_t right = i.find(';');
				std::string input = i.substr(divider + 1, right - divider - 1);
				if (inTmpParas(input) != NOTFOUND) {
					inputs.push_back(input);
				}
			}
		}
		else if (title == "output") {
			clock = true;
			size_t right = i.find(';');
			std::string output = i.substr(divider + 1, right - divider - 1);
			if (inTmpParas(output) != NOTFOUND) {
				outputs.push_back(output);
			}
		}
		else if (title == "wire") {
			size_t right = i.find(';');
			std::string wire = i.substr(divider + 1, right - divider - 1);
			if (inTmpParas(wire) == NOTFOUND) {
				tmpParas.push_back(wire);
			}
		}
		else if (title == "assign") {
			size_t right = i.find(';');
			std::string assign = i.substr(divider + 1, right - divider);
			assignAnalyse(assign);
		}
	}
}

void Analyser::writeV(std::string filename) {
	std::ofstream outfilestream(filename);

	outfilestream << ".model " << module << std::endl;

	outfilestream << ".inputs ";
	size_t inputsSize = inputs.size();
	for (std::string p : inputs) {
		outfilestream << p;
		inputsSize--;
		if (inputsSize != 0) {
			outfilestream << " ";
		}
		else {
			outfilestream << std::endl;
		}
	}

	outfilestream << ".outputs ";
	size_t outputsSize = outputs.size();
	for (std::string p : outputs) {
		outfilestream << p;
		outputsSize--;
		if (outputsSize != 0) {
			outfilestream << " ";
		}
		else {
			outfilestream << std::endl;
		}
	}

	for (auto p : names) {
		outfilestream << ".names ";

		for (auto q : p.parameters) {
			outfilestream << q << " ";
		}
		outfilestream << p.result << std::endl << p.form;
	}

	outfilestream << ".end";
	outfilestream.close();
}

void Analyser::paraAnalyse(std::string paras) {
	size_t right = paras.find(", ");
	if (right != std::string::npos) {
		tmpParas.push_back(paras.substr(0, right));
		paraAnalyse(paras.substr(right + 2));
		return;
	}
	else {
		right = paras.find(')');
		tmpParas.push_back(paras.substr(0, right));
		return;
	}
}

int Analyser::inTmpParas(std::string para) {
	for (int i = 0; i < tmpParas.size(); i++) {
		if (tmpParas[i] == para) {
			return i;
		}
	}
	return NOTFOUND;
}

void Analyser::assignAnalyse(std::string assign) {
	size_t eq = assign.find('=');
	name.result = assign.substr(0, eq - 1);

	std::string exp = assign.substr(eq + 2);
	if (exp == "0;") {
		name.form = "0\n";
	}
	else if (exp == "1;") {
		name.form = "1\n";
	}
	else {
		expAnalyse(exp);//第一次解析exp，递归拆出变量名
		formString.assign(name.parameters.size(), '-');//用于和变量名一一相对的数组

		formGenarate(exp, 1);//1或0表示范式类型，第二次解析exp，递归给出表达式
		name.form = formStream.str();
		formStream.str("");//清空范式，下次调用备用
	}

	names.push_back(name);
	name.parameters.clear();//清空变量，下次调用备用
}

void Analyser::expAnalyse(std::string exp) {
	size_t orGate;
	size_t andGate;
	size_t notGate;
	size_t semi;

	orGate = exp.find('|');
	if (orGate != std::string::npos) {
		std::string subExp1 = exp.substr(0, orGate - 1);
		std::string subExp2 = exp.substr(orGate + 2);
		expAnalyse(subExp1);
		expAnalyse(subExp2);
	}
	//无或门
	else {
		andGate = exp.find('&');
		if (andGate != std::string::npos) {
			std::string subExp1 = exp.substr(0, andGate - 1);
			std::string subExp2 = exp.substr(andGate + 2);
			expAnalyse(subExp1);
			expAnalyse(subExp2);
		}
		//无与门
		else {
			notGate = exp.find('!');
			if (notGate != std::string::npos) {
				std::string subExp = exp.substr(notGate + 1);
				expAnalyse(subExp);
			}
			else {
				semi = exp.find(';');
				if (semi != std::string::npos) {
					exp = exp.substr(0, semi);
					if (name.inParameters(exp) == NOTFOUND) {
						name.parameters.push_back(exp);
					}
				}
				else {
					if (name.inParameters(exp) == NOTFOUND) {
						name.parameters.push_back(exp);
					}
				}
			}
		}
	}
}

void Analyser::formGenarate(std::string exp, int flag) {
	size_t orGate;
	size_t andGate;
	size_t notGate;
	size_t semi;

	orGate = exp.find('|');
	if (orGate != std::string::npos) {
		std::string subExp1 = exp.substr(0, orGate - 1);
		std::string subExp2 = exp.substr(orGate + 2);
		formGenarate(subExp1, flag);//解析|前半部分，并输出到流中
		std::string str(formString.begin(), formString.end());
		formString.assign(name.parameters.size(), '-');
		formStream << str << " " << flag << std::endl;
		formGenarate(subExp2, flag);
	}
	//无或门
	else {
		andGate = exp.find('&');
		if (andGate != std::string::npos) {
			std::string subExp1 = exp.substr(0, andGate - 1);
			std::string subExp2 = exp.substr(andGate + 2);
			formGenarate(subExp1, flag);
			formGenarate(subExp2, flag);
		}
		//无与门
		else {
			notGate = exp.find('!');
			if (notGate != std::string::npos) {
				std::string subExp = exp.substr(notGate + 1);
				//非门flag反向
				switch (flag)
				{
				case 0:
					flag = 1;
					break;
				case 1:
					flag = 0;
					break;
				default:
					break;
				}
				formGenarate(subExp, flag);
			}
			else {
				//最后一个部分处理
				semi = exp.find(';');
				if (semi != std::string::npos) {
					exp = exp.substr(0, semi);
					int pos = inTmpParas(exp);
					if (pos != NOTFOUND) {
						formString[pos - 3] = flag + '0';
						std::string str(formString.begin(), formString.end());
						formString.assign(name.parameters.size(), '-');
						formStream << str << " " << flag << std::endl;
					}
				}
				else {
				//前面部分
					int pos = inTmpParas(exp);
					if (pos != NOTFOUND) {
						formString[pos-3] = flag + '0';
					}
				}
			}
		}
	}
}