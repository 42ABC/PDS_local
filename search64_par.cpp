//Search groups of order 64 for PDSs, in order to evaluate different error functions. This version of the code is parallelized for increased speed. 
//If it does not compile due to a parlay error, try adding -pthread option 
//g++ -o ac.out -I parlaylib/include -std=c++17 search64_par.cpp
//./ac.out folder_with_tables n k lambda mu NUM_TRIALS num_groups_to_check
//./ac.out tables64 64 18 2 6 100 267

#include "search_tools.h"
#include<string>

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/delayed_sequence.h"
#include "parlay/random.h"
#include "parlay/internal/get_time.h"
#include<mutex>

int main(int argc, char* argv[]) {
  int num_groups = atoi(argv[7]);
  int NUM_TRIALS = atoi(argv[6]);
  Dims d(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
  std::string folder = argv[1];
  parlay::internal::timer my_timer; //parlay (parallel library) equivalent of timer
  my_timer.start();
  //put different error calculators into array
  L1_error e1;
  L2_error e2;
  L4_error e4;
  std::vector<ErrorCalc*> errors;
  errors.push_back(&e1);
  errors.push_back(&e2);
  errors.push_back(&e4);
  //mutex for controlling access to cout, for printing
  std::mutex m;
  parlay::parallel_for(1,num_groups+1,[&] (size_t id) { //for each group, in parallel
    std::string fname = folder + "/table" + std::to_string(id) + ".txt"; //load the multiplication (convolution) table for that group
    std::ifstream file(fname);
    int n, group_id;
    file >> n >> group_id;
    std::vector<std::vector<int>> ct(d.n,std::vector<int>(d.n,0)); //conv table
    int temp;
    for (int i = 0; i < d.n; i++) {
      for (int j = 0; j < d.n; j++) {
        file >> temp;
        ct[i][j] = temp - 1; //for zero indexing
      }
    }
    std::random_device rd; 
    std::mt19937 rand_gen(rd()); //random number generators
    std::vector<int> successes(errors.size(),0); //keep track of number of successes for each error type
    for (int e_len = 0; e_len < errors.size(); e_len++) {  //for each error type
      ErrorCalc& e = *errors[e_len];
      for (int i = 0; i < NUM_TRIALS; i++) { //for the number of trials of search we want to run
        auto result_pair = search(d,ct,rand_gen,e); //run local search
        auto result = result_pair.first;
        int my_error = result_pair.second;
        if (my_error==0) successes[e_len] += 1; //if we found a PDS, increment # of successes by 1
      }
      
    }
    double nonabelness = prob_commute(ct); //calculate probability of 2 elements commuting
    m.lock(); //take the mutex to print out answers
    std::cout << "group, " << id << ", probcomm, " << nonabelness << ", ";
    for (int index = 0; index < errors.size(); index++) {
      std::cout << errors[index]->name() << ", " << successes[index] << ",";

    }
    std::cout << std::endl;
    m.unlock(); //give the mutex back

  });

  std::cout << "total time: " << my_timer.next_time() << std::endl;
  
}

