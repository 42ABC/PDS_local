//define Dimensions struct, basis of PDS searching (dimensions = parameters)
//https://www.win.tue.nl/~aeb/graphs/srg/srgtab1-50.html #for srg parms
//https://people.maths.bris.ac.uk/~matyd/GroupNames/index.html #for group ids

#ifndef DIMS_H
#define DIMS_H
struct Dims {
  int n;
  int k;
  int lam;
  int mu;
  Dims(int n, int k, int lam, int mu) : n(n),k(k),lam(lam),mu(mu) {}
  Dims() {}
};
#endif //DIMS_H