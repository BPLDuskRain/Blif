# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "analyzer.h"

int main() {
	Analyzer analyzer;
	analyzer.readBlif("test1.blif");
	analyzer.analyze();
	analyzer.writeV("test1.v");
}