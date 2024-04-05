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

//increment a pair, with carrying (a.first mod k, a.second mod n)
void increment(Dims d, std::pair<int,int>& a) {
  if (a.second==d.n-1 && a.first==d.k-1) {
    a.first=0;
    a.second=0;
  }
  else if (a.second == d.n-1) {
    a.second=0;
    a.first += 1;
  }
  else {
    a.second += 1;
  }
 

}

//go down the most possible in a single step 
std::vector<int> steep_search(Dims d, std::vector<std::vector<int>>& ct) {

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

  int real_steps = 0;

  bool changed_val = true; //makes sure we keep improving

  while(cur_error > 0 && changed_val) {
    std::cout << "mid error is " << cur_error << std::endl;

    changed_val = false;
    
    //index and new value pair for mutation
    std::pair<int,int> mutation(std::rand()%d.k,std::rand()%d.n);
    //std::cout << "mutation is " << mutation.first << " " << mutation.second << std::endl;
    std::pair<int,int> new_val(mutation.first,mutation.second);
    increment(d,new_val);

    std::vector<int> cand_set(d.k,-1);
    for (int i = 0; i < d.k; i++) {
      cand_set[i]=my_set[i];
    }
    std::pair<int,int> best_change(-1,-1);
   
    int best_error = cur_error;
    while (new_val != mutation) {
    //if the value we are adding is not currently in our set
      if (!is_member[new_val.second]) {
        
        cand_set[new_val.first]=new_val.second;
        int new_error = error(d,ct,cand_set); //idea: calculate error differential only (becuase full error too expensive) TODO 
        if (new_error < best_error) {
         // std::cout << "made an improvement" << std::endl;
          
          best_error = new_error;
          best_change.first = new_val.first;
          best_change.second = new_val.second;
          changed_val=true;
         
        }

        cand_set[new_val.first] = my_set[new_val.first]; //reset candidate 



    }
    increment(d,new_val);
  //  std::cout << "incremented is " << new_val.first << " " << new_val.second << std::endl;


  }
  if (best_error < cur_error) {
    is_member[my_set[best_change.first]]=0;
    my_set[best_change.first]=best_change.second;
    is_member[best_change.first]=1;
    cur_error = best_error;

  }
  else { //ran out of improvements
    break;
  }
 

}

  

  return my_set;


}

std::vector<int> search(Dims d, std::vector<std::vector<int>>& ct) {

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

  int real_steps = 0;

  bool changed_val = true; //makes sure we keep improving

  while(cur_error > 0 && changed_val) {
    std::cout << "mid error is " << cur_error << std::endl;

    changed_val = false;
    
    //index and new value pair for mutation
    std::pair<int,int> mutation(std::rand()%d.k,std::rand()%d.n);
    //std::cout << "mutation is " << mutation.first << " " << mutation.second << std::endl;
    std::pair<int,int> new_val(mutation.first,mutation.second);
    increment(d,new_val);

    std::vector<int> cand_set(d.k,-1);
    for (int i = 0; i < d.k; i++) {
      cand_set[i]=my_set[i];
    }
    while (new_val != mutation) {
    //if the value we are adding is not currently in our set
      if (!is_member[new_val.second]) {
        
        cand_set[new_val.first]=new_val.second;
        int new_error = error(d,ct,cand_set);
        if (new_error < cur_error) {
         // std::cout << "made an improvement" << std::endl;
          is_member[my_set[new_val.first]]=0;
          my_set[new_val.first]=new_val.second;
          is_member[new_val.first]=1;
          cur_error=new_error;
          real_steps += 1;
          changed_val=true;
          if (cur_error==0) {
            std::cout << "Found a PDS!" << std::endl;
            std::cout << "real steps is " << real_steps << std::endl;
          }

          break;
        }

        cand_set[new_val.first] = my_set[new_val.first]; //reset candidate 



    }
    increment(d,new_val);
  //  std::cout << "incremented is " << new_val.first << " " << new_val.second << std::endl;


  }

}

  

  return my_set;


}

int main() {

  std::srand(time(0)); //std::rand()%n for int between 0 and n

  std::string fname = "table_512-d8cubed_safe.txt";//"conv_table_64-226.txt";//
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


  // std::cout << "practice increment" << std::endl;
  // std::pair<int,int> hi(17,51);
  // std::cout << hi.first << " " << hi.second << std::endl;
  // increment(d,hi);
  // std::cout << hi.first << " " << hi.second << std::endl;
  // exit(1);

  int NUM_TRIALS = 1000;
  for (int i = 0; i < NUM_TRIALS; i++) {
    auto result = steep_search(d,ct);
    int my_error = error(d,ct,result);
    if (my_error==0) {
      std::cout << "Result is: ";
      for (int i = 0; i < result.size(); i++) {
        std::cout << result[i] << " ";
      }
      std::cout << std::endl;

    }
    std::cout << "error: " << my_error << std::endl;

    if (i % 50 == 0) {
      std::cout << ".";
    }
  }
  std::cout << std::endl;



}

