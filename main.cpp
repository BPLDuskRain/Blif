# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "Analyzer.h"
# include "Analyser.h"
# include "MyTree.h"

int main(int argc, char* argv[]) {
	Analyzer analyzer;
	std::string filename;
	if (argc == 1) {
		std::cerr << "No File Name" << std::endl;
		return -1;
	}
	filename.assign(argv[1]);
	if (argc == 2 || argc > 1+1+GateTypeNums) {
		std::cerr << "No enough ParaMeters" << std::endl;
		return -1;
	}
	std::array<int, GateTypeNums> paras = {0};
	for (int i = 1 + 1; i < argc; ++i) {
		paras[i - 1 - 1] = (std::stoi(argv[i]));
	}

	if (analyzer.readBlif(filename)) {
		//analyzer.toMidForm_elementary();
		analyzer.toMidForm();

		//analyzer.cycleConfirm_ASAP();
		//analyzer.cycleConfirm_ALAP();
		//analyzer.cycleConfirm_Hu(3);
		int cycle = analyzer.cycleConfirm_MLRCS(paras);
		//std::array<int, 3> res = analyzer.cycleConfirm_MRLCS(11);
		//analyzer.writeMidForm(UNIQUE);
		analyzer.writeMidForm(WEIGHT);
	}
	return 0;
}

int main01() {
	Analyzer analyzer;
	analyzer.readBlif("test1.blif");
	analyzer.analyze();
	analyzer.writeV("test1.v");
	return 0;
}

int main02() {
	Analyser analyser;
	analyser.readV("test1.v");
	analyser.analyse();
	analyser.writeBlif("test1.blif");
	return 0;
}