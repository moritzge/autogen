#include <iostream>
#include <chrono>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <Eigen/Core>

typedef std::chrono::high_resolution_clock Clock;

using namespace CodeGen;
using namespace Eigen;

typedef RecType<double> R;
typedef AutoDiffT<R, R> ADRec;
typedef AutoDiffT<ADRec, ADRec> ADDRec;
template <class S> using Vector3 = Matrix<S, 3, 1>;
template <class S> using Matrix3 = Matrix<S, 3, 3>;

double computeCG(const Vector3d &a) {

	double v0 = a(0);
	double v1 = 3.000000;
	double v2 = v0 + v1;
	double v3 = v2 + v2;
	double v4 = v2 * v2;
	double v5 = a(1);
	double v6 = 4.000000;
	double v7 = v5 + v6;
	double v8 = v7 * v7;
	double v9 = a(2);
	double v10 = 5.000000;
	double v11 = v9 + v10;
	double v12 = v11 * v11;
	double v13 = v8 + v12;
	double v14 = v4 + v13;
	double v15 = sqrt(v14);
	double v16 = v15 * v15;
	double v17 = v15 / v16;
	double v18 = v3 * v17;
	double v19 = 0.500000;
	double v20 = v19 / v15;
	double v21 = v20 * v3;
	double v22 = v14 / v16;
	double v23 = v21 * v22;
	double v24 = v18 - v23;

	return v24;
}

template<class S>
S compute(const Vector3<S> &a) {
	Vector3<S> b;
	b << 3, 4, 5;
	Vector3<S> c = a+b;
	return c.dot(c) / (c.norm());
}

//double computeByHand(const Vector3d &a) {
////	Vector3d b;
////	b << 3, 4, 5;

//	return a.dot(a) + a.norm() + a.dot(b) + b.norm();
//}


double computeFD(const Vector3d &a) {

	double h = 1e-5;

	Vector3d ap = a;
	ap(0) += h;
	Vector3d am = a;
	am(0) -= h;

	double ep = compute(ap);
	double em = compute(am);

	return (ep-em) / (2.*h);
}

void generateCode() {

	Vector3<ADRec> a;
	a(0) = ADRec("a(0)");
	a(1) = ADRec("a(1)");
	a(2) = ADRec("a(2)");

	CodeGenerator<double> generator;

	Vector3<R> g;

	for (int i = 0; i < 3; ++i) {
		a(i).deriv() = 1.0;
		ADRec n = compute(a);
		g(i) = n.deriv();
		g(i).addToGeneratorAsResult(generator, "g(" + std::to_string(i) + ")");
		a(i).deriv() = 0.0;
	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

void generateCodeHessian() {

	Vector3<ADDRec> a;
	a(0) = ADDRec("a(0)");
	a(1) = ADDRec("a(1)");
	a(2) = ADDRec("a(2)");

	CodeGenerator<double> generator;

	Matrix3<R> hess;

	for (int i = 0; i < 3; ++i) {
		a(i).deriv().value() = 1.0;
		for (int j = 0; j < 3; ++j) {
			a(j).value().deriv() = 1.0;
			ADDRec f = compute(a);
			hess(i,j) = f.deriv().deriv();
			hess(i,j).addToGeneratorAsResult(generator, "hess(" + std::to_string(i) + ")");
			a(j).value().deriv() = 0.0;
		}
		a(i).deriv().value() = 0.0;
	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

int main(int argc, char *argv[])
{

	generateCodeHessian();


//	std::cout << "# args:" << argc <<std::endl;

//	Vector3d a;
//	a << atof(argv[1]), atof(argv[2]), atof(argv[3]);

//	std::cout << "a: " << a.transpose() << std::endl;

//	std::cout << "fd: " << computeFD(a) << std::endl;
//	std::cout << "cg: " << computeCG(a) << std::endl;
//	int n = 1e8;

//	auto t1 = Clock::now();
//	double sum = 0;
//	for (int i = 0; i < n; ++i) {
//		sum += computeCG(a);
//	}
//	std::cout << "sum = " << sum << std::endl;

//	auto t2 = Clock::now();
//	sum = 0;
//	for (int i = 0; i < n; ++i) {
//		sum += computeByHand(a);
//	}
//	std::cout << "sum = " << sum << std::endl;

//	auto t3 = Clock::now();

//	std::cout << "Delta t2-t1: "
//			  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
//			  << " milliseconds" << std::endl;
//	std::cout << "Delta t3-t2: "
//			  << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()
//			  << " milliseconds" << std::endl;

	return 0;
}
