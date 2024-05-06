//Holds the functions that calculate the error of a given set (ie how far it is from being a PDS)

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
struct ErrorCalc {
  //error from sums = efs
  //given the target PDS dimensions, the pairwise sums of a set, and the set itself, determine the error of the set
  //returns: an int, the error of the set
  virtual int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {return -1;}
  //given the target PDS dimensions, the pairwise sums of a set, and the set itself in the form of a boolean is_member list, determine the error of the set
  //returns: an int, the error of the set
  virtual int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {return -2;}

  virtual std::string name() {return "uninit";}
};

struct L1_error : ErrorCalc {

  int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {
    std::vector<bool> checked(d.n,false); 

    int error = abs(d.k-sums[0]); //checks how many copies of the identity are present
    for (int i : my_set) { //for each element in the candidate set
      error += abs(d.lam-sums[i]); //there should be lambda copies in sums
      checked[i]=true;
    }
    for (int i = 1; i < d.n; i++) { //for each other element
      if (!checked[i]) {
        error += abs(d.mu-sums[i]); //there should be mu copies in sums
      }
    }
    return error;


  }

  int efs_from_member(std::vector<bool>& is_member, Dims& d, std::vector<int>& sums) {

    int error = abs(d.k-sums[0]); //checks how many copies of the identity are present
    for (int i = 1; i < d.n; i++) { //for each nonidentity element
      if (is_member[i]) { //if this element is in the set, compare count against lambda
        error += abs(d.lam-sums[i]); 
      }
      else { //otherwise, compare count against mu
        error += abs(d.mu-sums[i]);
      }
    }
   
    return error;


  }

  
  std::string name() {
    return "l1";
  }

};
//because L2_error is only being used comparatively, we don't take a square root, we just use the sum of squares
struct L2_error : ErrorCalc {

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
      error += temp*temp; // takes the sum of squares instead

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
