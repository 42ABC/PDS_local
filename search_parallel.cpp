//parallelized version of search
//g++ -o ac.out -std=c++17 -I parlaylib/include search_parallel.cpp
// ./ac.out ctpath n k lam mu num_tri
// ./ac.out group_tables/table85_1.txt 85 14 3 2 1000
// ./ac.out group_tables/conv_table_64-226.txt 64 28 12 12 1000
//./ac.out group_tables/conv_table_64-226.txt 64 18 2 6 
// ./ac.out group_tables/table81_12.txt 81 40 19 20 10000
#include "search_tools.h"

//parlay info 
//use -I parlaylib/include
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/delayed_sequence.h"
#include "parlay/random.h"
#include "parlay/internal/get_time.h"

int main(int argc, char* argv[]) {

  clock_t start_time = std::clock();
  parlay::internal::timer my_timer;
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
  for (int i = 0; i < d.n; i++) {
    for (int j = 0; j < d.n; j++) {
      file >> temp;
    
      ct[i][j] = temp - 1; //for zero indexing
    }

  }


  

 

  L1_error e = L1_error(); //instantiate to L1 error 

  int NUM_TRIALS = atoi(argv[6]);
  bool success = false;
  std::mutex m; //control access to PDS printing

  parlay::parallel_for(0, NUM_TRIALS,[&] (size_t i) {
    std::random_device rd;
    std::mt19937 rand_gen(rd());
    

    auto result_pair = search(d,ct,rand_gen,e);
    auto result_set = result_pair.first;
    int num_steps = result_pair.second;
    int my_error = error(d,ct,result_set,e);
    //std::cout << "error: " << my_error << std::endl;
    if (my_error==0) {
      m.lock();
      success = true;
      std::cout << "PDS: " << std::endl;
      
      for (int i = 0; i < result_set.size(); i++) {
        std::cout << result_set[i] << " ";
      }
      std::cout << std::endl;
      m.unlock();

    }
    

  });

  std::cout << std::endl;
  if (!success) {
    std::cout << "No PDSs found from " << NUM_TRIALS << " trials." << std::endl;
  }

 // std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  clock_t end_time = std::clock();
  std::cout << "parlay timer " << my_timer.next_time() << std::endl;
  std::cout << "time elapsed: " << (end_time-start_time)/1'000'000 << "s" << std::endl;



}

