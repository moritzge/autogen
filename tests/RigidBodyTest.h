#pragma once

#include "GTestEigen.h"

#include <RigidBody.h>

TEST(RigidBody, dJw_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor3d3 dJw_fd;

    const double h = 1e-5;
    for (int i = 0; i < 3; ++i) {
        Vector3d theta_p = theta, theta_m = theta;
        theta_p[i] += h;
        theta_m[i] -= h;
        dJw_fd[i] = (RigidBody::Jw(theta_p) - RigidBody::Jw(theta_m)) / (2*h);

    }

    auto dJw = RigidBody::dJw_dtheta(theta);

    ASSERT_PRED2(Tensor3Equality, dJw, dJw_fd);
}
