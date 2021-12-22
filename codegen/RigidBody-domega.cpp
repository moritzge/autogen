#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>

#include <RigidBody.h>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>
#include <Tensors.h>

using namespace AutoGen;

int main(int argc, char *argv[])
{
    typedef RecType<double> Rt;

    // record computation
    Vector3<Rt> v;
    for (int i = 0; i < 3; ++i) {
        v[i] = Rt("v[" + std::to_string(i) + "]");
    }

    CodeGenerator<double> generator;

    // compute gradient and add to code gen
    {
        Tensor4<Rt, 3,3,3,3> ddR = ExpCoords::ddR(v);
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k)
                    for (int l = 0; l < 3; ++l){
                        ddR[i][j](k,l).addToGeneratorAsResult(generator, "ddR[" + std::to_string(i) + "]["
                                                                              + std::to_string(j) + "]("
                                                                              + std::to_string(k) + ","
                                                                              + std::to_string(l) + ")");
                    }
        }
    }

    generator.sortNodes();

    std::string filename = AUTOGEN_GENERATED_CODE_FOLDER"/ddR.cpp";
    std::ofstream file(filename);
    file << generator.generateCode() << std::endl;
    file.close();
    std::cout << "generated code saved to `" << filename << "`" << std::endl;;
}
