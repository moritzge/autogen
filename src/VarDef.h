#pragma once

#include "AutoDiff.h"

#include <eigen3/Eigen/Eigen>

namespace AutoGen {

template<class S>
class VectorX;

template <class S, int N> using VectorXN = Eigen::Matrix<S, N, 1>;
template <class S> using Vector3 = VectorXN<S, 3>;

template<class S>
class RecType;

typedef AutoDiff<double, double> AD;
typedef AutoDiff<AD, AD> ADD;

typedef RecType<double> R;

typedef AutoDiff<R, R> ADR;
typedef AutoDiff<ADR, ADR> ADDR;

template <class S>
class VectorXn : public VectorXN<S, -1>
{
public:
	VectorXn() : VectorXN<S, -1>() {}
	VectorXn(int size, const std::string &name)
	{
		m_varName = name;
		this->resize(size);
		for (int i = 0; i < size; ++i) {
			(*this)[i] = S(name + "[" + std::to_string(i) + "]");
		}
	}

	virtual std::string getName()
	{
		return m_varName;
	}

	virtual std::string getGeneratedType()
	{
		return "Eigen::Matrix<" + (*this)[0].getGeneratedType() + ", " + std::to_string(this->size()) + ", 1>";
	}

protected:
	std::string m_varName;
};

template <class S>
class Vector3n : public VectorXn<S>
{
public:
	Vector3n() : VectorXn<S>(3, "NoName") {}
	Vector3n(const std::string &name) : VectorX<S>(3, name)
	{
	}
};

template <class S>
class VarListX : public VectorXN<S*, -1>
{
public:

	VarListX() : VectorXN<S*, -1>()
	{
	}

	VarListX(int size) : VectorXN<S*, -1>(size)
	{
	}

	void assignSegment(int idx, int size, VectorX<S> &vars)
	{
		for (int i = 0; i < size; i++)
		{
			(*this)[idx + i] = &vars[i];
		}
	}

	virtual std::string getInnerType()
	{
		std::string currentType = typeid(*this).name();

		// We assume the innerest node hase a value like double or float
		int start = currentType.find_last_of('<') + 1;
		std::string typeAndTail = currentType.substr(start);

		int end = typeAndTail.find_first_of('>');
		std::string type = typeAndTail.substr(0, end);

		return type;
	}

	virtual std::string getGeneratedType()
	{
		std::string innerType = getInnerType();
		return "Eigen::Matrix<" + innerType + ", " + std::to_string(this->size()) + ", 1>";
	}


};

} // namespace AutoGen
