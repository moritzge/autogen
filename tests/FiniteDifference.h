#pragma once

#include <Tensors.h>

#include <set>

namespace dde { namespace math {

// from http://numerical.recipes/book/book.html, Section 5.7, page 230
static const double step_size_default = std::pow(std::numeric_limits<double>::epsilon(), 0.333333);

Matrix3d FD3(std::function<Vector3d(const Vector3d&)> f, const Vector3d &x, double stepsize = step_size_default){
    Matrix3d df = Matrix3d::Zero();

    for (int i = 0; i < x.size(); ++i) {
        Vector3d dx = Vector3d::Zero();
        dx[i] = stepsize;
        df.col(i) = (f(x+dx) - f(x-dx)) / (2*stepsize);
    }

    return df;
}

template<class T, int M, int N>
Matrix<T,M,N> FD(std::function<Vector<T,M>(const Vector<T,N>&)> f, const Vector<T,N> &x, T stepsize = step_size_default){
    Matrix<T,M,N> df = Matrix<T,M,N>::Zero(f(x).size(), x.size());

    for (int i = 0; i < x.size(); ++i) {
        Vector<T,N> dx = Vector<T,N>::Zero();
        dx[i] = stepsize;
        df.col(i) = (f(x+dx) - f(x-dx)) / (2*stepsize);
    }

    return df;
}

//Tensor3d3 FD(std::function<Matrix3d (const Vector3d &)> f, const Vector3d &x, double stepsize = step_size_default){
//    Tensor3d3 df = MatrixXd::Zero(f(x).size(), x.size());

//    for (int i = 0; i < x.size(); ++i) {
//        VectorXd dx = VectorXd::Zero(x.size());
//        dx[i] = stepsize;
//        df.col(i) = (f(x+dx) - f(x-dx)) / (2*stepsize);
//    }

//    return df;
//}

template<class T, int M, int N, int O>
Tensor3<T,M,N,O> FD(std::function<Matrix<T,M,N> (const Vector<T,O> &)> f, const Vector<T,O> &x, double stepsize = step_size_default)
{
    Tensor3<T,M,N,O> df = Tensor3<T,M,N,O>::Zero();

    for (int i = 0; i < O; ++i) {
        Vector<T,O> dx = Vector<T,O>::Zero();
        dx[i] = stepsize;
        df[i] = (f(x+dx) - f(x-dx)) / (2*stepsize);
    }

    return df;
}

template<class T, int M, int N, int O, int P>
Tensor4<T,M,N,O,P> FD(std::function<Tensor3<T,M,N,O> (const Vector<T,P> &)> f, const Vector<T,P> &x, double stepsize = step_size_default)
{
    Tensor4<T,M,N,O,P> df = Tensor4<T,M,N,O,P>::Zero();

    for (int i = 0; i < O; ++i) {
        Vector<T,O> dx = Vector<T,O>::Zero();
        dx[i] = stepsize;
        df[i] = (f(x+dx) - f(x-dx)) / (2*stepsize);
    }

    return df;
}



} } // dde::tests
