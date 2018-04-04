#pragma once

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include <ostream>
#include <cmath>
#include <math.h>
#include <cassert>

#include <algorithm>

#include <Eigen/Eigen>

#include<utility>

namespace AutoGen {

template <class T> using Sp = std::shared_ptr<T>;

template<class S>
class CodeGenerator;


////////////////////////////////////////////////////////////////////////////////
template<class S>
class VectorX;

template <class S, int N> using VectorXN = Eigen::Matrix<S, N, 1>;

template<class S>
class Vector3;

template<class S>
class RecType;

typedef AutoDiff<double, double> AD;
typedef AutoDiff<AD, AD> ADD;

typedef RecType<double> R;

typedef AutoDiff<R, R> ADR;
typedef AutoDiff<ADR, ADR> ADDR;

template <class S>
class VectorX : public VectorXN<S, -1>
{
public:
	VectorX() : VectorXN<S, -1>() {}
	VectorX(int size, const std::string &name)
	{
		this->resize(size);
		for (int i = 0; i < size; ++i) {
			(*this)[i] = S(name + "[" + std::to_string(i) + "]");
		}
	}
};

template <class S>
class Vector3 : public VectorX<S>
{
public:
	Vector3() : VectorX<S>(3) {}
	Vector3(const std::string &name) : VectorX<S>(3, name)
	{
	}
};

template <class S>
class VarListX : public VectorXN<S*, -1>
{
public:
	
	VarListX(int size) : VectorXN<S*, -1>(size)
	{
	}

