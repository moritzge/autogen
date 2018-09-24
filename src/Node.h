#pragma once

#include <memory>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

namespace AutoGen {

template <class T> using Sp = std::shared_ptr<T>;

class CodeGenerator;

enum NodeType { REGULAR_NODE, INPUT_NODE, OUTPUT_NODE };

class NodeBase
{
public:
	NodeBase() {}

	virtual size_t getNumChildren() const = 0;

	virtual Sp<const NodeBase> getChild(size_t i) const = 0;

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

	virtual std::string getVarName() const {
		throw std::logic_error("`getVarName()` should not be called on this node!");
		return "";
	}

	virtual std::string getVarType() const {
		throw std::logic_error("`getVarType()` should not be called on this node!");
		return "";
	}

	virtual std::string generateCode(const CodeGenerator &generator) const = 0;

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

template<class S>
class Node : public NodeBase
{
public:

	Node() {
	}

	// Return the evaluated value of this node
	virtual S evaluate() const = 0;

	virtual bool evaluate(S &value) const = 0;

};

}
