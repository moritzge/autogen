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
template<int M, int N>
class NodeMatrix : public Node<Matrix<M, N>>
{
public:
	NodeMatrix() {}

	// Return the evaluated value of this node
//	virtual Matrix<M, N> evaluate() const = 0;

//	virtual bool evaluate(Matrix<M, N> &value) const = 0;
};

template<class Mat>
class NodeMatrixM : public NodeMatrix<Mat::sizeM, Mat::sizeN>
{
public:
	NodeMatrixM() {}

	std::string getVarTypeName() const {
		return "Matrix<" + std::to_string(Mat::sizeM) + "," + std::to_string(Mat::sizeN) + ">";
	}

	// Return the evaluated value of this node
//	virtual Mat evaluate() const = 0;

//	virtual bool evaluate(Mat &value) const = 0;
};


template<class Mat>
class NodeMatrixConst : public NodeMatrixM<Mat>
{
public:
	NodeMatrixConst (const Mat &mat)
		: mMat(mat) {
		this->init();
	}

	virtual size_t getNumChildren() const {
		return 0;
	}

	virtual Sp<const NodeBase> getChild(size_t i) const {
		throw std::logic_error("NodeConst does not have any children");
	}


	virtual Mat evaluate() const {
		return mMat;
	}

	virtual bool evaluate(Mat &value) const {
		value = mMat;
		return true;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		std::ostringstream stream;
		stream << NodeMatrixM<Mat>::getVarTypeName() << " " << generator.getVar(this).getVarName() << " = " << mMat;
		return stream.str();
	}

	virtual std::string getVarType() const {
		return "Matrix<" + std::to_string(Mat::sizeM) + "," + std::to_string(Mat::sizeN) + "> &";
	}

	virtual uint64_t computeHash() const {
		std::hash<double> hashS;
		uint64_t res = 0;
		for (int i = 0; i < Mat::sizeM; ++i) {
			for (int j = 0; j < Mat::sizeN; ++j)
				res += hashS(mMat(i, j));
		}
		return res;
	}

protected:
	Mat mMat;
};


template<class Mat>
class NodeMatrixVar : public NodeMatrixM<Mat>
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


	virtual Mat evaluate() const {
		throw std::logic_error("cannot evaluate a variable name");
		return Mat(0);
	}

	virtual bool evaluate(Mat &value) const {
		return false;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return NodeMatrixM<Mat>::getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + mVarName;
	}

	virtual NodeType getNodeType() const {
		return NodeType::INPUT_NODE;
	}

	virtual std::string getVarName() const {
		return mVarName;
	}

	virtual std::string getVarType() const {
		return NodeMatrixM<Mat>::getVarTypeName() + " &";
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mVarName);
	}

protected:
	std::string mVarName;
};

template<class Mat>
class NodeMatrixOut : public NodeMatrixVar<Mat>
{
public:
	NodeMatrixOut (const std::string &varName, Sp<const NodeMatrixM<Mat>> node)
		: mNode(node), NodeMatrixVar<Mat>(varName) {
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

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(this->mVarName) +  this->rol(this->mNode->getHash(), 3); // TODO: is this a good hash function?
	}

private:
	Sp<const NodeMatrixM<Mat>> mNode;
};

template<class MatInA, class MatInB, class MatOut>
class NodeMatrixBinaryOperation : public NodeMatrixM<MatOut>
{
public:
	NodeMatrixBinaryOperation (Sp<const NodeMatrixM<MatInA>> nodeA, Sp<const NodeMatrixM<MatInB>> nodeB)
		: mNodeA(nodeA), mNodeB(nodeB){}

	virtual size_t getNumChildren() const {
		return 2;
	}

	virtual Sp<const NodeBase> getChild(size_t i) const {
		if(i == 0) return mNodeA;
		if(i == 1) return mNodeB;

		throw std::logic_error("NodeBinaryOperation has only two children");
	}

protected:
	Sp<const NodeMatrixM<MatInA>> mNodeA;
	Sp<const NodeMatrixM<MatInB>> mNodeB;
};

template<class MatInA, class MatInB, class MatOut>
class NodeMatrixBinaryOperationBasic : public NodeMatrixBinaryOperation<MatInA, MatInB, MatOut>
{
public:
	NodeMatrixBinaryOperationBasic (Sp<const NodeMatrixM<MatInA>> nodeA, Sp<const NodeMatrixM<MatInB>> nodeB)
		: NodeMatrixBinaryOperation<MatInA, MatInB, MatOut> (nodeA, nodeB) {}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return NodeMatrixM<MatOut>::getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + generator.getVar(this->mNodeA.get()).getVarName() + " " + getOpName() + " " + generator.getVar(this->mNodeB.get()).getVarName();
	}

	virtual std::string getOpName() const = 0;
};

template<class MatOut>
class NodeMatrixAdd : public NodeMatrixBinaryOperationBasic<MatOut, MatOut, MatOut>
{
public:

	NodeMatrixAdd(Sp<const NodeMatrixM<MatOut>> nodeA, Sp<const NodeMatrixM<MatOut>> nodeB)
		: NodeMatrixBinaryOperationBasic<MatOut, MatOut, MatOut> (nodeA, nodeB) {
		this->init();
	}

	virtual MatOut evaluate() const {
		return this->mNodeA->evaluate() + this->mNodeB->evaluate();
	}

	virtual bool evaluate(MatOut &value) const {
		MatOut valA;
		MatOut valB;
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

// MatInA * MatInB = MatOut
// MxN    * NxO    = MxO
template<int N, class MatOut>
class NodeMatrixMul : public NodeMatrixBinaryOperationBasic<Matrix<MatOut::sizeM, N>, Matrix<N, MatOut::sizeN>, MatOut>
{
	typedef Matrix<MatOut::sizeM, N> MatInA;
	typedef Matrix<N, MatOut::sizeN> MatInB;

public:

	NodeMatrixMul(Sp<const NodeMatrixM<MatInA>> nodeA, Sp<const NodeMatrixM<MatInB>> nodeB)
		: NodeMatrixBinaryOperationBasic<MatInA, MatInB, MatOut> (nodeA, nodeB) {
		this->init();
	}

	virtual MatOut evaluate() const {
		return this->mNodeA->evaluate() * this->mNodeB->evaluate();
	}

	virtual bool evaluate(MatOut &value) const {
		MatInA valA;
		MatInB valB;
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


}
