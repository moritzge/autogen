#pragma once

#include <vector>

template<int N, int M>
class Matrix
{
public:
	Matrix() {}

	template<class S>
	Matrix(S value) {
		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < M; ++j) {
				data[i][j] = value;
			}
		}
	}

//	static int sizeN() { return  N; }
//	static int sizeM() { return  M; }

	static const int sizeN = N;
	static const int sizeM = M;

private:
	double data[N][M];
};
