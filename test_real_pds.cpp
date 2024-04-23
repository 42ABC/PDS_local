//the purpose of this file is to make sure that actual PDSs are showing up as error 0
#include "search_tools.h"

int main() {
  std::vector<int> test_pds = {3, 14, 16, 36, 4, 17, 18, 57, 24, 61, 27, 50, 54, 63, 43, 44, 45, 59};
  //adjust the indexing
  for (int i = 0; i < test_pds.size(); i++) {
    test_pds[i] -=1 ;
  }

  std::ifstream file("tables64/table226.txt");
  Dims d(64,18,2,6);

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

  L1_error e = L1_error();

  std::cout << "error is " << error(d,ct,test_pds,e);


}