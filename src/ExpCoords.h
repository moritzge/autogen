#include <AutoDiff.h>
#include <Tensors.h>

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

struct ExpCoords
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
    static Tensor4<T,3,3,3,3> ddR(const Vector3<T> &theta) {

        Tensor4<T,3,3,3,3> ddR_;

        // compute gradient and add to code gen
        {
            typedef AutoDiff<double, double> AD;

            // record computation
            Vector3<AD> v = theta;

            Tensor4<AD, 3,3,3,3> ddR;
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
        }

        return ddR_;
    }


    template<class T>
    static Vector3<T> theta(const Matrix3<T> &R)
    {
        // log map
//        T eps = 1e-5;
        T t = acos((R.trace() - (T)1.0) / (T)2.0);
        T sinc_theta = /*(t < eps) ? 1 :*/ t / ((T)2.0*sin(t));
//        Matrix3<T> S = (R - R.transpose());
        return sinc_theta * Vector3<T>{R(2,1)-R(1,2), R(0,2)-R(2,0), R(1,0)-R(0,1)};
    }

};
