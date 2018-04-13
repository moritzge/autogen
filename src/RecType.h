#pragma once

#include "NodeTypes.h"

#include <string>
#include <vector>

namespace AutoGen {

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


template<class S>
class RecTypeVec
{
public:
	RecTypeVec(const std::string &name, int size) : mName(name) {
		mRecTypes.resize(size);
	}

	RecType<S> &operator[](int i) {
		return mRecTypes[i];
	}

private:
	std::shared_ptr<const Node<S>> mNode;
	std::string mName;
};

} // namespace AutoGen
