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

	NodeMatrixVar<Matrix<1,2>> nMatrix("input");

	std::cout << "var type: " <<  nMatrix.getVarType() << std::endl;

	typedef AutoGen::RecTypeMatrix<Matrix<1, 2>> RMatrix;

	Matrix<1,2> mat;
	mat.data[0][0] = 1;

	std::cout << mat << std::endl;

	RMatrix y(mat);
	RMatrix x = y+y;

	CodeGenerator codeGenerator;
	x.addToGeneratorAsResult(codeGenerator, "x");
	codeGenerator.sortNodes();
	std:: cout << codeGenerator.generateCode() << std::endl;

//	nMatrix.generateCode(codeGenerator);

	return 0;
}
