#include <gtest/gtest.h>

#include "AutoLoadTest.h"
#include "ExpCoordsTest.h"
#include "RigidBodyTest.h"

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
