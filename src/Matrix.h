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

	Matrix(double value) {
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

	// MxN * NxO = MxO
	template<int O>
	Matrix<M, O> operator*(const Matrix<N, O> &other) const {
		Matrix<M, O> res(0.0);
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < O; ++j)
				for (int k = 0; k < N; ++k)
					res.data[i][j] += data[i][k] * other.data[k][j];
		}
		return res;
	}

	// MxN * 1x1 = MxN
//	template<int O>
	Matrix<M, N> operator*(const Matrix<1, 1> &other) const {
		Matrix<M, N> res(0.0);
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j)
				res.data[i][j] = data[i][j] * other.data[0][0];
		}
		return res;
	}

	double* getData() { return *data; }

//private:
	double data[M][N];
};

template<int M, int N>
std::ostream& operator<<(std::ostream& stream, const Matrix<M, N> &mat) {

	stream << "{ ";

	for (int i = 0; i < M; ++i) {
		stream << "{ ";
		for (int j = 0; j < N; ++j) {
			stream << mat(i, j);
			if(j < N-1)
				stream << ", ";
		}
		stream << " } ";
	}

	stream << "} ";

	return stream;
}
