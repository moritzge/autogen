#include <gtest/gtest.h>

#include <AutoDiff.h>
#include <RecType.h>
#include <AutoLoad.h>
#include <CodeGenerator.h>

template <class S> using Vector3 = Eigen::Matrix<S, 3, 1>;

////////////////////////////////////////////////////////////////////////// TEST 0

/*
 * Testing: AutoLoad
 * Test AutoLoad with a very simple test.
 * The test is a bit pointless, but shows how to use AutoLoad.
 */

TEST(GenerateCodeAndLoadLib, SuperSimpleTest) {
    using namespace AutoGen;

    // write some code to a string
    std::string code = "y[0] = x[0]*x[0] + 2.0;\n";

    // and wrap it in a function
    std::string libCode =
            "extern \"C\" void compute_extern(double* x, double* y) {\n"
            + code
            + "}\n";

    // build and load library
    std::string error;
    compute_extern* compute;
    EXPECT_TRUE(buildAndLoad(libCode, compute, error));

    // test it!
    {
        double x[1]; x[0] = 0.124;
        double y[1];
        compute(x, y);
        EXPECT_EQ(y[0], x[0]*x[0] + 2.0);
    }
}

////////////////////////////////////////////////////////////////////////// TEST 1

/*
 * Testing: CodeGenerator & AutoLoad
 * This test uses CodeGenerator to generate code from a c++ function, and then
 * builds the code and loads the resulting library.
 *
 */

template<class T>
T computeScalar(T a) {
    T b = 3;
    T c = 4;
    return pow(a,5.0)*b + c*sqrt(a);
}

TEST(GenerateCodeAndLoadLib, ScalarCompute) {
    using namespace AutoGen;

    // record computation
    RecType<double> y = computeScalar(RecType<double>("x[0]"));

    // generate the code
    std::string code = y.generateCode("y[0]");

    // and wrap it in a function
    std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
    libCode += code;
    libCode += "}\n";

    // build and load library
    std::string error;
    compute_extern* compute_y;
    EXPECT_TRUE(buildAndLoad(libCode, compute_y, "compute_y", error));

    // test it!
    {
        double x[1]; x[0] = 0.124;
        double y[1];
        compute_y(x, y);
        EXPECT_EQ(y[0], computeScalar(x[0]));
    }
}

////////////////////////////////////////////////////////////////////////// TEST 2

/*
 * Testing: Eigen & CodeGenerator & AutoLoad
 * This test generates, builds and loads code that uses Eigen.s
 */

template<class T>
T computeDotProduct(Vector3<T> &a) {
    return a.dot(a);
}

TEST(GenerateCodeAndLoadLib, DotProduct) {
    using namespace AutoGen;
    typedef RecType<double> R;

    // record computation
    Vector3<R> x;
    for (int i = 0; i < 3; ++i) {
        x[i] = R("x[" + std::to_string(i) + "]");
    }
    R y = computeDotProduct(x);

    // generate the code
    std::string code = y.generateCode("y[0]");

    // and wrap it in a function
    std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
    libCode += code;
    libCode += "}\n";

    // build and load library
    std::string error;
    compute_extern* compute;
    EXPECT_TRUE(buildAndLoad(libCode, compute, "computeDotProduct", error));

    // test it!
    {
        Vector3<double> x;
        x << 0.1243, 1.34, 5.67;
        double y[1];
        compute(x.data(), y);
        EXPECT_EQ(y[0], computeDotProduct(x));
    }
}

////////////////////////////////////////////////////////////////////////// TEST 3

/*
 * Testing: AutoDiff & CodeGenerator & AutoLoad
 */

template <class S> using Matrix3 = Eigen::Matrix<S, 3, 3>;

template<class T>
T computeDotAndCrossNorm(const Vector3<T> &a, const Vector3<T> &b) {
    return a.dot(b) + a.cross(b).norm();
}

void computeGradientFD(const Vector3<double> &a, const Vector3<double> &b, Vector3<double> &grad) {

    double h = 1e-5;

    for (int i = 0; i < 3; ++i) {
        Vector3<double> ap = a;
        ap(i) += h;
        Vector3<double> am = a;
        am(i) -= h;

        double ep = computeDotAndCrossNorm(ap, b);
        double em = computeDotAndCrossNorm(am, b);

        grad(i) = (ep-em) / (2.*h);
    }
}

