#pragma once

#include <ostream>

namespace AutoGen {

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

	double& operator()(int i, int j) {
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

	// MxN + MxN = MxN
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
	Matrix<M, N> operator*(const Matrix<1, 1> &other) const {
		Matrix<M, N> res(0.0);
		for (int i = 0; i < M; ++i) {
			for (int j = 0; j < N; ++j)
				res.data[i][j] = data[i][j] * other.data[0][0];
		}
		return res;
	}

	// MxN^T = NxM
	Matrix<N, M> transpose() const {
		Matrix<N,M> mat;
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < M; ++j)
				mat.data[i][j] = data[j][i];
		return mat;
	}

	double* getData() { return *data; }

//private:
	double data[M][N];
};

template<int M, int N>
Matrix<M, N> operator+(Matrix<M, N> a, Matrix<1, 1> b) {
	Matrix<M, N> res;
	for (int i = 0; i < M; ++i)
		for (int j = 0; j < N; ++j)
			res.data[i][j] = a.data[i][j] + b.data[0][0];	
	return res;
}

template<int M, int N>
Matrix<M, N> operator+(Matrix<1, 1> a, Matrix<M, N> b) {
    return b+a;
}

template<int M, int N>
Matrix<M, N> operator*(Matrix<1, 1> a, Matrix<M, N> b) {
	return b*a;
}

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
		if(i < M-1)
			stream << " }, ";
		else
			stream << " } ";
	}

	stream << "} ";

	return stream;
}

} // namespace AutoGen
