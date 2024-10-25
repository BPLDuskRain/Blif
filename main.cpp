# include <iostream>
# include <fstream>
# include <sstream>
# include <string>

# include "Analyzer.h"
# include "Analyser.h"
# include "MyTree.h"

int main() {
	Analyzer analyzer;
	//analyzer.readBlif("test0.blif");
	std::string filename;
	for (int i = 0; i < 3; i++) {
		filename = "test" + std::to_string(i) + ".blif";

		if (analyzer.readBlif(filename)) {
			//analyzer.toMidForm_elementary();
			analyzer.toMidForm();

			analyzer.cycleConfirm_ASAP();
			//analyzer.cycleConfirm_ALAP();
			//analyzer.cycleConfirm_Hu(3);
			//int cycle = analyzer.cycleConfirm_MLRCS({2, 1, 1});
			//std::array<int, 3> res = analyzer.cycleConfirm_MRLCS(11);
			analyzer.writeMidForm(UNIQUE);
			//analyzer.writeMidForm(WEIGHT);
		}
	}
	//analyzer.analyze();
	//analyzer.writeV("test0.v");

	//Analyser analyser;
	//analyser.readV("test2.v");
	//analyser.analyse();
	//analyser.writeBlif("test2.blif");
	return 0;
}

int main01() {
	MyTree myTree;
	myTree.add("v3");
	myTree.add(" & ");
	myTree.add("!");
	myTree.add("j");
	myTree.add(" | ");
	myTree.add("u78");
	myTree.add(" & ");
	myTree.add("j");
	myTree.add(" | ");
	myTree.add("!");
	myTree.add("v3");
	myTree.add(" & ");
	myTree.add("v6");
	myTree.add(" & ");
	myTree.add("j");
	std::vector<std::string>* post = myTree.post_order_toString();
	return 0;
}