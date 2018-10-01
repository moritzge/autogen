#pragma once

#include "Matrix.h"
#include "NodeMatrix.h"

#include "RecType.h"

#include <string>
#include <vector>

namespace AutoGen {

template<class Mat, class NodeT>
class RecTypeT
{
public:

	RecTypeT(Sp<const NodeT> node)
		: mNode(node) {

	}

	RecTypeT(const std::string &varName) {
		mNode = Sp<NodeMatrixVar<Mat>>(new NodeMatrixVar<Mat>(varName));
	}

	RecTypeT(const Mat &mat) {
		mNode = Sp<NodeMatrixConst<Mat>>(new NodeMatrixConst<Mat>(mat));
	}

	std::shared_ptr<const NodeMatrixM<Mat>> getNode() const {
		return mNode;
	}

//	template<class MatB>
	template<class NodeTB>
	RecTypeT<Mat, NodeMatrixAdd<Mat,Mat,Mat>> operator+(const RecTypeT<Mat, NodeTB> &other) const {

//		std::cout << typeid (NodeTB) << std::endl;

		Sp<const NodeMatrixAdd<Mat, Mat, Mat>> node(new NodeMatrixAdd<Mat, Mat, Mat>(mNode, other.getNode()));

		Mat value;

		// constant expression?
//		if(node->evaluate(value)){
//			auto nodeConst = Sp<const NodeMatrixConst<Mat>>(new NodeMatrixConst<Mat>(value));
//			return RecTypeT<Mat, NodeMatrixConst<Mat>>(nodeConst);
//		}
		// // 0+x = x
		// if(mNode->evaluate(value) && (value == Mat(0))){
		// 	if(MatB::sizeM == 1 && MatB::sizeN==1)
		// 		return RecTypeT<Mat>(Sp<const NodeMatrixM<Mat> > node);
		// }
//		MatB valueB;
//		// x+0 = x
//		if(other.getNode()->evaluate(valueB) && (valueB == 0)){
//			return RecTypeT<Mat>(this->mNode);
//		}

		return RecTypeT<Mat, NodeMatrixAdd<Mat,Mat,Mat>>(node);
	}

//	template<class MatB>
//	RecTypeT<Mat> operator+(const MatB &matB) const {
//		RecTypeT<MatB> other(matB);
//		return *this + other;
//	}

//	RecTypeT<Mat> operator+(double scalar) const {
//		RecTypeT<Mat> other(scalar);
//		return *this + other;
//	}

//	template<class MatB>
//	RecTypeT<Mat> operator-(const RecTypeT<MatB> &other) const {

//		Sp<const NodeMatrixM<Mat>> node(new NodeMatrixSub<Mat, MatB, Mat>(mNode, other.getNode()));

//		Mat value;

//		// constant expression?
//		if(node->evaluate(value)){
//			return RecTypeT<Mat>(Sp<const NodeMatrixM<Mat>>(new NodeMatrixConst<Mat>(value)));
//		}
//		// x-x = 0
//		if(mNode->getHash() == other.mNode->getHash()){
//			return RecTypeT<Mat>(Sp<const NodeMatrixM<Mat>>(new NodeMatrixConst<Mat>(0)));
//		}




//		// // 0+x = x
//		// if(mNode->evaluate(value) && (value == Mat(0))){
//		// 	if(MatB::sizeM == 1 && MatB::sizeN==1)
//		// 		return RecTypeT<Mat>(Sp<const NodeMatrixM<Mat> > node);
//		// }
//		MatB valueB;
//		// x+0 = x
//		if(other.getNode()->evaluate(valueB) && (valueB == 0)){
//			return RecTypeT<Mat>(this->mNode);
//		}

//		return RecTypeT<Mat>(node);
//	}

//	template<class MatB>
//	RecTypeT<Mat> operator-(const MatB &matB) const {
//		RecTypeT<MatB> other(matB);
//		return *this - other;
//	}

//	RecTypeT<Mat> operator-(double scalar) const {
//		RecTypeT<Mat> other(scalar);
//		return *this - other;
//	}

//	template<class MatB, class MatOut>
//	RecTypeT<MatOut> operator*(const RecTypeT<MatB> &other) const {

//		Sp<const NodeMatrixMul<Mat, MatB, MatOut>> node(new NodeMatrixMul<Mat, MatB, MatOut>(mNode, other.getNode()));

//		MatOut value;
//		// evaluatable?
//		if(node->evaluate(value)) {
//			return RecTypeT<MatOut>(Sp<const NodeMatrixM<MatOut>>(new NodeMatrixConst<MatOut>(value)));
//		}

//		return RecTypeT<MatOut>(node);
//	}

//	template<class MatB>
//	RecTypeT<Matrix<Mat::sizeM, MatB::sizeN>> operator*(const RecTypeT<MatB> &other) const {

//		typedef Matrix<Mat::sizeM, MatB::sizeN> MatOut;
//		return this->operator*<MatB, MatOut>(other);
//	}

//	RecTypeT<Mat> operator*(const RecTypeT<Matrix<1,1>> &other) const {

//		typedef Matrix<1, 1> MatB;
//		typedef Mat MatOut;
//		return this->operator*<MatB, MatOut>(other);
//	}

//	RecTypeT<Mat> operator*(double scalar) const {

//		typedef Matrix<1, 1> MatB;
//		typedef Mat MatOut;
//		RecTypeT<MatB> other(scalar);
//		return this->operator*<MatB, MatOut>(other);
//	}

//	RecTypeT<Matrix<Mat::sizeN, Mat::sizeM>> transpose() const {
//		typedef Matrix<Mat::sizeN, Mat::sizeM> MatT;
//		Sp<const NodeMatrixM<MatT>> node(new NodeMatrixTranspose<Mat::sizeM, Mat::sizeN>(mNode));
//		return RecTypeT<MatT>(node);
//	}


	void addToGeneratorAsResult(CodeGenerator &generator, const std::string &resVarName) {
		NodeBase* nodeRes = new NodeMatrixOut<Mat>(resVarName, mNode);

		generator.collectNodes(nodeRes);
	}

private:
	std::shared_ptr<const NodeT> mNode;
};

//// TODO: is this needed??
//template<class Mat>
//RecTypeT<Mat> operator+(const RecTypeT<Matrix<1, 1>> &a, const RecTypeT<Mat> &b) {
//	return a+b;
//}

//template<class Mat>
//RecTypeT<Mat> operator+(double value, const RecTypeT<Mat> &b) {

//	typedef Mat MatA;
//	typedef Matrix<1, 1> MatB;
//	typedef Mat MatOut;

//	RecTypeT<MatB> a((Matrix<1,1>(value)));

//	return b+a;
//}

//template<class Mat>
//RecTypeT<Mat> operator-(double value, const RecTypeT<Mat> &b) {

//	typedef Mat MatA;
//	typedef Matrix<1, 1> MatB;
//	typedef Mat MatOut;

//	RecTypeT<MatB> a((Matrix<1,1>(value)));

//	return a-b;
//}

//template<class Mat>
//RecTypeT<Mat> operator*(const RecTypeT<Matrix<1, 1>> &a, const RecTypeT<Mat> &b) {
//	return b * a;
//}

//template<class Mat>
//RecTypeT<Mat> operator*(double value, const RecTypeT<Mat> &b) {

//	typedef Mat MatA;
//	typedef Matrix<1, 1> MatB;
//	typedef Mat MatOut;

//	RecTypeT<MatB> a((Matrix<1,1>(value)));

//	return b * a;
//}

} // namespace AutoGen
