
/*
 * bobpp/bob_pqnode.h
 *
 * This file contains the definitions of Priority Queue-specific data in Nodes
 */


#ifndef PQNODE_HEADER
#define PQNODE_HEADER

#include <cstring>

namespace Bob {

/** class that stores the data for a node for a skew heap.
  */
template<class Node>
struct PQNodeSkew {
  /// Left heap.
  Node *left; 
  /// Right heap.
  Node *right;

};

/** class that stores all the data for a node for a priority
  * queue implemented with a simple linked list.
  */
template<class Node>
struct PQNodeSL {
  /// the next node
  Node *ne; 
  /// the precedent node
  Node *pr;
};

/** class that stores all the data for a node for a priority
  * queue with two criteria.
  */
struct PQNodeLBi {
  /** the index of the node in the two prioty Queue.
    */
  int idx[2];
};

/** PQNode class is inherited from the BBNode, SDCNode or CDCNode
  * the goal is to store all the information to be store in all the priority
  * queue proposed in bobpp 
  */
template<class Node>
class PQNode {
protected:
  /** Union of the different information used by each priority queue.
    */
  union {
    /// Data for a Skew Head
    PQNodeSkew<Node> sk;
    /// Data for a Linked list
    PQNodeSL<Node> sl;
    /// Data for a Double criteria PQ.
    PQNodeLBi lb;
  } u;/**< the union variable */
public:
  /** Constructor
    */
  PQNode() {
    memset(&u, 0, sizeof(u));
  }

  /** right child (for PQSkew)
    * @returns a reference on the pointer on a node
    */
  Node *&Right() {
    return u.sk.right;
  }
  /** left child (for PQSkew)
    * @returns a reference on the pointer on a node
    */
  Node *&Left() {
    return u.sk.left;
  }


  /** Next of the Label (for PQSL)
    * @returns a reference on the pointer on a node
    */
  Node *&N() {
    return u.sl.ne;
  }
  /** Previous of the Label (for PQSL)
    * @returns a reference on the pointer on a node
    */
  Node *&P() {
    return u.sl.pr;
  }

  /** index array (for PQLBi)
    * @returns the index of the nodes.
    */
  int &idx(int i) {
    return u.lb.idx[i];
  }
};

}; // namespace Bob


#endif //PQNODE_HEADER

