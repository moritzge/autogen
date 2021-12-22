#pragma once

#include <gtest/gtest.h>
#include <Tensors.h>

using namespace dde::math;

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
