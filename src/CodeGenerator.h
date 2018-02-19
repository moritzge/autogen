#include <gtest/gtest.h>
#include <iostream>

#include <memory>
#include <map>
#include <string>

#include "AutoDiff.h"

#include <stack>

namespace CodeGen {

template <class T> using Sp = std::shared_ptr<T>;

template<class S>
class CodeGenerator;

template<class S>
class Node
{
public:
	Node() {}

	virtual size_t getNumChildren() const = 0;

	virtual Sp<const Node<S>> getChild(size_t i) const = 0;

	// Return the evaluated value of this node
	virtual S evaluate() const = 0;

	virtual bool evaluate(S &value) const = 0;

	virtual std::string generateCode(const CodeGenerator<S> &generator) const = 0;

	virtual uint64_t getHash() const {
		if(mIsHashValid)
			return mCachedHash;

		mCachedHash = computeHash();\
		mIsHashValid = true;
	}

	virtual uint64_t getHashId() const { return 0; }

//	void collectNodes(CodeGenerator<S> &generator) const {

//		// does the generator already have a same-hashed node?
//		const Node* hashedNode = generator.getHashedNode(this);
//		if(hashedNode)
//			return;

//		// let's add this node
//		S eval;
//		if(false/*evaluate(eval)*/) {
//			// add a const node or something ...
//		}
//		else {
//			generator.addNode(this);
//			for (int i = 0; i < getNumChildren(); ++i) {
//				Sp<const Node<S>> child = getChild(i);
//				child->collectNodes(generator);
//			}
//		}
//	}

	static uint64_t rol(uint64_t x, int d) {
		return (x << d) | (x >> (64-d));
	}

private:
	virtual uint64_t computeHash() const = 0;

private:
	mutable bool mIsHashValid = false;
	mutable uint64_t mCachedHash;
};


class VarDef
{
public:
	VarDef(){}

	VarDef(int varIndex)
		: mVarIndex(varIndex)
	{}

	std::string getVarName() const {
		return std::string("v") + std::to_string(mVarIndex);
	}

	void setIndex(int index) {mVarIndex = index; }

private:
	int mVarIndex;
};

template<class S>
class NodeConst;

template<class S>
class CodeGenerator
{
public:
	CodeGenerator() {}

	void addNode(const Node<S>* node) {

		// check if we have the same hashed node already
		size_t hash = node->getHash();
		assert(mHashedNodes.find(hash) == mHashedNodes.end());

		mNodes.push_back(hash);
		mHashedNodes[hash] = std::make_pair(node, VarDef());
	}

	const VarDef& getVar(const Node<S>* node) const {
		assert(node);

		size_t h = node->getHash();
		auto it = mHashedNodes.find(h);
		assert(it != mHashedNodes.end());
		return it->second.second;
	}

	const Node<S>* getHashedNode(const Node<S>* node) const {
		auto it = mHashedNodes.find(node->getHash());
		if(it != mHashedNodes.end())
			return it->second.first;
		else
			return nullptr;
	}

	void collectNodes(const Node<S>* rootNode) {

		// go through graph and collect nodes
		const Node<S>* nodeVisiting = rootNode;
		std::vector<const Node<S>*> nodesToVisit;
		while (true) {
			// does the generator already have a same-hashed node?
			const Node<S>* hashedNode = getHashedNode(nodeVisiting);
			if(!hashedNode) {
				// let's add this node
				addNode(nodeVisiting);
				for (int i = 0; i < nodeVisiting->getNumChildren(); ++i) {
					nodesToVisit.push_back(nodeVisiting->getChild(i).get());
				}
			}

			// let's go to next node
			if(nodesToVisit.size() == 0) break;
//			nodeVisiting = nodesToVisit.top();
//			nodesToVisit.pop();
			nodeVisiting = nodesToVisit.back();
			nodesToVisit.pop_back();
		}

	}

	std::string generateCode() {

		// update variable index
		int counter = 0;
		for (int i = mNodes.size()-1; i >= 0; --i) {
			mHashedNodes[mNodes[i]].second.setIndex(counter++);
		}

		// write code
		std::string code;
		for (int i = mNodes.size()-1; i >= 0; --i) {
			code += mVarTypeName + " " + mHashedNodes[mNodes[i]].second.getVarName() + " = " + mHashedNodes[mNodes[i]].first->generateCode(*this) + ";\n";
		}

		return code;
	}

private:
	std::string mVarTypeName = "double";
	std::vector<size_t> mNodes;
	std::map<size_t, std::pair<const Node<S>*, VarDef>> mHashedNodes;
};

template<class S>
class NodeConst : public Node<S>
{
public:
	NodeConst (S value)
		: mValue(value){}

	virtual size_t getNumChildren() const {
		return 0;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		throw std::logic_error("NodeConst does not have any children");
	}


	virtual S evaluate() const {
		return mValue;
	}

	virtual bool evaluate(S &value) const {
		value = mValue;
		return true;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return std::to_string(mValue);
	}

	virtual uint64_t computeHash() const {
		std::hash<S> hashS;
		return hashS(mValue);
	}

private:
	S mValue;

};

template<class S>
class NodeVar : public Node<S>
{
public:
	NodeVar (const std::string &varName)
		: mVarName(varName){}

