# include <string>
# include <unordered_map>

# include"MyTree.h"

MyTree::Node::Node() {}
MyTree::Node::Node(const std::string& data) {
	this->data = data;
}

bool MyTree::isData(std::string data) const {
	return (values.find(data) == values.end());
}

void MyTree::add(const std::string& data) {
	add(root_pointer, data);
}
void MyTree::add(Node*& root, const std::string& data) {
	//无根，创树
	if (root == nullptr) {
		Node* node = new Node(data);
		root = node;
		return;
	}
	//根为单目操作符!
	if (root->data == "!") {
		//值为操作符&|，换根
		if (!isData(data)) {
			Node* node = new Node(data);
			node->lchild = root;
			root = node;
			return;
		}
		//值为数据
		add(root->lchild, data);
		return;
	}
	//根为双目操作符&|
	if (!isData(root->data)) {
		//值仍为&|操作符
		if (!isData(data)) {
			//操作符优先级小，换根
			if (values[root->data] < values[data]) {
				Node* node = new Node(data);
				node->lchild = root;
				root = node;
				return;
			}
			//否则往右子树丢
			else {
				add(root->rchild, data);
				return;
			}
		}
		else {
			//值是数据，右节点加数据
			add(root->rchild, data);
			return;
		}
	}
	//根为变量
	//换根
	Node* node = new Node(data);
	node->lchild = root;
	root = node;
	return;
}

std::vector<std::string>* MyTree::post_order_toString() {
	return post_order_toString(root_pointer);
}
std::vector<std::string>* MyTree::post_order_toString(const Node* root) const {
	std::vector<std::string>* str = new std::vector<std::string>();
	std::vector<std::string>* lstr = nullptr;
	std::vector<std::string>* rstr = nullptr;

	if (root->lchild != nullptr) lstr = post_order_toString(root->lchild);
	if (root->rchild != nullptr) rstr = post_order_toString(root->rchild);

	if (lstr != nullptr) str->insert(str->end(), lstr->begin(), lstr->end());
	if (rstr != nullptr) str->insert(str->end(), rstr->begin(), rstr->end());

	str->push_back(root->data);
	return str;
}