
/*
 * bobpp/bob_dspq.h
 *
 * This source include the abstract and real data types to implement
 * the data structures that store the pending nodes.
 */


#ifndef DSPQ_HEADER
#define DSPQ_HEADER

/*----------- Set of Nodes ----------*/

namespace  Bob {
/** @defgroup PQGRP Priority Queues
 *  \brief this module includes all Priority Queues definitions
 *  @ingroup GDS_GRP
 *  This module proposes several priority queue implementations.
 *  First it defines the basic classes (\ref BasicPQ). Then different implementations
 *  that use this basic definition are proposed.
 *  Two algorithms are proposed the Skew-Heap (A. Tarjan \ref SkewHeapGRP) and the classical
 *  N-heap (or implicit heap \ref HeapNGRP).
 *
 *  Another one (\ref  LBPQGRP) is also proposed to store the nodes according to Two different
 *  notion of priority. This is very usefull, when a load balancing algorithm is used 
 *  to equilibrate the nodes between several priotiy queues. One priority is used for the 
 *  search and another one priority is used to delete nodes from one priority queue to 
 *  another one.
 *  
 * @{
*/

/** @defgroup BasicPQ Basic classes for Priority Queue
 *  This module includes all basic definitions for priority Queue
 *  The class PQInterface defines only the methods that each priority queue must implement.
 *  the PQStat define the stat class used by priority queue. It is based on the Bob::stat class.
 *  The Bob::PQ class implements the Bob::PQInterface and also stores a PQStat object which store 
 *  the differents activities that an algo could made on this priority queue.
 * @{
*/

/** The Priority Queue Statisitic class
 */
class PQStat : public stat {
public:
  /** Constructor
    * @param id the identifer of the priority queue. 
    * @param g a bool if true the stat_counter are logged.
    */
  PQStat(const Id &id,bool g = true): stat('Q',id) {
    add_counter('i', "Inserted Nodes", g);
    add_counter('d', "Deleted Nodes", g);
    add_counter('g', "Delete Greater Nodes", g);
    //add_counter("l");
  }
  /** Destructor
    */
  virtual ~PQStat() {}
  /** Method to count the number of insertion
    * @param l the number of node inserted
    */
  virtual void Ins(long l = 0) {
    get_counter('i')++;
    //get_counter("l")+=l;
  }
  /** Method to count the number of delete
    * @param l the number of node inserted
    */
  virtual void Del(long l = 0) {
    get_counter('d')++;
    //get_counter("l")-=l;
  }
  /** Method to count the number of delete greater operation
    * @param nb the number of node inserted
    */
  virtual void DelG(int nb) {
    get_counter('g') += nb;
  };
  /** Get the number of nodes
    * @return the number of nodes currently stored in the priority queue.
    */
  int GetNbN() {
    return get_counter('i').get() - get_counter('d').get()-get_counter('g').get();
  }
};

/** The Priority Queue interface
 */
template<class Node, class PriComp, class Goal>
class PQInterface {
protected:
  /** this data counts the number of algorithms that use this
   * priority queue at the same time.
   */
  int ref;
public :
  /** Constructor (mainly do nothing)
    */
  PQInterface() : ref(0) {}
  /** Destructor (mainly do nothing)
    */
  virtual ~PQInterface() {}
  /** Remove a reference on the Priority Queue
    * The number of reference on a priority queue is the number of threads that access it.
    */
  virtual void remRef() {
    ref--;
  }
  /** Add a reference on the Priority Queue. 
    * The number of reference on a priority queue is the number of threads that access it.
    */
  virtual void addRef() {
    ref++;
  }
  /** Reset the Data structure
    */
  virtual void Reset() = 0;
  /** Insert a Node n with its associated priority
    * @param n the node to insert
    */
  virtual void Ins(Node *n) = 0;
  /** Delete the best prioritized Node
    * @return the deleted node
    */
  virtual Node *Del() = 0;
  /** Removes all unneeded Nodes with regard to the Goal
    * @param n the goal to compare...
    */
  virtual int DelG(Goal &n) = 0;
  /** Print the contents of the PQ.
    * @param os the std::ostream used to display the priority queue
    */
  virtual ostream &Prt(ostream &os = std::cout) const = 0;
  /** Get the Statistics of the priority queue
    * @return a pointer on the PQStat object.
    */
  //virtual PQStat *getStat() = 0;
  /** Method to return the number of nodes stored in th priority queue.
    * @return the number of nodes
    */
  virtual long nb_node()=0;

