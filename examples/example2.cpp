#include <iostream>
#include <chrono>

#include <CodeGenerator.h>
#include <Eigen/Core>

typedef std::chrono::high_resolution_clock Clock;

using namespace CodeGen;
using namespace Eigen;

typedef RecType<double> R;
typedef Matrix<R, 3, 1> Vector3R;

double computeCG(const Vector3d &a) {

	double v0 = a(0);
	double v1 = v0 * v0;
	double v2 = a(1);
	double v3 = v2 * v2;
	double v4 = a(2);
	double v5 = v4 * v4;
	double v6 = v3 + v5;
	double v7 = v1 + v6;
	double v8 = sqrt(v7);
	double v9 = v7 + v8;
	double v10 = 3.000000;
	double v11 = v0 * v10;
	double v12 = 4.000000;
	double v13 = v2 * v12;
	double v14 = 5.000000;
	double v15 = v4 * v14;
	double v16 = v13 + v15;
	double v17 = v11 + v16;
	double v18 = v9 + v17;
	double v19 = v10 * v10;
	double v20 = v12 * v12;
	double v21 = v14 * v14;
	double v22 = v20 + v21;
	double v23 = v19 + v22;
	double v24 = sqrt(v23);
	double v25 = v18 + v24;

	return v25;
}

double computeByHand(const Vector3d &a) {
	Vector3d b;
	b << 3, 4, 5;

	return a.dot(a) + a.norm() + a.dot(b) + b.norm();
}

void generateCode() {

	Vector3R a;
	a(0) = R("a0");
	a(1) = R("a1");
	a(2) = R("a2");

	Vector3R b;
	b << 3, 4, 5;

	R n = a.dot(a) + a.norm() + a.dot(b) + b.norm();

	n.printCode();
}

int main(int argc, char *argv[])
{

	generateCode();

	std::cout << "# args:" << argc <<std::endl;

	Vector3d a;
	a << atof(argv[1]), atof(argv[2]), atof(argv[3]);

	std::cout << "a: " << a.transpose() << std::endl;

	int n = 1e8;

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
