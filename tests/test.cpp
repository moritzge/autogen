#include <gtest/gtest.h>
#include <iostream>

#include <AutoDiff.h>
#include <CodeGenerator.h>

TEST(CodeGenTest, BasicTest) {
		using namespace CodeGen;

		RecType<double> a("a");
	RecType<double> b = 1;
	RecType<double> c = 1;
	RecType<double> y = a+b + c;

//	std::cout << v2.getNode()->evaluate() << std::endl;

	y.printCode();

}


TEST(CodeGenTest, SimpleTest) {
		using namespace CodeGen;

//	RecType<double> v1("a");
		RecType<double> v1 = 0.24;
	RecType<double> v0 = 1.4;
	RecType<double> v2 = v0-v1;
	v2 = v0+(v2+v1)*2.0;
	v2 /= 2.0;
	v0 *= 3.0;
	v2 += v0;

	std::cout << v2.getNode()->evaluate() << std::endl;

	v2.printCode();

	{
			double a = 0.24;

//		double v0 = 1.400000;
//		double v1 = a;
//		double v2 = v0 - v1;
//		double v3 = v2 + v1;
//		double v4 = 2.000000;
//		double v5 = v3 * v4;
//		double v6 = v0 + v5;
//		double v7 = v6 / v4;
//		double v8 = 4.200000;
//		double v9 = v7 + v8;

		double v5 = 2.000000;
		double v7 = 1.400000;
		double v8 = 3.000000;
		double v1 = a;
		double v0 = v7 - v1;
		double v2 = v0 + v1;
		double v3 = v2 * v5;
		double v4 = v7 + v3;
		double v6 = v4 / v5;
		double v9 = v7 * v8;
		double v10 = v6 + v9;

		std::cout << v10 << std::endl;
	}
}


/*****************
TODO:

- orphaned nodes do not get removed
- messy code in addTo in code gen
- check hash computation

*****************/

TEST(CodeGenTest, AutoDiff) {
		using namespace CodeGen;

		typedef AutoDiffT<RecType<double>, RecType<double>> AD;

		AD a("a");
	a.deriv() = 1.0;
	AD b = a*a*a + a*2.0;
	AD c = b*b;

	c.deriv().printCode();

//	std::cout << c.deriv().getNode()->evaluate() << std::endl;

	{
			double a = 3;
		std::cout << "dc/da = " << 2.*(pow(a,3) + 2.*a)*(3.*pow(a,2) + 2) << std::endl;
	}

//	{
//		double a = 3;

//		double v6 = 0.000000;
//		double v8 = 1.000000;
//		double v15 = 2.000000;
//		double v11 = v5 + v10;
//		double v13 = v12 * v14;
//		double v16 = v14 * v15;
//		double v17 = v13 + v16;
//		double v0 = v11 * v17;
//		double v1 = v8 * v12;
//		double v12 = v14 * v14;
//		double v2 = v8 * v14;
//		double v3 = v2 + v2;
//		double v4 = v3 * v14;
//		double v5 = v1 + v4;
//		double v7 = v6 * v14;
//		double v9 = v8 * v15;
//		double v10 = v7 + v9;
//		double v14 = a;
//		double v18 = v11 * v17;
//		double v19 = v0 + v18;

//		std::cout << v18 << std::endl;
//	}
}
