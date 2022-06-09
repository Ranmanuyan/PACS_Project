
#include<bobpp/bobpp>

int main(int n, char ** v) {

  Bob::bit_vector bv(20);


  std::cout << bv<<std::endl;
  bv.set(1,0);
  bv.set(10,1);
  bv.set(0,1);
  bv.set(8,1);
  bv.set(11,0);
  bv.set(19,1);
  std::cout << bv<<std::endl;
  std::cout << bv.get(10) << std::endl;
  std::cout << bv.get(1) << std::endl;
  std::cout << bv.get(4) << std::endl;
  std::cout << bv.get(11) << std::endl;
  std::cout << bv.get(19) << std::endl;
}
