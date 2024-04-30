#include "dims.h"
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>
#include<chrono>
#include<algorithm>
#include<random>

#ifndef ERROR_H
#define ERROR_H
//ErrorCalc is object that holds error calculation function 
//This interface requires efs to be defined
//TODO define with virtual? the list of errors not working if virtual
struct ErrorCalc {
  virtual int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {return -1;}
  virtual int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {return -2;}

  virtual std::string name() {return "uninit";}
};

struct L1_error : ErrorCalc {

  //error from sums = efs
  int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {
    std::vector<bool> checked(d.n,false); //TODO avoid repeated memory usage here? 

    int error = abs(d.k-sums[0]);
    for (int i : my_set) {
      error += abs(d.lam-sums[i]);
      checked[i]=true;
    }
    for (int i = 1; i < d.n; i++) {
      if (!checked[i]) {
        error += abs(d.mu-sums[i]);
      }
    }
    return error;


  }

  //error from sums = efs
  int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {

    int error = abs(d.k-sums[0]);
    for (int i = 1; i < d.n; i++) {
      if (is_member[i]) {
        error += abs(d.lam-sums[i]);
      }
      else {
        error += abs(d.mu-sums[i]);
      }
    }
   
    return error;


  }

  
  std::string name() {
    return "l1";
  }

};
struct L2_error : ErrorCalc {

  //error from squared sums
  int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {

    int error = abs(d.k-sums[0]);
    error *= error;
    int temp = 0;
    for (int i = 1; i < d.n; i++) {
      if (is_member[i]) {
        temp = abs(d.lam-sums[i]);

      }
      else {
        temp = abs(d.mu-sums[i]);

      }
      error += temp*temp;

    }
   
    return error;


  }

  //error from sums = efs
  int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {
    std::vector<bool> checked(d.n,false); //TODO avoid repeated memory usage here? 

    int error = abs(d.k-sums[0]);
    error *= error;
    int temp = 0;
    
    for (int i : my_set) {
      temp = abs(d.lam-sums[i]);
      error += temp*temp;
      checked[i]=true;
    }
    for (int i = 1; i < d.n; i++) {
      if (!checked[i]) {
        temp = abs(d.mu-sums[i]);
        error += temp*temp;
      }
    }
    return error;


  }


  std::string name() {
    return "L2";
  }

};

struct L4_error : ErrorCalc {

  //error from l4 error sums
  int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {
    std::vector<bool> checked(d.n,false); //TODO avoid repeated memory usage here? 


    int error = abs(d.k-sums[0]);
    int temp = error*error;
    int t2 = 0;

    error = temp*temp;

    for (int i : my_set) {
      temp = abs(d.lam-sums[i]);
      t2 = temp*temp;
      error += t2*t2;
      checked[i]=true;
    }
    for (int i = 1; i < d.n; i++) {
      if (!checked[i]) {
        temp = abs(d.mu-sums[i]);
        t2 = temp*temp;
        error += t2*t2;
      }
    }
    return error;


  }

   //error from quartic sums
  int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {

    int error = abs(d.k-sums[0]);
    int temp = error * error;
    error = temp*temp;
    int t2 =0;

    for (int i = 1; i < d.n; i++) {
      if (is_member[i]) {
        temp = abs(d.lam-sums[i]);
        

      }
      else {
        temp = abs(d.mu-sums[i]);

      }
      t2 = temp*temp;
      error += t2*t2;

    }
   
    return error;


  }


  std::string name() {
    return "L4";
  }

};

struct LInf_error : ErrorCalc {

  //error from sums = efs (with inf norm)
  int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {
    std::vector<bool> checked(d.n,false); //TODO avoid repeated memory usage here? 

    int error = abs(d.k-sums[0]);
    for (int i : my_set) {
      error = std::max(abs(d.lam-sums[i]),error);
      checked[i]=true;
    }
    for (int i = 1; i < d.n; i++) {
      if (!checked[i]) {
        error = std::max(abs(d.mu-sums[i]),error);
      }
    }
    return error;


  }


  //error from sums = efs
  int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {

    int error = abs(d.k-sums[0]);
    for (int i = 1; i < d.n; i++) {
      if (is_member[i]) {
        error = std::max(error,abs(d.lam-sums[i]));
      }
      else {
        error = std::max(error,abs(d.mu-sums[i]));
      }
    }
   
    return error;


  }

  


  std::string name() {
    return "LINF";
  }



};

#endif //ERROR_H
