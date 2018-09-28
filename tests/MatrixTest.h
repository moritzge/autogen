#include <gtest/gtest.h>

#include <Matrix.h>

#include <AutoLoad.h>

#include <RecTypeMatrix.h>

TEST(Matrix, MatrixClass) {
	using namespace AutoGen;

	const static int M = 1;
	const static int N = 2;

	Matrix<M, N> a;
	Matrix<N, M> aTranspose;

	// assignement
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j){
			a(i, j) = (double)N*i + j;
			aTranspose(j, i) = (double)N*i + j;
		}
	}
	Matrix<N, M> b = (a + a).transpose();
	Matrix<N, M> c = (a + 2).transpose();

//	// to stream
//	std::cout << "a = " << a << std::endl;
//	std::cout << "a^T = " << a.transpose() << std::endl;
//	std::cout << "b = " << b << std::endl;
//	std::cout << "b^T = " << b.transpose() << std::endl;
//	std::cout << "a*b = " << a*b << std::endl;

	EXPECT_TRUE(a+a == a*2);
	EXPECT_TRUE(2.0*a == a*2);
	EXPECT_TRUE(2.0+a == a+2);
	EXPECT_TRUE(2*a == a + a);
	EXPECT_TRUE(2*a + a == a*3);

	EXPECT_TRUE(a.transpose() == aTranspose);
	EXPECT_TRUE(a*a.transpose() == (a*a.transpose()).transpose());
	EXPECT_TRUE(a.transpose()*a == (a.transpose()*a).transpose());
	EXPECT_TRUE(a*(b+c) == a*b + a*c);
	EXPECT_TRUE((b+c).transpose() == b.transpose() + c.transpose());
	EXPECT_EQ((a*b).transpose(), b.transpose()*a.transpose());
}

template<class MatIn, class MatOut>
MatOut compute_stuff(const MatIn &x) {
	return (x + x).transpose();
}

template <int M, int N> using RMat = AutoGen::RecTypeMatrix<AutoGen::Matrix<M, N>>;

TEST(RecTypeMatrix, SuperSimpleTest) {
	using namespace AutoGen;

	const static int M = 2;
	const static int N = 3;

	// record computation
	RMat<M, N> x("x");
	RMat<N, M> y = compute_stuff<RMat<M,N>, RMat<N,M>>(x);

	// generate the code
	CodeGenerator generator;
//	addToGeneratorAsResult(y, generator, "y");
	y.addToGeneratorAsResult(generator, "y");

	generator.sortNodes();
	std::string code = generator.generateCode("compute_extern");

	// and wrap it in a function
	std::string libCode =
			"#include <cmath>\n"
			"#include <Matrix.h>\n"
			"using namespace AutoGen;\n"
			"extern \"C\" " + code;

	// build and load library
	std::string error;
	compute_extern* compute_y;
	EXPECT_TRUE(buildAndLoad(libCode, compute_y, "compute_y", "compute_extern", error));

	// test it!
	{
		Matrix<N, M> y;
		Matrix<M, N> x;
		compute_y(x.data[0], y.data[0]);
		Matrix<N, M> y2 = compute_stuff<Matrix<M,N>, Matrix<N,M>>(x);
		EXPECT_EQ(y, y2);
	}
}
