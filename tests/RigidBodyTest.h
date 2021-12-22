#pragma once

#include "GTestEigen.h"
#include "FiniteDifference.h"

#include <RigidBody.h>

TEST(RigidBody, dJw_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor3d3 dJw_fd = dde::math::FD<double,3,3,3>([](const Vector3d &x){
        return RigidBody::Jw(x);
    }, theta);

    auto dJw = RigidBody::dJw_dtheta(theta);

    ASSERT_PRED2(Tensor3Equality, dJw, dJw_fd);
}

TEST(RigidBody, domega) {

    Vector3d theta = Vector3d::Random();
    Vector3d theta_dot = Vector3d::Random();

    Matrix3d domega_fd = dde::math::FD<double,3,3>(
        [&theta_dot](const Vector3d &x)->Vector3d{
            return RigidBody::omega(x, theta_dot);
        }, theta);

    auto domega = RigidBody::domega_dtheta(theta, theta_dot);

    ASSERT_PRED2(MatrixEquality, domega, domega_fd);
}
