//Contains the actual local search function and associated helper functions

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


//picks the random starting set for the local search
//repeatedly select elements, throwing out repeats, until the set is filled
//works well because k is not close to n
//d <- PDS dimensions
//rand_gen <- random # generator
//my_set <- vector in which we place the chosen elements
//is_member <- boolean vector, where is_member[i] is true iff i \in my_set
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

//given a set, calculate its error
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

  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }
 
  return e.efs(d,my_set,sums);

}

//calculate the new sums array after we have swapped an element
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


//Given a group, measure the probability that two elements commute, which is a measure of how nonabelian a group is 
//See (Gallian Contemporary Abstract Algebra 8th edition, Ch24 p.311-312)
double prob_commute(std::vector<std::vector<int>>& ct) {
  int n = ct.size();
  double num_commute = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) { //for each pair of elements
      if (ct[i][j]==ct[j][i]) { //if they commute
        num_commute += 1; //add 1 to the commute counter 
      }
    }
  }
  return num_commute/(n*n);

}


//Do a trial of local search, returns the candidate set and the error of that set. The candidate set is a regular PDS iff the error is 0.
//d <- PDS dimensions
//ct <- Group multiplication (convolution) table
//rand_gen <- Random number generator
//e <- Error calculator object
std::pair<std::vector<int>,int> search(Dims& d, std::vector<std::vector<int>>& ct, std::mt19937& rand_gen, ErrorCalc& e) {

  std::vector<int> my_set(d.k,-1); //my_set is the candidate PDS (the candidate set)
  std::vector<bool> is_member(d.n,false); //is_member[i] is true iff my_set contains i

  select_start_set(d,rand_gen,my_set,is_member); //select random starting values for my_set
  std::vector<int> sums(d.n,0); //sums is a list containing the pairwise sums of my_set. sums[i] is the number of copies of i that appears in my_set^2

  int cur_error = calculate_starting_error(d,ct,my_set,sums, e); //calculate the starting error of my_set
  
  int real_steps = 0; //keep track of how many swaps were made

  bool changed_val = true; //makes sure we keep improving -- if we don't make a swap, we are stuck, end the search
  int new_error = 0; //holds the error of our candidate set after a potential swap 

  while(cur_error > 0 && changed_val) { //while we have not found a PDS but we are still making progress  

    changed_val = false; //so far, a swap has not been implemented
    
    //index and new value pair for mutation
    //mutation.first is index, mutation.second is new group element
    //a mutation is a swap -- the value in my_set[initial_mutation.first] will be replaced with mutation.second
    //this is the mutation we start out with
    //note that of the (n-1)*k possible swaps, we check all of them to see if they reduce the error EXCEPT FOR the initial mutation --
    //future versions of the code should check the initial mutation as well, there is no reason not to
    std::pair<int,int> initial_mutation(rand_gen()%d.k,(rand_gen()%(d.n-1)+1));

    //this is the mutation (swap) we will increment as we try out different swaps
    std::pair<int,int> mutation(initial_mutation.first,initial_mutation.second);
    bool first_time = true; //use of first time variable allows proper looping (check all (n-1)*k possible neighbors)
    // int old_member = -1; // TODO delete
    // int new_error2 = -1; //TODO delete


    while (mutation != initial_mutation || first_time) {
      first_time=false;
    //if the value we are adding is not currently in our set
      if (!is_member[mutation.second]) {
        adjust_sums(d,ct,my_set,sums,mutation,1); //adjust sums by the mutation

        is_member[my_set[mutation.first]] = false; //adjust is_member by the mutation
        is_member[mutation.second]=true;
        new_error = e.efs_from_member(is_member,d,sums); //get the new error //TODO uncomment
        // old_member = my_set[mutation.first];
        // my_set[mutation.first]=mutation.second;//TODO delete
        // new_error2 = error(d,ct,my_set,e); 
        // my_set[mutation.first]=old_member;
        // if (new_error != new_error2) {
        //   std::cout << "BAD" << std::endl;
        //   exit(177);
        // }



        if (new_error < cur_error) { //if our new error is lower
         
          my_set[mutation.first]=mutation.second; //make the mutation permanent 

          
          cur_error=new_error; //our current error is now new error
          real_steps += 1; //increment the amount of steps (swaps) we've made by 1
          changed_val=true; //set to true, we did make a swap

          break; //break as we made a swap to pick a new random initial mutation
        }
        else {
          //undo the mutation
          adjust_sums(d,ct,my_set,sums,mutation,-1);
          //undo the membership change
          is_member[my_set[mutation.first]] = true;
          is_member[mutation.second]=false;
         
        }
      }
      increment(d,mutation); //if the mutation did not work, increment it

    }
  }  

  return std::make_pair(my_set,cur_error); //return the candidate set and its associated error
}

