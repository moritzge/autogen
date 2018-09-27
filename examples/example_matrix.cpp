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

	RMat<2,3> y(std::string("y"));
	RMat<2,3> x = y + y + Matrix<2,3>(0);
	RMat<2,3> a = Matrix<2, 3>(1);
	
	// Addition
	RMat<2,3> z = a + x; 	// addition of same-sized matrix
	z = z + 1.0;			// addition of scalar, right
	z = 1.1 + z;			// addition of scalar, left

	// multiplication
	RMat<2,2> z1 = z * z.transpose();	// matrix matrix
	RMat<3,3> z2 = z.transpose() * z;	// matrix matrix
	z = z * 2.0;						// matrix scalar
	z = 3.0 * z;						// matrix scalar

{
	Matrix<1, 2> a(1.0);
	// Matrix<2, 2> b(1.0);
	Matrix<1, 2> c = 1.0 + a + 1.0;
	std::cout << c << std::endl;
}
	CodeGenerator codeGenerator;
	z1.addToGeneratorAsResult(codeGenerator, "z1");
	z2.addToGeneratorAsResult(codeGenerator, "z2");
	z.addToGeneratorAsResult(codeGenerator, "z");
	codeGenerator.sortNodes();
	std:: cout << codeGenerator.generateCode() << std::endl;

	return 0;
}
