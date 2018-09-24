#pragma once

#include <ostream>

template<int M, int N>
class Matrix
{
public:
	static const int sizeM = M;
	static const int sizeN = N;

	Matrix() {
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j)
				data[i][j] = 0;
		}
	}

	template<class S>
	Matrix(S value) {
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j) {
				data[i][j] = value;
			}
		}
	}

	double operator()(int i, int j) const {
		return data[i][j];
	}

	bool operator==(const Matrix<M, N> &other) const {
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j)
				if(data[i][j] != other.data[i][j])
					return false;
		}
		return true;
	}

	Matrix<M, N> operator+(const Matrix<M, N> &other) const {
		Matrix<M, N> res;
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j)
				res.data[i][j] = data[i][j] + other.data[i][j];
		}
		return res;
	}

	double* getData() { return *data; }

//private:
	double data[M][N];
};

template<int M, int N>
std::ostream& operator<<(std::ostream& stream, const Matrix<M, N> &mat) {

	stream << "{ "; // << "(" << s.deriv() << ")";

	for (int i = 0; i < M; ++i) {
		stream << "{ ";
		for (int j = 0; j < N; ++j) {
			stream << mat(i, j);
			if(j < N-1)
				stream << ", ";
		}
		stream << "} ";
	}

	stream << "} ";

	return stream;
}
