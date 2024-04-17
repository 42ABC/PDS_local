#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>
#include<chrono>
#include<algorithm>
#include<random>

struct Dims {
  int n;
  int k;
  int lam;
  int mu;
  Dims(int n, int k, int lam, int mu) : n(n),k(k),lam(lam),mu(mu) {}
  Dims() {}
};

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


//repeatedly select elements, throwing out repeats, until the set is filled
//works well because k is not close to n
void select_start_set(Dims& d,std::mt19937& rand_gen, std::vector<int>& my_set, std::vector<bool>& is_member) {
 

  int new_member=0;

  for (int i = 0; i < d.k; i++) {
    //TODO is this the best way to generate a random set? <- function that generates d.k elements all at once? 
    while (is_member[new_member]==1 || new_member==0) {
      new_member = ((rand_gen())%(d.n-1))+1; //force no zero value
    }
    my_set[i]=new_member;
    is_member[my_set[i]]=1;
  }

}



//checks error of some set. Error 0 <=> PDS
//TODO more efficient way? 
int error(Dims d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set) {
  std::vector<int> sums(d.n,0);
  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }
  std::vector<bool> checked(d.n,false);
  int error = 0;
  error += abs(d.k-sums[0]);
  checked[0]=true;
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
int calculate_starting_error(Dims& d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set, std::vector<int>& sums) {
  std::vector<bool> checked(d.n,false);

 //calculate starting error, which we will transform from
  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      //std::cout << "ct is " << ct[my_set[i]][my_set[j]] << std::endl;
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }
 
  int cur_error = abs(d.k-sums[0]);
  for (int i : my_set) {
    cur_error += abs(d.lam-sums[i]);
    checked[i]=true;
  }
  for (int i = 1; i < d.n; i++) {
    if (!checked[i]) {
      cur_error += abs(d.mu-sums[i]);
    }
  }

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

//return real_steps as well as PDS
std::pair<std::vector<int>,int> search(Dims& d, std::vector<std::vector<int>>& ct, std::mt19937& rand_gen) {

  std::vector<int> my_set(d.k,-1);
  std::vector<bool> is_member(d.n,false);

  select_start_set(d,rand_gen,my_set,is_member);
  std::vector<int> sums(d.n,0);

  int cur_error = calculate_starting_error(d,ct,my_set,sums);

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

        int new_error = efs(d,my_set,sums);

        if (new_error < cur_error) {
         // std::cout << "made an improvement" << std::endl;
          apply_mutation(my_set,is_member,mutation);
          
          cur_error=new_error;
          real_steps += 1;
          changed_val=true;
          if (cur_error==0) {
            std::cout << "Found a PDS!" << std::endl;
            std::cout << "real steps is " << real_steps << std::endl;
          }

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

//g++ -o ac.out -std=c++17 search.cpp
//./ac.out conv_table_64-226.txt 64 18 2 6
//g++ -o ac.out -std=c++17 search.cpp table_512-d8cubed_safe.txt 512 70 6 10
//./ac.out table_512-d8cubed_safe.txt 512 70 6 10
int main(int argc, char* argv[]) {

  std::srand(time(0)); //std::rand()%n for int between 0 and n

  clock_t start_time = std::clock();

  std::string fname = argv[1];
  std::cout << "filename is " << fname << std::endl;

  Dims d(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
 
  std::ifstream file(fname);


  std::cout << "Reading from file: " << std::endl;

  int n, group_id;

  file >> n >> group_id;
 std::cout << "n, group_id: " << n << " " << group_id << std::endl;


  std::vector<std::vector<int>> ct(d.n,std::vector<int>(d.n,0)); //conv table
  int temp;
  for (int i = 0; i < d.n; i++) {
    for (int j = 0; j < d.n; j++) {
      file >> temp;
    
      ct[i][j] = temp - 1; //for zero indexing
    }
   // std::cout << "finished line " << i << std::endl;

  }

  // std::cout << "printing table: " << std::endl;

  // for (int i = 0; i < ct.size(); i++) {
  //   for (int j = 0; j < ct.size(); j++) {
  //     std::cout << ct[i][j] << " ";
  //   }
  //   std::cout << std::endl;
  // }
  // exit(3);

  // auto result = search(d,ct);

  // std::cout << "resulting set has error: " << error(d,ct,result) << std::endl;
  // std::cout << "Result is: ";
  // for (int i = 0; i < result.size(); i++) {
  //   std::cout << result[i] << " ";
  // }
  // std::cout << std::endl;


  // std::cout << "practice increment" << std::endl;
  // std::pair<int,int> hi(17,51);
  // std::cout << hi.first << " " << hi.second << std::endl;
  // increment(d,hi);
  // std::cout << hi.first << " " << hi.second << std::endl;
  // exit(1);

  int NUM_TRIALS = 10000;
  int successes = 0;
  for (int i = 0; i < NUM_TRIALS; i++) {
    auto result_pair = search(d,ct);
    auto result = result_pair.first;
    int num_steps = result_pair.second;
    int my_error = error(d,ct,result);
    std::cout << "error: " << my_error << std::endl;
    if (my_error==0) {
      successes += 1;
      std::cout << "With reported error : " << num_steps << ", Result is: ";
      for (int i = 0; i < result.size(); i++) {
        std::cout << result[i] << " ";
      }
      std::cout << std::endl;

    }
    

    if (i % 50 == 0) {
      std::cout << ".";
    }
  }
  std::cout << std::endl;

  std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  clock_t end_time = std::clock();
  std::cout << "time elapsed: " << (end_time-start_time) << std::endl;



}

