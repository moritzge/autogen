#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <Eigen/Core>
#include <array>

namespace dde { namespace math {

// using Eigen::Vector; // works only with Eigen > ~3.3.6
template <typename Type, int Size> using Vector = Eigen::Matrix<Type, Size, 1>;
using Eigen::Vector3d;
using Eigen::Matrix;
using Eigen::Matrix3d;

template <class T, int M, int N, int O>
class Tensor3 : public std::array<Matrix<T, M, N>, O> {
public:
    typedef Tensor3<T,M,N,O> T3;

    Tensor3() {}
    Tensor3(std::array<Matrix<T, M, N>, O> a)
        : std::array<Matrix<T, M, N>, O>(a) {}

    static Tensor3<T,M,N,O> Zero() {
        auto m = Matrix<T, M, N>::Zero();
        Tensor3 t;
        for (int i = 0; i < O; ++i) {
            t[i] = m;
        }
        return t;
    }

    static Tensor3<T,M,N,O> Identity() {
        auto m = Matrix<T, M, N>::Identity();
        Tensor3 t;
        for (int i = 0; i < O; ++i)
            t[i] = m;
        return t;
    }

    Tensor3<T,N,M,O> transpose() const {
        // TODO: this copies everything. rather return something like a
        //       UnaryOperator that references this Tensor, similar to Eigen.
        Tensor3<T,N,M,O> t;
        for (int i = 0; i < O; ++i)
            t[i] = this->at(i).transpose();
        return t;
    }

    T3 operator+(const T3 &m) const {
        T3 res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i) + m[i];
        return res;
    }

    T3 operator-(const T3 &m) const {
        T3 res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i) - m[i];
        return res;
    }


    template<int P>
    Tensor3<T, M, P, O> operator*(const Matrix<T, N, P> &m) const {
        static_assert(P != 1, "not allowed to call this with P = 1");
        Tensor3<T, M, P, O> res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i)*m;
        return res;
    }

    // A[NxO] = T[NxMxO] * V[M]
    Matrix<T, M, O> operator*(const Vector<T, N> &v) {
        Matrix<T, M, O> A;
        for (int i = 0; i < O; ++i)
            A.col(i) = this->at(i)*v;
        return A;
    }

    Matrix<T, M, O> compWiseProduct(const Vector<T, M> &v) const {
        Matrix<T, M, O> A = Matrix<T, M, O>::Zero();// = dJw_dtheta_ * dtheta; // \dot{J_\omega}
        for (int i = 0; i < 3; ++i)
            A += this->at(i) * v[i];
        return A;
    }


    T3 operator*(T s) const {
        T3 res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i)*s;
        return res;
    }

    T3 operator/(T s) const {
        T3 res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i)/s;
        return res;
    }

    double norm() const {
        double norm = 0;
        for (int i = 0; i < O; ++i) {
            norm += this->at(i).norm();
        }
        return norm;
    }
};

template <class T, int M, int N, int O>
std::ostream& operator<<(std::ostream& stream, const Tensor3<T,N,M,O> &x) {
    for (int i = 0; i < O; ++i)
        stream << i << ":\n" << x[i] << std::endl;
    return stream;
}

//template <class T, int N, int M, int O> using Tensor3 = std::array<Eigen::Matrix<T, N, M>, O>;
template <int M, int N, int O> using Tensor3d = Tensor3<double, M, N, O>;
typedef Tensor3d<3,3,3> Tensor3d3; // ugly name ...

// Tensor overloads
template<class T, int N, int M, unsigned long O>
Matrix<T, N, M> operator*(const Tensor3<T, N, M, O> &t, const Vector<T, M> &v) {
    Matrix<T, N, M> res;
    for (int i = 0; i < O; ++i) {
        res.col(i) = t[i]*v;
    }
    return res;
}

template<class T, int M, int N, int O>
Matrix<T, N, O> multTranspose(const Tensor3<T, M, N, O> &t, const Vector<T, M> &v) {
    Matrix<T, N, O> res;
    for (int i = 0; i < O; ++i) {
        res.col(i) = t[i].transpose()*v;
    }
    return res;
}

template <class T, int M, int N, int O, int P>
class Tensor4 : public std::array<Tensor3<T, M, N, O>, P> {
public:
    typedef Tensor4<T,M,N,O,P> T4;
    typedef Tensor3<T, M, N, O> T3;

    Tensor4() {}
    Tensor4(std::array<Tensor3<T, M, N, O>, P> a)
        : std::array<Tensor3<T, M, N, O>, P>(a) {}

    static T4 Zero() {
        auto t3 = T3::Zero();
        T4 t4;
        for (int i = 0; i < P; ++i) {
            t4[i] = t3;
        }
        return t4;
    }

    T4 operator-(const T4 &m) const {
        T4 res;
        for (int i = 0; i < O; ++i) {
            res[i] = this->at(i) - m[i];
        }
        return res;
    }

    T4 operator/(T s) const {
        T4 res;
        for (int i = 0; i < O; ++i)
            res[i] = this->at(i)/s;
        return res;
    }

    double norm() const {
        double norm = 0;
        for (int i = 0; i < O; ++i) {
            norm = this->at(i).norm();
        }
        return norm;
    }
};

template <class T, int M, int N, int O, int P>
std::ostream& operator<<(std::ostream& stream, const Tensor4<T,M,N,O,P> &x) {
    for (int i = 0; i < O; ++i)
        stream << i << ":\n" << x[i] << std::endl;
    return stream;
}

template <int M, int N, int O, int P>
using Tensor4d = Tensor4<double, M, N, O, P>;
typedef Tensor4d<3,3,3,3> Tensor4d3;

template <class T, int M, int N, int O, int P, int Q>
using Tensor5 = std::array<Tensor4<T, M, N, O, P>, Q>;
template <int M, int N, int O, int P, int Q>
using Tensor5d = Tensor5<double, M, N, O, P, Q>;
typedef Tensor5d<3,3,3,3,3> Tensor5d3;

} } // namespace dde::math
