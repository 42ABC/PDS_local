#include "search_tools.h"

//g++ -o ac.out -std=c++17 search.cpp
//./ac.out GROUP_FILE n k lambda mu NUM_TRIALS
//./ac.out tables64/table226.txt 64 18 2 6 1000
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
  for (int i = 0; i < d.n; i++) { //read in conv table
    for (int j = 0; j < d.n; j++) {
      file >> temp;
    
      ct[i][j] = temp - 1; //for zero indexing
    }

  }

  std::random_device rd;
  std::mt19937 rand_gen(rd());
  L2_error e = L2_error(); //choose error function here 
  double avg_error = 0;

  int NUM_TRIALS = atoi(argv[6]);
  int successes = 0;
  for (int i = 0; i < NUM_TRIALS; i++) { //for each trial
    auto result_pair = search_old(d,ct,rand_gen,e); //do the search //TODO change back to search
    auto result_set = result_pair.first; //get out the potential PDS
    int my_error = result_pair.second; //get out the error
    avg_error += my_error;
   
    if (my_error==0) { //if error is 0, we found a PDS!
      successes += 1;
      std::cout << "PDS: " << std::endl; 
      
      for (int i = 0; i < result_set.size(); i++) { //we print the PDS we found
        std::cout << result_set[i] << " ";
      }
      std::cout << std::endl;
      //break; //uncomment if one wants max 1 success per group

    }
    
    

   
  }
  std::cout << std::endl;
  if (successes==0) {
    std::cout << "No PDSs found from trials: " << NUM_TRIALS << std::endl;
  }

  std::cout << "succ: " << successes <<", total: " << NUM_TRIALS << std::endl;

  clock_t end_time = std::clock();
  std::cout << "time elapsed: " << (end_time-start_time)/1'000'000 << "s" << std::endl;

  std::cout << "avg error: " << avg_error/NUM_TRIALS << std::endl;



}

