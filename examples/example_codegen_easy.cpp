#include <iostream>
#include <chrono>

//#include <RecType.h>
#include <Eigen/Eigen>
#include <AutoDiff.h>
#include <RecType.h>

typedef std::chrono::high_resolution_clock Clock;

/* CodeGen example easy
 * ====================
 *
 * This example shows how to use CodeGen to generate code of a function.
 * It also shows how to combine AutoDiff and CodeGen to get code for
 * the gradient and hessian.
 *
 * This example is called 'easy', because we first write helper functions
 * that make generating the gradient and hessian much easier.
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

////template<typename T>
//class my_function
//{
//  double operator()(/*const Vector<double> &x*/)
//  {
////	  Vector3<double> a = x.template segment<3>(0);
////	  Vector3<double> b = x.template segment<3>(3);

////	  return a.cross(b).norm();
//	  return 0.1;
//  }
//};

//template<class func>
//class EasyGen
//{
//public:
//	std::string generate(const Vector<double> &x) {
////		Vector<double> xx("x", x.size());

//		double f = func();

////		AutoGen::CodeGenerator<double> generator;
////		return f.generateCode("my_function");
//	}
//};

template<typename T>
int f(T v)
{
  std::cout << "Called" << std::endl;
  return 0; // just do something for example
}

template<typename T, template<typename> class func>
struct C
{
  int generateCode()
  {
	return func<T>(3);
  }
};

template <class T>
struct FuncAdapt
{
  T x_;
  template <class U>
  FuncAdapt( U x ) : x_( x ) {}

  operator int() const
  {
	return f<T>( x_ );
  }
};

int main(int argc, char *argv[])
{
	// generate code for the evaluation of my_function
	{
//		Vector<double> x(6);
//		EasyGen<my_function> gen;
//		std::cout << gen.generate(x) << std::endl;

		C<float,FuncAdapt > c;
		c.generateCode();

	}

#if 0
	// generate code for the evaluation of gradient of my_function
	{
		Vector<AD<R>> x("x", 6);
		Vector<R> gradient("gradient", 6);
		for (int i = 0; i < x.size(); ++i) {
			x[i].deriv() = 1;
			gradient[i] = my_function(x).deriv();
			x[i].deriv() = 0;
		}

		AutoGen::CodeGenerator<double> generator;
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

		AutoGen::CodeGenerator<double> generator;
		AutoGen::addToGeneratorAsResult(hessian, generator, "hessian");

		generator.sortNodes();
		std::cout << generator.generateCode("my_function_hessian") << std::endl;
	}


	// generate code for the evaluation of gradient of my_function
	{
		AutoGen::CodeGenerator<double> generator;

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
#endif
	return 0;
}
