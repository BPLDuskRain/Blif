# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "Analyzer.h"
# include "Analyser.h"

int main() {
	Analyzer analyzer;
	std::string filename;
	for (int i = 1; i <= 3; i++) {
		filename = "test" + std::to_string(i) + ".blif";

		if (analyzer.readBlif(filename)) {
			//analyzer.analyze();
            //analyzer.writeV("test1.v");
			analyzer.toMidForm();
			//analyzer.cycleConfirm_ASAP();
			//analyzer.cycleConfirm_ALAP();
			analyzer.cycleConfirm_Hu(3);
			//analyzer.cycleConfirm_MLRCS();
			//analyzer.cycleConfirm_MRLCS();
			analyzer.writeMidForm();
		}

	}


	//Analyser analyser;
	//analyser.readV("test2.v");
	//analyser.analyse();
	//analyser.writeBlif("test2.blif");
}