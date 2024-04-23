#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>
#include<chrono>
#include<algorithm>
#include<random>

//https://www.win.tue.nl/~aeb/graphs/srg/srgtab1-50.html #for srg parms
//https://people.maths.bris.ac.uk/~matyd/GroupNames/index.html #for group ids

struct Dims {
  int n;
  int k;
  int lam;
  int mu;
  Dims(int n, int k, int lam, int mu) : n(n),k(k),lam(lam),mu(mu) {}
  Dims() {}
};

//ErrorCalc is object that holds error calculation function 
//This interface requires efs to be defined
//TODO define with virtual? the list of errors not working if virtual
struct ErrorCalc {
  virtual int efs(Dims& d, std::vector<int>& my_set, std::vector<int>& sums) {return -1;}
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
  std::string name() {
    return "l1";
  }

};
struct L2_error : ErrorCalc {

  //error from squared sums
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
        error += temp * temp;
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

  std::string name() {
    return "LINF";
  }

};


//repeatedly select elements, throwing out repeats, until the set is filled
//works well because k is not close to n
void select_start_set(Dims& d,std::mt19937& rand_gen, std::vector<int>& my_set, std::vector<bool>& is_member) {
 

  int new_member=0;

  for (int i = 0; i < d.k; i++) {
    while (is_member[new_member]==1 || new_member==0) {
      new_member = ((rand_gen())%(d.n-1))+1; //force no zero value
    }
    my_set[i]=new_member;
    is_member[my_set[i]]=1;
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

//adjust the sums resulting in adding a new element
//fac = 1 means adding the mutation, fac = -1 means undoing it
void adjust_sums(Dims& d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set, std::vector<int>& sums, std::pair<int,int>& mutation, int fac) {
  for (int i = 0; i < d.k; i++) {
    if (i != mutation.first) {
      //TODO handling nonabelian correctly? I think so
      sums[ct[my_set[i]][mutation.second]] += fac;
      sums[ct[mutation.second][my_set[i]]] += fac;
      sums[ct[my_set[i]][my_set[mutation.first]]] -= fac;
      sums[ct[my_set[mutation.first]][my_set[i]]] -= fac;

    }
  }

  //mulitplying the element we are removing with itself
  sums[ct[my_set[mutation.first]][my_set[mutation.first]]] -= fac;
  //multiply the element we are adding with itself
  sums[ct[mutation.second][mutation.second]] += fac;

}

//apply the mutation to the base potential PDS (my_set)
void apply_mutation(std::vector<int>& my_set, std::vector<bool>& is_member, std::pair<int,int>& mutation) {
  is_member[my_set[mutation.first]]=0;
  my_set[mutation.first]=mutation.second;
  is_member[mutation.first]=1;

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

  while(cur_error > 0 && changed_val) {
   // std::cout << "mid error is " << cur_error << std::endl;

    changed_val = false;
    
    //index and new value pair for mutation
    std::pair<int,int> initial_mutation(std::rand()%d.k,(std::rand()%(d.n-1)+1));
    //std::cout << "mutation is " << mutation.first << " " << mutation.second << std::endl;
    //mutation.first is index, mutation.second is new group element
    std::pair<int,int> mutation(initial_mutation.first,initial_mutation.second);
    increment(d,mutation); //increment new val

    while (mutation != initial_mutation) {
    //if the value we are adding is not currently in our set
      if (!is_member[mutation.second]) {

        adjust_sums(d,ct,my_set,sums,mutation,1);

        int new_error = e.efs(d,my_set,sums);

        if (new_error < cur_error) {
         // std::cout << "made an improvement" << std::endl;
          apply_mutation(my_set,is_member,mutation);
          
          cur_error=new_error;
          real_steps += 1;
          changed_val=true;

          break;
        }
        else {
          //undo the mutation
          adjust_sums(d,ct,my_set,sums,mutation,-1);
        }


      }
    increment(d,mutation);


    }

  }

  

  return std::make_pair(my_set,cur_error);


}
