#pragma once

#include <ExpCoords.h>

template<class T>
Matrix3<T> skew_sym_matrices_to_matrix(const std::array<Matrix3<T>, 3> &j) {
    Matrix3<T> Jw;
    Jw.col(0) = vector_from_skew_symmetric(j[0]);
    Jw.col(1) = vector_from_skew_symmetric(j[1]);
    Jw.col(2) = vector_from_skew_symmetric(j[2]);
    return Jw;
}


struct RigidBody {


    template<class T>
    static Matrix3<T> Jw(const Matrix3<T> &R_, const Tensor3<T, 3, 3, 3> &dR_) {

        Matrix3<T> Jw = skew_sym_matrices_to_matrix(
            std::array<Matrix3<T>, 3>{
                dR_[0] * R_.transpose(),
                dR_[1] * R_.transpose(),
                dR_[2] * R_.transpose()
            }
            );
        return Jw;
    }

    template<class T>
    static Matrix3<T> Jw(const Vector3<T> &theta) {
        return Jw(ExpCoords::R(theta), ExpCoords::dR(theta));
    }

    template<class T>
    static Tensor3<T,3,3,3> dJw_dtheta(const Matrix3<T> &R_, const Tensor3<T,3,3,3> &dR_, const Tensor4<T,3,3,3,3> &ddR_) {

        Tensor3<T,3,3,3> dJw;
        for (int k = 0; k < 3; ++k) {
            for (int i = 0; i < 3; ++i) {
                int r=(i+2)%3, c=(i+1)%3;
                for (int j = 0; j < 3; ++j)
                {
                    dJw[k](i,j)  = ddR_[j][k].row(r)*R_.transpose().col(c);
                    dJw[k](i,j) += dR_[j].row(r)*dR_[k].transpose().col(c);
                }
            }
        }

        return dJw;
    }

    template<class T>
    static Tensor3<T,3,3,3> dJw_dtheta(const Vector3<T> &theta)
    {
        return dJw_dtheta(ExpCoords::R(theta), ExpCoords::dR(theta), ExpCoords::ddR(theta));
    }

    template<class T>
    static Vector3<T> omega(const Vector3<T> &theta, const Vector3<T> &theta_dot)
    {
        return Jw(theta) * theta_dot;
    }

    template<class T>
    static Matrix3<T> domega_dtheta(const Vector3<T> &theta, const Vector3<T> &theta_dot)
    {
        return dJw_dtheta(theta) * theta_dot;
    }


};
