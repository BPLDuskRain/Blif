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
	//�޸�������
	if (root == nullptr) {
		Node* node = new Node(data);
		root = node;
		return;
	}
	//��Ϊ��Ŀ������!
	if (root->data == "!") {
		//ֵΪ������&|������
		if (!isData(data)) {
			Node* node = new Node(data);
			node->lchild = root;
			root = node;
			return;
		}
		//ֵΪ����
		add(root->lchild, data);
		return;
	}
	//��Ϊ˫Ŀ������&|
	if (!isData(root->data)) {
		//ֵ��Ϊ&|������
		if (!isData(data)) {
			//���������ȼ�С������
			if (values[root->data] < values[data]) {
				Node* node = new Node(data);
				node->lchild = root;
				root = node;
				return;
			}
			//��������������
			else {
				add(root->rchild, data);
				return;
			}
		}
		else {
			//ֵ�����ݣ��ҽڵ������
			add(root->rchild, data);
			return;
		}
	}
	//��Ϊ����
	//����
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