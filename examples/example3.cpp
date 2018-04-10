#include <iostream>
#include <chrono>
#include <iomanip>

#include <CodeGenerator.h>
#include <AutoDiff.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

typedef std::chrono::high_resolution_clock Clock;

using namespace AutoGen;
using namespace Eigen;

typedef RecType<double> R;
typedef AutoDiff<R, R> ADRec;
typedef AutoDiff<ADRec, ADRec> ADDRec;

template <class S> using Matrix3 = Matrix<S, 3, 3>;

void computeGradientCG(const Vector3d &a, Vector3d &grad) {
	double v0 = a(1);
	double v1 = a(2);
	double v2 = a(0);
	double v3 = 0.500000;
	double v4 = 4.000000;
	double v5 = v0 + v4;
	double v6 = v5 * v1;
	double v7 = 5.000000;
	double v8 = v1 + v7;
	double v9 = v8 * v0;
	double v10 = v6 - v9;
	double v11 = v10 * v10;
	double v12 = v8 * v2;
	double v13 = 3.000000;
	double v14 = v2 + v13;
	double v15 = v14 * v1;
	double v16 = v12 - v15;
	double v17 = v16 * v16;
	double v18 = v14 * v0;
	double v19 = v5 * v2;
	double v20 = v18 - v19;
	double v21 = v20 * v20;
	double v22 = v17 + v21;
	double v23 = v11 + v22;
	double v24 = sqrt(v23);
	double v25 = v3 / v24;
	double v26 = v8 - v1;
	double v27 = v26 * v16;
	double v28 = v27 + v27;
	double v29 = v0 - v5;
	double v30 = v29 * v20;
	double v31 = v30 + v30;
	double v32 = v28 + v31;
	double v33 = v25 * v32;
	double v34 = v14 * v14;
	double v35 = v5 * v5;
	double v36 = v8 * v8;
	double v37 = v35 + v36;
	double v38 = v34 + v37;
	double v39 = v33 * v38;
	double v40 = v14 + v14;
	double v41 = v40 * v24;
	double v42 = v39 + v41;
	double v43 = v1 - v8;
	double v44 = v43 * v10;
	double v45 = v44 + v44;
	double v46 = v14 - v2;
	double v47 = v46 * v20;
	double v48 = v47 + v47;
	double v49 = v45 + v48;
	double v50 = v25 * v49;
	double v51 = v50 * v38;
	double v52 = v5 + v5;
	double v53 = v52 * v24;
	double v54 = v51 + v53;
	double v55 = v5 - v0;
	double v56 = v55 * v10;
	double v57 = v56 + v56;
	double v58 = v2 - v14;
	double v59 = v58 * v16;
	double v60 = v59 + v59;
	double v61 = v57 + v60;
	double v62 = v25 * v61;
	double v63 = v62 * v38;
	double v64 = v8 + v8;
	double v65 = v64 * v24;
	double v66 = v63 + v65;
	grad(0) = v42;
	grad(1) = v54;
	grad(2) = v66;

}

