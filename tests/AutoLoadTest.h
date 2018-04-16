#include <gtest/gtest.h>

#include <AutoDiff.h>
#include <RecType.h>
#include <AutoLoad.h>
#include <CodeGenerator.h>

template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;

////////////////////////////////////////////////////////////////////////// TEST 0

/*
 * Testing: AutoLoad
 * Test AutoLoad with a very simple test.
 * The test is a bit pointless, but shows how to use AutoLoad.
 */

TEST(GenerateCodeAndLoadLib, SuperSimpleTest) {
	using namespace AutoGen;

	// write some code to a string
	std::string code = "y[0] = x[0]*x[0] + 2.0;\n";

	// and wrap it in a function
	std::string libCode =
			"extern \"C\" void compute_extern(double *x, double *y) {\n"
			+ code
			+ "}\n";

	// build and load library
	std::string error;
	compute_extern* compute;
	EXPECT_TRUE(buildAndLoad(libCode, compute, error));

	// test it!
	{
		double x[1]; x[0] = 0.124;
		double y[1];
		compute(x, y);
		EXPECT_EQ(y[0], x[0]*x[0] + 2.0);
	}
}

////////////////////////////////////////////////////////////////////////// TEST 1

/*
 * Testing: CodeGenerator & AutoLoad
 * This test uses CodeGenerator to generate code from a c++ function, and then
 * builds the code and loads the resulting library.
 *
 */

template<class T>
T computeScalar(T a) {
	T b = 3;
	T c = 4;
	return pow(a,5.0)*b + c*sqrt(a);
}

TEST(GenerateCodeAndLoadLib, ScalarCompute) {
	using namespace AutoGen;

	// record computation
	RecTypeVec<double> x("x", 1);
	Eigen::Matrix<RecType<double>, 1, -1> y(1);
	y[0] = computeScalar(x[0]);

	// generate the code
	CodeGenerator<double> generator;
	addToGeneratorAsResult(y, generator, "y");

	generator.sortNodes();
	std::string code = generator.generateCode("compute_extern");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" " + code;
	std::cout << libCode << std::endl;

	// build and load library
	std::string error;
	compute_extern* compute_y;
	EXPECT_TRUE(buildAndLoad(libCode, compute_y, "compute_y", error));

	// test it!
	{
		double x[1]; x[0] = 0.124;
		double y[1];
		compute_y(x, y);
		EXPECT_EQ(y[0], computeScalar(x[0]));
	}
}

////////////////////////////////////////////////////////////////////////// TEST 2

/*
 * Testing: Eigen & CodeGenerator & AutoLoad
 * This test generates, builds and loads code that uses Eigen.s
 */

template<class T>
T computeDotProduct(Vector3<T> &a) {
	return a.dot(a);
}

TEST(GenerateCodeAndLoadLib, DotProduct) {
	using namespace AutoGen;
	typedef RecType<double> R;

	// record computation
	RecTypeVec<double> x("x", 3);
	Vector3<R> xv; xv << x[0], x[1], x[2];
	Eigen::Matrix<RecType<double>, 1, -1> y(1);
	y[0] = computeDotProduct(xv);

	// generate the code
	CodeGenerator<double> generator;
	addToGeneratorAsResult(y, generator, "y");
	generator.sortNodes();
	std::string code = generator.generateCode("compute_extern");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" " + code;
	std::cout << libCode << std::endl;

	// build and load library
	std::string error;
	compute_extern* compute;
	EXPECT_TRUE(buildAndLoad(libCode, compute, "computeDotProduct", error));

	// test it!
	{
		Vector3<double> x;
		x << 0.1243, 1.34, 5.67;
		double y[1];
		compute(x.data(), y);
		EXPECT_EQ(y[0], computeDotProduct(x));
	}
}

////////////////////////////////////////////////////////////////////////// TEST 3

/*
 * Testing: AutoDiff & CodeGenerator & AutoLoad
 */

template <class S> using Matrix3 = Eigen::Matrix<S, 3, 3>;

template<class T>
T computeDotAndCrossNorm(const Vector3<T> &a, const Vector3<T> &b) {
	return a.cross(b).norm();
//	return a.dot(a);
}

