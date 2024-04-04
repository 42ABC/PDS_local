#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>

struct Dims {
  int n;
  int k;
  int lam;
  int mu;
};


//checks error of some set. Error 0 <=> PDS
//TODO more efficient way? 
int error(Dims d, std::vector<std::vector<int>>& ct, std::vector<int>& my_set) {
  std::vector<int> sums(d.n,0);
  for (int i = 0; i < d.k; i++) {
    for (int j = 0; j < d.k; j++) {
      sums[ct[my_set[i]][my_set[j]]] += 1;
    }
  }
  std::vector<bool> checked(d.n,false);
  int error = 0;
  error += abs(d.k-sums[0]);
  checked[0]=true;
  for (int i : my_set) {
    error += abs(d.lam-sums[i]);
    checked[i]=true;
  }
  for (int i = 1; i < d.n; i++) {
    if (!checked[i]) {
      error += abs(d.mu-sums[i]);

    }
  }
  return error;


}

std::vector<int> search(Dims d, std::vector<std::vector<int>>& ct) {
  std::srand(time(0)); //std::rand()%n for int between 0 and n
  std::vector<int> my_set(d.k,-1);
  std::vector<bool> is_member(d.n,0); //memory inefficient, but small domain so okay
  for (int i = 0; i < d.k; i++) {
    my_set[i]=std::rand()%d.n;
    is_member[my_set[i]]=1;
  }
  // std::cout << "Start is ";
  // for (int i = 0; i < my_set.size(); i++) {
  //   std::cout << my_set[i] << " ";
  // }
  // std::cout << std::endl;

  int cur_error = error(d,ct,my_set);
  //std::cout << "Error of start is " << cur_error << std::endl;

  int neighbor_counter = 0;
  int real_steps = 0;

  while(true) {
    //TODO adjust this stop condition (randomized so not a guarantee)
    if (neighbor_counter >= 10*d.n*d.n) {
    //  std::cout << "no more moves" << std::endl;
      break;
    }
    int index_mutate = std::rand()%d.k;
    int new_val = std::rand()%d.n;
    std::vector<int> cand_set(d.k,-1);
    //if the value we are adding is not currently in our set
    if (!is_member[new_val]) {
      for (int i = 0; i < d.k; i++) {
        cand_set[i]=my_set[i];

      }
      cand_set[index_mutate]=new_val;
      int new_error = error(d,ct,cand_set);
      if (new_error < cur_error) {
        is_member[my_set[index_mutate]]=0;
        my_set[index_mutate]=new_val;
        is_member[new_val]=1;
        cur_error=new_error;
        real_steps += 1;
        if (cur_error==0) {
          std::cout << "Found a PDS!" << std::endl;
          std::cout << "real steps is " << real_steps << std::endl;
          break;
        }
      }



    }
    neighbor_counter += 1;


  }

  

  return my_set;


}

int main() {

  std::string fname = "table_512-d8cubed_safe.txt";
  Dims d;
  d.n=512;
  d.k=196;
  d.lam=6;
  d.mu=10;
  std::ifstream file(fname);

  std::cout << "Reading from file: " << std::endl;

  int n, group_id;

  file >> n >> group_id;
  std::cout << "n, group_id: " << n << " " << group_id << std::endl;

  std::vector<std::vector<int>> ct(n,std::vector<int>(n,0)); //conv table

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      file >> ct[i][j];
      ct[i][j] -= 1; //for zero indexing
    }
   // std::cout << "finished line " << i << std::endl;

  }

  // std::cout << "printing table: " << std::endl;

  // for (int i = 0; i < ct.size(); i++) {
  //   for (int j = 0; j < ct.size(); j++) {
  //     std::cout << ct[i][j] << " ";
  //   }
  //   std::cout << std::endl;
  // }

  // auto result = search(d,ct);

  // std::cout << "resulting set has error: " << error(d,ct,result) << std::endl;
  // std::cout << "Result is: ";
  // for (int i = 0; i < result.size(); i++) {
  //   std::cout << result[i] << " ";
  // }
  // std::cout << std::endl;

  int NUM_TRIALS = 1000;
  for (int i = 0; i < NUM_TRIALS; i++) {
    auto result = search(d,ct);
    int my_error = error(d,ct,result);
    if (my_error==0) {
      std::cout << "Result is: ";
      for (int i = 0; i < result.size(); i++) {
        std::cout << result[i] << " ";
      }
      std::cout << std::endl;

    }

    if (i % 50 == 0) {
      std::cout << ".";
    }
  }
  std::cout << std::endl;



}

