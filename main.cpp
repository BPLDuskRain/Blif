# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "Analyzer.h"
# include "Analyser.h"
# include "MyTree.h"

int main00(int argc, char* argv[]) {
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
	//std::array<int, GateTypeNums> paras = {0};
	//for (int i = 1 + 1; i < argc; ++i) {
	//	paras[i - 1 - 1] = (std::stoi(argv[i]));
	//}
	int para = std::stoi(argv[2]);

	int cycle = 0;
	if (analyzer.readBlif(filename)) {
		//analyzer.toMidForm_elementary();
		analyzer.toMidForm();

		//analyzer.cycleConfirm_ASAP();
		//analyzer.cycleConfirm_ALAP();
		//analyzer.cycleConfirm_Hu(3);
		//cycle = analyzer.cycleConfirm_MLRCS(paras);
		std::array<int, GateTypeNums> res = analyzer.cycleConfirm_MRLCS(para);
		//analyzer.writeMidForm(UNIQUE);
		analyzer.writeMidForm(WEIGHT);
		std::cout << "&: " << res[0] << std::endl;
		std::cout << "|: " << res[1] << std::endl;
		std::cout << "!: " << res[2] << std::endl;
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

int main() {
	Analyser analyser;
	analyser.readV("test1.v");
	analyser.analyse();
	analyser.writeBlif("test4.blif");
	return 0;
}