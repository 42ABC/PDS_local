//Given a list of viable PDS dimensions (n,k,lambda,mu), test all groups of order n on that dimension

//If it does not compile due to a parlay error, try adding -pthread option 
//g++ -o ac.out -std=c++17 -I parlaylib/include searchMe.cpp
//s = start index (inclusive) from dimension list
//t = end index (exclusive) from dimension list
//MULTIPLIER <- for each group, MULTIPLIER * n^2 trials will be run. Thus Multiplier controls how many trials are run. 
//M n^2 trials are run because the search space is bigger for groups with higher order, and so more trials are warranted
//./ac.out s t MULTIPLIER
//./ac.out 0 133 5


#include "search_tools.h"

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/delayed_sequence.h"
#include "parlay/random.h"
#include "parlay/internal/get_time.h"
#include<mutex>
#include<algorithm>


int main(int argc, char* argv[]) {

  std::cout << "Program started" << std::endl;

  parlay::internal::timer my_timer;
  my_timer.start();


 
  std::ifstream file("run_input/HigherDims.txt"); //read in viable parameter values

  std::cout << "Opened HIGHER dims file" << std::endl;

  int num_dims;
  file >> num_dims;

  int t1,t2,t3,t4;
  std::vector<Dims> allDims;
  for (int i = 0; i < num_dims; i++) {
    file >> t1 >> t2 >> t3 >> t4;
    allDims.push_back(Dims(t1,t2,t3,t4));


  }
  file.close();

  std::ifstream file2("run_input/num_groups.txt"); //read in how many groups there are of each order
  int num_group_sizes;
  file2 >> num_group_sizes;
  std::vector<int> num_groups_of_order(num_group_sizes+1,0);
  for (int i = 0; i < num_group_sizes; i++) {
    file2 >> t1 >> t2;
    num_groups_of_order[t1]=t2;
  }
  file2.close();

  //search some of the Dims (allow this work to break up over splits)
  int startDim = atoi(argv[1]);
  int endDim = atoi(argv[2]);
  int MULTIPLIER = atoi(argv[3]); //controls how many trials to run

  std::mutex m; // for even printing

  //Note: we load in a group more than once -- once per valid dimension, in fact -- but loading is (relatively) quick so this is okay (the parallelization is overall smoother like this)

  parlay::parallel_for(startDim,endDim,[&] (size_t dim_index) { //in parallel, look at each PDS dimension tuple 
    Dims& d = allDims[dim_index];

    parlay::parallel_for(1,num_groups_of_order[d.n]+1,[&] (size_t g_id) { //in parallel, look at each group that could create this type of PDS


      std::ifstream g_file("tablesAll/table" + std::to_string(d.n) + "_" + std::to_string(g_id) + ".txt"); //read in the group table

      g_file >> t1 >> t2; //table heading info


      std::vector<std::vector<int>> ct(d.n,std::vector<int>(d.n,0)); //conv table
      int temp;
      for (int i = 0; i < d.n; i++) {
        for (int j = 0; j < d.n; j++) {
          g_file >> temp;
        
          ct[i][j] = temp - 1; //for zero indexing
        }

      }
      g_file.close();



      std::random_device rd;
      std::mt19937 rand_gen(rd());
      L2_error e = L2_error(); //instantiate to L2 error because it works the best, per experiment

      int NUM_TRIALS = MULTIPLIER * d.n * d.n; //more trials for higher order groups
      bool found_success = false;
      for (int i = 0; i < NUM_TRIALS; i++) { //for each trial
        auto result_pair = search(d,ct,rand_gen,e); //do search
        auto result_set = result_pair.first;
        int my_error = result_pair.second;  //use the incremental error calculated in the search already
     
        if (my_error==0) { //if we found a PDS
          found_success = true; //mark we found a PDS
          std::sort(result_set.begin(),result_set.end()); //sort PDS for nicer output
          m.lock(); //take mutex for printing
          std::cout << "PDS(" << d.n << "," << d.k << "," << d.lam << "," << d.mu <<")" << " SmGrp(" << d.n << "," << g_id << "): ";
          
          for (int j = 0; j < result_set.size(); j++) {
            std::cout << result_set[j] << " ";
          }
          std::cout << ".";
          std::cout << "Tri: " << i+1 << std::endl;
          m.unlock(); //give back mutex
          break; //max 1 success per parameter-group pair

        }
      }
      if (!found_success) { //if we found no PDSs, indicate this
        m.lock();
        std::cout << "PDS(" << d.n << "," << d.k << "," << d.lam << "," << d.mu <<")" << " SmGrp(" << d.n << "," << g_id << "): ";
        std::cout << "None in Tri: " << NUM_TRIALS << std::endl;
        m.unlock();

      }
        



    });
    

  });


 
}