  /** Method to test if node are required in th priority queue.
    * @return true if the priority queue is empty
    */
  virtual bool need_node()=0;
  
  /** Method to return the number of waiting threads
    * @return number of waiting threads
    */
  virtual int waiting_threads()=0;

  /** display the statistics of the priority Queue
    */
  virtual void display_stat(std::ostream &os = std::cout) = 0;
  /** Write the stat header on the log
    */
  virtual void log_header() =0;
  /** dump the stat (used in the collect)
    */
  virtual void stat_dump(strbuff<> &s) =0;

  /** Delete and Pack all the internal nodes
    * @param se the Serialize object
    */
  virtual void Pack(Serialize &se) {
      Node *n;
      int v=1;
      while (( n=Del())!=0 ) {
         se.Pack(&v,1);
         n->Pack(se);
      }
      v=0;
      se.Pack(&v,1);
  }
  /** UnPack and insert nodes
    * @param se the DeSerialize object
    */
  virtual void UnPack(DeSerialize &se) {
      Node *n;
      int v=1;
      se.UnPack(&v,1);
      while ( v ) {
         n = new Node();
         n->UnPack(se);
         Ins(n);
         se.UnPack(&v,1);
      }
  }
};

/**
  The PQ class implements some features of the PQInterface. 
  The PQ class stores a PQStat object, and counts the number of reference (ref) on the priority that counts the number of threads accessing to the priority queue.
*/
template<class Node, class PriComp,class Goal>
class PQ : public PQInterface<Node, PriComp,Goal> {
  PQStat *pqs;
  bool local_alloc_pqs;
public:
  /** Constructor
    */
  PQ() : PQInterface<Node, PriComp,Goal>(), pqs(new PQStat(CId('?'),false)), local_alloc_pqs(true) { }
  /** Constructor
    * @param id the algorithm identifier (used for logging the stat).
    * @param g a bool, true the PQStat counter are displayed.
    */
  PQ(const Id &id,bool g = true) : PQInterface<Node, PriComp,Goal>(), pqs(new PQStat(id,g)), local_alloc_pqs(true) { }
  /** Constructor
    * @param _pqs the priority queue statistics.
    */
  PQ(stat *_pqs) : PQInterface<Node, PriComp,Goal>(), pqs((PQStat*) _pqs), local_alloc_pqs(false) { }
  /// Destructor
  virtual ~PQ() {
    BOBASSERT(ref == 0, ReferenceException, "PQ", "The PQ is deleted and ref!=0");
    if ( local_alloc_pqs ) delete pqs;
  }
  /** Init the reference for the internal statistics
    * @param s the identifier of the stat.
    */
  void Init(const Id &s) {
    pqs->init(s);
  }
  /** Init the reference for the internal statistics
    * @param s the identifier of the stat.
    * @param disp the stat will be displayed or not
    */
  void Init(const Id &s,bool disp) {
    pqs->init(s,disp);
  }
  /// Statistics for insert.
  void StIns(Node *n = 0) {
    pqs->Ins();
  }
  /// Statistics for delete.
  void StDel(Node *n = 0) {
    pqs->Del();
  }
  /// Statistics for deleteGreater.
  void StDelG(int nb = 1) {
    pqs->DelG(nb);
  }
  /// Inserts a Node n with its associated priority p
  virtual void Ins(Node *n) {};
  /// Reset the Data structure
  virtual void Reset() {
    pqs->reset();
  }
  /// Deletes the best prioritized Node
  virtual Node *Del() {
    return 0;
  };
  /// Deletes all the Node that have a priority less than p
  virtual int DelG(Goal &g) {
    //std::cout<<"PQ::DelG()"<<std::endl;
    return 0;
  }
  /** Method to return the number of nodes stored in th priority queue.
    * @return the number of nodes
    */
  virtual long nb_node() {  return getStat()->GetNbN(); }

  /** Method to test if node are required in th priority queue.
    * @return true if the priority queue is empty
    */
  virtual bool need_node() {  return nb_node()==0; }
  
   /** Method to return the number of waiting threads
    * @return number of waiting threads
    */
  virtual int waiting_threads(){return 0;}
  