void computeHessianCG(const Vector3d &a, Matrix3d &hess) {
	double v0 = a(0);
	double v1 = a(1);
	double v2 = a(2);
	double v3 = 3.000000;
	double v4 = v0 + v3;
	double v5 = v4 + v4;
	double v6 = 0.500000;
	double v7 = 4.000000;
	double v8 = v1 + v7;
	double v9 = v8 * v2;
	double v10 = 5.000000;
	double v11 = v2 + v10;
	double v12 = v11 * v1;
	double v13 = v9 - v12;
	double v14 = v13 * v13;
	double v15 = v11 * v0;
	double v16 = v4 * v2;
	double v17 = v15 - v16;
	double v18 = v17 * v17;
	double v19 = v4 * v1;
	double v20 = v8 * v0;
	double v21 = v19 - v20;
	double v22 = v21 * v21;
	double v23 = v18 + v22;
	double v24 = v14 + v23;
	double v25 = sqrt(v24);
	double v26 = v6 / v25;
	double v27 = v11 - v2;
	double v28 = v27 * v17;
	double v29 = v28 + v28;
	double v30 = v1 - v8;
	double v31 = v30 * v21;
	double v32 = v31 + v31;
	double v33 = v29 + v32;
	double v34 = v26 * v33;
	double v35 = v5 * v34;
	double v36 = v27 * v27;
	double v37 = v36 + v36;
	double v38 = v30 * v30;
	double v39 = v38 + v38;
	double v40 = v37 + v39;
	double v41 = v40 * v26;
	double v42 = v25 * v25;
	double v43 = v6 / v42;
	double v44 = v34 * v43;
	double v45 = -v44;
	double v46 = v45 * v33;
	double v47 = v41 + v46;
	double v48 = v4 * v4;
	double v49 = v8 * v8;
	double v50 = v11 * v11;
	double v51 = v49 + v50;
	double v52 = v48 + v51;
	double v53 = v47 * v52;
	double v54 = v35 + v53;
	double v55 = 2.000000;
	double v56 = v55 * v25;
	double v57 = v35 + v56;
	double v58 = v54 + v57;
	double v59 = v8 + v8;
	double v60 = v59 * v34;
	double v61 = v4 - v0;
	double v62 = v61 * v30;
	double v63 = v62 + v62;
	double v64 = v63 * v26;
	double v65 = v2 - v11;
	double v66 = v65 * v13;
	double v67 = v66 + v66;
	double v68 = v61 * v21;
	double v69 = v68 + v68;
	double v70 = v67 + v69;
	double v71 = v26 * v70;
	double v72 = v71 * v43;
	double v73 = -v72;
	double v74 = v73 * v33;
	double v75 = v64 + v74;
	double v76 = v75 * v52;
	double v77 = v60 + v76;
	double v78 = v71 * v5;
	double v79 = v77 + v78;
	double v80 = v11 + v11;
	double v81 = v80 * v34;
	double v82 = v0 - v4;
	double v83 = v82 * v27;
	double v84 = v83 + v83;
	double v85 = v84 * v26;
	double v86 = v8 - v1;
	double v87 = v86 * v13;
	double v88 = v87 + v87;
	double v89 = v82 * v17;
	double v90 = v89 + v89;
	double v91 = v88 + v90;
	double v92 = v26 * v91;
	double v93 = v92 * v43;
	double v94 = -v93;
	double v95 = v94 * v33;
	double v96 = v85 + v95;
	double v97 = v96 * v52;
	double v98 = v81 + v97;
	double v99 = v92 * v5;
	double v100 = v98 + v99;
	double v101 = v45 * v70;
	double v102 = v64 + v101;
	double v103 = v102 * v52;
	double v104 = v78 + v103;
	double v105 = v104 + v60;
	double v106 = v59 * v71;
	double v107 = v65 * v65;
	double v108 = v107 + v107;
	double v109 = v61 * v61;
	double v110 = v109 + v109;
	double v111 = v108 + v110;
	double v112 = v111 * v26;
	double v113 = v73 * v70;
	double v114 = v112 + v113;
	double v115 = v114 * v52;
	double v116 = v106 + v115;
	double v117 = v106 + v56;
	double v118 = v116 + v117;
	double v119 = v80 * v71;
	double v120 = v86 * v65;
	double v121 = v120 + v120;
	double v122 = v121 * v26;
	double v123 = v94 * v70;
	double v124 = v122 + v123;
	double v125 = v124 * v52;
	double v126 = v119 + v125;
	double v127 = v92 * v59;
	double v128 = v126 + v127;
	double v129 = v45 * v91;
	double v130 = v85 + v129;
	double v131 = v130 * v52;
	double v132 = v99 + v131;
	double v133 = v132 + v81;
	double v134 = v73 * v91;
	double v135 = v122 + v134;
	double v136 = v135 * v52;
	double v137 = v127 + v136;
	double v138 = v137 + v119;
	double v139 = v80 * v92;
	double v140 = v86 * v86;
	double v141 = v140 + v140;
	double v142 = v82 * v82;
	double v143 = v142 + v142;
	double v144 = v141 + v143;
	double v145 = v144 * v26;
	double v146 = v94 * v91;
	double v147 = v145 + v146;
	double v148 = v147 * v52;
	double v149 = v139 + v148;
	double v150 = v139 + v56;
	double v151 = v149 + v150;
	hess(0, 0) = v58;
	hess(0, 1) = v79;
	hess(0, 2) = v100;
	hess(1, 0) = v105;
	hess(1, 1) = v118;
	hess(1, 2) = v128;
	hess(2, 0) = v133;
	hess(2, 1) = v138;
	hess(2, 2) = v151;
}

