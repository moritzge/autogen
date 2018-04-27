#include <iostream>
#include <chrono>

#include <RecType.h>
#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

typedef std::chrono::high_resolution_clock Clock;

using namespace AutoGen;
using namespace Eigen;

template <class S> using Matrix2 = Matrix<S, 2, 2>;
template <class S> using Matrix3 = Matrix<S, 3, 3>;

typedef AutoDiff<double, double> AD;
typedef AutoDiff<AD, AD> ADD;


template <class S> using Vector2 = Matrix<S, 2, 1>;
template <class S> using Vector3 = Matrix<S, 3, 1>;

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

typedef RecType<double> R;
typedef AutoDiff<R, R> ADR;
typedef AutoDiff<ADR, ADR> ADDR;

template<class T>
T compute_f(Vector2<T> x) {
//	return pow(sqrt(x[0]) + sqrt(x[1]),2);
	return pow(x.norm() + 34.*x.dot(x), 1.23);
}

void compute_from_code_generator(double x1, double x2, double *gradient, double *hessian)
{
	double v0 = x1;
	double v1 = x2;
	double v2 = 0.500000;
	double v3 = v0 * v0;
	double v4 = v1 * v1;
	double v5 = v3 + v4;
	double v6 = sqrt(v5);
	double v7 = v2 / v6;
	double v8 = v0 + v0;
	double v9 = v7 * v8;
	double v10 = v1 + v1;
	double v11 = v7 * v10;
	double v12 = 2.000000;
	double v13 = v12 * v7;
	double v14 = v6 * v6;
	double v15 = v2 / v14;
	double v16 = v9 * v15;
	double v17 = -v16;
	double v18 = v17 * v8;
	double v19 = v13 + v18;
	double v20 = v17 * v10;
	double v21 = v11 * v15;
	double v22 = -v21;
	double v23 = v22 * v8;
	double v24 = v22 * v10;
	double v25 = v13 + v24;
	gradient[0] = v9;
	gradient[1] = v11;
	hessian[0] = v19;
	hessian[1] = v20;
	hessian[2] = v23;
	hessian[3] = v25;
}

int main(int argc, char *argv[])
{

	{
		Matrix2<double> hessian;
		Vector2<ADD> x; x[0] = 1; x[1] = 0.5;

		for (int i = 0; i < 2; ++i){
			x[i].value().deriv() = 1;

			for (int j = 0; j < 2; ++j){
				x[j].deriv().value() = 1;

				hessian(i,j) = compute_f(x).deriv().deriv();

				x[j].deriv().value() = 0;
			}

			x[i].value().deriv() = 0;
		}

		std::cout << hessian << std::endl;
	}

	{
		// gradient
		Vector2<R> gradient;
		{
			Vector2<ADR> x; x[0] = "x1"; x[1] = "x2";
			for (int i = 0; i < 2; ++i){
				x[i].deriv() = 1;
				gradient(i) = compute_f(x).deriv();
				x[i].deriv() = 0;
			}
		}

		// hessian
		Matrix2<R> hessian;
		{
			Vector2<ADDR> x; x[0] = "x1"; x[1] = "x2";
			for (int i = 0; i < 2; ++i){
				x[i].value().deriv() = 1;

				for (int j = 0; j < 2; ++j){
					x[j].deriv().value() = 1;

					hessian(i,j) = compute_f(x).deriv().deriv();

					x[j].deriv().value() = 0;
				}

				x[i].value().deriv() = 0;
			}
		}

		// generate code
		CodeGenerator<double> generator;
		addToGeneratorAsResult(gradient, generator, "gradient");
		addToGeneratorAsResult(hessian, generator, "hessian");
		generator.sortNodes();
		std::cout << generator.generateCode() << std::endl;

		Vector2d gradient_result;
		Matrix2d hessian_result;
		compute_from_code_generator(1, 0.5, gradient_result.data(), hessian_result.data());
		std::cout << gradient_result << std::endl;
		std::cout << hessian_result << std::endl;
	}

	return 0;
}
