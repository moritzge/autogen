#include <iostream>
#include <chrono>

#include <RecTypeT.h>
#include <NodeMatrixT.h>

template <class Mat, class NodeT> using RMat = AutoGen::RecTypeT<Mat, NodeT>;

int main(int argc, char *argv[])
{

	using namespace AutoGen;

//	typedef Matrix<2,3> Mat;
//	RMat<Mat,NodeMatrixVar<Mat>> y(std::string("y"));
//	auto x = y + y;

//	CodeGenerator codeGenerator;
////	z1.addToGeneratorAsResult(codeGenerator, "z1");
////	z2.addToGeneratorAsResult(codeGenerator, "z2");
////	z.addToGeneratorAsResult(codeGenerator, "z");
//	x.addToGeneratorAsResult(codeGenerator, "x");
//	codeGenerator.sortNodes();
//	std:: cout << codeGenerator.generateCode() << std::endl;

	typedef Matrix<2,3> Mat;

	Sp<const NodeMatrixTVar<Mat>> y( new NodeMatrixTVar<Mat>("y"));
	Sp<const NodeMatrixTOut<NodeMatrixTVar<Mat>, Mat>> z(new NodeMatrixTOut<NodeMatrixTVar<Mat>, Mat>("z", y));

	return 0;
}