	virtual size_t getNumChildren() const {
		return 0;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		throw std::logic_error("NodeVar does not have any children");
	}


	virtual S evaluate() const {
		throw std::logic_error("cannot evaluate a variable name");
		return 0;
	}

	virtual bool evaluate(S &value) const {
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return mVarName;
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mVarName);
	}

private:
	std::string mVarName;

};

template<class S>
class NodeNeg : public Node<S>
{
public:
	NodeNeg (Sp<Node<S>> node)
		: mNode(node) {}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		return mNode;
	}


	virtual S evaluate() const {
		return -mNode->evaluate();
	}

	virtual bool evaluate(S &value) const {
		if(mNode->evaluate(value))
		{
			value = -value;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return "-" + generator.getVar(mNode.get()).getVarName();
	}

	virtual uint64_t computeHash() const {
		return this->rol(mNode->computeHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 1; }

private:
	Sp<Node<S>> mNode;

};

template<class S>
class NodeBinaryOperation : public Node<S>
{
public:
	NodeBinaryOperation (Sp<Node<S>> nodeA, Sp<Node<S>> nodeB)
		: mNodeA(nodeA), mNodeB(nodeB){}

	virtual size_t getNumChildren() const {
		return 2;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		if(i == 0) return mNodeA;
		if(i == 1) return mNodeB;

		throw std::logic_error("NodeBinaryOperation has only two children");
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const = 0;

protected:
	Sp<Node<S>> mNodeA;
	Sp<Node<S>> mNodeB;
};

template<class S>
class NodeAdd : public NodeBinaryOperation<S>
{
public:

	NodeAdd(Sp<Node<S>> nodeA, Sp<Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() + this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		double valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA + valB;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVar(this->mNodeA.get()).getVarName() + " + " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 2; }
};

template<class S>
class NodeSub : public NodeBinaryOperation<S>
{
public:

	NodeSub(Sp<Node<S>> nodeA, Sp<Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() - this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		double valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA - valB;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVar(this->mNodeA.get()).getVarName() + " - " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	// order of subtraction matters, thus different rolling shift (3, 5)
	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
	}

	virtual uint64_t getHashId() const { return 3; }
};

template<class S>
class NodeMul : public NodeBinaryOperation<S>
{
public:
	NodeMul(Sp<Node<S>> nodeA, Sp<Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() * this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		double valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA * valB;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVar(this->mNodeA.get()).getVarName() + " * " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 4; }
};

template<class S>
class NodeDiv : public NodeBinaryOperation<S>
{
public:
	NodeDiv(Sp<Node<S>> nodeA, Sp<Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() / this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		double valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA / valB;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVar(this->mNodeA.get()).getVarName() + " / " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
	}

	virtual uint64_t getHashId() const { return 5; }
};

template<class S>
class RecType
{
public:
//	RecType() {}

	RecType(const S &value) {
		mNode = Sp<Node<S>>(new NodeConst<S>(value));
	}

	RecType(Sp<Node<S>> node)
		: mNode(node) {

	}

	RecType(const std::string &varName) {
		mNode = Sp<Node<S> >(new NodeVar<S>(varName));
	}

	RecType<S> operator-() const {
		return RecType(Sp<Node<S>>(new NodeNeg<S>(mNode)));
	}

	RecType<S> operator+(const RecType<S> &other) const {
		return RecType<S>(Sp<Node<S>>(new NodeAdd<S>(mNode, other.mNode)));
	}

	RecType<S> &operator+=(const RecType<S> &other) {
		*this = *this + other;
		return *this;
	}

	RecType<S> operator-(const RecType<S> &other) const {
		return RecType<S>(Sp<Node<S>>(new NodeSub<S>(mNode, other.mNode)));
	}

	RecType<S> &operator-=(const RecType<S> &other) {
		*this = *this - other;
		return *this;
	}

	RecType<S> operator*(const RecType<S> &other) const {
		return RecType<S>(Sp<Node<S>>(new NodeMul<S>(mNode, other.mNode)));
	}

	RecType<S> &operator*=(const RecType<S> &other) {
		*this = *this * other;
		return *this;
	}

	RecType<S> operator/(const RecType<S> &other) const {
		return RecType<S>(Sp<Node<S>>(new NodeDiv<S>(mNode, other.mNode)));
	}

	RecType<S> &operator/=(const RecType<S> &other) {
		*this = *this / other;
		return *this;
	}

	Sp<Node<S>> getNode() const {
		assert(mNode != nullptr);
		return mNode;
	}

	void printCode() const {
		CodeGenerator<S> generator;
//		mNode->collectNodes(generator);

		generator.collectNodes(mNode.get());

		std::cout << generator.generateCode();
	}

private:
	std::shared_ptr<Node<S>> mNode;
};

template<class S>
RecType<S> operator+(S value, const RecType<S> &other) {
	return RecType<S>(value) + other;
}

template<class S>
RecType<S> operator*(S value, const RecType<S> &other) {
	return RecType<S>(value) * other;
}

}
