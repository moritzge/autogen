#pragma once

#include "Node.h"

#include <cassert>
#include <vector>

namespace AutoGen {

template<class S>
class NodeConst : public Node<S>
{
public:
	NodeConst (S value)
		: mValue(value){
		this->init();
	}

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
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + std::to_string(mValue);
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
		: mVarName(varName) {
		this->init();
	}

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
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + mVarName;
	}

	virtual NodeType getNodeType() const {
		return NodeType::INPUT_NODE;
	}

	virtual std::string getVarName() const {
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
class NodeVarVecEl : public Node<S>
{
public:
	NodeVarVecEl (Sp<const NodeVar<S>> node, int index)
		: mNode(node), mIndex(index) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		return mNode;
	}


	virtual S evaluate() const {
		return mNode->evaluate();
	}

	virtual bool evaluate(S &value) const {
		return mNode->evaluate(value);
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName()
				+ " = " + generator.getVar(mNode.get()) + "[" + std::to_string(mIndex) + "]";
	}

	virtual NodeType getNodeType() const {
		return NodeType::INPUT_NODE;
	}

	virtual uint64_t computeHash() const {
		return this->rol(mNode->getHash(), 3) + mIndex;
	}

private:
	Sp<const NodeVar<S>> mNode; // this is the node that this node is an element of
	int mIndex;					// index of this element in the vector

};

//template<class S>
//class NodeVarVec : public Node<S>
//{
//public:
//	NodeVarVec (const std::string &varName, int size)
//		: mVarName(varName) {
//		mNodes.resize(size);
//		this->init();
//	}

//	virtual size_t getNumChildren() const {
//		return mNodes.size();
//	}

//	virtual Sp<const Node<S>> getChild(size_t i) const {
//		throw std::logic_error("NodeVar does not have any children");
//	}


//	virtual S evaluate() const {
//		throw std::logic_error("cannot evaluate a variable name");
//		return 0;
//	}

//	virtual bool evaluate(S &value) const {
//		return false;
//	}

//	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
//		std::string code;
//		for (int i = 0; i < mNodes; ++i) {
//			code += generator.getVarTypeName() + " " + mNodes[] + " = " + + generator.getVar(this).getVarName() + "[" + std::to_string(i) + "];\n";
//		}
//		return code;
//	}

//	virtual NodeType getNodeType() const {
//		return NodeType::INPUT_NODE;
//	}

//	virtual std::string getVarName() const {
//		return mVarName;
//	}

//	virtual uint64_t computeHash() const {
//		std::hash<std::string> hashS;
//		return hashS(mVarName);
//	}

//	void setNode(int i, Sp<const Node<S>> node) {
//		mNodes[i] = node;
//	}

//private:
//	std::string mVarName;
//	std::vector<Sp<const Node<S>>> mNodes;
//};

template<class S>
class NodeResult : public Node<S>
{
public:
	NodeResult (const std::string &varName, Sp<const Node<S>> node)
		: mResVarName(varName), mNode(node) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		assert(i == 0);
		return mNode;
	}

	virtual S evaluate() const {
		throw std::logic_error("cannot evaluate a variable name");
		return 0;
	}

	virtual bool evaluate(S &value) const {
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return mResVarName + " = " + generator.getVar(mNode.get()).getVarName();
	}

	virtual NodeType getNodeType() const {
		return NodeType::OUTPUT_NODE;
	}

	virtual std::string getVarName() const {
		return mResVarName;
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mResVarName) +  this->rol(this->mNode->getHash(), 3); // TODO: is this a good hash function?
	}

private:
	std::string mResVarName;
	Sp<const Node<S>> mNode;
};

