#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include <ExpCoords.h>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>
#include <Tensors.h>

#include "angle-axis-cg.h"

using namespace AutoGen;

void generateCode_dR(){
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
        Tensor3<Rt, 3,3,3> dR;
        for (int i = 0; i < 3; ++i) {
            v(i).deriv() = 1.0;
            Matrix3<AD> R = ExpCoords::R<AD>(v);
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k){
                    dR[i](j,k) = R(j,k).deriv();
                    dR[i](j,k).addToGeneratorAsResult(generator, "dR[" + std::to_string(i) + "]("
                                                                       + std::to_string(j) + ","
                                                                       + std::to_string(k) + ")");
                }
            v(i).deriv() = 0.0;
        }
    }

    generator.sortNodes();

    std::cout << generator.generateCode() << std::endl;
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

                out << "Matrix3d ddR_" << i << "_" << j << "() {\n";
                out << "    Matrix3d ddR_" << i << "_" << j << ";\n";
                out << generator.generateCode("    ") << std::endl;
                out << "    return ddR_" << i << "_" << j << ";\n";
                out << "}\n\n";

            }
        }

    }

    out.close();
}

void generateCode_dddR(){
    typedef RecType<double> Rt;
    typedef AutoDiff<Rt, Rt> AD;
    typedef AutoDiff<AD, AD> ADD;

    // record computation
    Vector3<ADD> v;
    for (int i = 0; i < 3; ++i) {
        v[i] = Rt("v[" + std::to_string(i) + "]");
    }

    CodeGenerator<double> generator;

    // compute gradient and add to code gen
    {
        Tensor5<Rt, 3,3,3,3,3> dddR;
        for (int h = 0; h < 3; ++h) {
            v(h).deriv().value() = 1.0;
            for (int i = 0; i < 3; ++i) {
                v(i).value().deriv() = 1.0;
                Tensor3<ADD,3,3,3> dR = ExpCoords::dR<ADD>(v);
                for (int j = 0; j < 3; ++j)
                    for (int k = 0; k < 3; ++k)
                        for (int l = 0; l < 3; ++l){
                            dddR[h][i][j](k,l) = dR[j](k,l).deriv().deriv();
                            dddR[h][i][j](k,l).addToGeneratorAsResult(generator, "dddR_["
                                                                                   + std::to_string(h) + "]["
                                                                                   + std::to_string(i) + "]["
                                                                                   + std::to_string(j) + "]("
                                                                                   + std::to_string(k) + ","
                                                                                   + std::to_string(l) + ")");
                        }
                v(i).value().deriv() = 1.0;
            }
            v(h).deriv().value() = 0.0;
        }
    }

    generator.sortNodes();

    std::cout << generator.generateCode() << std::endl;
}

void generateCode_dtheta(){
    typedef RecType<double> Rt;
    typedef AutoDiff<Rt, Rt> AD;

    // record computation
    Matrix3<AD> R;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            R(i,j) = Rt("R(" + std::to_string(i) + "," + std::to_string(j) + ")");
        }
    }

    CodeGenerator<double> generator;

    // compute gradient and add to code gen
    {
        Tensor4<Rt, 3,1,3,3> dtheta_dR;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                R(i,j).deriv() = 1.0;
                Vector3<AD> theta = ExpCoords::theta<AD>(R);
                for (int k = 0; k < 3; ++k){
                    dtheta_dR[i][j](k,0) = theta[k].deriv();
                    dtheta_dR[i][j](k,0).addToGeneratorAsResult(generator,
                                                                "dtheta[" + std::to_string(i) +
                                                                "][" + std::to_string(j) +
                                                                "](" + std::to_string(k) + ",0)"
                                                                );
                }
                R(i,j).deriv() = 0.0;
            }
        }
    }

    generator.sortNodes();

    std::cout << generator.generateCode() << std::endl;
}


int main(int argc, char *argv[])
{

//    generateCode_dR();
//    generateCode_dddR();
    generateCode_ddR_ij();
//    generateCode_dtheta();


//    {
//        const Vector3d v = {1,0,0};
//        auto dR = dR_cg(v);
//        auto dR_ = ExpCoords::dR<double>(v);

//        Tensor3d3 dR_fd = Tensor3d3::Zero();
//        {
//            const double h = 1e-8;
//            for (int i = 0; i < 3; ++i) {
//                Vector3d dq = Vector3d::Zero();
//                dq[i] = h;
//                dR_fd[i] = (ExpCoords::R<double>(v+dq) - ExpCoords::R<double>(v-dq)) / (2*h);
//            }
//        }

//        std::cout << dR << std::endl;
//        std::cout << dR_ << std::endl;
//        std::cout << (dR - dR_fd).norm() << std::endl;
//        std::cout << (dR - dR_).norm() << std::endl;

//    }

//    {
//        const Vector3d v = {1,0,0};
//        auto ddR = cg::ddR_cg(v);

//        Tensor4d3 ddR_fd = Tensor4d3::Zero();
//        {
//            const double h = 1e-8;
//            for (int i = 0; i < 3; ++i) {
//                Vector3d dq = Vector3d::Zero();
//                dq[i] = h;
//                ddR_fd[i] = (ExpCoords::dR<double>(v+dq) - ExpCoords::dR<double>(v-dq)) / (2*h);
//            }
//        }

//        std::cout << ddR << std::endl;
//        std::cout << (ddR - ddR_fd).norm() << std::endl;

//    }

    {
        const Matrix3d R = ExpCoords::R(Vector3d{1,0,.1});
        auto dtheta = cg::dtheta(R);

        Tensor4d<3,1,3,3> dtheta_fd = Tensor4d<3,1,3,3>::Zero();
        {
            const double h = 1e-8;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    Matrix3d dR = Matrix3d::Zero();
                    dR(i,j) = h;
                    dtheta_fd[i][j] = (ExpCoords::theta<double>(R+dR) - ExpCoords::theta<double>(R-dR)) / (2*h);
                }
            }
        }

        std::cout << "dtheta" << std::endl;
        std::cout << dtheta << std::endl;
        std::cout << "dtheta_fd" << std::endl;
        std::cout << dtheta_fd << std::endl;
        std::cout << "dtheta_fd-dtheta" << std::endl;
        std::cout << dtheta_fd-dtheta << std::endl;
        std::cout << (dtheta - dtheta_fd).norm() << std::endl;

    }


}
