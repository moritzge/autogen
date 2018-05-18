#include <iostream>
#include <chrono>

//#include <RecType.h>
#include <Eigen/Eigen>
#include <AutoDiff.h>

typedef std::chrono::high_resolution_clock Clock;

/* AutoDiff example
 * ================
 *
 * This example shows how to use AutoDiff to compute the gradient and hessian
 * of a function.
 */

// some template typedefs for convenience
template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;
//template <class S> using Vector = Eigen::Matrix<S, -1, 1>;
template <class S> using Matrix = Eigen::Matrix<S, -1, -1>;

// also for convenience. this allows us to do:
// Vector<S> x; Vector<T> y = x;
// when S and T are different classes
template<class T>
class Vector : public Eigen::Matrix<T, -1, 1> {
public:
	template<class S>
	Vector(const Vector<S> &other) {
		this->resizeLike(other);
		for (int i = 0; i < other.rows(); ++i) {
			this->data()[i] = other.data()[i];
		}
	}

	Vector(int size) : Eigen::Matrix<T, -1, 1>(size) {}
};

// This is the function we want to take derivatives from.
template<class T>
T my_function(Vector<T> &x) {
	Vector3<T> a = x.template segment<3>(0);
	Vector3<T> b = x.template segment<3>(3);

	return a.cross(b).norm();
}


int main(int argc, char *argv[])
{
	Vector<double> x(6);
	x << 0, 1, 2, 3, 4, 5;

	// evaluate my_function
	{
		double f = my_function(x);
		std::cout << "f(x) = " << f << std::endl;
	}

	// compute gradient of my_function
	{
		Vector<AutoDiff<double, double>> xd = x;
		Vector<double> gradient(6);
		for (int i = 0; i < xd.size(); ++i) {
			xd[i].deriv() = 1;
			gradient[i] = my_function(xd).deriv();
			xd[i].deriv() = 0;
		}

		std::cout << "df/dx(x) = " << gradient.transpose() << std::endl;
	}

	// compute hessian of my_function
	{
		Vector<AutoDiff<AutoDiff<double, double>, AutoDiff<double, double>>> xdd = x;
		Matrix<double> hessian(6, 6);
		for (int i = 0; i < hessian.rows(); ++i) {
			xdd[i].deriv() = 1;
			for (int j = 0; j < hessian.rows(); ++j) {
				xdd[j].value().deriv() = 1;
				hessian(i, j) = my_function(xdd).deriv().deriv();
				xdd[j].value().deriv() = 0;
			}
			xdd[i].deriv() = 0;
		}

		std::cout << "ddf/ddx(x) = \n" << hessian << std::endl;

	}

	return 0;
}
