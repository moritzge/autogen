#include <iostream>
#include <chrono>

#include <CodeGenerator.h>

typedef std::chrono::high_resolution_clock Clock;

double computeCG(double a) {
	double v0 = 1.000000;
	double v1 = a;
	double v2 = v1 * v1;
	double v3 = v0 * v2;
	double v4 = v0 * v1;
	double v5 = v4 + v4;
	double v6 = v5 * v1;
	double v7 = v3 + v6;
	double v8 = 0.000000;
	double v9 = v8 * v1;
	double v10 = 2.000000;
	double v11 = v0 * v10;
	double v12 = v9 + v11;
	double v13 = v7 + v12;
	double v14 = v2 * v1;
	double v15 = v1 * v10;
	double v16 = v14 + v15;
	double v17 = v13 * v16;
	double v18 = v17 + v17;

	return v18;
}

double computeByHand(double a) {
	return 2.*(a*a*a + 2.*a)*(3.*a*a + 2.);
}

int main(int argc, char *argv[])
{
//	using namespace AutoGen;

//	typedef RecType<double> R;

//	R a("a");
//	R b("b");

//	R c = a*b;

//	c.printCode();

	std::cout << "# args:" << argc <<std::endl;

	double a = atof(argv[1]);

	std::cout << "a: " << a << std::endl;

	int n = 1e9;

	auto t1 = Clock::now();
	double sum = 0;
	for (int i = 0; i < n; ++i) {
		sum += computeCG(a);
	}
	std::cout << "sum = " << sum << std::endl;

	auto t2 = Clock::now();
	sum = 0;
	for (int i = 0; i < n; ++i) {
		sum += computeByHand(a);
	}
	std::cout << "sum = " << sum << std::endl;

	auto t3 = Clock::now();

	std::cout << "Delta t2-t1: "
			  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
			  << " milliseconds" << std::endl;
	std::cout << "Delta t3-t2: "
			  << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()
			  << " milliseconds" << std::endl;

	return 0;
}
