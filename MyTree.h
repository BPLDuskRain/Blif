#pragma once
# include <string>
# include <vector>
# include <unordered_map>

class MyTree {
private:
	std::unordered_map<std::string, int> values = { {" & ", 1}, {" | ", 2} };
	struct Node {
		std::string data;
		Node* lchild = nullptr;
		Node* rchild = nullptr;
		Node();
		Node(const std::string& data);
	};

	bool isData(std::string data) const;
public:
	Node* root_pointer = nullptr;

	void add(const std::string& data);
	void add(Node*& root, const std::string& data);

	std::vector<std::string>* post_order_toString();
	std::vector<std::string>* post_order_toString(const Node* root) const;
};