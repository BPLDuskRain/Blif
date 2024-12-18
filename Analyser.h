#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <vector>

# define FLAG 1
# define NOTFOUND -1

class Analyser {
private:
	struct Name {
		std::string result;
		std::vector<std::string> parameters;
		std::string form;

		int inParameters(std::string);
	};

	std::vector<std::string> tmpLines;
	bool clock;
	std::vector<std::string> tmpParas;
	Name name;
	std::vector<char> formString;
	std::stringstream formStream;

	void paraAnalyse(std::string);
	int inTmpParas(std::string);
	void assignAnalyse(std::string);
	void expAnalyse(std::string);
	void formGenerate(std::string, int flag);
public:

	std::string module;
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;
	std::vector<Name> names;

	void readV(std::string filename);
	void analyse();
	void writeBlif(std::string filename);
};