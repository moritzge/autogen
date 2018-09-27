#pragma once

#include "Matrix.h"
#include "NodeMatrix.h"

#include "RecType.h"

#include <string>
#include <vector>

namespace AutoGen {

template<class Mat>
class RecTypeMatrix
{
public:

	RecTypeMatrix(Sp<const NodeMatrixM<Mat>> node)
		: mNode(node) {

	}

	RecTypeMatrix(const std::string &varName) {
		mNode = Sp<NodeMatrixVar<Mat>>(new NodeMatrixVar<Mat>(varName));
	}

	RecTypeMatrix(const Mat &mat) {
		mNode = Sp<NodeMatrixM<Mat>>(new NodeMatrixConst<Mat>(mat));
	}

	std::shared_ptr<const NodeMatrixM<Mat>> getNode() const {
		return mNode;
	}

	template<class MatB>
	RecTypeMatrix<Mat> operator+(const RecTypeMatrix<MatB> &other) const {

		Sp<const NodeMatrixM<Mat>> node(new NodeMatrixAdd<Mat, MatB, Mat>(mNode, other.getNode()));

		Mat value;

		// constant expression?
		if(node->evaluate(value)){
			return RecTypeMatrix<Mat>(Sp<const NodeMatrixM<Mat>>(new NodeMatrixConst<Mat>(value)));
		}
		// // 0+x = x
		// if(mNode->evaluate(value) && (value == Mat(0))){
		// 	if(MatB::sizeM == 1 && MatB::sizeN==1)
		// 		return RecTypeMatrix<Mat>(Sp<const NodeMatrixM<Mat> > node);
		// }
		MatB valueB;
		// x+0 = x
		if(other.getNode()->evaluate(valueB) && (valueB == 0)){
			return RecTypeMatrix<Mat>(this->mNode);
		}

		return RecTypeMatrix<Mat>(node);
	}

	template<class MatB>
	RecTypeMatrix<Mat> operator+(const MatB &matB) const {
		RecTypeMatrix<MatB> other(matB);
		return *this + other;
	}

	RecTypeMatrix<Mat> operator+(double scalar) const {
		RecTypeMatrix<Mat> other(scalar);
		return *this + other;
	}

	template<class MatB, class MatOut>
	RecTypeMatrix<MatOut> operator*(const RecTypeMatrix<MatB> &other) const {

		Sp<const NodeMatrixMul<Mat, MatB, MatOut>> node(new NodeMatrixMul<Mat, MatB, MatOut>(mNode, other.getNode()));

		MatOut value;
		// evaluatable?
		if(node->evaluate(value)) {
			return RecTypeMatrix<MatOut>(Sp<const NodeMatrixM<MatOut>>(new NodeMatrixConst<MatOut>(value)));
		}

		return RecTypeMatrix<MatOut>(node);
	}

	template<class MatB>
	RecTypeMatrix<Matrix<Mat::sizeM, MatB::sizeN>> operator*(const RecTypeMatrix<MatB> &other) const {

		typedef Matrix<Mat::sizeM, MatB::sizeN> MatOut;
		return this->operator*<MatB, MatOut>(other);
	}

	RecTypeMatrix<Mat> operator*(const RecTypeMatrix<Matrix<1,1>> &other) const {

		typedef Matrix<1, 1> MatB;
		typedef Mat MatOut;
		return this->operator*<MatB, MatOut>(other);
	}

	RecTypeMatrix<Mat> operator*(double scalar) const {

		typedef Matrix<1, 1> MatB;
		typedef Mat MatOut;
		RecTypeMatrix<MatB> other(scalar);
		return this->operator*<MatB, MatOut>(other);
	}

	RecTypeMatrix<Matrix<Mat::sizeN, Mat::sizeM>> transpose() {
		typedef Matrix<Mat::sizeN, Mat::sizeM> MatT;
		Sp<const NodeMatrixM<MatT>> node(new NodeMatrixTranspose<Mat::sizeM, Mat::sizeN>(mNode));
		return RecTypeMatrix<MatT>(node);
	}


