
extern "C" {
#if __GNUC__ < 4
#  include <sys/send_to_ppe.h>
#elif __GNUC_MINOR__ == 1
#  include <sys/send_to_ppe.h>
#else
#  include <sys/syscall.h>
#endif
}

#include <cstdio>
#include "spe_defs.hpp"

namespace Bob {

class SPECallback {
public:
  static void cb(int id, cbdata *d) {
    __send_to_ppe(0x2110, id, (void *)d);
  }

  static void newNode(uint64_t &ea) {
    cb(CB_NEWNODE, (cbdata*)&ea);
  }

  static bool GPQGet(uint64_t &ea, int &dist) {
    struct { uint64_t ea; uint32_t dist; } d;
    cb(CB_GPQGET, (cbdata*)(void*)&d);
    ea = d.ea;
    dist = d.dist;
    return ea != 0;
  }

  static void Search(uint64_t &ea) {
    cb(CB_SEARCH, (cbdata*)&ea);
  }

  template <class T>
  static void setEval(uint64_t &ea, T eval) {
    struct { uint64_t ea; T eval; } d = {ea, eval};
    cb(CB_SETEVAL, (cbdata*)&d);
  }

  template <class T>
  static T getEval(uint64_t &ea) {
    struct { uint64_t ea; T eval; } d = {ea};
    cb(CB_GETEVAL, (cbdata*)&d);
    return d.eval;
  }
};

class SPENode {
public:
  uint64_t ea;
  int dist;

  SPENode() : ea(0), dist(0) {}
  virtual ~SPENode() {}

  bool isNull() const {
    return ea == 0;
  }
};

template <class T>
class SPEBBNode : public SPENode {

public:
  SPEBBNode() : SPENode() {}
  virtual ~SPEBBNode() {}

  /// Method to change the evaluation value 
  void setEval(T t) {
    SPECallback::setEval<T>(ea, t);
  }

  /// Method to retrieve the value of the evaluation
  T getEval() const {
    return SPECallback::getEval<T>(ea);
  }
};

template <class Trait>
class SPEGenChild {
  typedef typename Trait::Node TheNode;

public:
  SPEGenChild() {}
  virtual ~SPEGenChild() {}
  virtual void operator()(TheNode &n) const = 0;
};

template <class Trait>
class SPEAlgo {
  typedef typename Trait::Node TheNode;
  typedef typename Trait::GenChild TheGenChild;

  TheGenChild gc;
  bool GPQGet(TheNode &n) {
    return SPECallback::GPQGet(n.ea, n.dist);
  }

public:
  SPEAlgo() : gc() {}

  void operator()() {
    TheNode n;
    while (GPQGet(n)) {
      gc(n);
    }
  }
};

}

