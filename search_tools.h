
//https://www.win.tue.nl/~aeb/graphs/srg/srgtab1-50.html #for srg parms
//https://people.maths.bris.ac.uk/~matyd/GroupNames/index.html #for group ids

#ifndef SEARCH_TOOLS_H
#define SEARCH_TOOLS_H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>
#include<chrono>
#include<algorithm>
#include<random>

#include "dims.h"
#include "error_calc.h"


//repeatedly select elements, throwing out repeats, until the set is filled
//works well because k is not close to n
void select_start_set(Dims& d,std::mt19937& rand_gen, std::vector<int>& my_set, std::vector<bool>& is_member) {
 

  int new_member=0;

  for (int i = 0; i < d.k; i++) {
    while (is_member[new_member]==1 || new_member==0) {
      new_member = ((rand_gen())%(d.n-1))+1; //force no zero value
    }
    my_set[i]=new_member;
    is_member[my_set[i]]=true;
  }

}

//sum and error calculation in one function
int error(Dims d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set, ErrorCalc& e) {
  std::vector<int> sums(d.n,0);
  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }


  return e.efs(d,my_set,sums);
  
}

//increment a pair, with carrying (a.first mod k, a.second mod n)
void increment(Dims d, std::pair<int,int>& a) {
  if (a.second==d.n-1 && a.first==d.k-1) {
    a.first=0;
    a.second=1; //skips 0 (don't assign identity to a PDS)
  }
  else if (a.second == d.n-1) {
    a.second=1; //skips 0
    a.first += 1;
  }
  else {
    a.second += 1;
  }
 

}

//calculate starting error and put initial tallies in sums array
int calculate_starting_error(Dims& d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set, std::vector<int>& sums, ErrorCalc& e) {

 //calculate starting error, which we will transform from
  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      //std::cout << "ct is " << ct[my_set[i]][my_set[j]] << std::endl;
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }
 
 
  return e.efs(d,my_set,sums);

}

//(D-a+b)^2 = D^2-Da+Db-aD+a^2-ab+bD-ba+b^2
void adjust_sums(Dims& d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set, std::vector<int>& sums, std::pair<int,int>& mutation, int fac) {
  //-Da - aD + bD + Db
  for (int i = 0; i < d.k; i++) {
    sums[ct[my_set[i]][my_set[mutation.first]]] -= fac;
    sums[ct[my_set[mutation.first]][my_set[i]]] -= fac;
    sums[ct[my_set[i]][mutation.second]] += fac;
    sums[ct[mutation.second][my_set[i]]] += fac;

  }
  //mulitplying the element we are removing with itself (a^2)
  sums[ct[my_set[mutation.first]][my_set[mutation.first]]] += fac;
  //multiply the element we are adding with itself (b^2)
  sums[ct[mutation.second][mutation.second]] += fac;
  //cross multiply! (-ab - ba)
  sums[ct[my_set[mutation.first]][mutation.second]] -= fac;
  sums[ct[mutation.second][my_set[mutation.first]]] -= fac;

}


//given a group, measure the probability that two elements commute, which is a measure of how nonabelian a group is (Gallian Ch24)
double prob_commute(std::vector<std::vector<int>>& ct) {
  int n = ct.size();
  double num_commute = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (ct[i][j]==ct[j][i]) {
        num_commute += 1;
      }
    }
  }
  return num_commute/(n*n);

}

//returns PDS as well as error
std::pair<std::vector<int>,int> search(Dims& d, std::vector<std::vector<int>>& ct, std::mt19937& rand_gen, ErrorCalc& e) {

  std::vector<int> my_set(d.k,-1);
  std::vector<bool> is_member(d.n,false);

  select_start_set(d,rand_gen,my_set,is_member);
  std::vector<int> sums(d.n,0);

  int cur_error = calculate_starting_error(d,ct,my_set,sums, e);
  

  int real_steps = 0;

  bool changed_val = true; //makes sure we keep improving
  int new_error = 0;

  while(cur_error > 0 && changed_val) {
  

    changed_val = false;
    
    //index and new value pair for mutation
    std::pair<int,int> initial_mutation(rand_gen()%d.k,(rand_gen()%(d.n-1)+1));
    //std::cout << "mutation is " << mutation.first << " " << mutation.second << std::endl;
    //mutation.first is index, mutation.second is new group element
    std::pair<int,int> mutation(initial_mutation.first,initial_mutation.second);
    increment(d,mutation); //increment new val

    //sanity checks/tests
    // int true_error = error(d,ct,my_set,e);
    // if (true_error != cur_error) {
    //   std::cout << "BAD!" <<std::endl;
    //   exit(51);
    // }
    // for (int i = 0; i < d.k; i++) {
    //   if (is_member[my_set[i]] != true) {
    //     std::cout << "BAD IS MEMBER" << std::endl;
    //     exit(53);
    //   }
    // }
    // int test_num_mems = 0;
    // for (int i = 0; i < d.n; i++) {
    //   if (is_member[i]==true) test_num_mems += 1;
    // }
    // if (test_num_mems != d.k || my_set.size() != d.k) {
    //   std::cout << "Bad num mems" << std::endl;
    //   exit(54);
    // }

    //std::cout << "cur error: " << cur_error << std::endl;
    

    while (mutation != initial_mutation) {
    //if the value we are adding is not currently in our set
      if (!is_member[mutation.second]) {

        

        adjust_sums(d,ct,my_set,sums,mutation,1);

        is_member[my_set[mutation.first]] = false;
        is_member[mutation.second]=true;
        new_error = e.efs_from_member(is_member,d,sums);

        //std::cout << "new error? " << new_error << std::endl;

        if (new_error < cur_error) {
         // std::cout << "made an improvement" << std::endl;
         // apply_mutation(my_set,is_member,mutation); //TAKE OUT
          my_set[mutation.first]=mutation.second; //mutate my_set

          
          cur_error=new_error;
          real_steps += 1;
          changed_val=true;

          break;
        }
        else {
          //undo the mutation
          adjust_sums(d,ct,my_set,sums,mutation,-1);
          //undo the membership change
          is_member[my_set[mutation.first]] = true;
          is_member[mutation.second]=false;
         
        }


      }
      increment(d,mutation);


    }

  }

 //std::cout << cur_error << std::endl;

  

  return std::make_pair(my_set,cur_error);


}

#endif