	void assignSegment(int idx, int size, VectorX<S> &vars)
	{
		for (int i = 0; i < size; i++)
		{
			(*this)[idx + i] = &vars[i];
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

enum NodeType { REGULAR_NODE, INPUT_NODE, OUTPUT_NODE };

template<class S>
class Node
{
public:
	Node() {
	}

	virtual size_t getNumChildren() const = 0;

	virtual Sp<const Node<S>> getChild(size_t i) const = 0;

	// Return the evaluated value of this node
	virtual S evaluate() const = 0;

	virtual bool evaluate(S &value) const = 0;

	virtual std::string generateCode(const CodeGenerator<S> &generator) const = 0;

	virtual uint64_t getHash() const {
		if (!mIsHashValid)
		{
			mCachedHash = this->computeHash();
			mIsHashValid = true;

		}

		return mCachedHash;
	}

	virtual uint64_t getHashId() const { return 0; }

	virtual NodeType getNodeType() const {
		return REGULAR_NODE;
	}

	static uint64_t rol(uint64_t x, int d) {
		return (x << d) | (x >> (64-d));
	}

protected:
	uint64_t computeHashRand() const {
		return rand();
	}

	virtual uint64_t computeHash() const = 0;

	void init() {
		mCachedHash = this->computeHash();
		mIsHashValid = true;
	}

protected:
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
class RecType;

template<class S>
class CodeGenerator
{
public:
	CodeGenerator() {}

	void addNode(const Node<S>* node) {

		// check if we have the same hashed node already
		uint64_t hash = node->getHash();
		assert(mHashedNodes.find(hash) == mHashedNodes.end());

		mNodes.push_back(hash);
		mHashedNodes[hash] = std::make_pair(node, VarDef());
	}

	const VarDef& getVar(const Node<S>* node) const {
		assert(node);

		uint64_t h = node->getHash();
		auto it = mHashedNodes.find(h);
		assert(it != mHashedNodes.end());
		return it->second.second;
	}

	const std::string &getVarTypeName() const { return mVarTypeName; }

	const Node<S>* getHashedNode(uint64_t nodeHash) const {
		auto it = mHashedNodes.find(nodeHash);
		if(it != mHashedNodes.end())
			return it->second.first;
		else
			return nullptr;
	}

	const Node<S>* getHashedNode(const Node<S>* node) const {
		return getHashedNode(node->getHash());
	}

	void collectNodes(const Node<S>* rootNode) {

		// this is where we store all nodes that we still need to visit
		std::vector<const Node<S>*> nodesToVisit;

		// node we are currently visiting
		const Node<S>* nodeVisiting = rootNode;

		// go through graph and visit nodes
		while (true) {
			// does the generator already have a same-hashed node?
			const Node<S>* hashedNode = getHashedNode(nodeVisiting);
			// if not, let's add it and remember to visit children
			if(hashedNode == nullptr) {

				addNode(nodeVisiting);
				for (size_t i = 0; i < nodeVisiting->getNumChildren(); ++i) {
					nodesToVisit.push_back(nodeVisiting->getChild(i).get());
				}
			}
			else {
//				std::cout << "node already in!\n";
			}

			// let's go to next node
			if(nodesToVisit.size() == 0) break;
			nodeVisiting = nodesToVisit.front();
			nodesToVisit.erase(nodesToVisit.begin());
		}
	}

	void sortNodes(){

		// topological sort
		std::vector<uint64_t> nodesNew;
//		for (auto h : mNodes) {
		for (size_t i = 0; i < mNodes.size(); ++i) {
			uint64_t h = mNodes[i];
			const Node<S>* node = mHashedNodes[h].first;
			assert(node != nullptr);
			addChildrenTo(node, nodesNew);
		}

		// move input/output nodes to front/back
		std::vector<int> nodesIn, nodesOut;
		for (size_t i = 0; i < nodesNew.size(); ++i) {
			uint64_t h = nodesNew[i];
			const Node<S>* node = mHashedNodes[h].first;
			if(node->getNodeType() == INPUT_NODE) nodesIn.push_back(i);
			else if(node->getNodeType() == OUTPUT_NODE) nodesOut.push_back(i);
		}

		std::vector<uint64_t> nodesNew2;
		for (int i : nodesIn) {
			nodesNew2.push_back(nodesNew[i]);
		}

		for (size_t i = 0; i < nodesNew.size(); ++i) {
			if(std::find(nodesIn.begin(), nodesIn.end(), i) != nodesIn.end())
				continue;
			if(std::find(nodesOut.begin(), nodesOut.end(), i) != nodesOut.end())
				continue;
			nodesNew2.push_back(nodesNew[i]);
		}

		for (int i : nodesOut) {
			nodesNew2.push_back(nodesNew[i]);
		}

		mNodes = nodesNew2;
	}

	std::string generateCode() {

		// update variable index
		int counter = 0;
		for (size_t i = 0; i < mNodes.size(); ++i) {
			mHashedNodes[mNodes[i]].second.setIndex(counter++);
		}

		// write code
		std::string code;
		for (size_t i = 0; i < mNodes.size(); ++i) {
			code += mHashedNodes[mNodes[i]].first->generateCode(*this) + ";\n";
		}

		return code;
	}

	template<typename F, typename... A>
	std::string generateGradientCode(const VarListX<ADR> &variables, F &&f, A&&... a);

	template<typename F, typename... A>
	std::string generateGradientCode(const VarListX<ADDR> &variables, F &&f, A&&... a);

	template<typename F, typename... A>
	std::string generateHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a);

	template<typename F, typename... A>
	std::string generateGradientAndHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a);

private:
	void addChildrenTo(const Node<S>* node, std::vector<uint64_t> &nodesNew) const {
		// is this node already in the new list?
		uint64_t h = node->getHash();
		if(std::find(nodesNew.begin(), nodesNew.end(), h) == nodesNew.end()) {
			// first make sure children are added
			for (size_t i = 0; i < node->getNumChildren(); ++i) {
				addChildrenTo(node->getChild(i).get(), nodesNew);
			}
			// and then add this node
			nodesNew.push_back(node->getHash());
		}
	}

private:
	std::string mVarTypeName = "double";
	std::vector<uint64_t> mNodes;
	std::map<uint64_t, std::pair<const Node<S>*, VarDef>> mHashedNodes;
};

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

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mVarName);
	}

private:
	std::string mVarName;

};

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

template<class S>
class RecType
{
public:
	RecType() {}

	RecType(const S &value) {
		mNode = Sp<Node<S>>(new NodeConst<S>(value));
	}

	RecType(Sp<const Node<S>> node)
		: mNode(node) {

	}

	RecType(const std::string &varName) {
		mNode = Sp<Node<S> >(new NodeVar<S>(varName));
	}

