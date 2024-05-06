//This code uses different error functions to find PDSs in groups of order 64.
//The point of this code is to compare different error functions to find which are most effective. 
//g++ -o ac.out -std=c++17 search64.cpp
//./.acout FOLDER n k lam mu NUM_TRIALS num_groups
//FOLDER contains all tables of that group size
//the code will check groups with id {1,...,num_groups} for PDSs
//./ac.out tables64 64 18 2 6 10 267

#include "search_tools.h"
#include<string>

int main(int argc, char* argv[]) {
  int num_groups = atoi(argv[7]);
  std::string folder = argv[1];

  clock_t first_time = std::clock();

  L1_error e1;
  L2_error e2;
  L4_error e4;
  std::vector<ErrorCalc*> errors; //an array of different errors to use
  errors.push_back(&e1);
  errors.push_back(&e2);
  errors.push_back(&e4);


  for (int id = 1; id <= num_groups; id++) { 

    std::string fname = folder + "/table" + std::to_string(id) + ".txt";

    Dims d(atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
  
    std::ifstream file(fname);

    int n, group_id;

    file >> n >> group_id;

    std::vector<std::vector<int>> ct(d.n,std::vector<int>(d.n,0)); //conv table
    int temp;
    for (int i = 0; i < d.n; i++) { //read in group table
      for (int j = 0; j < d.n; j++) {
        file >> temp;
      
        ct[i][j] = temp - 1; //for zero indexing (GAP indexes by 1, but C++ indexes by 0)
      }

    }

  
  std::random_device rd;
  std::mt19937 rand_gen(rd()); //random number generator
  int NUM_TRIALS = atoi(argv[6]);

  std::vector<int> successes(errors.size(),0);
  for (int e_len = 0; e_len < errors.size(); e_len++) { //for each error function
    ErrorCalc& e = *errors[e_len]; //extract the error we are using

    for (int i = 0; i < NUM_TRIALS; i++) { //for each trial
      auto result_pair = search(d,ct,rand_gen,e); //run a trial of local search
      auto result = result_pair.first;
      int my_error = result_pair.second;
      if (my_error==0) { //if we found a PDS, increase the success counter by 1
        successes[e_len] += 1;
        // for (int i = 0; i < result.size(); i++) { //uncomment if one wants the PDS found to be printed
        //   std::cout << result[i] << " ";
        // }
        // std::cout << std::endl;

      }
    
    }
   
  }
  
  //calculate the probability of 2 elements in the group commuting, a measure of how nonabelian a group is 
  double nonabelness = prob_commute(ct); 
  std::cout << "group, " << id << ", probcomm, " << nonabelness << ", ";
  for (int index = 0; index < errors.size(); index++) {
    std::cout << errors[index]->name() << ", " << successes[index] << ",";

  }
  std::cout << std::endl;

  }
  std::cout << std::endl;

  clock_t last_time = std::clock();
  std::cout << "overall runtime: " << (last_time-first_time)/1'000'000 << std::endl;


}