	void addToGeneratorAsResult(CodeGenerator &generator, const std::string &resVarName) {
		NodeBase* nodeRes = new NodeMatrixOut<Mat>(resVarName, mNode);

		generator.collectNodes(nodeRes);
	}

private:
	std::shared_ptr<const NodeMatrixM<Mat>> mNode;
};

template<class Mat>
RecTypeMatrix<Mat> operator+(const RecTypeMatrix<Matrix<1, 1>> &a, const RecTypeMatrix<Mat> &b) {
	return a+b;
}

template<class Mat>
RecTypeMatrix<Mat> operator+(double value, const RecTypeMatrix<Mat> &b) {

	typedef Mat MatA;
	typedef Matrix<1, 1> MatB;
	typedef Mat MatOut;

	RecTypeMatrix<MatB> a((Matrix<1,1>(value)));

	return b+a;
}

template<class Mat>
RecTypeMatrix<Mat> operator*(const RecTypeMatrix<Matrix<1, 1>> &a, const RecTypeMatrix<Mat> &b) {
	return b * a;
}

template<class Mat>
RecTypeMatrix<Mat> operator*(double value, const RecTypeMatrix<Mat> &b) {

	typedef Mat MatA;
	typedef Matrix<1, 1> MatB;
	typedef Mat MatOut;

	RecTypeMatrix<MatB> a((Matrix<1,1>(value)));

	return b * a;
}


//template<class S>
//class RecType
//{
//public:
//	RecType() {}

//	RecType(const S &value) {
//		mNode = Sp<Node<S>>(new NodeConst<S>(value));
//	}

//	RecType(Sp<const Node<S>> node)
//		: mNode(node) {

//	}

//	RecType(const std::string &varName) {
//		mNode = Sp<Node<S> >(new NodeVar<S>(varName));
//	}

//	RecType<S> operator-() const {
//		return RecType(Sp<Node<S>>(new NodeNeg<S>(mNode)));
//	}

//	RecType<S> operator+(const RecType<S> &other) const {

//		Sp<const Node<S>> node(new NodeAdd<S>(mNode, other.mNode));

//		S value;

//		// constant expression?
//		if(node->evaluate(value)){
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
//		}
//		// 0+x = x
//		if(mNode->evaluate(value) && (value == 0 || value == -0)){
//			return other;
//		}
//		// x+0 = x
//		if(other.mNode->evaluate(value) && (value == 0)){
//			return RecType<S>(this->mNode);
//		}

//		return RecType<S>(node);
//	}

//	RecType<S> &operator+=(const RecType<S> &other) {
//		*this = *this + other;
//		return *this;
//	}

//	RecType<S> operator-(const RecType<S> &other) const {
//		Sp<const Node<S>> node(new NodeSub<S>(mNode, other.mNode));

//		S value;
//		// constant expression?
//		if(node->evaluate(value)){
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
//		}
//		// x-x = 0
//		if(mNode->getHash() == other.mNode->getHash()){
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
//		}
//		// 0-x = -x
//		if(mNode->evaluate(value) && (value == 0 || value == -0)){
//			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(other.mNode)));
//		}
//		// x-0 = x
//		if(other.mNode->evaluate(value) && (value == 0 || value == -0)){
//			return RecType<S>(mNode);
//		}

//		return RecType<S>(node);
//	}

//	RecType<S> &operator-=(const RecType<S> &other) {
//		*this = *this - other;
//		return *this;
//	}

//	RecType<S> operator*(const RecType<S> &other) const {
//		Sp<const Node<S>> node(new NodeMul<S>(mNode, other.mNode));

//		S value;
//		// evaluatable?
//		if(node->evaluate(value)) {
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
//		}
//		// 0*x or 0*x = 0
//		if((mNode->evaluate(value) && value == 0) || (other.mNode->evaluate(value) && value == 0)){
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
//		}
//		// 1*x = x
//		if(mNode->evaluate(value) && value == 1){
//			return RecType<S>(other.mNode);
//		}
//		// x*1 = x
//		if(other.mNode->evaluate(value) && value == 1){
//			return RecType<S>(mNode);
//		}
//		// -1*x = -x
//		if(mNode->evaluate(value) && value == -1)
//			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(other.mNode)));
//		// x*-1 = -x
//		if(other.mNode->evaluate(value) && value == -1)
//			return RecType<S>(Sp<const Node<S>>(new NodeNeg<S>(mNode)));

//		return RecType<S>(node);
//	}

//	RecType<S> &operator*=(const RecType<S> &other) {
//		*this = *this * other;
//		return *this;
//	}

//	RecType<S> operator/(const RecType<S> &other) const {
//		Sp<const Node<S>> node(new NodeDiv<S>(mNode, other.mNode));

//		S value;
//		// is constant expression?
//		if(node->evaluate(value)) {
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(value)));
//		}
//		// 0/x = 0
//		if(mNode->evaluate(value) && value == 0){
//			return RecType<S>(Sp<const Node<S>>(new NodeConst<S>(0)));
//		}
//		// TODO: what to do when divided by 0?
//		// x/1 = x
//		else if(other.mNode->evaluate(value) && value == 1){
//			return RecType<S>(mNode);
//		}

//		return RecType<S>(node);
//	}

