#include <iostream>
#include <chrono>

#include <RecType.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef std::chrono::high_resolution_clock Clock;

using namespace AutoGen;
using namespace Eigen;

typedef RecType<double> R;

double computeCG(const Vector3d &a) {

	double v0 = a(0);
	double v1 = a(1);
	double v2 = a(2);
	double v3 = v0 * v0;
	double v4 = v1 * v1;
	double v5 = v2 * v2;
	double v6 = v4 + v5;
	double v7 = v3 + v6;
	double v8 = sqrt(v7);
	double v9 = 3.000000;
	double v10 = v1 * v9;
	double v11 = 4.000000;
	double v12 = v2 * v11;
	double v13 = v10 - v12;
	double v14 = v13 * v13;
	double v15 = 32.000000;
	double v16 = v2 * v15;
	double v17 = v0 * v9;
	double v18 = v16 - v17;
	double v19 = v18 * v18;
	double v20 = v0 * v11;
	double v21 = v1 * v15;
	double v22 = v20 - v21;
	double v23 = v22 * v22;
	double v24 = v19 + v23;
	double v25 = v14 + v24;
	double v26 = sqrt(v25);
	double v27 = v8 + v26;
	return v27;
}

template<class S>
S compute(const Vector3<S> &a) {
	Vector3<S> b;
	b << 32, 4, 3;
	return a.norm() + (a.cross(b)).norm();
}

double computeByHand(const Vector3d &a) {
	return compute(a);
}

void generateCodeGradient() {

	Vector3<R> a;
	for (int i = 0; i < 3; ++i) {
		a(i) = R("a(" + std::to_string(i) + ")");
	}

	R r = compute(a);

	r.printCode();
}

int main(int argc, char *argv[])
{

	generateCodeGradient();

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
