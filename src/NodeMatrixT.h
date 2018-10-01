#pragma once

#include "Node.h"
#include "NodeTypes.h"
#include "Matrix.h"

#include "CodeGenerator.h"

namespace AutoGen {

/****
 * NodeMatrixT:
 *
 * Takes as input an `Ni x Mi` and outputs a `No x Mo` matrix.
 *
 * TODO: if Ni/Mi/No/Mo is -1, the consider it to be of dynamic size.
 *       similar to Eigen
 *
 *
 */
template<int M, int N>
class NodeMatrixT : public Node<Matrix<M, N>>
{
public:
	NodeMatrixT() {}

	// Return the evaluated value of this node
//	virtual Matrix<M, N> evaluate() const = 0;

//	virtual bool evaluate(Matrix<M, N> &value) const = 0;
};

template<class Mat>
class NodeMatrixTM : public NodeMatrixT<Mat::sizeM, Mat::sizeN>
{
public:

//	const static type TMat = Mat;

	NodeMatrixTM() {}

	std::string getVarTypeName() const {
		return "Matrix<" + std::to_string(Mat::sizeM) + "," + std::to_string(Mat::sizeN) + ">";
	}

	// Return the evaluated value of this node
//	virtual Mat evaluate() const = 0;

//	virtual bool evaluate(Mat &value) const = 0;
};


template<class Mat>
class NodeMatrixTConst : public NodeMatrixTM<Mat>
{
public:
	NodeMatrixTConst (const Mat &mat)
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
		stream << NodeMatrixTM<Mat>::getVarTypeName() << " " << generator.getVar(this).getVarName() << " = " << mMat;
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
class NodeMatrixTVar : public NodeMatrixTM<Mat>
{
public:
	NodeMatrixTVar (const std::string &varName)
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
		return NodeMatrixTM<Mat>::getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + mVarName;
	}

	virtual NodeType getNodeType() const {
		return NodeType::INPUT_NODE;
	}

	virtual std::string getVarName() const {
		return mVarName;
	}

	virtual std::string getVarType() const {
		return NodeMatrixTM<Mat>::getVarTypeName() + " &";
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(mVarName);
	}

protected:
	std::string mVarName;
};

template<class NodeIn, class MatIn, class MatOut>
class NodeMatrixTUnary : public NodeMatrixTM<MatOut>
{
public:

	NodeMatrixTUnary (Sp<const NodeIn> node)
		: mNode(node) {}

	virtual size_t getNumChildren() const {
		return 1;
	}

	virtual Sp<const NodeBase> getChild(size_t i) const {
		return mNode;
	}

protected:
	Sp<const NodeIn> mNode;
};

template<class NodeIn, class Mat>
class NodeMatrixTOut : public NodeMatrixTUnary<NodeIn, Mat, Mat>
{
public:
	NodeMatrixTOut (const std::string &varName, Sp<const NodeIn> node)
		: NodeMatrixTUnary<NodeIn, Mat, Mat>(node), mVarName(varName) {
		this->init();

		std::cout << "type of Out chid is: " << typeid(NodeIn).name() << std::endl;

	}

	virtual Mat evaluate() const {
		throw std::logic_error("cannot evaluate a variable name");
		return Mat(0);
	}

	virtual bool evaluate(Mat &value) const {
		return false;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return this->mVarName + " = " + generator.getVar(this->mNode.get()).getVarName();
	}

	virtual NodeType getNodeType() const {
		return NodeType::OUTPUT_NODE;
	}

	virtual std::string getVarName() const {
		return mVarName;
	}

	virtual std::string getVarType() const {
		return NodeMatrixTM<Mat>::getVarTypeName() + " &";
	}

	virtual uint64_t computeHash() const {
		std::hash<std::string> hashS;
		return hashS(this->mVarName) +  this->rol(this->mNode->getHash(), 3); // TODO: is this a good hash function?
	}

protected:
	std::string mVarName;
};

template<class NodeIn, int M, int N>
class NodeMatrixTTranspose : public NodeMatrixTUnary<NodeIn, Matrix<M, N>, Matrix<N, M>>
{
public:
	typedef Matrix<M, N> MatIn;
	typedef Matrix<N, M> MatOut;