template<class S>
class NodeNeg : public Node<S>
{
public:
	NodeNeg (Sp<const Node<S>> node)
		: mNode(node) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		assert(i == 0);
		return mNode;
	}

	virtual S evaluate() const {
		return -mNode->evaluate();
	}

	virtual bool evaluate(S &value) const {
		S val;
		if(mNode->evaluate(val))
		{
			value = -val;
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName() + " = -" + generator.getVar(mNode.get()).getVarName();
	}

	virtual uint64_t computeHash() const {
		return this->rol(mNode->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 1; }

private:
	Sp<const Node<S>> mNode;

};

template<class S>
class NodeBinaryOperation : public Node<S>
{
public:
	NodeBinaryOperation (Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: mNodeA(nodeA), mNodeB(nodeB){}

	virtual size_t getNumChildren() const {
		return 2;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		if(i == 0) return mNodeA;
		if(i == 1) return mNodeB;

		throw std::logic_error("NodeBinaryOperation has only two children");
	}

protected:
	Sp<const Node<S>> mNodeA;
	Sp<const Node<S>> mNodeB;
};

template<class S>
class NodeBinaryOperationBasic : public NodeBinaryOperation<S>
{
public:
	NodeBinaryOperationBasic (Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + generator.getVar(this->mNodeA.get()).getVarName() + " " + getOpName() + " " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	virtual std::string getOpName() const = 0;
};

template<class S>
class NodeAdd : public NodeBinaryOperationBasic<S>
{
public:

	NodeAdd(Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperationBasic<S>(nodeA, nodeB) {
		this->init();
	}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() + this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		S valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA + valB;
			return true;
		}
		return false;
	}

	virtual std::string getOpName() const { return "+"; }

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 2; }
};

template<class S>
class NodeSub : public NodeBinaryOperationBasic<S>
{
public:

	NodeSub(Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperationBasic<S>(nodeA, nodeB) {
		this->init();
	}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() - this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		S valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA - valB;
			return true;
		}
		return false;
	}

	virtual std::string getOpName() const { return "-"; }

	// order of subtraction matters, thus different rolling shift (3, 5)
	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
	}

	virtual uint64_t getHashId() const { return 3; }
};

template<class S>
class NodeMul : public NodeBinaryOperationBasic<S>
{
public:
	NodeMul(Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperationBasic<S>(nodeA, nodeB) {
		this->init();
	}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() * this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		S valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA * valB;
			return true;
		}
		return false;
	}

	virtual std::string getOpName() const { return "*"; }

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 4; }
};

template<class S>
class NodeDiv : public NodeBinaryOperationBasic<S>
{
public:
	NodeDiv(Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperationBasic<S>(nodeA, nodeB) {
		this->init();
	}

	virtual S evaluate() const {
		return this->mNodeA->evaluate() / this->mNodeB->evaluate();
	}

	virtual bool evaluate(S &value) const {
		S valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = valA / valB;
			return true;
		}
		return false;
	}

	virtual std::string getOpName() const { return "/"; }

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
	}

	virtual uint64_t getHashId() const { return 5; }
};

template<class S>
class NodePow : public NodeBinaryOperation<S>
{
public:
	NodePow(Sp<const Node<S>> nodeA, Sp<const Node<S>> nodeB)
		: NodeBinaryOperation<S>(nodeA, nodeB) {
		this->init();
	}

	virtual S evaluate() const {
		return pow(this->mNodeA->evaluate(), this->mNodeB->evaluate());
	}

	virtual bool evaluate(S &value) const {
		S valA, valB;
		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
		{
			value = pow(valA, valB);
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName()
				+ " = pow(" + generator.getVar(this->mNodeA.get()).getVarName()
				+ ", " + generator.getVar(this->mNodeB.get()).getVarName() + ")";
	}

	virtual uint64_t computeHash() const {
		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
	}

	virtual uint64_t getHashId() const { return 5; }
};

template<class S>
class NodeSqrt : public Node<S>
{
public:
	NodeSqrt (Sp<const Node<S>> node)
		: mNode(node) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const Node<S>> getChild(size_t i) const {
		assert(i == 0);
		return mNode;
	}


	virtual S evaluate() const {
		return sqrt(mNode->evaluate());
	}

	virtual bool evaluate(S &value) const {
		S val;
		if(mNode->evaluate(val))
		{
			value = sqrt(val);
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator<S> &generator) const {
		return generator.getVarTypeName() + " " + generator.getVar(this).getVarName() + " = sqrt(" + generator.getVar(mNode.get()).getVarName() + ")";
	}

	virtual uint64_t computeHash() const {
		return this->rol(mNode->getHash(), 13) + getHashId();
	}

	virtual uint64_t getHashId() const { return 7; }

private:
	Sp<const Node<S>> mNode;

};

} // namespace AutoGen
