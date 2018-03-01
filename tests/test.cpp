#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

#include <AutoDiff.h>
#include <CodeGenerator.h>

#include <Eigen/Eigen>

#include "execCmd.h"
#include <dlfcn.h>


////////////////////////////////////////////////////////////////////////// helper functions for building and loading library

// general function for code generator that is loaded during runtime
//                            in     out
typedef void compute_extern(double*,double*);

void buildLibrary(std::string code, std::string libName) {

	// make dir
	exec("mkdir -p " + libName);

	// create cpp file
	std::ofstream cppFile(libName+"/"+libName+".cpp");
	cppFile << code;
	cppFile.close();

	// create CMakeLists.txt
	std::ofstream cmakeFile(libName+"/CMakeLists.txt");
	cmakeFile <<
				 "cmake_minimum_required(VERSION 3.5 FATAL_ERROR)\n"
				 "project(" << libName << ")\n"
				 "file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.cpp)\n"
				 "add_library(${PROJECT_NAME} SHARED ${sources})";
	cmakeFile.close();

//	std::string res = exec("g++ -fPIC -shared -o"+libName+"/"+libName+".so "+libName+"/"+libName+".cpp");
	exec("cmake "+libName+"/CMakeLists.txt");
	exec("cmake --build "+libName);

}

compute_extern* loadLibrary(std::string libName) {
	// load the library
	void* libCompute = dlopen((libName+"/lib"+ libName + ".so").c_str(), RTLD_LAZY);
	if (!libCompute) {
		std::cerr << "Cannot load library: " << dlerror() << '\n';
	}

	// reset errors
	dlerror();

	// load the symbols
	compute_extern* comp_y = (compute_extern*) dlsym(libCompute, "compute_extern");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
	}

	return comp_y;
}


////////////////////////////////////////////////////////////////////////// TEST 1

template<class T>
T computeScalar(T a) {
	T b = 3;
	T c = 4;
	return pow(a,5.0)*b + c*sqrt(a);
}

TEST(GenerateCodeAndLoadLib, ScalarCompute) {
	using namespace CodeGen;

	// record computation
	RecType<double> y = computeScalar(RecType<double>("x[0]"));

	// generate the code
	std::string code = y.generateCode("y[0]");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
	libCode += code;
	libCode += "}\n";

	// make and load library
	std::string libName = "compute_y";
	buildLibrary(libCode, libName);
	compute_extern* comp_y = loadLibrary(libName);

	// test it!
	{
		double x[1]; x[0] = 0.124;
		double y[1];
		comp_y(x, y);
		EXPECT_EQ(y[0], computeScalar(x[0]));
	}
}

////////////////////////////////////////////////////////////////////////// TEST 2

template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;

template<class T>
T computeDotProduct(Vector3<T> &a) {
	return a.dot(a);
}

TEST(GenerateCodeAndLoadLib, DotProduct) {
	using namespace CodeGen;
	typedef RecType<double> R;

	// record computation
	Vector3<R> x;
	for (int i = 0; i < 3; ++i) {
		x[i] = R("x[" + std::to_string(i) + "]");
	}
	R y = computeDotProduct(x);

	// generate the code
	std::string code = y.generateCode("y[0]");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
	libCode += code;
	libCode += "}\n";

	// make and load library
	std::string libName = "compute_somethingElse";
	buildLibrary(libCode, libName);
	compute_extern* compute = loadLibrary(libName);

	// test it!
	{
		Eigen::Vector3d x;
		x << 0.1243, 1.34, 5.67;
		double y[1];
		compute(x.data(), y);
		EXPECT_EQ(y[0], computeDotProduct(x));
	}
}

////////////////////////////////////////////////////////////////////////// TEST 3

template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;

