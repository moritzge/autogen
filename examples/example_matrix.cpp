#include <iostream>
#include <chrono>

//#include <RecType.h>
#include <Eigen/Eigen>
#include <AutoDiff.h>
#include <RecTypeMatrix.h>
#include <NodeMatrix.h>

int main(int argc, char *argv[])
{

	using namespace AutoGen;

	NodeMatrixVar<Matrix<1,2>, Matrix<3,4>> nMatrix("input");

	std::cout << "var type: " <<  nMatrix.getVarType() << std::endl;

	typedef AutoGen::RecTypeMatrix<Matrix<1, 2>, Matrix<1, 2>> RMatrix;

	RMatrix y("y");
	RMatrix x = y;

	CodeGenerator codeGenerator;
	y.addToGeneratorAsResult(codeGenerator, "x");
	codeGenerator.sortNodes();
	std:: cout << codeGenerator.generateCode() << std::endl;

//	nMatrix.generateCode(codeGenerator);

	return 0;
}
