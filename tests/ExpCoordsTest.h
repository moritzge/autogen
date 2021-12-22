#pragma once

#include "GTestEigen.h"

#include <ExpCoords.h>

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
