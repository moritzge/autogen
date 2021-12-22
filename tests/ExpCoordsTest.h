#pragma once

#include "GTestEigen.h"
#include "FiniteDifference.h"

#include <ExpCoords.h>

TEST(ExpCoords, dR_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor3d3 dR_fd = dde::math::FD<double,3,3,3>([](const Vector3d &x){
        return ExpCoords::R(x);
    }, theta);

    auto dR = ExpCoords::dR(theta);

    ASSERT_PRED2(Tensor3Equality, dR, dR_fd);
}

TEST(ExpCoords, ddR_FD) {

    Vector3d theta = Vector3d::Random();

    Tensor4d3 ddR_fd = dde::math::FD<double,3,3,3,3>([](const Vector3d &x){
        return ExpCoords::dR(x);
    }, theta);

    auto ddR = ExpCoords::ddR(theta);

    ASSERT_PRED2(Tensor4Equality, ddR, ddR_fd);
}
