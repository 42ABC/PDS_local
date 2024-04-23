//g++ -o ac.out -std=c++17 search64.cpp
//./ac.out tables64 64 18 2 6 10 267
//./ac.out tables64 64 18 2 6 10 3

//./ac.out tables64 64 18 2 6 0 267

#include "search_tools.h"
#include<string>

int main(int argc, char* argv[]) {
  int num_groups = atoi(argv[7]);
  std::string folder = argv[1];

  clock_t first_time = std::clock();
  int num_groups_with_pds = 0;
  std::vector<int> with_pds;
  L1_error e1;
  L2_error e2;
  L4_error e4;
  LInf_error einf;
  std::vector<ErrorCalc*> errors;
  errors.push_back(&e1);
  errors.push_back(&e2);
  errors.push_back(&e4);
  errors.push_back(&einf);

  //std::vector<ErrorCalc*> errors = {new L1_error(),new L2_error(),new L4_error(),new LInf_error()};

  //TODO change 55 and 266 to # of groups to check*
  for (int id = 1; id <= num_groups; id++) {

    // if (id % 20 == 0) {
    //   std::cout << "Q" << std::endl;
    // }
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
  //std::vector<std::unique_ptr<ErrorCalc>> errors = {std::make_unique<L1_error>(),std::make_unique<L2_error>(),std::make_unique<L4_error>(),std::make_unique<LInf_error>()};

  std::vector<int> successes(errors.size(),0);
  for (int e_len = 0; e_len < errors.size(); e_len++) {
    ErrorCalc& e = *errors[e_len];
    //std::cout << "Error is " << e.name() << std::endl;

    for (int i = 0; i < NUM_TRIALS; i++) {
      auto result_pair = search(d,ct,rand_gen,e);
      auto result = result_pair.first;
      int my_error = result_pair.second;
      //std::cout << "error: " << my_error << std::endl;
      if (my_error==0) {
        successes[e_len] += 1;
        // for (int i = 0; i < result.size(); i++) {
        //   std::cout << result[i] << " ";
        // }
        // std::cout << std::endl;

      }
    

    // if (i % 50 == 0) {
    //   std::cout << ".";
      // }
    }
  }
  

  double nonabelness = prob_commute(ct);
  // std::cout << std::endl;
  std::cout << "group, " << id << ", probcomm, " << nonabelness << " ";
  for (int index = 0; index < errors.size(); index++) {
    std::cout << errors[index]->name() << ", " << successes[index] << ",";

  }
  std::cout << std::endl;

  // std::cout << "on group " << id << std::endl;

  // std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  // clock_t end_time = std::clock();
  // std::cout << "time elapsed: " << (end_time-start_time)/1'000'000 << "s" << std::endl;


  }
  std::cout << std::endl;

  clock_t last_time = std::clock();
  std::cout << "overall runtime: " << (last_time-first_time)/1'000'000 << std::endl;
  //std::cout << "groups found: " << num_groups_with_pds << std::endl;
  for (int i =0 ; i < with_pds.size(); i++) {
    std::cout << with_pds[i] << " ";
  }
  std::cout << std::endl;

  // //give back memory
  // for (int i = 0; i < errors.size(); i++) {
  //   delete errors[i];
  // }



}

