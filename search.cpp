#include "search_tools.h"

//g++ -o ac.out -std=c++17 search.cpp
//./ac.out conv_table_64-226.txt 64 18 2 6
//./ac.out conv_table_64-226.txt 64 28 12 12 1000
//g++ -o ac.out -std=c++17 search.cpp 
//./ac.out table_512-d8cubed_safe.txt 512 70 6 10
//./ac.out table_512_ea.txt 512 70 6 10 1000
//./ac.out table_36_c2d9_safe.txt 36 10 4 2 10000 (no exist)
//./ac.out table_36_c2d9_safe.txt 36 21 10 15 10000
//./ac.out table_144_8233_safe.txt 144 52 16 20 1000
//./ac.out table_144_8233_safe.txt 144 33 12 6 1000
//./ac.out group_tables/table_64_ea_safe.txt 64 28 12 12 1000
int main(int argc, char* argv[]) {

  clock_t start_time = std::clock();

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

  std::random_device rd;
  std::mt19937 rand_gen(rd());
  L1_error e = L1_error(); //instantiate to L1 error 

  int NUM_TRIALS = atoi(argv[6]);
  int successes = 0;
  for (int i = 0; i < NUM_TRIALS; i++) {
    auto result_pair = search(d,ct,rand_gen,e);
    auto result_set = result_pair.first;
    int num_steps = result_pair.second;
    int my_error = error(d,ct,result_set,e);
    //std::cout << "error: " << my_error << std::endl;
    if (my_error==0) {
      successes += 1;
      // for (int i = 0; i < result_set.size(); i++) {
      //   std::cout << result_set[i] << " ";
      // }
      // std::cout << std::endl;

    }
    

    if (i % 50 == 0) {
      std::cout << ".";
    }
  }
  std::cout << std::endl;

  std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  clock_t end_time = std::clock();
  std::cout << "time elapsed: " << (end_time-start_time)/1'000'000 << "s" << std::endl;



}

