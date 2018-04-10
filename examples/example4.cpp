#include <iostream>
#include <chrono>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

typedef std::chrono::high_resolution_clock Clock;

using namespace AutoGen;
using namespace Eigen;

template <class S> using Matrix3 = Matrix<S, 3, 3>;

typedef AutoDiff<double, double> AD;
typedef AutoDiff<AD, AD> ADD;


typedef AutoDiff<double, Vector3<double>> ADV3;

template<class T>
class V3T : public Vector3<T> {
public:
	V3T<T>() : Vector3<T> () {}

	template<class U>
	V3T<T>(const Vector3<U> &other)
	{
		this->data()[0] = other(0);
		this->data()[1] = other(1);
		this->data()[2] = other(2);
	}
};

class Test
{
public:
	Test() {}

	template<class T>
	T compute(const Vector3<T> &x){

		return x.dot((V3T<T>)a)*2.0;
	}

	Vector3<double> a;
};

int main(int argc, char *argv[])
{
	Test test;
	test.a << 3, 4, 5;
	Vector3<double> xi; xi << 1,2,3;

	{
		double energy = test.compute(xi);
		std::cout << energy <<std::endl;
	}

	{
		Vector3<AD> x;
		for (int i = 0; i < 3; ++i) {
			x[i] = xi[i];
		}

		Vector3<double> grad;
		for (int i = 0; i < 3; ++i) {
			x[i].deriv() = 1.0;
			AD energy = test.compute(x);
			grad[i] = energy.deriv();
			x[i].deriv() = 0.0;
		}

		std::cout << grad <<std::endl;
	}

	{
		Vector3<ADD> x;
		for (int i = 0; i < 3; ++i) {
			x[i] = xi[i];
		}

		Matrix3<double> hess;
		for (int i = 0; i < 3; ++i) {
			x[i].deriv().value() = 1.0;
			for (int j = 0; j < 3; ++j) {
				x[j].value().deriv() = 1.0;
				ADD energy = test.compute(x);
				hess(i,j) = energy.deriv().deriv();
				x[j].value().deriv() = 0.0;
			}
			x[i].deriv() = 0.0;
		}

		std::cout << hess <<std::endl;
	}

//	{
//		Vector3<ADV3> x;
//		for (int i = 0; i < 3; ++i) {
//			x(i).value() = xi(i);
//			Vector3<double> g(0,0,0);
//			g(i) = 1;
//			x(i).deriv() = g;
//		}


//		ADV3 energy = test.compute(x);
//		Vector3<double> gradient = energy.deriv();

//		std::cout  << gradient << std::endl;
//	}

//	Vector3<double> z(0);

	return 0;
}
