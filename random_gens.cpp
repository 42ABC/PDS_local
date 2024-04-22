//compare the speed of 2 different ways to generate a random set

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_set>
#include<time.h>
#include<chrono>
#include<algorithm>
#include<random>

//repeatedly select elements, throwing out repeats, until the set is filled
void repeat_select(int n, int k,std::mt19937& rand_gen) {


  std::vector<int> my_set(k,-1);
  std::vector<bool> is_member(n,0); //memory inefficient, but small domain so okay

  int new_member=0;

  for (int i = 0; i < k; i++) {
    //TODO is this the best way to generate a random set? <- function that generates d.k elements all at once? 
    while (is_member[new_member]==1 || new_member==0) {
      new_member = ((rand_gen())%(n-1))+1; //force no zero value
    }
    my_set[i]=new_member;
    is_member[my_set[i]]=1;
  }

}

//shuffle a list of all possible elements, then take the first k in the list
void with_shuffle(int n, int k, std::mt19937& rand_gen) {


  std::vector<int> my_set(k,-1);
  std::vector<bool> is_member(n,0); //memory inefficient, but small domain so okay


 

  //don't include 0 in range
  std::vector<int> ran(n-1,-1);
  for (int i = 0; i < ran.size(); i++) {
    ran[i]=i+1;
  }

  std::shuffle(ran.begin(),ran.end(),rand_gen);
  for (int i = 0; i < k; i++) {
    my_set[i]=ran[i];
    is_member[my_set[i]]=1;
  }
}

//g++ -o ac.out random_gens.cpp -std=c++17
//./ac.out n k
//./ac.out 64 18 1000000
int main(int argc, char* argv[]) {
  std::random_device rd;
  std::mt19937 rand_gen(rd());
 
  int n = atoi(argv[1]);
  int k = atoi(argv[2]);
  int num_reps = atoi(argv[3]);

  clock_t start_time_shuffle = std::clock();
  for (int i = 0; i < num_reps; i++) {
    with_shuffle(n,k,rand_gen);
  }
  clock_t end_time_shuffle = std::clock();

  clock_t start_time_select = std::clock();
  for (int i = 0; i < num_reps; i++) {
    repeat_select(n,k,rand_gen);
  }
  clock_t end_time_select = std::clock();

  std::cout << "Time with shuffle: " << (end_time_shuffle-start_time_shuffle)/100'000 << std::endl;
  std::cout << "Time with select: " << (end_time_select-start_time_select)/100'000 << std::endl;





}