#ifndef AVL_NODE
#define AVL_NODE
#include <exception>
#include <string>

typedef enum {
    LEAF,
    HAS_LEFT_CHILD,
    HAS_RIGHT_CHILD,
    HAS_CHILDREN
} NodeType;

template <typename Data>
class AVLNode {
    Data* data;
    AVLNode* parentNode;
    AVLNode* rightChild;
    AVLNode* leftChild;

    NodeType type;
    int height;
    int bf;

    void updateType();
public:
    AVLNode(Data* data);
    ~AVLNode();

    Data* getData();
    void setData(Data*);

    void setRChild(AVLNode*);
    void setLChild(AVLNode*);
    void setParent(AVLNode*);

    AVLNode* getLChild();
    AVLNode* getRChild();
    AVLNode* getParent();

    //returns true if the child to be killed is the left one
    const bool removeChild(AVLNode*);

    int getBF();
    int getHeight();
    NodeType getType();
    void calculateStats();

    const bool operator<(AVLNode& rhs); //uses comparator
    const bool operator<(Data* data);
    const bool operator<(int id);
    const bool operator>(AVLNode& rhs); //uses comparator
    const bool operator>(Data* data);
    const bool operator>(int id);
    const bool operator==(AVLNode& rhs); //uses comparator
    const bool operator==(Data* data);
    const bool operator==(int id);

    class AVLTree_Node_Exception : public std::exception
    {
        std::string info;
    public:
        AVLTree_Node_Exception(const std::string& message) { info = message; }
        const std::string& what() { return info; }
    };
};


template<typename Data>
AVLNode<Data>::AVLNode(Data* data)
{
    if (data == NULL) throw AVLTree_Node_Exception("Data cannot be NULL");

    Data* data_clone = new Data();
    if (data_clone == NULL) throw AVLTree_Node_Exception("OUT OF MEMORY");
    *data_clone = *data;
    this->data = data_clone;
    this->parentNode = NULL;
    this->leftChild = NULL;
    this->rightChild = NULL;
    this->bf = 0;
    this->height = 0;
    this->type = LEAF;
}


template<typename Data>
AVLNode<Data>::~AVLNode()
{
	if (data != NULL) {
		delete data;
		data = NULL;
	}
}

template<typename Data>
AVLNode<Data>* AVLNode<Data>::getLChild()
{
	return this->leftChild;
}

template<typename Data>
AVLNode<Data>* AVLNode<Data>::getRChild()
{
	return this->rightChild;
}

template<typename Data>
AVLNode<Data>* AVLNode<Data>::getParent()
{
	return this->parentNode;
}

template<typename Data>
const bool AVLNode<Data>::removeChild(AVLNode* node)
{
	if (this->leftChild == node) {
		this->leftChild = NULL;
		this->calculateStats();
		return true;
	}
	else if (this->rightChild == node) {
		this->rightChild = NULL;
		this->calculateStats();
	}

	return false;
}

template<typename Data>
const bool AVLNode<Data>::operator<(AVLNode& rhs)
{
	return *(this->data) < *(rhs->getData());
}

template<typename Data>
const bool AVLNode<Data>::operator<(Data* data)
{
	if (data == NULL) throw AVLTree_Node_Exception("Data cannot be NULL!");

	return *(this->data) < *data;
}

template<typename Data>
const bool AVLNode<Data>::operator<(int id)
{
	return *(this->data) < id;
}

template<typename Data>
const bool AVLNode<Data>::operator>(AVLNode& rhs)
{
	return *(this->data) > *(rhs->getData());
}

template<typename Data>
const bool AVLNode<Data>::operator>(Data* data)
{
	if (data == NULL) throw AVLTree_Node_Exception("Data cannot be NULL!");

	return *(this->data) > *data;
}

template<typename Data>
const bool AVLNode<Data>::operator>(int id)
{
	return *(this->data) > id;
}

template<typename Data>
const bool AVLNode<Data>::operator==(AVLNode& rhs)
{
	return *data == *(rhs->getData());
}

template<typename Data>
const bool AVLNode<Data>::operator==(Data* data)
{
	if (data == NULL) throw AVLTree_Node_Exception("Data cannot be NULL!");

	return *(this->data) == *data;
}

template<typename Data>
const bool AVLNode<Data>::operator==(int id)
{
	return *(this->data) == id;
}

template<class Data>
void AVLNode<Data>::calculateStats()
{
	this->updateType();

	switch (this->type) {
	case LEAF:
		this->bf = 0;
		this->height = 1;
		break;
	case HAS_CHILDREN:
		this->height = this->leftChild->height >= this->rightChild->height ?
			this->leftChild->height + 1 : this->rightChild->height + 1;
		this->bf = this->leftChild->height - this->rightChild->height;
		break;
	case HAS_LEFT_CHILD:
		this->height = this->leftChild->height + 1;
		this->bf = this->leftChild->height;
		break;
	case HAS_RIGHT_CHILD:
		this->height = this->rightChild->height + 1;
		this->bf = -1 * this->rightChild->height;
		break;
	}
}

template<typename Data>
int AVLNode<Data>::getBF()
{
	calculateStats();
	return this->bf;
}

template<typename Data>
int AVLNode<Data>::getHeight()
{
	calculateStats();
	return this->height;
}

template<typename Data>
NodeType AVLNode<Data>::getType()
{
	return this->type;
}

template<typename Data>
void AVLNode<Data>::updateType()
{
	auto* lchild = this->leftChild;
	auto* rchild = this->rightChild;

	if (lchild == NULL && rchild == NULL) {
		type = LEAF;
		return;
	}
	else if (lchild != NULL && rchild != NULL) {
		type = HAS_CHILDREN;
		return;
	}
	else if (lchild != NULL) {
		type = HAS_LEFT_CHILD;
		return;
	}
	else type = HAS_RIGHT_CHILD;
}

template<typename Data>
Data* AVLNode<Data>::getData()
{
	return this->data;
}

template<typename Data>
void AVLNode<Data>::setData(Data* d)
{
	if (d == NULL) throw AVLTree_Node_Exception("Data cannot be null.");
	this->data = d;
}


template<typename Data>
void AVLNode<Data>::setRChild(AVLNode<Data>* node)
{
	if (this->parentNode == node && node != NULL)
		this->setParent(node->getParent());

	this->rightChild = node;
	if (node != NULL)
		node->setParent(this);

	calculateStats();

}

template<typename Data>
void AVLNode<Data>::setLChild(AVLNode<Data>* node)
{
	if (this->parentNode == node && node != NULL)
		this->setParent(node->getParent());

	this->leftChild = node;
	if (node != NULL)
		node->setParent(this);

	calculateStats();

}

template<typename Data>
void AVLNode<Data>::setParent(AVLNode* node)
{
	this->parentNode = node;
}


#endif //AVL_NODE