  /** Get the Statistics of the Priority Queue.
    * @return the Statistics pointer object.
    */
  PQStat *getStat() {
    return pqs;
  }
  /** display the statistics of the priority Queue
    */
  virtual void display_stat(std::ostream &os = std::cout) {
    getStat()->display(os);
  }
  /** write the stat header on the log
    */
  virtual void log_header() { getStat()->log_header(); }
  /** dump the stat (used in the collect)
    */
  virtual void stat_dump(strbuff<> &s) {
     getStat()->dump(s);
  }
};

//@}




/** @defgroup SkewHeapGRP Skew-Heap implementation
    @name a PQ implementations of Priority Queue : the Skew Heap.
   @{
  */

/**
  * Priority Queue implementation the Skew Heap
  */
template<class Node, class PriComp, class Goal >
class PQSkew : public PQ<Node, PriComp, Goal> {
private:
  /// the root of the tree
  Node *r;

  /// the meld method
  void Meld(Node *q1, Node *q2) {
    Node *p;

    if (q1 == 0) {
      r = q2;
      return;
    }
    if (q2 == 0) {
      r = q1;
      return;
    }

    Swap(&q1, &q2);
    r = q1;
    p = q1;
    q1 = (Node*)p->Right();
    p->Right() = p->Left();

    while (q1 != 0) {
      Swap(&q1, &q2);
      p->Left() = q1;
      p = q1;
      q1 = (Node*)p->Right();
      p->Right() = p->Left();
    }

    p->Left() = q2;
  }
  /// the swap method
  void Swap(Node **q1, Node **q2) {
    Node *dum;
    if (*q2 == 0) return;
    if (PriComp::isBetter(**q2, **q1)) {
      dum = *q1;
      *q1 = *q2;
      *q2 = dum;
    }
  };

public:

