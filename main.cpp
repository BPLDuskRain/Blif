# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "Analyzer.h"
# include "Analyser.h"

int main() {
	Analyzer analyzer;
	analyzer.readBlif("test1.blif");
	//analyzer.analyze();
	//analyzer.writeV("test1.v");
	analyzer.toMidForm();
	analyzer.writeMidForm();

	//Analyser analyser;
	//analyser.readV("test2.v");
	//analyser.analyse();
	//analyser.writeBlif("test2.blif");
}