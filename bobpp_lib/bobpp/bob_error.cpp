


//#include <cstdio>
//#include <cstdlib>
#include <iostream>
#include <cstdlib>
#include "bob_error.h"

namespace Bob {

const char* Exception::what() const {
  std::string s = str + m;
  return s.c_str();
}

// --------------------------------------------------------------------
void System_abort(const Exception& e) {
  std::cerr << e.what() << std::endl;
  std::cerr << "Abort" << std::endl;
  abort();
}

void __bob_Exception_throw(const Exception& err) {
  std::cout << "[Core::throw] called:" << err.what() << std::endl;
}

// --------------------------------------------------------------------
std::ostream& Exception::print(std::ostream& o) const {
  return o << this->what();
}

}