  /** Default Constructor
    */
  PQSkew() : PQ<Node, PriComp,Goal>(),r(0) {  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQSkew(const Id &id,bool g = true) : PQ<Node, PriComp,Goal>(id,g),r(0) {
  }
  /** Constructor
    * @param st the Priority queue statistics
    */
  PQSkew(stat *st) : PQ<Node, PriComp,Goal>(st),r(0) {
  }
  /// destructor
  virtual ~PQSkew() {
    Node *f;
    while ((f = Del()) != 0) {
      if (f->isDel()) delete f;
    }
  }
  /** Reset the priority queue
    */
  void Reset() {
    Node *f;
    while ((f = Del()) != 0) {
      if (f->isDel()) delete f;
    }
    PQ<Node, PriComp, Goal>::Reset();
  }
  /// Insert a Node n with the priority p
  void Ins(Node *n) {
    n->addRef();
    PQ<Node, PriComp, Goal>::StIns();
    Meld(n, r);
  }
  /// Delete the Node with the highest priority
  Node *Del() {
    Node *p;

    p = r;
    if (p == 0) return 0 ;
    Meld((Node*)p->Right(), (Node*)p->Left());
    PQ<Node, PriComp,Goal>::StDel();
    p->remRef();
    return p;
  }
  /// Delete the Nodes with a lower priority than p
  virtual int DelG(Goal &g) {
    return 0;
  }
  /// Displays statistics and Contents
  ostream &Prt(ostream &o = std::cout) const {
    o << "Bob::PQSkew: no display" << std::endl;
    return o;
  }
};

/** @}
  */

/** @defgroup DBLKPQGRP doubled linked list implementation of a priority Queue.
 *  @name  Another implementation of Priority queue, represented by a double linked list.
 *  This implementation is just for fun, it has been never really used.
 * @{ 
 */

/**
  * Priority Queue implementation : doubled linked list
  */
template<class Node, class PriComp,class Goal>
class PQSL : public PQ<Node, PriComp, Goal> {
protected:
/// the pointer on the root node
  Node *r;
/// the pointer on the last node
  Node *l;
/// the max number of nodes stored
  int Max;

public:
  /** Default Constructor
    */
  PQSL() : PQ<Node, PriComp,Goal>() { 
    Max = -1;
    r = 0;
    l = 0;
  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQSL(const Id &id,bool g = true) : PQ<Node, PriComp, Goal>(id,g) {
    Max = -1;
    r = 0;
    l = 0;
  }
  /** Constructor
    * @param st the Priority queue statistics
    */
  PQSL(stat *st) : PQ<Node, PriComp,Goal>(st) {
    Max = -1;
    r = 0;
    l = 0;
  }
  /** Constructor
    * @param PMax indicates the mximum number of solutions
    *        that could be stored.
    */
  PQSL(int PMax) : PQ<Node, PriComp, Goal>() {
    Max = PMax;
    r = 0;
    l = 0;
  }
  /// destructor
  virtual ~PQSL() {
    Node *f;
    while ((f = Del()) != 0) {
      delete f;
    }
  }
  /// Reset the priority queue
  virtual void Reset() {
    Node *f;
    while ((f = Del()) != 0) {
      delete f;
    }
    PQ<Node, PriComp, Goal>::Reset();
  }
  /// Insert a Node n 
  virtual void Ins(Node *n) {
    Node *c;
    n->N()=n->P()=0;
    //if ( n==0 ) std::cerr << "Strang Insert null node\n";
    for (c = r;c != 0 && PriComp::isBetter(*c, *n) ;c = (Node*)c->N());
    if (c == 0) {
      if (l != 0) {
        l->N() = n;
        n->P() = l;
      }
      l = n;
      if (r == 0) r = n;
    } else {
      n->N() = c;
      n->P() = c->P();
      c->P() = n;
      if (n->P() == 0) {
        r = n;
      } else {
        n->P()->N() = n;
      }
    }
    PQ<Node, PriComp,Goal>::StIns();
  }
  /// Delete the Node with the highest priority
  virtual Node *Del() {
    Node *b;
    b = r;
    if (b == 0) return 0;
    r = (Node*)b->N();
    if (r == 0) l = 0;
    else r->P() = 0;
    PQ<Node, PriComp,Goal>::StDel(b);
    return b;
  }
  // Delete the Nodes with a lower priority than p
  virtual int DelG(Goal &g) {
    return 0;
  }
  /** Reorganisation of the list.
    * This priority queue is used with the space simulation algorithm.
    * In the context, a node could become ready since it receives messages by its links
    * Then the priority of a node could change whereas it stays in the priority queue.
    * In a multithreaded environment it is difficult to move a node at each time its priority changes.
    * Then, we have choosen to get the best prioritized node in one pass a each delete operation.
    */
  void reorg() {
     Node *c,*n;
     int i=0;
     if ( l==0 ) { return;}
     n=l;
     for (c=(Node *)n->P() ;c != 0 ;) {
         if ( PriComp::isBetter(*n, *c)) {
           if ( n->N()==0 ) l = c;
           else  n->N()->P() = c;
           c->N() = n->N();
           if ( c->P()==0 ) r = n;
           else c->P()->N()=n;
           n->P() = c->P();
           n->N() = c;
           c->P()=n;
           c=n;c = (Node*)c->P();
         } else {
           n=c;c = (Node*)c->P();
         }
         i++;
     }
  }

  /// Displays statistics and Contents
  virtual ostream &Prt(ostream &o = std::cout) const {
    return o << "No Display for the PQSL at this time\n";
  }
};

/**@}
  */

/** @defgroup HeapNGRP n-Heap implementation
  * @name a PQ implementations of Priority Queue : a vector based n-Heap.
  * @{
  */

/**
  * Base class for n-Heap based priority queues
  */
template <class Node, class PriComp, class Goal, int level>
class PQHnBase : public PQ<Node, PriComp, Goal> {
protected:
  /// The node n-heap
  std::vector<Node*> h;

  /** Spans a path down from a specified Node to the root, eventually swapping
   * nodes with respect to the Heap structure.
   * Used after having inserted a new node or after reorganizing the leaf
   * nodes because of deletions of the biggest nodes.
   * @param n index of leaf node
   */
  virtual void down(int n) = 0;