//Do a trial of local search, returns the candidate set and the error of that set. The candidate set is a regular PDS iff the error is 0.
//d <- PDS dimensions
//ct <- Group multiplication (convolution) table
//rand_gen <- Random number generator
//e <- Error calculator object
//this is the version used in most of the paper calculations: the new version (search) is better, use that instead
std::pair<std::vector<int>,int> search_old(Dims& d, std::vector<std::vector<int>>& ct, std::mt19937& rand_gen, ErrorCalc& e) {

  std::vector<int> my_set(d.k,-1); //my_set is the candidate PDS (the candidate set)
  std::vector<bool> is_member(d.n,false); //is_member[i] is true iff my_set contains i

  select_start_set(d,rand_gen,my_set,is_member); //select random starting values for my_set
  std::vector<int> sums(d.n,0); //sums is a list containing the pairwise sums of my_set. sums[i] is the number of copies of i that appears in my_set^2

  int cur_error = calculate_starting_error(d,ct,my_set,sums, e); //calculate the starting error of my_set
  
  int real_steps = 0; //keep track of how many swaps were made

  bool changed_val = true; //makes sure we keep improving -- if we don't make a swap, we are stuck, end the search
  int new_error = 0; //holds the error of our candidate set after a potential swap 

  while(cur_error > 0 && changed_val) { //while we have not found a PDS but we are still making progress
  

    changed_val = false; //so far, a swap has not been implemented
    
    //index and new value pair for mutation
    //mutation.first is index, mutation.second is new group element
    //a mutation is a swap -- the value in my_set[initial_mutation.first] will be replaced with mutation.second
    //this is the mutation we start out with
    //note that of the (n-1)*k possible swaps, we check all of them to see if they reduce the error EXCEPT FOR the initial mutation --
    //future versions of the code should check the initial mutation as well, there is no reason not to
    std::pair<int,int> initial_mutation(rand_gen()%d.k,(rand_gen()%(d.n-1)+1));

    //this is the mutation (swap) we will increment as we try out different swaps
    std::pair<int,int> mutation(initial_mutation.first,initial_mutation.second);
    increment(d,mutation); //increment new val


    while (mutation != initial_mutation) {
    //if the value we are adding is not currently in our set
      if (!is_member[mutation.second]) {
        adjust_sums(d,ct,my_set,sums,mutation,1); //adjust sums by the mutation

        is_member[my_set[mutation.first]] = false; //adjust is_member by the mutation
        is_member[mutation.second]=true;
        new_error = e.efs_from_member(is_member,d,sums); //get the new error


        if (new_error < cur_error) { //if our new error is lower
         
          my_set[mutation.first]=mutation.second; //make the mutation permanent 

          
          cur_error=new_error; //our current error is now new error
          real_steps += 1; //increment the amount of steps (swaps) we've made by 1
          changed_val=true; //set to true, we did make a swap

          break; //break as we made a swap to pick a new random initial mutation
        }
        else {
          //undo the mutation
          adjust_sums(d,ct,my_set,sums,mutation,-1);
          //undo the membership change
          is_member[my_set[mutation.first]] = true;
          is_member[mutation.second]=false;
         
        }
      }
      increment(d,mutation); //if the mutation did not work, increment it

    }
  }  

  return std::make_pair(my_set,cur_error); //return the candidate set and its associated error
}

#endif