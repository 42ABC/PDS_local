//parallelized version of search
//if does not compile (parlay error), add -pthread
//g++ -o ac.out -std=c++17 -I parlaylib/include search_parallel.cpp
//./ac.out ctpath n k lam mu num_trials
//./ac.out tables64/table226.txt 64 28 12 12 1000

#include "search_tools.h"

//parlay functionality
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/delayed_sequence.h"
#include "parlay/random.h"
#include "parlay/internal/get_time.h"
#include<mutex>

int main(int argc, char* argv[]) {

  parlay::internal::timer my_timer; //parlay version of timer
  my_timer.start();

  std::string fname = argv[1];
  std::cout << "filename is " << fname << std::endl;

  Dims d(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
 
  std::ifstream file(fname);

  int n, group_id;

  file >> n >> group_id;
  std::cout << "n, group_id: " << n << " " << group_id << std::endl;


  std::vector<std::vector<int>> ct(d.n,std::vector<int>(d.n,0)); //conv table
  int temp;
  for (int i = 0; i < d.n; i++) { //read in convolution table
    for (int j = 0; j < d.n; j++) {
      file >> temp;
    
      ct[i][j] = temp - 1; //for zero indexing
    }

  }

 

  L2_error e = L2_error(); //instantiate to error of choice

  int NUM_TRIALS = atoi(argv[6]);
  bool success = false; //if we found at least one PDS, success will be true
  std::mutex m; //control access to PDS printing

  //if deadlocks, can try adding conservative flag (see CMU parlay parallel_for docs) 
  parlay::parallel_for(0, NUM_TRIALS,[&] (size_t i) { //run i=0,1,2,...,NUM_TRIALS-1 in parallel 
    std::random_device rd;
    std::mt19937 rand_gen(rd()); //random number generator
    

    auto result_pair = search(d,ct,rand_gen,e); //do search
    auto result_set = result_pair.first; //get output
    int my_error = result_pair.second;
    if (my_error==0) { //if we found a PDS
      //use a mutex to make sure that two threads don't try to print at the same time
      //because finding a PDS is rare, using a mutex doesn't lose much efficiency
      m.lock(); 
      success = true;
      std::cout << "PDS: " << std::endl; //print out the PDS
      
      for (int index = 0; index < result_set.size(); index++) {
        std::cout << result_set[index] << " ";
      }
      std::cout << std::endl;
    
      m.unlock(); //give back the mutex

    }
    

  });

  std::cout << std::endl;
  if (!success) {
    std::cout << "No PDSs found from " << NUM_TRIALS << " trials." << std::endl;
  }

  std::cout << "parlay timer " << my_timer.next_time() << std::endl;



}