  /** Spans a path up from a specific Node top a leaf, as long as swapping
   * nodes is necessary to reorganize the tree structure.
   *
   * Used after root deletion or arbitrary node deletion
   * @param n index of starting node
   */
  virtual void up(int n) = 0;

public:
  /** Default Constructor
    */
  PQHnBase() : PQ<Node, PriComp,Goal>(),h() { 
  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQHnBase(const Id &id,bool g = true) : PQ<Node, PriComp,Goal>(id,g), h() {
  }
  /** Constructor
    * @param st the priority queue statistics
    */
  PQHnBase(stat *st) : PQ<Node, PriComp,Goal>(st), h() {
  }

  /// destructor
  virtual ~PQHnBase() {
  }

  /// Reset the priority queue
  virtual void Reset() {
    Node *f;
    while ((f = Del()) != 0) {
      if (f->isDel()) delete f;
    }
    PQ<Node, PriComp, Goal>::Reset();
  }

  /// Insert a Node n with the priority p
  virtual void Ins(Node *n) {
    n->addRef();
    h.push_back(n);
    down(h.size() - 1);
  }
  /// Delete the Node at an arbitrary position in the heap
  virtual Node *Del(int pos) {
    if (h.size() == 0)
      return 0;
    Node *b = h[pos];
    h[pos] = h[h.size()-1];
    h.pop_back();
    if (pos > 0 && PriComp::isBetter(*h[pos], *h[(pos-1)/level]))
      down(pos);
    else
      up(pos);
    b->remRef();
    return b;
  }
  /// Delete the Node with the highest priority
  Node *Del() {
    return Del(0);
  }
  /// Removes all unneeded Nodes with regard to the Goal
  virtual int DelG(Goal &g) {
    /* Span all Nodes from the end to the beginning of the vector */
    int i, p, count = 0;
    int last;
    Node *nd;
    for (i = h.size() - 1; i >= 0; i--) {
      if ( !g.is4Search((nd = h[i]))) {
        if (nd->remRef() == 0) {
          delete nd;
        }
        count ++;
        last = h.size() - 1;
        if (i < last) {
          h[i] = h[last];
          h.pop_back();
          p = (i-1) / level;
          if (PriComp::isBetter(*h[i], *h[p]))
            down(i++);
          else
            up(i);
        } else {
          h.pop_back();
        }
      }
    }
    return count;
  }
  /// Displays statistics and Contents
  virtual ostream &Prt(ostream &o = std::cout) const {
    o << "Bob::PQHnBase: no display" << std::endl;
    return o;
  }
};



/**
  * Priority Queue implementation: the vector-based n-Heap
  */
template <class Node, class PriComp, class Goal, int level = 3>
class PQHn : public PQHnBase<Node, PriComp, Goal, level> {
  typedef PQHnBase<Node, PriComp, Goal, level> base;
protected:
  /** Spans a path down from a specified Node to the root, eventually swapping
   * nodes with respect to the Heap structure.
   * Used after having inserted a new node or after reorganizing the leaf
   * nodes because of deletions of the biggest nodes.
   * @param n index of leaf node
   */
  void down(int n) {
    int p = (n-1) / level;
    Node *m = base::h[n];
    while (n > 0 && PriComp::isBetter(*m, *base::h[p])) {
      base::h[n] = base::h[p];
      n = p;
      p = (p-1) / level;
    }
    base::h[n] = m;
  }

