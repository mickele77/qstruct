#include "functionzeros.h"

#include <cmath>

double zeroSecant( double x0, double x1, double (*f)( double ), double delta, bool *ok, int maxIter ){
    if( x0 < x1 ){
        double fx0 = (*f)(x0);
        double fx1 = (*f)(x1);

        // c e' l'approssimazione corrente della radice
        double c = x1;
        double fc = fx1;
        if( fabs(fx1) > fabs(fx0) ){
            c = x0;
            fc = fx0;
        }
        if( fabs(fc) <= delta ){
            *ok = true;
            return c;
        }
        int nIter = 0;
        while( (fabs(fc) > delta) && nIter < maxIter ){
            double fpc = (fx1-fx0) / (x1/x0);
            if( fpc == 0.0 ){
                break;
            }
            x0 = x1;
            fx0 = fx1;
            x1 = x1 - fx1/fpc;
            fx1 = (*f)(x1);
            if( fabs(fx1) < fabs(fx0) ){
                c = x1;
                fc = fx1;
            } else {
                c = x0;
                fc = fx0;
            }

            nIter++;
        }
        if( fabs(fc) <= delta ){
            *ok = true;
            return c;
        }
    }
    *ok = false;
    return 0.0;
}