TEST(GenerateCodeAndLoadLib, Gradient) {
    using namespace AutoGen;
    typedef RecType<double> R;
    typedef AutoDiff<R, R> AD;

    // record computation
    Vector3<AD> a, b;
    for (int i = 0; i < 3; ++i) {
        a[i] = R("x[" + std::to_string(i) + "]");
        b[i] = R("x[" + std::to_string(3+i) + "]");
    }

    Vector3<R> grad;
    for (int i = 0; i < 3; ++i) {
        a(i).deriv() = 1.0;
        AD y = computeDotAndCrossNorm(a, b);
        grad(i) = y.deriv();
        a(i).deriv() = 0.0;
    }

    CodeGenerator<double> generator;
    for (int i = 0; i < 3; ++i) {
        grad(i).addToGeneratorAsResult(generator, "y[" + std::to_string(i) + "]");
    }
    generator.sortNodes();
    std::string code = generator.generateCode();

    // and wrap it in a function
    std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
    libCode += code;
    libCode += "}\n";

    // make and load library
    std::string error;
    compute_extern* compute_CG;
    EXPECT_TRUE(buildAndLoad(libCode, compute_CG, "compute_grad", error));

    // test it!
    {
        Vector3<double> a;
        a << 1.2, 3.4, 5.5;
        Vector3<double> b;
        b << 4.2, -0.1, 1.8;
        double x[6];
        for (int i = 0; i < 3; ++i) {
            x[i] = a(i);
            x[3+i] = b(i);
        }

        // compute with CG
        Vector3<double> grad_CG;
        compute_CG(x, grad_CG.data());

        // compute with FD
        Vector3<double> grad_FD;
        computeGradientFD(a, b, grad_FD);

        EXPECT_NEAR(grad_CG.norm(), grad_FD.norm(), 1e-5);
    }
}

////////////////////////////////////////////////////////////////////////// TEST 4

/*
 * Testing: AutoDiff & CodeGenerator & AutoLoad
 * Generate code for gradient and hessian
 */


void computeHessianFD(const Vector3<double> &a, const Vector3<double> &b, Matrix3<double> &hess) {

    double h = 1e-5;

    for (int i = 0; i < 3; ++i) {
        Vector3<double> ap = a;
        ap(i) += h;
        Vector3<double> am = a;
        am(i) -= h;

        Vector3<double> gradp;
        computeGradientFD(ap, b, gradp);
        Vector3<double> gradm;
        computeGradientFD(am, b, gradm);

        for (int j = 0; j < 3; ++j) {
            hess(i,j) = (gradp(j) - gradm(j)) / (2.*h);
        }
    }
}

TEST(GenerateCodeAndLoadLib, GradientAndHessian) {
    using namespace AutoGen;
    typedef RecType<double> R;
    typedef AutoDiff<R, R> AD;
    typedef AutoDiff<AD, AD> ADD;

    // record computation
    Vector3<ADD> a, b;
    for (int i = 0; i < 3; ++i) {
        a[i] = R("x[" + std::to_string(i) + "]");
        b[i] = R("x[" + std::to_string(3+i) + "]");
    }

    CodeGenerator<double> generator;

    // compute gradient and add to code gen
    {
        Vector3<R> grad;
        for (int i = 0; i < 3; ++i) {
            a(i).deriv() = 1.0;
            ADD y = computeDotAndCrossNorm(a, b);
            grad(i) = y.deriv().value();
            a(i).deriv() = 0.0;
        }
        for (int i = 0; i < 3; ++i) {
            grad(i).addToGeneratorAsResult(generator, "y[" + std::to_string(i) + "]");
        }
    }

    //compute hessian and add to code gen
    {
        Matrix3<R> hess;
        for (int i = 0; i < 3; ++i) {
            a(i).deriv() = 1.0;
            for (int j = 0; j < 3; ++j) {
                a(j).value().deriv() = 1.0;
                ADD y = computeDotAndCrossNorm(a, b);
                hess(i,j) = y.deriv().deriv();
                a(j).value().deriv() = 0.0;
            }
            a(i).deriv() = 0.0;
        }

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j)
                hess(i,j).addToGeneratorAsResult(generator, "y[" + std::to_string(3+3*i+j) + "]");
        }
    }


    generator.sortNodes();
    std::string code = generator.generateCode();

    // and wrap it in a function
    std::string libCode = "#include <cmath>\nextern \"C\" void compute_extern(double* x, double* y) {\n;\n";
    libCode += code;
    libCode += "}\n";

    // make and load library
    std::string error;
    compute_extern* compute_CG;
    EXPECT_TRUE(buildAndLoad(libCode, compute_CG, "compute_gradAndHess", error));

    // test it!
    {
        Vector3<double> a;
        a << 1.2, 3.4, 5.5;
        Vector3<double> b;
        b << 4.2, -0.1, 1.8;
        double x[6];
        for (int i = 0; i < 3; ++i) {
            x[i] = a(i);
            x[3+i] = b(i);
        }

        // compute with CG
        double y[12];
        compute_CG(x, y);
        Vector3<double> grad_CG;
        for (int i = 0; i < 3; ++i) {
            grad_CG(i) = y[i];
        }
        Matrix3<double> hess_CG;
        for (int i = 0; i < 9; ++i) {
            hess_CG.data()[i] = y[3+i];
        }

        // compute with FD
        Vector3<double> grad_FD;
        computeGradientFD(a, b, grad_FD);
        Matrix3<double> hess_FD;
        computeHessianFD(a, b, hess_FD);

        EXPECT_NEAR(grad_CG.norm(), grad_FD.norm(), 1e-5);
        EXPECT_NEAR(hess_CG.norm(), hess_FD.norm(), 1e-3);
    }
}