void computeGradientFD(const Vector3<double> &a, const Vector3<double> &b, Vector3<double> &grad) {

	double h = 1e-5;

	for (int i = 0; i < 3; ++i) {
		Vector3<double> ap = a;
		ap(i) += h;
		Vector3<double> am = a;
		am(i) -= h;

		double ep = computeDotAndCrossNorm(ap, b);
		double em = computeDotAndCrossNorm(am, b);

		grad(i) = (ep-em) / (2.*h);
	}
}

void testCompute(double *x, double *y) {
	double v1 = 1.000000;
	double v2 = 2.000000;
	double v3 = v1 / v2;
	double v4 = v3 * v1;
	double v5 = x[1];
	double v6 = x[5];
	double v7 = v5 * v6;
	double v8 = v7 - v7;
	double v9 = v8 * v8;
	double v10 = x[2];
	double v11 = x[3];
	double v12 = v10 * v11;
	double v13 = v12 - v12;
	double v14 = v13 * v13;
	double v15 = x[0];
	double v16 = x[4];
	double v17 = v15 * v16;
	double v18 = v17 - v17;
	double v19 = v18 * v18;
	double v20 = v14 + v19;
	double v21 = v9 + v20;
	double v22 = sqrt(v21);
	double v23 = v4 / v22;
	double v24 = 0.000000;
	double v25 = v24 * v5;
	double v26 = v24 * v6;
	double v27 = v25 + v26;
	double v28 = v27 - v27;
	double v29 = v28 * v8;
	double v30 = v29 + v29;
	double v31 = v24 * v10;
	double v32 = v24 * v11;
	double v33 = v31 + v32;
	double v34 = v24 * v15;
	double v35 = v1 * v6;
	double v36 = v34 + v35;
	double v37 = v33 - v36;
	double v38 = v37 * v13;
	double v39 = v38 + v38;
	double v40 = v1 * v16;
	double v41 = v34 + v40;
	double v42 = v25 + v32;
	double v43 = v41 - v42;
	double v44 = v43 * v18;
	double v45 = v44 + v44;
	double v46 = v39 + v45;
	double v47 = v30 + v46;
	double v48 = v23 * v47;
	double v49 = v25 + v35;
	double v50 = v49 - v27;
	double v51 = v50 * v8;
	double v52 = v51 + v51;
	double v53 = v33 - v33;
	double v54 = v53 * v13;
	double v55 = v54 + v54;
	double v56 = v42 - v41;
	double v57 = v56 * v18;
	double v58 = v57 + v57;
	double v59 = v55 + v58;
	double v60 = v52 + v59;
	double v61 = v23 * v60;
	double v62 = v27 - v49;
	double v63 = v62 * v8;
	double v64 = v63 + v63;
	double v65 = v36 - v33;
	double v66 = v65 * v13;
	double v67 = v66 + v66;
	double v68 = v42 - v42;
	double v69 = v68 * v18;
	double v70 = v69 + v69;
	double v71 = v67 + v70;
	double v72 = v64 + v71;
	double v73 = v23 * v72;
	y[0] = v48;
	y[1] = v61;
	y[2] = v73;
}

TEST(GenerateCodeAndLoadLib, Gradient) {
	using namespace AutoGen;
	typedef RecType<double> R;
	typedef AutoDiff<R, R> ADR;

	// record computation
	RecTypeVec<double> x("x", 6);
	Vector3<ADR> a; a << x[0], x[1], x[2];
	Vector3<ADR> b; b << x[3], x[4], x[5];

	CodeGenerator<double> generator;
//	Eigen::Matrix<RecType<double>, 1, -1> grad(3);
	Vector3<R> grad;
	for (int i = 0; i < 3; ++i) {
		a(i).deriv() = 1.0;
		ADR y = computeDotAndCrossNorm(a, b);
		grad(i) = y.deriv();
		grad(i).addToGeneratorAsResult(generator, "y(" + std::to_string(i) + ")");
		a(i).deriv() = 0.0;
	}

//	// generate the code
//	CodeGenerator<double> generator;
//	addToGeneratorAsResult(grad, generator, "y");
	generator.sortNodes();
	std::string code = generator.generateCode("compute_extern");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" " + code;
	std::cout << libCode << std::endl;

	// make and load library
	std::string error;
	compute_extern* compute_CG;
	EXPECT_TRUE(buildAndLoad(libCode, compute_CG, "compute_grad", error));

	// test it!
	{
		Vector3<double> a;
		a << 1, 0, 0;
		Vector3<double> b;
		b << 0, 1, 0;
		double x[6];
		for (int i = 0; i < 3; ++i) {
			x[i] = a(i);
			x[3+i] = b(i);
		}

		// compute with CG
		Vector3<double> grad_CG;
		testCompute(x, grad_CG.data());

		// compute with FD
		Vector3<double> grad_FD;
		computeGradientFD(a, b, grad_FD);

		EXPECT_NEAR(grad_CG.norm(), grad_FD.norm(), 1e-5);
	}
}

