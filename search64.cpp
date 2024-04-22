//g++ -o ac.out -std=c++17 search64.cpp
//./ac.out tables64 64 18 2 6 100 267
#include "search_tools.h"
#include<string>

int main(int argc, char* argv[]) {
  int num_groups = atoi(argv[7]);
  std::string folder = argv[1];

  clock_t first_time = std::clock();
  int num_groups_with_pds = 0;
  std::vector<int> with_pds;

  //TODO change 55 and 266 to # of groups to check*
  for (int id = 1; id <= num_groups; id++) {

    if (id % 20 == 0) {
      std::cout << "Q" << std::endl;
    }
    clock_t start_time = std::clock();


    std::string fname = folder + "/table" + std::to_string(id) + ".txt";

    Dims d(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
  
    std::ifstream file(fname);

    int n, group_id;

    file >> n >> group_id;
   // std::cout << "n, group_id: " << n << " " << group_id << std::endl;



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

  int NUM_TRIALS = atoi(argv[6]);
  int successes = 0;
  for (int i = 0; i < NUM_TRIALS; i++) {
    auto result_pair = search(d,ct,rand_gen);
    auto result = result_pair.first;
    int num_steps = result_pair.second;
    int my_error = error(d,ct,result);
    //std::cout << "error: " << my_error << std::endl;
    if (my_error==0) {
      successes += 1;
      // for (int i = 0; i < result.size(); i++) {
      //   std::cout << result[i] << " ";
      // }
      // std::cout << std::endl;

    }
    

    // if (i % 50 == 0) {
    //   std::cout << ".";
    // }
  }
  if (successes > 0) {
    num_groups_with_pds += 1;
    with_pds.push_back(id);
  }
  // std::cout << std::endl;

  // std::cout << "on group " << id << std::endl;

  // std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  // clock_t end_time = std::clock();
  // std::cout << "time elapsed: " << (end_time-start_time)/1'000'000 << "s" << std::endl;


  }
  std::cout << std::endl;

  clock_t last_time = std::clock();
  std::cout << "overall runtime: " << (last_time-first_time)/1'000'000 << std::endl;
  std::cout << "groups found: " << num_groups_with_pds << std::endl;
  for (int i =0 ; i < with_pds.size(); i++) {
    std::cout << with_pds[i] << " ";
  }
  std::cout << std::endl;



}