void computeGradientHessianCG(const Vector3d &a, Vector3d &grad, Matrix3d &hess){
	double v0 = a(1);
	double v1 = a(2);
	double v2 = a(0);
	double v3 = 0.500000;
	double v4 = 4.000000;
	double v5 = v0 + v4;
	double v6 = v5 * v1;
	double v7 = 5.000000;
	double v8 = v1 + v7;
	double v9 = v8 * v0;
	double v10 = v6 - v9;
	double v11 = v10 * v10;
	double v12 = v8 * v2;
	double v13 = 3.000000;
	double v14 = v2 + v13;
	double v15 = v14 * v1;
	double v16 = v12 - v15;
	double v17 = v16 * v16;
	double v18 = v14 * v0;
	double v19 = v5 * v2;
	double v20 = v18 - v19;
	double v21 = v20 * v20;
	double v22 = v17 + v21;
	double v23 = v11 + v22;
	double v24 = sqrt(v23);
	double v25 = v3 / v24;
	double v26 = v8 - v1;
	double v27 = v26 * v16;
	double v28 = v27 + v27;
	double v29 = v0 - v5;
	double v30 = v29 * v20;
	double v31 = v30 + v30;
	double v32 = v28 + v31;
	double v33 = v25 * v32;
	double v34 = v14 * v14;
	double v35 = v5 * v5;
	double v36 = v8 * v8;
	double v37 = v35 + v36;
	double v38 = v34 + v37;
	double v39 = v33 * v38;
	double v40 = v14 + v14;
	double v41 = v40 * v24;
	double v42 = v39 + v41;
	double v43 = v1 - v8;
	double v44 = v43 * v10;
	double v45 = v44 + v44;
	double v46 = v14 - v2;
	double v47 = v46 * v20;
	double v48 = v47 + v47;
	double v49 = v45 + v48;
	double v50 = v25 * v49;
	double v51 = v50 * v38;
	double v52 = v5 + v5;
	double v53 = v52 * v24;
	double v54 = v51 + v53;
	double v55 = v5 - v0;
	double v56 = v55 * v10;
	double v57 = v56 + v56;
	double v58 = v2 - v14;
	double v59 = v58 * v16;
	double v60 = v59 + v59;
	double v61 = v57 + v60;
	double v62 = v25 * v61;
	double v63 = v62 * v38;
	double v64 = v8 + v8;
	double v65 = v64 * v24;
	double v66 = v63 + v65;
	double v67 = v40 * v33;
	double v68 = v26 * v26;
	double v69 = v68 + v68;
	double v70 = v29 * v29;
	double v71 = v70 + v70;
	double v72 = v69 + v71;
	double v73 = v72 * v25;
	double v74 = v24 * v24;
	double v75 = v3 / v74;
	double v76 = v33 * v75;
	double v77 = -v76;
	double v78 = v77 * v32;
	double v79 = v73 + v78;
	double v80 = v79 * v38;
	double v81 = v67 + v80;
	double v82 = 2.000000;
	double v83 = v82 * v24;
	double v84 = v67 + v83;
	double v85 = v81 + v84;
	double v86 = v52 * v33;
	double v87 = v46 * v29;
	double v88 = v87 + v87;
	double v89 = v88 * v25;
	double v90 = v50 * v75;
	double v91 = -v90;
	double v92 = v91 * v32;
	double v93 = v89 + v92;
	double v94 = v93 * v38;
	double v95 = v86 + v94;
	double v96 = v50 * v40;
	double v97 = v95 + v96;
	double v98 = v64 * v33;
	double v99 = v58 * v26;
	double v100 = v99 + v99;
	double v101 = v100 * v25;
	double v102 = v62 * v75;
	double v103 = -v102;
	double v104 = v103 * v32;
	double v105 = v101 + v104;
	double v106 = v105 * v38;
	double v107 = v98 + v106;
	double v108 = v62 * v40;
	double v109 = v107 + v108;
	double v110 = v77 * v49;
	double v111 = v89 + v110;
	double v112 = v111 * v38;
	double v113 = v96 + v112;
	double v114 = v113 + v86;
	double v115 = v52 * v50;
	double v116 = v43 * v43;
	double v117 = v116 + v116;
	double v118 = v46 * v46;
	double v119 = v118 + v118;
	double v120 = v117 + v119;
	double v121 = v120 * v25;
	double v122 = v91 * v49;
	double v123 = v121 + v122;
	double v124 = v123 * v38;
	double v125 = v115 + v124;
	double v126 = v115 + v83;
	double v127 = v125 + v126;
	double v128 = v64 * v50;
	double v129 = v55 * v43;
	double v130 = v129 + v129;
	double v131 = v130 * v25;
	double v132 = v103 * v49;
	double v133 = v131 + v132;
	double v134 = v133 * v38;
	double v135 = v128 + v134;
	double v136 = v62 * v52;
	double v137 = v135 + v136;
	double v138 = v77 * v61;
	double v139 = v101 + v138;
	double v140 = v139 * v38;
	double v141 = v108 + v140;
	double v142 = v141 + v98;
	double v143 = v91 * v61;
	double v144 = v131 + v143;
	double v145 = v144 * v38;
	double v146 = v136 + v145;
	double v147 = v146 + v128;
	double v148 = v64 * v62;
	double v149 = v55 * v55;
	double v150 = v149 + v149;
	double v151 = v58 * v58;
	double v152 = v151 + v151;
	double v153 = v150 + v152;
	double v154 = v153 * v25;
	double v155 = v103 * v61;
	double v156 = v154 + v155;
	double v157 = v156 * v38;
	double v158 = v148 + v157;
	double v159 = v148 + v83;
	double v160 = v158 + v159;
	grad(0) = v42;
	grad(1) = v54;
	grad(2) = v66;
	hess(0, 0) = v85;
	hess(0, 1) = v97;
	hess(0, 2) = v109;
	hess(1, 0) = v114;
	hess(1, 1) = v127;
	hess(1, 2) = v137;
	hess(2, 0) = v142;
	hess(2, 1) = v147;
	hess(2, 2) = v160;
}

