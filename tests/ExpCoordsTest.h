#include <gtest/gtest.h>

#include <ExpCoords.h>

bool MatrixEquality(const Matrix3d &lhs, const Matrix3d &rhs) {
    return lhs.isApprox(rhs, 1e-8);
}

bool Tensor3Equality(const Tensor3d3 &lhs, const Tensor3d3 &rhs) {
    bool ret = true;
    for (int i = 0; i < 3; ++i) {
        ret &= MatrixEquality(lhs[i], rhs[i]);
    }
    return ret;
}

bool Tensor4Equality(const Tensor4d3 &lhs, const Tensor4d3 &rhs) {
    bool ret = true;
    for (int i = 0; i < 3; ++i) {
        ret &= Tensor3Equality(lhs[i], rhs[i]);
    }
    return ret;
}

TEST(ExpCoords, dR_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor3d3 dR_fd;

    const double h = 1e-5;
    for (int i = 0; i < 3; ++i) {
        Vector3d theta_p = theta, theta_m = theta;
        theta_p[i] += h;
        theta_m[i] -= h;
        dR_fd[i] = (ExpCoords::R(theta_p) - ExpCoords::R(theta_m)) / (2*h);

    }

    auto dR = ExpCoords::dR(theta);

    ASSERT_PRED2(Tensor3Equality, dR, dR_fd);
}

TEST(ExpCoords, ddR_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor4d3 ddR_fd;

    const double h = 1e-5;
    for (int i = 0; i < 3; ++i) {
        Vector3d theta_p = theta, theta_m = theta;
        theta_p[i] += h;
        theta_m[i] -= h;
        ddR_fd[i] = (ExpCoords::dR(theta_p) - ExpCoords::dR(theta_m)) / (2*h);

    }

    auto ddR = ExpCoords::ddR(theta);

    ASSERT_PRED2(Tensor4Equality, ddR, ddR_fd);
}
