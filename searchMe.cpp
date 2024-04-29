#include "search_tools.h"

#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/delayed_sequence.h"
#include "parlay/random.h"
#include "parlay/internal/get_time.h"
#include<mutex>
#include<algorithm>

//TODO use optimization flag? 
//g++ -o ac.out -std=c++17 -I parlaylib/include searchMe.cpp
//./ac.out 0 133
int main(int argc, char* argv[]) {

  std::cout << "Program started" << std::endl;

  parlay::internal::timer my_timer;
  my_timer.start();


 
  std::ifstream file("run_input/AllDims.txt");

  std::cout << "Opened all dims file" << std::endl;

  int num_dims;
  file >> num_dims;

  int t1,t2,t3,t4;
  std::vector<Dims> allDims;
  for (int i = 0; i < num_dims; i++) {
    file >> t1 >> t2 >> t3 >> t4;
    allDims.push_back(Dims(t1,t2,t3,t4));


  }
  file.close();

  std::ifstream file2("run_input/num_groups.txt");
  int num_group_sizes;
  file2 >> num_group_sizes;
  std::vector<int> num_groups_of_order(num_group_sizes+1,0);
  for (int i = 0; i < num_group_sizes; i++) {
    file2 >> t1 >> t2;
    num_groups_of_order[t1]=t2;
  }



  //search some of the Dims (allow this work to break up over splits)
  int startDim = atoi(argv[1]);
  int endDim = atoi(argv[2]);

  std::mutex m; // for even printing

  //Note: we load in a group more than once -- once per valid dimension, in fact -- but loading is (relatively) quick so this is okay (the parallelization is overall smoother like this)

  parlay::parallel_for(0,allDims.size(),[&] (size_t dim_index) {
    Dims& d = allDims[dim_index];

    parlay::parallel_for(1,num_groups_of_order[d.n]+1,[&] (size_t g_id) {


      std::ifstream g_file("tablesAll/table" + std::to_string(d.n) + "_" + std::to_string(g_id) + ".txt");


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
      L2_error e = L2_error(); //instantiate to L2 error 

      int NUM_TRIALS = 5 * d.n * d.n; //more trials for higher order groups
      bool found_success = false;
      for (int i = 0; i < NUM_TRIALS; i++) {
        auto result_pair = search(d,ct,rand_gen,e);
        auto result_set = result_pair.first;
        int my_error = result_pair.second;  //use the incremental error calculated in the search already
        //std::cout << "error: " << my_error << std::endl;
        if (my_error==0) {
          found_success = true;
          std::sort(result_set.begin(),result_set.end()); //sort to be nicew
          m.lock();
          std::cout << "PDS(" << d.n << "," << d.k << "," << d.lam << "," << d.mu <<")" << " SmGrp(" << d.n << "," << g_id << "): ";
          
          for (int i = 0; i < result_set.size(); i++) {
            std::cout << result_set[i] << " ";
          }
          std::cout << ".";
          std::cout << "Tri: " << i << std::endl;
          m.unlock();
          break; //only 1 success per run
        //break; //only 1 success per run 

        }
      }
      if (!found_success) {
        m.lock();
        std::cout << "PDS(" << d.n << "," << d.k << "," << d.lam << "," << d.mu <<")" << " SmGrp(" << d.n << "," << g_id << "): ";
        std::cout << "None in Tri: " << NUM_TRIALS << std::endl;
        m.unlock();

      }
        



    });
    

  });


 
}