template<class S>
S compute(const Vector3<S> &a) {
	Vector3<S> b = a+a;
	Vector3<S> c = a+b;
	Vector3<S> d = c.cross(a);
	return c.dot(c) * d.norm();
//	c.dot(c) / (c.norm());
}



void computeGradientFD(const Vector3d &a, Vector3d &grad) {

	double h = 1e-5;

	for (int i = 0; i < 3; ++i) {
		Vector3d ap = a;
		ap(i) += h;
		Vector3d am = a;
		am(i) -= h;

		double ep = compute(ap);
		double em = compute(am);

		grad(i) = (ep-em) / (2.*h);
	}
}

void computeHessianFD(const Vector3d &a, Matrix3d &hess, double h = 1e-5) {

	for (int i = 0; i < 3; ++i) {
		Vector3d ap = a;
		ap(i) += h;
		Vector3d am = a;
		am(i) -= h;

		Vector3d gradp; computeGradientCG(ap, gradp);
		Vector3d gradm; computeGradientCG(am, gradm);

		for (int j = 0; j < 3; ++j) {
			hess(i,j) = (gradp(j)-gradm(j)) / (2.*h);
		}
	}
}

void generateCodeGradient() {

	Vector3<ADRec> a;
	a(0) = ADRec("a(0)");
	a(1) = ADRec("a(1)");
	a(2) = ADRec("a(2)");

	CodeGenerator<double> generator;

	Vector3<R> g;

	for (int i = 0; i < 3; ++i) {
		a(i).deriv() = 1.0;
		ADRec n = compute(a);
		g(i) = n.deriv();
		g(i).addToGeneratorAsResult(generator, "grad(" + std::to_string(i) + ")");
		a(i).deriv() = 0.0;
	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

void generateCodeGradientNew() {
	typedef AutoDiff<R, Vector3<R>> ADRV3;

	Vector3<ADRV3> a;
	for (int i = 0; i < 3; ++i) {
		a(i).value() = R("a("+std::to_string(i)+")");
		Vector3<R> g(0,0,0);
		g(i) = 1;
		a(i).deriv() = g;
	}


	ADRV3 e = compute(a);
	Vector3<R> gradient = e.deriv();
	CodeGenerator<double> generator;

	for (int i = 0; i < 3; ++i) {
		gradient(i).addToGeneratorAsResult(generator, "grad(" + std::to_string(i) + ")");
	}


//	CodeGenerator<double> generator;

//	Vector3<R> g;
//	for (int i = 0; i < 3; ++i) {
//		a(i).deriv() = 1.0;
//		ADRec n = compute(a);
//		g(i) = n.deriv();
//		g(i).addToGeneratorAsResult(generator, "grad(" + std::to_string(i) + ")");
//		a(i).deriv() = 0.0;
//	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

void generateCodeHessian() {

	Vector3<ADDRec> a;
	a(0) = ADDRec("a(0)");
	a(1) = ADDRec("a(1)");
	a(2) = ADDRec("a(2)");
//	a(0) = ADDRec(1);
//	a(1) = ADDRec(2);
//	a(2) = ADDRec(3);

	CodeGenerator<double> generator;

	Matrix3<R> hess;

	for (int i = 0; i < 3; ++i) {
		a(i).deriv().value() = 1.0;
		for (int j = 0; j < 3; ++j) {
			a(j).value().deriv() = 1.0;
			ADDRec f = compute(a);
			hess(i,j) = f.deriv().deriv();
			hess(i,j).addToGeneratorAsResult(generator, "hess(" + std::to_string(i) + ", " + std::to_string(j) + ")");
			a(j).value().deriv() = 0.0;
		}
		a(i).deriv().value() = 0.0;
	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

void generateCodeGradientAndHessian() {

	CodeGenerator<double> generator;

	{
		Vector3<ADRec> a;
		a(0) = ADRec("a(0)");
		a(1) = ADRec("a(1)");
		a(2) = ADRec("a(2)");

		Vector3<R> grad;

		for (int i = 0; i < 3; ++i) {
			a(i).deriv() = 1.0;
			ADRec n = compute(a);
			grad(i) = n.deriv();
			grad(i).addToGeneratorAsResult(generator, "grad(" + std::to_string(i) + ")");
			a(i).deriv() = 0.0;
		}
	}

	{
		Vector3<ADDRec> a;
		a(0) = ADDRec("a(0)");
		a(1) = ADDRec("a(1)");
		a(2) = ADDRec("a(2)");

		Matrix3<R> hess;

		for (int i = 0; i < 3; ++i) {
			a(i).deriv().value() = 1.0;
			for (int j = 0; j < 3; ++j) {
				a(j).value().deriv() = 1.0;
				ADDRec f = compute(a);
				hess(i,j) = f.deriv().deriv();
				hess(i,j).addToGeneratorAsResult(generator, "hess(" + std::to_string(i) + ", " + std::to_string(j) + ")");
				a(j).value().deriv() = 0.0;
			}
			a(i).deriv().value() = 0.0;
		}
	}

	generator.sortNodes();

	std::cout << generator.generateCode() << std::endl;

}

int main(int argc, char *argv[])
{

	generateCodeGradient();
	generateCodeGradientNew();
//	generateCodeHessian();
//	generateCodeGradientAndHessian();

	std::cout << std::setprecision(10);

	// get input
	std::cout << "# args:" << argc <<std::endl;
	Vector3d a;
	a << atof(argv[1]), atof(argv[2]), atof(argv[3]);
	std::cout << "a: " << a.transpose() << std::endl;

	// compute gradient and hessian
	Vector3d gradCG; Matrix3d hessCG;
	computeGradientHessianCG(a, gradCG, hessCG);


	// check gradient
	{
		std::cout << "cg: " << gradCG.transpose() << std::endl;
		Vector3d gradFD; computeGradientFD(a, gradFD);
		std::cout << "fd: " << gradFD.transpose() << std::endl;

		// check to true gradient
		Vector3d gradTrue; gradTrue << -623.8033878, -35.92584956, 551.9516887;
		double gradError = (gradTrue-gradCG).norm();
		std::cout << "\nGradient Check: " << ((gradError < 1e-5) ? "OK!" : "NOT OK!") << std::endl << std::endl;
	}

	// check hessian
	{
		std::cout << "cg: \n" << hessCG << std::endl;
		Matrix3d hessFD; computeHessianFD(a, hessFD);
		std::cout << "fd: \n" << hessFD << std::endl;

		for (int i = 0; i < 10; ++i) {
			double h = pow(10, -i);
			Matrix3d hessFD; computeHessianFD(a, hessFD, h);
			//		std::cout << "fd: \n" << hessFD << std::endl;

			std::cout << "h = " << h << "\t";
			std::cout << "error = " << (hessCG-hessFD).norm() << std::endl;
		}

		Matrix3d hessTrue;
		hessTrue <<     115.670349, -469.7576996,  138.5322532 ,
				-469.7576996,  779.4820693, -358.7141646 ,
				138.5322532, -358.7141646,   337.757419;

		std::cout << "true hess:\n" << hessTrue << std::endl;

		std::cout << "true hess error CG: " << (hessTrue-hessCG).norm() << std::endl;
		std::cout << "true hess error FD: " << (hessTrue-hessFD).norm() << std::endl;

		double hessError = (hessTrue-hessCG).norm();
		std::cout << "\nHessian Check: " << ((hessError < 1e-5) ? "OK!" : "NOT OK!") << std::endl << std::endl;
	}


	//	int n = 1e8;

//	auto t1 = Clock::now();
//	double sum = 0;
//	for (int i = 0; i < n; ++i) {
//		computeGradientCG(a, grad);
//	}
//	std::cout << "sum = " << sum << std::endl;

//	auto t2 = Clock::now();
//	sum = 0;
//	for (int i = 0; i < n; ++i) {
//		computeGradientFD(a, gradFD);
//	}
//	std::cout << "sum = " << sum << std::endl;

//	auto t3 = Clock::now();

//	std::cout << "Delta t2-t1: "
//			  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
//			  << " milliseconds" << std::endl;
//	std::cout << "Delta t3-t2: "
//			  << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()
//			  << " milliseconds" << std::endl;

	return 0;
}
