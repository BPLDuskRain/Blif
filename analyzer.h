#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>

class Analyzer {
private:
	std::vector<std::string> tmpLines;

	bool reOutput(std::string name);
	bool reInput(std::string name);
	bool reMiddle(std::string name);
public:
	std::string model;
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;
	std::string clock;
	std::vector<std::string> middles;
	std::vector<std::string> expressions;

	void readBlif(std::string filename);
	void analyze();
	void writeV(std::string filename);
};