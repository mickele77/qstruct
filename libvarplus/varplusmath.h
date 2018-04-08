#ifndef VARPLUSMATH_H
#define VARPLUSMATH_H

class Point2DPlus;

double DyDz( Point2DPlus * p1, Point2DPlus * p2);
double yDyDz( Point2DPlus * p1, Point2DPlus * p2);
double y2DyDz( Point2DPlus * p1, Point2DPlus * p2);
double y3DyDz( Point2DPlus * p1, Point2DPlus * p2);
double zDyDz( Point2DPlus * p1, Point2DPlus * p2);
double z2DyDz( Point2DPlus * p1, Point2DPlus * p2);
double z3DyDz( Point2DPlus * p1, Point2DPlus * p2);
double yzDyDz( Point2DPlus * p1, Point2DPlus * p2);
double yz2DyDz( Point2DPlus * p1, Point2DPlus * p2);
double y2zDyDz( Point2DPlus * p1, Point2DPlus * p2);

#endif // VARPLUSMATH_H
