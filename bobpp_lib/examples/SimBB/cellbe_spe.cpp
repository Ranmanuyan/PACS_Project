
/* cellbe_spe.cpp
 * This is the SPE program for the Cell B.E. BB example
 */

#include <stdint.h>
#include "bobpp/cell/spe_algo.hpp"
#include "cellbe_defs.hpp"

class MyNode;
class MyGenChild;
class MyCallback;

class MyTrait {
public:
  typedef Bob::SPEAlgo<MyTrait> Algo;
  typedef MyNode Node;
  typedef MyGenChild GenChild;
  typedef MyCallback Callback;
};


class MyNode : public Bob::SPEBBNode<int> {
};


class MyCallback : Bob::SPECallback {
public:
  static void gc(uint64_t ea) {
    cb(CB_GENCHILD, (cbdata*)&ea);
  }
};


class MyGenChild : Bob::SPEGenChild<MyTrait> {
public:
  virtual bool operator()(MyNode &n) const {
    return MyCallback::gc(n.ea);
  }
};

/* The main function only runs the B&B algo */
int main(uint64_t spe_id, uint64_t pdata) {
  MyTrait::Algo a;
  a();

  return 0;
}