	NodeMatrixTTranspose (Sp<const NodeIn> node)
		: NodeMatrixTUnary<NodeIn, Matrix<M, N>, Matrix<N, M>>(node) {
		this->init();

	}

	virtual MatOut evaluate() const {
		return this->mNode->evaluate().transpose();
	}

	virtual bool evaluate(MatOut &value) const {
		MatIn mat;
		if(this->mNode->evaluate(mat)){
			value = mat.transpose();
			return true;
		}
		return false;
	}

	virtual std::string generateCode(const CodeGenerator &generator) const {
		return NodeMatrixTM<MatOut>::getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + generator.getVar(this->mNode.get()).getVarName() + ".transpose()";
	}

	virtual uint64_t computeHash() const {
		return this->rol(this->mNode->getHash(), 3) + getHashId();
	}

	virtual uint64_t getHashId() const { return 6; }

private:

};

//template<class MatInA, class MatInB, class MatOut>
//class NodeMatrixTBinaryOperation : public NodeMatrixTM<MatOut>
//{
//public:
//	NodeMatrixTBinaryOperation (Sp<const NodeMatrixTM<MatInA>> nodeA, Sp<const NodeMatrixTM<MatInB>> nodeB)
//		: mNodeA(nodeA), mNodeB(nodeB){}

//	virtual size_t getNumChildren() const {
//		return 2;
//	}

//	virtual Sp<const NodeBase> getChild(size_t i) const {
//		if(i == 0) return mNodeA;
//		if(i == 1) return mNodeB;

//		throw std::logic_error("NodeBinaryOperation has only two children");
//	}

//protected:
//	Sp<const NodeMatrixTM<MatInA>> mNodeA;
//	Sp<const NodeMatrixTM<MatInB>> mNodeB;
//};

//template<class MatInA, class MatInB, class MatOut>
//class NodeMatrixTBinaryOperationBasic : public NodeMatrixTBinaryOperation<MatInA, MatInB, MatOut>
//{
//public:
//	NodeMatrixTBinaryOperationBasic (Sp<const NodeMatrixTM<MatInA>> nodeA, Sp<const NodeMatrixTM<MatInB>> nodeB)
//		: NodeMatrixTBinaryOperation<MatInA, MatInB, MatOut> (nodeA, nodeB) {}

//	virtual std::string generateCode(const CodeGenerator &generator) const {
//		return NodeMatrixTM<MatOut>::getVarTypeName() + " " + generator.getVar(this).getVarName() + " = " + generator.getVar(this->mNodeA.get()).getVarName() + " " + getOpName() + " " + generator.getVar(this->mNodeB.get()).getVarName();
//	}

//	virtual std::string getOpName() const = 0;
//};

//template<class MatA, class MatB, class MatOut>
//class NodeMatrixTAdd : public NodeMatrixTBinaryOperationBasic<MatA, MatB, MatOut>
//{
//public:

//	// Matrix + Matrix
//	NodeMatrixTAdd(Sp<const NodeMatrixTM<MatOut>> nodeA, Sp<const NodeMatrixTM<MatOut>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<MatOut, MatOut, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	// Matrix + Scalar
//	NodeMatrixTAdd(Sp<const NodeMatrixTM<MatOut>> nodeA, Sp<const NodeMatrixTM<Matrix<1,1>>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<MatOut, Matrix<1, 1>, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	virtual MatOut evaluate() const {
//		return this->mNodeA->evaluate() + this->mNodeB->evaluate();
//	}

//	virtual bool evaluate(MatOut &value) const {
//		MatA valA;
//		MatB valB;
//		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
//		{
//			value = valA + valB;
//			return true;
//		}
//		return false;
//	}

//	virtual std::string getOpName() const { return "+"; }

//	virtual uint64_t computeHash() const {
//		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
//	}

//	virtual uint64_t getHashId() const { return 2; }
//};

//template<class MatA, class MatB, class MatOut>
//class NodeMatrixTSub : public NodeMatrixTBinaryOperationBasic<MatA, MatB, MatOut>
//{
//public:

//	// Matrix + Matrix
//	NodeMatrixTSub(Sp<const NodeMatrixTM<MatOut>> nodeA, Sp<const NodeMatrixTM<MatOut>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<MatOut, MatOut, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	// Matrix + Scalar
//	NodeMatrixTSub(Sp<const NodeMatrixTM<MatOut>> nodeA, Sp<const NodeMatrixTM<Matrix<1,1>>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<MatOut, Matrix<1, 1>, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	virtual MatOut evaluate() const {
//		return this->mNodeA->evaluate() - this->mNodeB->evaluate();
//	}

//	virtual bool evaluate(MatOut &value) const {
//		MatA valA;
//		MatB valB;
//		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
//		{
//			value = valA - valB;
//			return true;
//		}
//		return false;
//	}

//	virtual std::string getOpName() const { return "-"; }

//	virtual uint64_t computeHash() const {
//		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 3) + getHashId();
//	}

//	virtual uint64_t getHashId() const { return 3; }
//};

//// MatInA * MatInB = MatOut
//// MxN    * OxP    = QxR
//template<class MatA, class MatB, class MatOut>
//class NodeMatrixTMul : public NodeMatrixTBinaryOperationBasic<MatA, MatB, MatOut>
//{
//	static const int M = MatA::sizeM;
//	static const int N = MatA::sizeN;
//	static const int O = MatB::sizeM;
//	static const int P = MatB::sizeN;
//	static const int Q = MatOut::sizeM;
//	static const int R = MatOut::sizeN;

//public:

//	// Matrix-Matrix multiplication
//	// QxN * NxR = QxR
//	NodeMatrixTMul(Sp<const NodeMatrixTM<Matrix<Q, N>>> nodeA, Sp<const NodeMatrixTM<Matrix<N, R>>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<Matrix<Q, N>, Matrix<N, R>, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	// Matrix-Scalar multiplication, where Scalar = 1x1 Matrix
//	// QxR * 1x1 = QxR
//	NodeMatrixTMul(Sp<const NodeMatrixTM<Matrix<Q, R>>> nodeA, Sp<const NodeMatrixTM<Matrix<1, 1>>> nodeB)
//		: NodeMatrixTBinaryOperationBasic<Matrix<Q, R>, Matrix<1, 1>, MatOut> (nodeA, nodeB) {
//		this->init();
//	}

//	virtual MatOut evaluate() const {
//		return this->mNodeA->evaluate() * this->mNodeB->evaluate();
//	}

//	virtual bool evaluate(MatOut &value) const {
//		MatA valA;
//		MatB valB;
//		if(this->mNodeA->evaluate(valA) && this->mNodeB->evaluate(valB))
//		{
//			value = valA * valB;
//			return true;
//		}
//		return false;
//	}

//	virtual std::string getOpName() const { return "*"; }

//	virtual uint64_t computeHash() const {
//		// We use a different bit-shift for the `rol` function for each node, because matrix multiplication is not associative

//		bool isAssociative = false;
//		isAssociative |= (M == N && N == O && O == P);	// MatA and MatB both square and of same size
//		isAssociative |= M==1 && N==1;					// MatA is 1x1 Matrix
//		isAssociative |= O==1 && P==1;					// MatB is 1x1 Matrix

//		int dA = 3, dB = (isAssociative) ? 3 : 5; // bit-shifts for nodeA and nodeB
//		return this->rol(this->mNodeA->getHash(), 3) + this->rol(this->mNodeB->getHash(), 5) + getHashId();
//	}

//	virtual uint64_t getHashId() const { return 4; }
//};


}
