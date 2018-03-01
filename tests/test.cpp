#include <gtest/gtest.h>
#include <iostream>
#include <fstream>

#include <AutoDiff.h>
#include <CodeGenerator.h>

#include "execCmd.h"
#include <dlfcn.h>

// the types of the class factories
typedef double compute_y(double);

void makeLibrary(std::string code, std::string libName) {
	std::ofstream out(libName + ".cpp");
	out << code;
	out.close();

	std::string res = exec("g++ -fPIC -shared -o"+libName+".so "+libName+".cpp");
}

compute_y* loadLibrary(std::string libName) {
	// load the library
	void* libCompute = dlopen(("./"+ libName + ".so").c_str(), RTLD_LAZY);
	if (!libCompute) {
		std::cerr << "Cannot load library: " << dlerror() << '\n';
	}

	// reset errors
	dlerror();

	// load the symbols
	compute_y* comp_y = (compute_y*) dlsym(libCompute, "compute_y");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
	}

	return comp_y;
}

template<class T>
T computeSomething(T a) {
	T b = 3;
	T c = 4;
	return pow(a,2.0)*b + c;
}

TEST(CodeGenTest, GenerateCodeAndLoadLib) {
	using namespace CodeGen;

	// record computation
	RecType<double> y = computeSomething(RecType<double>("a"));

	// generate the code
	std::string code = y.generateCode("y");

	// and wrap it in a function
	std::string libCode = "#include <cmath>\nextern \"C\" double compute_y(double a) {\ndouble y;\n";
	libCode += code;
	libCode += "return y;\n}\n";

	// make and load library
	std::string libName = "compute_y";
	makeLibrary(libCode, libName);
	compute_y* comp_y = loadLibrary(libName);

	// test it!
	double x = 0.123;
	double res = comp_y(x);
	EXPECT_EQ(res, computeSomething(x));
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
