#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>

#include "ExpCoords.h"

using namespace AutoGen;

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