	RecType<S> operator-() const {
		return RecType(Sp<Node<S>>(new NodeNeg<S>(mNode)));
	}

	RecType<S> operator+(const RecType<S> &other) const {

		Sp<const Node<S>> node(new NodeAdd<S>(mNode, other.mNode));

		S value;

		// constant expression?
		if(node->evaluate(value)){
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
		}
		// 0+x = x
		if(mNode->evaluate(value) && (value == 0 || value == -0)){
			return other;
		}
		// x+0 = x
		if(other.mNode->evaluate(value) && (value == 0)){
			return RecType<S>(this->mNode);
		}

		return RecType<S>(node);
	}

	RecType<S> &operator+=(const RecType<S> &other) {
		*this = *this + other;
		return *this;
	}

	RecType<S> operator-(const RecType<S> &other) const {
		Sp<const Node<S>> node(new NodeSub<S>(mNode, other.mNode));

		S value;
		// constant expression?
		if(node->evaluate(value)){
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
		}
		// x-x = 0
		if(mNode->getHash() == other.mNode->getHash()){
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
		}
		// 0-x = -x
		if(mNode->evaluate(value) && (value == 0 || value == -0)){
			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(other.mNode)));
		}
		// x-0 = x
		if(other.mNode->evaluate(value) && (value == 0 || value == -0)){
			return RecType<S>(mNode);
		}

		return RecType<S>(node);
	}

	RecType<S> &operator-=(const RecType<S> &other) {
		*this = *this - other;
		return *this;
	}

	RecType<S> operator*(const RecType<S> &other) const {
		Sp<const Node<S>> node(new NodeMul<S>(mNode, other.mNode));

		S value;
		// evaluatable?
		if(node->evaluate(value)) {
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
		}
		// 0*x or 0*x = 0
		if((mNode->evaluate(value) && value == 0) || (other.mNode->evaluate(value) && value == 0)){
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
		}
		// 1*x = x
		if(mNode->evaluate(value) && value == 1){
			return RecType<S>(other.mNode);
		}
		// x*1 = x
		if(other.mNode->evaluate(value) && value == 1){
			return RecType<S>(mNode);
		}
		// -1*x = -x
		if(mNode->evaluate(value) && value == -1)
			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(other.mNode)));
		// x*-1 = -x
		if(other.mNode->evaluate(value) && value == -1)
			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(mNode)));

		return RecType<S>(node);
	}

	RecType<S> &operator*=(const RecType<S> &other) {
		*this = *this * other;
		return *this;
	}

	RecType<S> operator/(const RecType<S> &other) const {
		Sp<const Node<S>> node(new NodeDiv<S>(mNode, other.mNode));

		S value;
		// is constant expression?
		if(node->evaluate(value)) {
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
		}
		// 0/x = 0
		if(mNode->evaluate(value) && value == 0){
			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
		}
		// TODO: what to do when divided by 0?
		// x/1 = x
		else if(other.mNode->evaluate(value) && value == 1){
			return RecType<S>(mNode);
		}

		return RecType<S>(node);
	}

	RecType<S> &operator/=(const RecType<S> &other) {
		*this = *this / other;
		return *this;
	}

	Sp<const Node<S>> getNode() const {
		assert(mNode != nullptr);
		return mNode;
	}

	std::string generateCode(std::string resVarName = "res") const {
		CodeGenerator<S> generator;
		Node<S>* nodeRes = new NodeResult<S>(resVarName, mNode);

		generator.collectNodes(nodeRes);
		generator.sortNodes();

		return generator.generateCode();
	}

	void printCode(std::string resVarName = "res") const {

		std::cout << generateCode(resVarName);
	}

	void addToGeneratorAsResult(CodeGenerator<S> &generator, const std::string &resVarName) {
		Node<S>* nodeRes = new NodeResult<S>(resVarName, mNode);

		generator.collectNodes(nodeRes);
	}

private:
	std::shared_ptr<const Node<S>> mNode;
};

template<class S>
RecType<S> operator+(S value, const RecType<S> &other) {
	return RecType<S>(value) + other;
}

