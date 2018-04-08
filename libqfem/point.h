#ifndef POINT_H
#define POINT_H

#include <Eigen/Core>
#include <QList>

class Point : public Eigen::Vector4d
{
public:
    Point( double, double, double);
    QList<int> indexes;

    static int DOF();
};

#endif // POINT_H