////////////////////////////////////////////////////////////////////////// TEST 4

/*
 * Testing: AutoDiff & CodeGenerator & AutoLoad
 * Generate code for gradient and hessian
 */


void computeHessianFD(const Vector3<double> &a, const Vector3<double> &b, Matrix3<double> &hess) {

	double h = 1e-5;

	for (int i = 0; i < 3; ++i) {
		Vector3<double> ap = a;
		ap(i) += h;
		Vector3<double> am = a;
		am(i) -= h;

		Vector3<double> gradp;
		computeGradientFD(ap, b, gradp);
		Vector3<double> gradm;
		computeGradientFD(am, b, gradm);

		for (int j = 0; j < 3; ++j) {
			hess(i,j) = (gradp(j) - gradm(j)) / (2.*h);
		}
	}
}

TEST(GenerateCodeAndLoadLib, GradientAndHessian) {
	using namespace AutoGen;
	typedef RecType<double> R;
	typedef AutoDiff<R, R> AD;
	typedef AutoDiff<AD, AD> ADD;

	// record computation
	Vector3<ADD> a, b;
	for (int i = 0; i < 3; ++i) {
		a[i] = R("x[" + std::to_string(i) + "]");
		b[i] = R("x[" + std::to_string(3+i) + "]");
	}

	CodeGenerator<double> generator;

	// compute gradient and add to code gen
	{
		Vector3<R> grad;
		for (int i = 0; i < 3; ++i) {
			a(i).deriv() = 1.0;
			ADD y = computeDotAndCrossNorm(a, b);
			grad(i) = y.deriv().value();
			a(i).deriv() = 0.0;
		}
		for (int i = 0; i < 3; ++i) {
			grad(i).addToGeneratorAsResult(generator, "y[" + std::to_string(i) + "]");
		}
	}

	//compute hessian and add to code gen
	{
		Matrix3<R> hess;
		for (int i = 0; i < 3; ++i) {
			a(i).deriv() = 1.0;
			for (int j = 0; j < 3; ++j) {
				a(j).value().deriv() = 1.0;
				ADD y = computeDotAndCrossNorm(a, b);
				hess(i,j) = y.deriv().deriv();
				a(j).value().deriv() = 0.0;
			}
			a(i).deriv() = 0.0;
		}

		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j)
				hess(i,j).addToGeneratorAsResult(generator, "y[" + std::to_string(3+3*i+j) + "]");
		}
	}


	generator.sortNodes();
	std::string code = generator.generateCode();

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
	libCode += code;
	libCode += "}\n";

	// make and load library
	std::string error;
	compute_extern* compute_CG;
	EXPECT_TRUE(buildAndLoad(libCode, compute_CG, "compute_gradAndHess", error));

	// test it!
	{
		Vector3<double> a;
		a << 1.2, 3.4, 5.5;
		Vector3<double> b;
		b << 4.2, -0.1, 1.8;
		double x[6];
		for (int i = 0; i < 3; ++i) {
			x[i] = a(i);
			x[3+i] = b(i);
		}

		// compute with CG
		double y[12];
		compute_CG(x, y);
		Vector3<double> grad_CG;
		for (int i = 0; i < 3; ++i) {
			grad_CG(i) = y[i];
		}
		Matrix3<double> hess_CG;
		for (int i = 0; i < 9; ++i) {
			hess_CG.data()[i] = y[3+i];
		}

		// compute with FD
		Vector3<double> grad_FD;
		computeGradientFD(a, b, grad_FD);
		Matrix3<double> hess_FD;
		computeHessianFD(a, b, hess_FD);

		EXPECT_NEAR(grad_CG.norm(), grad_FD.norm(), 1e-5);
		EXPECT_NEAR(hess_CG.norm(), hess_FD.norm(), 1e-3);
	}
}
