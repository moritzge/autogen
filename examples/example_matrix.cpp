#include <iostream>
#include <chrono>

//#include <RecType.h>
#include <Eigen/Eigen>
#include <AutoDiff.h>
#include <RecTypeMatrix.h>
#include <NodeMatrix.h>

template <int M, int N> using RMat = AutoGen::RecTypeMatrix<Matrix<M, N>>;

int main(int argc, char *argv[])
{

	using namespace AutoGen;

	NodeMatrixVar<Matrix<1,2>> nMatrix("input");

	RMat<1,3> y(std::string("y"));
	RMat<1,3> x = y + y + Matrix<1,3>(0);
	RMat<1,3> a = Matrix<1, 3>(1);
	RMat<1,3> z = a * 1.0 + x;
	z = 3.0 * z;
	z = z * 2.5;

	CodeGenerator codeGenerator;
	z.addToGeneratorAsResult(codeGenerator, "z");
	codeGenerator.sortNodes();
	std:: cout << codeGenerator.generateCode() << std::endl;

//	nMatrix.generateCode(codeGenerator);

	return 0;
}
