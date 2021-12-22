
#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>

#include "ExpCoords.h"

#include "ddR_i_j.h"

using namespace AutoGen;

struct Result {
    bool ok;
    std::string msg;
};

int main(int argc, char *argv[])
{
    srand((unsigned int) time(0));
    Vector3d v = 2*Vector3d::Random() - Vector3d::Ones();
    v /= v.norm();
    v *= 4*M_PI;

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {

            auto ddR = ddR_i_j(v, i, j);

            Matrix3d ddR_fd;
            {
                const double h = 1e-8;
                Vector3d dq = Vector3d::Zero();
                dq[j] += h;
                ddR_fd =(ExpCoords::dR<double>(v+dq)[i] - ExpCoords::dR<double>(v-dq)[i]) / (2*h);
            }

//            std::cout << ddR << std::endl;
//            std::cout << ddR_fd << std::endl;
            std::cout << (ddR - ddR_fd).norm() << std::endl;
        }
    }

}
