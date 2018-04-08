#include "varplusmath.h"

#include "point2dplus.h"
#include "doubleplus.h"

#include <cmath>

double DyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    double ret = ( p2->y->valueNormal() - p1->y->valueNormal() ) *
            ( p2->z->valueNormal() + p1->z->valueNormal() ) / 2.0 ;
    return ret;
}

double yDyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) * \
            ( p2->y->valueNormal() * ( 2.0 * p2->z->valueNormal() + p1->z->valueNormal() ) + \
             p1->y->valueNormal() * ( p2->z->valueNormal() + 2.0 * p1->z->valueNormal() ) ) / 6.0 );
}

double y2DyDz(Point2DPlus * p1, Point2DPlus * p2) {
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) * \
            (pow(p2->y->valueNormal(), 2.0) * (3.0 * p2->z->valueNormal() + p1->z->valueNormal()) + \
             2.0 * p2->y->valueNormal() * p1->y->valueNormal() * (p2->z->valueNormal()+p1->z->valueNormal()) + \
             pow(p1->y->valueNormal(), 2.0) * (3.0 * p1->z->valueNormal() + p2->z->valueNormal())) / 12.0 );
}

double y3DyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) * \
            ( pow(p2->y->valueNormal(), 3.0 ) * (4.0 * p2->z->valueNormal()+p1->z->valueNormal()) +
             pow(p2->y->valueNormal(), 2.0 ) * p1->y->valueNormal() * (3.0 * p2->z->valueNormal()+ 2.0 * p1->z->valueNormal()) + \
             p2->y->valueNormal() * pow(p1->y->valueNormal(), 2.0 ) * (2.0 * p2->z->valueNormal()+ 3.0 * p1->z->valueNormal()) + \
             pow(p1->y->valueNormal(), 3.0 ) * (p2->z->valueNormal()+4.0*p1->z->valueNormal()) ) / 20.0 );
}

double zDyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) *	\
            ( p2->z->valueNormal() * p2->z->valueNormal() + \
             p2->z->valueNormal() * p1->z->valueNormal() + \
             p1->z->valueNormal() * p1->z->valueNormal()) / 6.0 );
}

double z2DyDz(Point2DPlus * p1, Point2DPlus * p2) {
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) *	\
            ( p2->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() + \
             p2->z->valueNormal() * p2->z->valueNormal() * p1->z->valueNormal() + \
             p2->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() + \
             p1->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() ) / 12.0 );
}

double z3DyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) *	\
            ( p2->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() + \
             p2->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() * p1->z->valueNormal() + \
             p2->z->valueNormal() * p2->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() + \
             p2->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() + \
             p1->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() ) / 20.0 ) ;
}

double yzDyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) * \
            ( pow( p1->z->valueNormal(), 2.0) * ( 3.0*p1->y->valueNormal() + p2->y->valueNormal()) + \
             2.0 * p1->z->valueNormal() * p2->z->valueNormal() * (p1->y->valueNormal() + p2->y->valueNormal()) + \
             pow( p2->z->valueNormal(), 2.0) * ( p1->y->valueNormal() + 3.0*p2->y->valueNormal())) / 24.0 );
}

double yz2DyDz(Point2DPlus * p1, Point2DPlus * p2) {
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) * \
            ( p1->z->valueNormal() * p1->z->valueNormal() * p1->z->valueNormal() * ( 4.0*p1->y->valueNormal() + p2->y->valueNormal()) + \
             p1->z->valueNormal() * p1->z->valueNormal() * p2->z->valueNormal() * ( 3.0*p1->y->valueNormal() + 2.0*p2->y->valueNormal()) + \
             p1->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() * ( 2.0*p1->y->valueNormal() + 3.0*p2->y->valueNormal()) + \
             p2->z->valueNormal() * p2->z->valueNormal() * p2->z->valueNormal() * ( p1->y->valueNormal() + 4.0*p2->y->valueNormal())) / 60.0 );
}

double y2zDyDz(Point2DPlus * p1, Point2DPlus * p2)
{
    return ( ( p2->y->valueNormal() - p1->y->valueNormal() ) *	    \
            ( pow(p2->z->valueNormal(), 2.0) * (6.0 * pow(p2->y->valueNormal(), 2.0) + 3.0 * p2->y->valueNormal() * p1->y->valueNormal() + pow(p1->y->valueNormal(), 2.0)) + \
             p2->z->valueNormal() * p1->z->valueNormal() * (3.0 * pow(p2->y->valueNormal(), 2.0) + 4.0 * p2->y->valueNormal() * p1->y->valueNormal() + 3.0 * pow(p1->y->valueNormal(), 2.0)) + \
             pow(p1->z->valueNormal(), 2.0) * (pow(p2->y->valueNormal(), 2.0) + 3.0 * p2->y->valueNormal() * p1->y->valueNormal() + 6.0 * pow(p1->y->valueNormal(), 2.0)) ) / 60.0 );
}
