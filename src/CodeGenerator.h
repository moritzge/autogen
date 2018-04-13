#pragma once

#include "Node.h"

#include <iostream>
#include <memory>
#include <map>
#include <string>
#include <vector>

#include <ostream>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <experimental/filesystem>
#include <cmath>
#include <math.h>
#include <cassert>

#include <algorithm>

#include <eigen3/Eigen/Eigen>

#include <utility>

namespace AutoGen {

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

	std::string generateCode(std::string functionName = "compute_from_code_generator") {

		// update variable index
		int counter = 0;
		for (size_t i = 0; i < mNodes.size(); ++i) {
			mHashedNodes[mNodes[i]].second.setIndex(counter++);
		}


		// write declaration
		std::string codeInVars;
		std::string codeOutVars;
		for (uint64_t hash : mNodes) {
			const Node<S>* node = mHashedNodes[hash].first;
			if(node->getNodeType() == NodeType::INPUT_NODE)
				codeInVars += ((codeInVars.empty()) ? "" : ", ") + node->getVarType() + node->getVarName();
			else if(node->getNodeType() == NodeType::OUTPUT_NODE)
				codeOutVars += ((codeOutVars.empty()) ? ""  : ", ") + node->getVarType() + node->getVarName();
		}

		std::string code;
		code += "void " + functionName + "(" + codeInVars + ", " + codeOutVars + ")\n{\n";

		// write code
		for (size_t i = 0; i < mNodes.size(); ++i) {
			code += mHashedNodes[mNodes[i]].first->generateCode(*this) + ";\n";
		}

		code += "}\n";

		return code;
	}

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

}  // namespace AutoGen
