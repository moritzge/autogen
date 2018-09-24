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

	std::cout << "var type: " <<  nMatrix.getVarType() << std::endl;


	Matrix<1,2> mat;
	mat.data[0][0] = 1;

	std::cout << mat << std::endl;

	RMat<1,2> y(std::string("y"));
	RMat<1,2> x = y + y + Matrix<1,2>(0);
	RMat<1,3> z = x * Matrix<2, 3>(1);

	CodeGenerator codeGenerator;
	z.addToGeneratorAsResult(codeGenerator, "z");
	codeGenerator.sortNodes();
	std:: cout << codeGenerator.generateCode() << std::endl;

//	nMatrix.generateCode(codeGenerator);

	return 0;
}
