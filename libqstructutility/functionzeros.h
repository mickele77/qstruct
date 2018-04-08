#ifndef FUNCTIONZERO_H
#define FUNCTIONZERO_H

double zeroSecant(double x0, double x1, double (*f)(double), double delta, bool *ok, int maxIter = 100);

#endif // FUNCTIONZERO_H
