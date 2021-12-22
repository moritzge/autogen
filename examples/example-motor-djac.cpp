#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>

#include "ExpCoords.h"

using namespace AutoGen;

using Eigen::VectorXd;
template <int N, int M> using Matrixd = Eigen::Matrix<double, N, M>;

template<class T>
Matrixd<3, 12> jacobian6(const Vector3<T> &q, T angle)
{
    auto v0 = rbs(0)->vWorld(q, axis[0]);
    auto u0 = rbs(0)->vWorld(q, vRot[0]);
    auto u1 = rbs(1)->vWorld(q, vRot[1]);
    Matrixd<3, 6> dv0_drb0 = rbs(0)->dvWorld_dq(q, axis[0]);
    Matrixd<3, 6> du0_drb0 = rbs(0)->dvWorld_dq(q, vRot[0]);
    Matrixd<3, 6> du1_drb1 = rbs(1)->dvWorld_dq(q, vRot[1]);

    Matrixd<3, 6*Motor::nRbs> jac = Matrixd<3, 6*Motor::nRbs>::Zero();

    // dC/drb0
    //u0 - ExpCoords::R_(theta*v0)*u1;
    jac.block<3, 6>(0,0) = du0_drb0 ;//- exp.dR_(theta*v0)* u1 * dv0_drb0 * theta;
//    jac.block<3, 6>(0,6) = -exp.R_(theta*v0)*du1_drb1;

    return jac*relativeAngularStiffness;
}

void generateCode_ddR(){
    typedef RecType<double> Rt;
    typedef AutoDiff<Rt, Rt> AD;

    // record computation
    Vector3<AD> v;
    for (int i = 0; i < 3; ++i) {
        v[i] = Rt("v[" + std::to_string(i) + "]");
    }

    CodeGenerator<double> generator;

    // compute gradient and add to code gen
    {
        Tensor4<Rt, 3,3,3,3> ddR;
        for (int i = 0; i < 3; ++i) {
            v(i).deriv() = 1.0;
            Tensor3<AD,3,3,3> dR = ExpCoords::dR<AD>(v);
            for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                for (int l = 0; l < 3; ++l){
                    ddR[i][j](k,l) = dR[j](k,l).deriv();
                    ddR[i][j](k,l).addToGeneratorAsResult(generator, "ddR[" + std::to_string(i) + "]["
                                                                            + std::to_string(j) + "]("
                                                                            + std::to_string(k) + ","
                                                                            + std::to_string(l) + ")");
                }
            v(i).deriv() = 0.0;
        }
    }

    generator.sortNodes();

    std::cout << generator.generateCode() << std::endl;
}

void generateCode_ddR_ij(){
    typedef RecType<double> Rt;
    typedef AutoDiff<Rt, Rt> AD;

    // record computation
    Vector3<AD> v;
    for (int i = 0; i < 3; ++i) {
        v[i] = Rt("v[" + std::to_string(i) + "]");
    }

    // compute gradient and add to code gen
    Tensor4<Rt, 3,3,3,3> ddR;
    for (int i = 0; i < 3; ++i) {
        v(i).deriv() = 1.0;
        Tensor3<AD,3,3,3> dR = ExpCoords::dR<AD>(v);
        for (int j = 0; j < 3; ++j)
        for (int k = 0; k < 3; ++k)
            for (int l = 0; l < 3; ++l){
                ddR[i][j](k,l) = dR[j](k,l).deriv();
            }
        v(i).deriv() = 0.0;
    }

    std::ofstream out("out.hpp");

    {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                CodeGenerator<double> generator;

                for (int k = 0; k < 3; ++k) {
                    for (int l = 0; l < 3; ++l) {

                        ddR[i][j](k,l).addToGeneratorAsResult(generator, "ddR_"
                                                              + std::to_string(i) + "_"
                                                              + std::to_string(j) + "("
                                                              + std::to_string(k) + ","
                                                              + std::to_string(l) + ")");
                    }
                }
                generator.sortNodes();

                out << "Matrix3d ddR_" << i << "_" << j << "(const Vector3d & v) {\n";
                out << "    Matrix3d ddR_" << i << "_" << j << ";\n";
                out << generator.generateCode("    ") << std::endl;
                out << "    return ddR_" << i << "_" << j << ";\n";
                out << "}\n\n";

            }
        }

    }

    out.close();
}

int main(int argc, char *argv[])
{
//    generateCode_dR();
//    generateCode_dddR();
    generateCode_ddR_ij();
}
