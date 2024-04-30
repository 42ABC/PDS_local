//define Dimensions struct, basis of PDS searching

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