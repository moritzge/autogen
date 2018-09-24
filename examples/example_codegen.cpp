#include <iostream>
#include <chrono>

//#include <RecType.h>
#include <Eigen/Eigen>
#include <AutoDiff.h>
#include <RecType.h>

typedef std::chrono::high_resolution_clock Clock;

/* CodeGen example
 * ================
 *
 * This example shows how to use CodeGen to generate code of a function.
 * It also shows how to combine AutoDiff and CodeGen to get code for
 * the gradient and hessian.
 *
 */

// some template typedefs for convenience
template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;
//template <class S> using Vector = Eigen::Matrix<S, -1, 1>;
template <class S> using Matrix = Eigen::Matrix<S, -1, -1>;

typedef AutoGen::RecType<double> R;
template <class S> using AD = AutoDiff<S, S>;

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

template<>
class Vector<AutoGen::RecType<double>> : public Eigen::Matrix<AutoGen::RecType<double>, -1, 1> {
public:
	Vector(const std::string &varName, int size) : Eigen::Matrix<AutoGen::RecType<double>, -1, 1>(size) {
		AutoGen::RecTypeVec<double> vec(varName, size);
		for (int i = 0; i < size; ++i) {
			this->operator[](i) = vec[i];
		}
	}
};

template<>
class Vector<AD<R>> : public Eigen::Matrix<AD<R>, -1, 1> {
public:
	Vector(const std::string &varName, int size) : Eigen::Matrix<AD<R>, -1, 1>(size) {
		AutoGen::RecTypeVec<double> vec(varName, size);
		for (int i = 0; i < size; ++i) {
			this->operator[](i) = vec[i];
		}
	}
};

template<>
class Vector<AD<AD<R>>> : public Eigen::Matrix<AD<AD<R>>, -1, 1> {
public:
	Vector(const std::string &varName, int size) : Eigen::Matrix<AD<AD<R>>, -1, 1>(size) {
		AutoGen::RecTypeVec<double> vec(varName, size);
		for (int i = 0; i < size; ++i) {
			this->operator[](i) = vec[i];
		}
	}
};



// This is the function we want to generate code from
template<class T>
T my_function(Vector<T> &x) {
	Vector3<T> a = x.template segment<3>(0);
	Vector3<T> b = x.template segment<3>(3);

	return a.cross(b).norm();
}

int main(int argc, char *argv[])
{
	// generate code for the evaluation of my_function
	{
		Vector<R> x("x", 6);

		R f = my_function(x);

		AutoGen::CodeGenerator generator;
		std::cout << f.generateCode("my_function") << std::endl;
	}

	// generate code for the evaluation of gradient of my_function
	{
		Vector<AD<R>> x("x", 6);
		Vector<R> gradient("gradient", 6);
		for (int i = 0; i < x.size(); ++i) {
			x[i].deriv() = 1;
			gradient[i] = my_function(x).deriv();
			x[i].deriv() = 0;
		}

		AutoGen::CodeGenerator generator;
		AutoGen::addToGeneratorAsResult(gradient, generator, "gradient");

		generator.sortNodes();
		std::cout << generator.generateCode("my_function_gradient") << std::endl;
	}

	// compute hessian of my_function
	{
		Vector<AD<AD<R>>> x("x", 6);
		Matrix<R> hessian(6, 6);
		for (int i = 0; i < hessian.rows(); ++i) {
			x[i].deriv() = 1;
			for (int j = 0; j < hessian.rows(); ++j) {
				x[j].value().deriv() = 1;
				hessian(i, j) = my_function(x).deriv().deriv();
				x[j].value().deriv() = 0;
			}
			x[i].deriv() = 0;
		}

		AutoGen::CodeGenerator generator;
		AutoGen::addToGeneratorAsResult(hessian, generator, "hessian");

		generator.sortNodes();
		std::cout << generator.generateCode("my_function_hessian") << std::endl;
	}


	// generate code for the evaluation of gradient of my_function
	{
		AutoGen::CodeGenerator generator;

		{
			Vector<R> x("x", 6);

			R f = my_function(x);
			f.addToGeneratorAsResult(generator, "f");
		}

		{
			Vector<R> gradient("gradient", 6);
			Vector<AD<R>> x("x", 6);
			for (int i = 0; i < x.size(); ++i) {
				x[i].deriv() = 1;
				gradient[i] = my_function(x).deriv();
				x[i].deriv() = 0;
			}
			AutoGen::addToGeneratorAsResult(gradient, generator, "gradient");
		}

		{
			Vector<AD<AD<R>>> x("x", 6);
			Matrix<R> hessian(6, 6);
			for (int i = 0; i < hessian.rows(); ++i) {
				x[i].deriv() = 1;
				for (int j = 0; j < hessian.rows(); ++j) {
					x[j].value().deriv() = 1;
					hessian(i, j) = my_function(x).deriv().deriv();
					x[j].value().deriv() = 0;
				}
				x[i].deriv() = 0;
			}

			AutoGen::addToGeneratorAsResult(hessian, generator, "hessian");
		}

		generator.sortNodes();
		std::cout << generator.generateCode("my_function_gradient_hessian") << std::endl;
	}

	return 0;
}