  /** Spans a path up from a specific Node top a leaf, as long as swapping
   * nodes is necessary to reorganize the tree structure.
   *
   * Used after root deletion or arbitrary node deletion
   * @param n index of starting node
   */
  void up(int n) {
    int max = base::h.size();
    int i, first, best;
    Node *m = base::h[n];
    bool ok = true;
    while (ok) {
      // Look for the best child node of node #n
      first = n * level + 1;
      ok = first < max;
      best = first;
      for (i = first + 1; i < first + level && i < max; i++) {
        if (PriComp::isBetter(*base::h[i], *base::h[best]))
          best = i;
      }
      if (ok) {
        if (PriComp::isBetter(*base::h[best], *m)) {
          base::h[n] = base::h[best];
          n = best;
        } else ok = false;
      }
    }
    base::h[n] = m;
  }

public:
  /** Default Constructor
    */
  PQHn() : PQHnBase<Node, PriComp,Goal,level>() { 
  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQHn(const Id &id,bool g = true) : 
                 PQHnBase<Node, PriComp, Goal, level>(id,g) { }
  /** Constructor
    * @param st the priority queue statistics
    */
  PQHn(stat *st) : 
                 PQHnBase<Node, PriComp, Goal, level>(st) { }

  /// Destructor
  virtual ~PQHn() {  
    PQHnBase<Node, PriComp,Goal,level>::Reset();
  }

  /// Insert a Node n with the priority p
  virtual void Ins(Node *n) {
    base::Ins(n);
    PQ<Node, PriComp,Goal>::StIns();
  }
  /// Delete the Node at an arbitrary position in the heap
  virtual Node *Del(int pos) {
    Node *b = base::Del(pos);
    if (b)
      PQ<Node, PriComp,Goal>::StDel();
    return b;
  }
  /// Removes all unneeded Nodes with regard to the Goal
  virtual int DelG(Goal &g) {
    int count = base::DelG(g);
    PQ<Node, PriComp,Goal>::StDelG(count);
    return count;
  }  
  /// Reset the priority queue
  virtual void Reset() {
  }

};

/** @}
  */



/** @defgroup LBPQGRP double priority implementation of a priority Queue.
 *  @name  Another implementation of Priority queue with a second priority for
 *         load balancing.
 * 
 *  The internal data structure is a pair of base n-Heap's.
 * @{
 */

/**
  * @brief Utility priority queue used in PQLB class.
  *
  * This is basically a PQHn which stores NodeLB elements, enabling coherency
  * with another symmetrical queue with a different evaluation function
  */
template <class Node, class PriComp, class Goal, int rank, int level=3>
class PQLBi : public PQHnBase<Node, PriComp, Goal, level> {
  typedef PQHnBase<Node, PriComp, Goal, level> base;
protected:
  /** Spans a path down from a specified Node to the root, eventually swapping
   * nodes with respect to the Heap structure.
   * Used after having inserted a new node or after reorganizing the leaf
   * nodes because of deletions of the biggest nodes.
   * @param n index of leaf node
   */
  virtual void down(int n) {
    int p = (n-1) / level;
    Node *m = base::h[n];
    while (n > 0 && PriComp::isBetter(*m, *base::h[p])) {
      base::h[n] = base::h[p];
      base::h[n]->idx(rank) = n;
      n = p;
      p = (p-1) / level;
    }
    base::h[n] = m;
    m->idx(rank) = n;
  }

  /** Spans a path up from a specific Node top a leaf, as long as swapping
   * nodes is necessary to reorganize the tree structure.
   *
   * Used after root deletion or arbitrary node deletion
   * @param n index of starting node
   */
  virtual void up(int n) {
    int max = base::h.size();
    int i, first, best;
    Node *m = base::h[n];
    bool ok = true;
    while (ok) {
      // Look for the best child node of node #n
      first = n * level + 1;
      ok = first < max;
      best = first;
      for (i = first + 1; i < first + level && i < max; i++) {
        if (PriComp::isBetter(*base::h[i], *base::h[best]))
          best = i;
      }
      if (ok) {
        if (PriComp::isBetter(*base::h[best], *m)) {
          base::h[n] = base::h[best];
          base::h[n]->idx(rank) = n;
          n = best;
        } else ok = false;
      }
    }
    base::h[n] = m;
    m->idx(rank) = n;
  }

public:
  /** Default Constructor
    */
  PQLBi() : PQHnBase<Node, PriComp, Goal, level>() {}
  /** Constructor
    * @param id the stat identifier 
    * @param l a bool true to log the PQ stat.
    */
  PQLBi(const Id &id, bool l) : PQHnBase<Node, PriComp, Goal, level>(id,l) {}
  /** Constructor
    * @param st the Priority queue statistics
    */
  PQLBi(stat *st) : PQHnBase<Node, PriComp, Goal, level>(st) {}

