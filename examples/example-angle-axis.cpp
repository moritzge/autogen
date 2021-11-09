#include <iostream>
#include <chrono>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <RecType.h>
#include <Tensors.h>

#include "angle-axis-cg.h"

using namespace AutoGen;
using namespace dde::math;

template <typename Type, int Size> using Vector = Eigen::Matrix<Type, Size, 1>;
template <typename Type> using Vector3 = Eigen::Matrix<Type, 3, 1>;

template <typename Type, int Rows, int Cols> using Matrix = Eigen::Matrix<Type, Rows, Cols>;
template <typename Type> using Matrix3 = Eigen::Matrix<Type, 3, 3>;

template<class T>
Matrix3<T> skew_sym(const Vector3<T> &v){
    Matrix3<T> v_hat;
    v_hat << 0, -v(2),  v(1),
        v(2),     0, -v(0),
        -v(1),  v(0),     0;
    return v_hat;
};

template<class T>
Vector3<T> vector_from_skew_symmetric(const Matrix3<T> &S) {
    return Vector3<T>(S(2, 1), S(0, 2), S(1, 0));
}

struct AngleAxis
{
    template<class T>
    static Matrix3<T> mul(Matrix3<T> a, Matrix3<T> b){
        Matrix3<T> r;
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                r(j,k) = a.row(j).dot(b.col(k).transpose());
        return r;
    }

    template<class T>
    static Vector3<T> mul(Matrix3<T> a, Vector3<T> b){
        Vector3<T> r;
        for (int j = 0; j < 3; ++j)
            r[j] = a.row(j).dot(b);
        return r;
    }

    template<class T>
    static Matrix3<T> R(const Vector3<T> &theta) {
        T t = theta.norm();
        Vector3<T> v = theta / t;
        Matrix3<T> v_ = skew_sym(v);
        Matrix3<T> tmp = mul(v_, v_);


        return Matrix3<T>::Identity() + sin(t)*v_ + ((T)2*sin(0.5*t)*sin(0.5*t))*tmp;
    }

    template<class T>
    static Tensor3<T,3,3,3> dR(const Vector3<T> &theta) {

        Tensor3<T,3,3,3> dR_;

        T t = theta.norm();
        Vector3<T> v  = theta;
        Matrix3<T> vx = skew_sym(v);

        Matrix3<T> R_ = R<T>(theta);


        for (int i = 0; i < 3; ++i) {
            Vector3<T> e_i = Vector3<T>::Unit(i);

            Matrix3<T> t0 = mul(vx, Matrix3<T>(Matrix3<T>::Identity() - R_));

            Matrix3<T> t1 = skew_sym(Vector3<T>(v[i]*v + mul(t0,e_i) ))/ (t*t);

            dR_[i] = mul(t1,R_);
        }

        return dR_;
    }



    template<class T>
    static Vector3<T> theta(const Matrix3<T> &R)
    {
        // log map
//        T eps = 1e-5;
        T t = acos((R.trace() - (T)1.0) / (T)2.0);
        T sinc_theta = /*(t < eps) ? 1 :*/ t / ((T)2.0*sin(t));
        Matrix3<T> S = (R - R.transpose());
        return sinc_theta * Vector3<T>{R(2,1)-R(1,2), R(0,2)-R(2,0), R(1,0)-R(0,1)};
    }

};

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
            Matrix3<AD> R = AngleAxis::R<AD>(v);
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
            Tensor3<AD,3,3,3> dR = AngleAxis::dR<AD>(v);
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
                Tensor3<ADD,3,3,3> dR = AngleAxis::dR<ADD>(v);
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
                Vector3<AD> theta = AngleAxis::theta<AD>(R);
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
    generateCode_dtheta();

//    {
//        const Vector3d v = {1,0,0};
//        auto dR = dR_cg(v);
//        auto dR_ = AngleAxis::dR<double>(v);

//        Tensor3d3 dR_fd = Tensor3d3::Zero();
//        {
//            const double h = 1e-8;
//            for (int i = 0; i < 3; ++i) {
//                Vector3d dq = Vector3d::Zero();
//                dq[i] = h;
//                dR_fd[i] = (AngleAxis::R<double>(v+dq) - AngleAxis::R<double>(v-dq)) / (2*h);
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
//                ddR_fd[i] = (AngleAxis::dR<double>(v+dq) - AngleAxis::dR<double>(v-dq)) / (2*h);
//            }
//        }

//        std::cout << ddR << std::endl;
//        std::cout << (ddR - ddR_fd).norm() << std::endl;

//    }

    {
        const Matrix3d R = AngleAxis::R(Vector3d{1,0,.1});
        auto dtheta = cg::dtheta(R);

        Tensor4d<3,1,3,3> dtheta_fd = Tensor4d<3,1,3,3>::Zero();
        {
            const double h = 1e-8;
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    Matrix3d dR = Matrix3d::Zero();
                    dR(i,j) = h;
                    dtheta_fd[i][j] = (AngleAxis::theta<double>(R+dR) - AngleAxis::theta<double>(R-dR)) / (2*h);
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
