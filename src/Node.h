#pragma once

#include <memory>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>

namespace AutoGen {

template <class T> using Sp = std::shared_ptr<T>;

template<class S> class CodeGenerator;

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

	virtual std::string generateCode(const CodeGenerator<S> &generator) const = 0;

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
}