//	RecType<S> &operator/=(const RecType<S> &other) {
//		*this = *this / other;
//		return *this;
//	}

//	Sp<const Node<S>> getNode() const {
//		assert(mNode != nullptr);
//		return mNode;
//	}

//	std::string generateCode(std::string resVarName = "res", std::string functionName = "compute_extern") const {
//		CodeGenerator generator;
//		Node<S>* nodeRes = new NodeOut<S>(resVarName, mNode);

//		generator.collectNodes(nodeRes);
//		generator.sortNodes();

//		return generator.generateCode(functionName);
//	}

//	void addToGeneratorAsResult(CodeGenerator &generator, const std::string &resVarName) {
//		Node<S>* nodeRes = new NodeOut<S>(resVarName, mNode);

//		generator.collectNodes(nodeRes);
//	}

//private:
//	std::shared_ptr<const Node<S>> mNode;
//};

//template<class S>
//RecType<S> operator+(S value, const RecType<S> &other) {
//	return RecType<S>(value) + other;
//}

//template<class S>
//RecType<S> operator-(S value, const RecType<S> &other) {
//	return RecType<S>(value) - other;
//}

//template<class S>
//RecType<S> operator*(S value, const RecType<S> &other) {
//	return RecType<S>(value) * other;
//}

//template<class S>
//RecType<S> operator/(S value, const RecType<S> &other) {
//	return RecType<S>(value) / other;
//}

//template<class S>
//RecType<S> sqrt(const RecType<S> &other) {
//	return RecType<S>(Sp<const Node<S>>(new NodeSqrt<S>(other.getNode())));
//}

//template<class S>
//RecType<S> pow(const RecType<S> &a, const RecType<S> &b) {
//	return RecType<S>(Sp<const Node<S>>(new NodePow<S>(a.getNode(), b.getNode())));
//}

//template<class S>
//RecType<S> pow(const RecType<S> &a, S b) {

//	// TODO: test this
//	if(b == 1)
//		return a;
//	if(b == 0)
//		return 1;

//	Sp<const Node<S>> nodeB(new NodeConst<S>(b));
//	return RecType<S>(Sp<const Node<S>>(new NodePow<S>(a.getNode(), nodeB)));
//}


//template<class S>
//class RecTypeVec
//{
//public:
//	RecTypeVec(const std::string &name, int size) : mName(name) {
//		mRecVec.resize(size);
//		Sp<NodeVarVecBase<S>> nodeVecBase(new NodeVarVecBase<S>(mName));
//		for (int i = 0; i < size; ++i) {
//			Sp<NodeVarVecEl<S>> nodeEl(new NodeVarVecEl<S>(nodeVecBase, i));
//			mRecVec[i] = RecType<S>(nodeEl);
//		}
//	}

//	const RecType<S> &operator[](int i) const {
//		return mRecVec[i];
//	}

//private:
//	std::vector<RecType<S>> mRecVec;
//	std::string mName;
//};

//template<class S>
//void addToGeneratorAsResult(const Eigen::Matrix<RecType<S>, -1, -1> &mat, CodeGenerator &generator, const std::string &varName) {

//	NodeOutMat<S>* nodeOutMat = new NodeOutMat<S>(varName, mat.rows(), mat.cols());

//	for (int i = 0; i < mat.rows(); ++i) {
//		for (int j = 0; j < mat.cols(); ++j) {
//			nodeOutMat->setElement(i*mat.cols() + j, mat(i,j).getNode());
//		}
//	}

//	generator.collectNodes(nodeOutMat);
//}

//template<int N, class S>
//void addToGeneratorAsResult(const Eigen::Matrix<RecType<S>, N, -1> &v, CodeGenerator &generator, const std::string &varName) {

//	NodeOutVec<S>* nodeOutVec = new NodeOutVec<S>(varName, v.size());

//	for (int i = 0; i < v.size(); ++i) {
//		nodeOutVec->setElement(i, v[i].getNode());
//	}

//	generator.collectNodes(nodeOutVec);
//}

//template<int N, int M, class S>
//void addToGeneratorAsResult(const Eigen::Matrix<RecType<S>, N, M> &mat, CodeGenerator &generator, const std::string &varName) {

//	NodeOutMat<S>* nodeOutMat = new NodeOutMat<S>(varName, mat.rows(), mat.cols());

//	for (int i = 0; i < mat.rows(); ++i) {
//		for (int j = 0; j < mat.cols(); ++j) {
//			nodeOutMat->setElement(i*mat.cols() + j, mat(i,j).getNode());
//		}
//	}

//	generator.collectNodes(nodeOutMat);
//}

} // namespace AutoGen
