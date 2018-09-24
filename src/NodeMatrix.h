#pragma once

#include "Node.h"
#include "NodeTypes.h"
#include "Matrix.h"

#include "CodeGenerator.h"

namespace AutoGen {

/****
 * NodeMatrix:
 *
 * Takes as input an `Ni x Mi` and outputs a `No x Mo` matrix.
 *
 * TODO: if Ni/Mi/No/Mo is -1, the consider it to be of dynamic size.
 *       similar to Eigen
 *
 *
 */
template<int Ni, int Mi, int No, int Mo>
class NodeMatrix : public Node<Matrix<No, Mo>>
{
public:
	NodeMatrix() {}

	// Return the evaluated value of this node
	virtual Matrix<No, Mo> evaluate() const = 0;

	virtual bool evaluate(Matrix<No, Mo> &value) const = 0;
};

template<class MatrixIn, class MatrixOut>
class NodeMatrixM : public NodeMatrix<MatrixIn::sizeN, MatrixIn::sizeM, MatrixOut::sizeN, MatrixOut::sizeM>
{
public:
	NodeMatrixM() {}

	// Return the evaluated value of this node
	virtual MatrixOut evaluate() const = 0;

	virtual bool evaluate(MatrixOut &value) const = 0;
};


template<class MatrixIn, class MatrixOut>
class NodeMatrixVar : public NodeMatrixM<MatrixIn, MatrixOut>
{
public:
	NodeMatrixVar (const std::string &varName)
		: mVarName(varName) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 0;
	}

	virtual Sp<const NodeBase> getChild(size_t i) const {
		throw std::logic_error("NodeVar does not have any children");
	}


	virtual MatrixOut evaluate() const {
		throw std::logic_error("cannot evaluate a variable name");
		return MatrixOut(0);
	}

	virtual bool evaluate(MatrixOut &value) const {
		return false;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return "Matrix " + generator.getVar(this).getVarName() + " = " + mVarName;
	}

	virtual NodeType getNodeType() const {
		return NodeType::INPUT_NODE;
	}

	virtual std::string getVarName() const {
		return mVarName;
	}

	virtual std::string getVarType() const {
		return "Matrix &";
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mVarName);
	}

protected:
	std::string mVarName;
};

template<class MI, class MO>
class NodeMatrixOut : public NodeMatrixVar<MI, MO>
{
public:
	NodeMatrixOut (const std::string &varName, Sp<const NodeMatrixM<MI, MO>> node)
		: mNode(node), NodeMatrixVar<MI, MO>(varName) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const NodeBase> getChild(size_t i) const {
		assert(i == 0);
		return mNode;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return this->mVarName + " = " + generator.getVar(mNode.get()).getVarName();
	}

	virtual NodeType getNodeType() const {
		return NodeType::OUTPUT_NODE;
	}

	virtual std::string getVarType() const {
		return "Matrix &";
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(this->mVarName) +  this->rol(this->mNode->getHash(), 3); // TODO: is this a good hash function?
	}

private:
	Sp<const NodeMatrixM<MI, MO>> mNode;
};


}
