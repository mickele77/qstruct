#ifndef EIGENCORE_H
#define EIGENCORE_H

#include <Eigen/Core>

typedef  Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::RowMajor> VectorXd;
typedef  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::RowMajor> > MapVectorXd;

typedef  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXd;
typedef  Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> > MapMatrixXd;

#endif // EIGENCORE_H
