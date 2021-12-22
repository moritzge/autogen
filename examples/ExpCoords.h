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

};
