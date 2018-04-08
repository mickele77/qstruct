#include "point.h"

Point::Point( double xx, double yy, double zz):
    Eigen::Vector4d(xx, yy, zz, 0.0){

    for( int i=0; i < DOF(); i++ ){
        indexes.append(0);
    }
}

int Point::DOF(){
    return 6;
}