template<class S>
RecType<S> operator-(S value, const RecType<S> &other) {
	return RecType<S>(value) - other;
}

template<class S>
RecType<S> operator*(S value, const RecType<S> &other) {
	return RecType<S>(value) * other;
}

template<class S>
RecType<S> operator/(S value, const RecType<S> &other) {
	return RecType<S>(value) / other;
}

template<class S>
RecType<S> sqrt(const RecType<S> &other) {
	return RecType<S>(Sp<const Node<S>>(new NodeSqrt<S>(other.getNode())));
}

double sqrt(const double &other) {
	return std::sqrt(other);
}

template<class S>
RecType<S> pow(const RecType<S> &a, const RecType<S> &b) {
	return RecType<S>(Sp<const Node<S>>(new NodePow<S>(a.getNode(), b.getNode())));
}

template<class S>
RecType<S> pow(const RecType<S> &a, S b) {

	// TODO: test this
	if(b == 1)
		return a;
	if(b == 0)
		return 1;

	Sp<const Node<S>> nodeB(new NodeConst<S>(b));
	return RecType<S>(Sp<const Node<S>>(new NodePow<S>(a.getNode(), nodeB)));
}

double pow(const double &a, const double &b) {
	return std::pow(a, b);
}

template<class S>
template<typename F, typename... A>
std::string CodeGenerator<S>::generateGradientCode(const VarListX<ADR> &variables, F &&f, A&&... a)
{
	for (size_t i = 0; i < 3; i++)
	{
		(*variables[i]).deriv() = 1;
		ADR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<S> grad = energy.deriv();
		grad.addToGeneratorAsResult(*this, "g[" + std::to_string(i) + "]");
		(*variables[i]).deriv() = 0;
	}

	sortNodes();
	return generateCode();
}

template<class S>
template<typename F, typename... A>
std::string CodeGenerator<S>::generateGradientCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	for (size_t i = 0; i < 3; i++)
	{
		(*variables[i]).deriv() = 1;
		ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<S> grad = energy.deriv().value();
		grad.addToGeneratorAsResult(*this, "g[" + std::to_string(i) + "]");
		(*variables[i]).deriv() = 0;
	}

	sortNodes();
	return generateCode();
}

template<class S>
template<typename F, typename... A>
std::string CodeGenerator<S>::generateHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	for (size_t i = 0; i < 3; i++)
	{
		(*variables[i]).deriv() = 1;

		ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
		RecType<S> grad = energy.deriv().value();
		grad.addToGeneratorAsResult(*this, "g[" + std::to_string(i) + "]");

		for (size_t j = 0; j < 3; j++)
		{
			(*variables[i]).value().deriv() = 1;
			ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
			RecType<S> hess = energy.deriv().deriv();
			hess.addToGeneratorAsResult(*this, "hess[" + std::to_string(i) + "][" + std::to_string(j) + "]");
			(*variables[i]).value().deriv() = 0;
		}
		(*variables[i]).deriv() = 0;
	}

	sortNodes();
	return generateCode();
}

template<class S>
template<typename F, typename... A>
std::string CodeGenerator<S>::generateGradientAndHessianCode(const VarListX<ADDR> &variables, F &&f, A&&... a)
{
	for (size_t i = 0; i < 3; i++)
	{
		(*variables[i]).deriv() = 1;
		for (size_t j = 0; j < 3; j++)
		{
			(*variables[i]).value().deriv() = 1;
			ADDR energy = std::forward<F>(f)(std::forward<A>(a)...);
			RecType<S> hess = energy.deriv().deriv();
			hess.addToGeneratorAsResult(*this, "hess(" + std::to_string(i) + ", " + std::to_string(j) + ")");
			(*variables[i]).value().deriv() = 0;
		}
		(*variables[i]).deriv() = 0;
	}

	sortNodes();
	return generateCode();
}


// TODO: needed?
//template<class S>
//RecType<S> pow(S a, const RecType<S> &b) {
//	Sp<const Node<S>> nodeA(new NodeConst<S>(a));
//	return RecType<S>(Sp<const Node<S>>(new NodePow<S>(nodeA, b.getNode())));
//}

// Helpers


}