  /// destructor
  virtual ~PQLBi() { }
};

/**
  * Priority Queue implementation with additional load balancing priority
  */
template<class Node, class PriComp, class PriLB, class Goal>
class PQLB : public PQ<Node, PriComp, Goal> {
protected:
  /** the priority Queue with the priority for the search
    */
  PQLBi<Node, PriComp, Goal, 0> pq;
  /** the priority Queue with the priority of the Load balancing
    */
  PQLBi<Node, PriLB, Goal, 1> pqlb;

public:
  /** Default Constructor
    */
  PQLB(): PQ<Node, PriComp, Goal>(), pq(), pqlb() {
    PQ<Node, PriComp, Goal>::getStat()->add_counter('l',"delLb",true);
  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQLB(const Id &id, bool g=true): PQ<Node, PriComp, Goal>(id,g), pq(), pqlb() {
    PQ<Node, PriComp, Goal>::getStat()->add_counter('l',"delLb",true);
  }
  /** Constructor
    * @param st the Priority Queue statistics
    */
  PQLB(stat *st): PQ<Node, PriComp, Goal>(st), pq(), pqlb() {
    PQ<Node, PriComp, Goal>::getStat()->add_counter('l',"delLb",true);
  }

  /** Destructor
    */
  virtual ~PQLB() { pq.Reset(); }
  
  /// Reset the priority queue
  virtual void Reset() {
  }
  /** insert a node
    * @param b the node to insert
    */
  virtual void Ins(Node *b) {
    pq.Ins(b);
    pqlb.Ins(b);
    PQ<Node, PriComp,Goal>::StIns();
  }
  /// Delete node with highest search priority
  virtual Node *Del() {
    Node *b = pq.Del();
    if (b) {
      pqlb.Del(b->idx(1));
      PQ<Node, PriComp, Goal>::StDel();
    }
    return b;
  }
  /// Delete node with highest load balancing priority
  virtual Node *DelLB() {
    Node *b = pqlb.Del();
    if (b) {
      pq.Del(b->idx(0));
      PQ<Node, PriComp, Goal>::getStat()->get_counter('l')++;
      PQ<Node, PriComp, Goal>::StDel();
    }
    return b;
  }
  /// Removes all unneeded Nodes with regard to the Goal
  virtual int DelG(Goal &g) {
    int count = pq.DelG(g);
    pqlb.DelG(g);
    PQ<Node, PriComp,Goal>::StDelG(count);
    return count;
  }

  /// Displays statistics and Contents
  virtual ostream &Prt(ostream &o = std::cout) const {
    o << "Bob::PQLB: no display" << std::endl;
    return o;
  }
};




/** @}
  */
/** @defgroup SimSpPQGRP Priority Queue used in a simulation state space
 *  @name  Specific implementation of Priority queue used to manage the 
 *         node that are not ready
 * 
 * @{
 */


/**
  * Priority Queue implementation with additional load balancing priority
  */
template<class Node, class PriComp, class PriLB , class Goal>
class PQSP : public PQ<Node, PriComp, Goal> {
protected:
  /** the priority Queue with the priority for the search
    */
  PQHn<Node, PriComp, Goal,3> pq;
  /** the priority Queue that stores the not ready nodes
    */
  PQSL<Node, PriComp, Goal> pqnr;

public:
  /** Default Constructor
    */
  PQSP(): PQ<Node, PriComp, Goal>(), pq(), pqnr() {
  }
  /** Constructor
    * @param id the stat identifier 
    * @param g a bool true to log the PQ stat.
    */
  PQSP(const Id &id, bool g=true): PQ<Node, PriComp, Goal>(id,g), pq(), pqnr() {
  }
  /** Constructor
    * @param st the Priority Queue statistics
    */
  PQSP(stat *st): PQ<Node, PriComp, Goal>(st), pq(), pqnr() {
  }

  /** Destructor
    */
  virtual ~PQSP() {}
  /// Reset the priority queue
  virtual void Reset() {
  }
  /** insert a node
    * @param b the node to insert
    */
  virtual void Ins(Node *b) {
    if ( b->is_ready() ) {
      pq.Ins(b);
    } else
      pqnr.Ins(b);
    PQ<Node, PriComp,Goal>::StIns();
  }
  /// Delete node with highest search priority
  virtual Node *Del() {
    Node *b,*br;
    //do {
      pqnr.reorg();
      br = pqnr.Del();
      if ( br ) {
        if ( br->is_ready() ) { pq.Ins(br); }
        else pqnr.Ins(br);
      } 
    b = pq.Del(0);
    PQ<Node, PriComp, Goal>::StDel();
    return b;
  }
  /// Delete node with highest load balancing priority
  virtual Node *DelLB() {
    return Del();
  }
  /// Removes all unneeded Nodes with regard to the Goal
  virtual int DelG(Goal &g) {
    return 0;
  }

  /// Displays statistics and Contents
  virtual ostream &Prt(ostream &o = std::cout) const {
    o << "Bob::PQLB: no display" << std::endl;
    return o;
  }
};

/** @}
  */

/** @}
  */


};
// end of namespace.

#endif