template<class T>
T computeDotAndCrossNorm(const Vector3<T> &a, const Vector3<T> &b) {
	return a.dot(b) + a.cross(b).norm();
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

TEST(GenerateCodeAndLoadLib, Gradient) {
	using namespace CodeGen;
	typedef RecType<double> R;
	typedef AutoDiff<R, R> AD;

	// record computation
	Vector3<AD> a, b;
	for (int i = 0; i < 3; ++i) {
		a[i] = R("x[" + std::to_string(i) + "]");
		b[i] = R("x[" + std::to_string(3+i) + "]");
	}

	Vector3<R> grad;
	for (int i = 0; i < 3; ++i) {
		a(i).deriv() = 1.0;
		AD y = computeDotAndCrossNorm(a, b);
		grad(i) = y.deriv();
		a(i).deriv() = 0.0;
	}

	CodeGenerator<double> generator;
	for (int i = 0; i < 3; ++i) {
		grad(i).addToGeneratorAsResult(generator, "y[" + std::to_string(i) + "]");
	}
	generator.sortNodes();
	std::string code = generator.generateCode();

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
	libCode += code;
	libCode += "}\n";

	// make and load library
	std::string libName = "compute_gradient";
	buildLibrary(libCode, libName);
	compute_extern* compute_CG = loadLibrary(libName);

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
		Vector3<double> grad_CG;
		compute_CG(x, grad_CG.data());

		// compute with FD
		Vector3<double> grad_FD;
		computeGradientFD(a, b, grad_FD);

		EXPECT_NEAR(grad_CG.norm(), grad_FD.norm(), 1e-5);
	}
}


//TEST(CodeGenTest, SimpleTest) {
//		using namespace CodeGen;

////	RecType<double> v1("a");
//	RecType<double> v1 = 0.24;
//	RecType<double> v0 = 1.4;
//	RecType<double> v2 = v0-v1;
//	v2 = v0+(v2+v1)*2.0;
//	v2 /= 2.0;
//	v0 *= 3.0;
//	v2 += v0;

//	std::cout << v2.getNode()->evaluate() << std::endl;

//	v2.printCode();

//	{
//			double a = 0.24;

////		double v0 = 1.400000;
////		double v1 = a;
////		double v2 = v0 - v1;
////		double v3 = v2 + v1;
////		double v4 = 2.000000;
////		double v5 = v3 * v4;
////		double v6 = v0 + v5;
////		double v7 = v6 / v4;
////		double v8 = 4.200000;
////		double v9 = v7 + v8;

//		double v5 = 2.000000;
//		double v7 = 1.400000;
//		double v8 = 3.000000;
//		double v1 = a;
//		double v0 = v7 - v1;
//		double v2 = v0 + v1;
//		double v3 = v2 * v5;
//		double v4 = v7 + v3;
//		double v6 = v4 / v5;
//		double v9 = v7 * v8;
//		double v10 = v6 + v9;

//		std::cout << v10 << std::endl;
//	}
//}

//TEST(CodeGenTest, SimpleTest2) {
//	using namespace CodeGen;

//	RecType<double> a("a");
//	RecType<double> b("b");

//	RecType<double> c = a*b;
//	RecType<double> d = a*b;
//	RecType<double> e = c+d;

////	std::cout << v2.getNode()->evaluate() << std::endl;

//	e.printCode();

//	{
//	}
//}



///*****************
//TODO:

//- orphaned nodes do not get removed
//- messy code in addTo in code gen
//- check hash computation

//*****************/

//TEST(CodeGenTest, AutoDiff) {
//	using namespace CodeGen;

//	typedef AutoDiffT<RecType<double>, RecType<double>> AD;

//	AD a("a");
//	a.deriv() = 1.0;
//	AD b = a*a*a + a*2.0;
//	AD c = b*b;

//	c.deriv().printCode();

////	std::cout << c.deriv().getNode()->evaluate() << std::endl;

//	{
//		double a = 3;
//		std::cout << "dc/da = " << 2.*(pow(a,3) + 2.*a)*(3.*pow(a,2) + 2) << std::endl;
//	}

//	{
//		double a = 3;

//		double v0 = 1.000000;
//		double v1 = a;
//		double v2 = v1 * v1;
//		double v3 = v0 * v2;
//		double v4 = v0 * v1;
//		double v5 = v4 + v4;
//		double v6 = v5 * v1;
//		double v7 = v3 + v6;
//		double v8 = 0.000000;
//		double v9 = v8 * v1;
//		double v10 = 2.000000;
//		double v11 = v0 * v10;
//		double v12 = v9 + v11;
//		double v13 = v7 + v12;
//		double v14 = v2 * v1;
//		double v15 = v1 * v10;
//		double v16 = v14 + v15;
//		double v17 = v13 * v16;
//		double v18 = v17 + v17;

//		std::cout << v18 << std::endl;
//	}
//}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
