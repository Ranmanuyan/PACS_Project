/*
 * Bob++/bob_eval.h
 *
 * This source include the abstract data type for the evaluation notion in Bob++
 */

#include <string>


#ifndef BOBEVAL_HEADER
#define BOBEVAL_HEADER
namespace Bob {

/** @defgroup EvalFunctGRP  Eval functionnality.
 *  @brief The module describes all the classes used to handle a search when a node is evaluated.
 *  You do not have to use them directly.
 *  But one of them is a type parameter for various other types.
 *  @ingroup BBAlgoGRP
    When for an algorithm a node is evaluated, the pruning functionnality
    could be used. This is the case in Branch and Bound, and A* for example.

    The main goal of this functionnality is to prune some parts of the
    search space, using an evaluation of the solutions that can be reached.
    For a minimization, if the evaluation of the node is worst than the actual
    best solution, all the search space form this node could be pruned.
    That also evident that the evaluation of the nodes could be used to
    schedule the search (Best first search).

    Thus this module proposes the class for
      - An evaluated node
      - The priorities
      - The Algorithm interface for this functionality.
 */

/** The class that represents an evaluation
 * The type of the evaluation could be int, long float, double.
 * But also some user defined types.
 * An evaluated node must also be costed. Thus the Eval class inherits
 * from the Cost class.
 * @ingroup EvalFunctGRP
 */
template < class T = int, class Compare = Min<T> >
class  Eval {
public :
  /// Type declation of a Eval with a specific Type t and comparison class Compare.
  typedef Eval<T, Compare> ThisEval;
  /// Type of the evaluation.
  typedef T EvalType;
  /// The value of the evaluation.
  T val;
  /** Constructor 
    * @param sol if true the default evaluation value is the 
    * Compare::sol_value() (@see Min or Max).
    */
  Eval(bool sol=false) {
    val = (sol? Compare::sol_value(): Compare::node_value());
  }
  /** Constructor with the initialization of the evaluation.
    * @param p the evaluation value.
    */
  Eval(T p) {
    val = p;
  }
  /** Constructor with an initialization of the evaluation.
    * @param te an Eval object 
    */
  Eval(ThisEval &te) {
    val = te.val;
  }
  /// Constructor with an initialization of the evaluation.
  virtual ~Eval() { }
  /// Method to change the evaluation value
  void setEval(T t) {
    val = t;
  }
  /// Method to returns the value of the evaluation
  T getEval() const {
    return val;
  }
  /// Operator to compare two evaluations.
  bool operator==(const ThisEval &t1) const {
    return Compare::isEq(val, t1.val);
  }
  /// Operator to compare two evaluations.
  bool operator>(const ThisEval &t1) const {
    return Compare::isBetter(val, t1.val);
  }
  /// Operator to compare two evaluations.
  bool operator<=(const ThisEval &t1) const {
    return !Compare::isBetter(val, t1.val);
  }
  /// copy method.
  virtual void copy(const ThisEval &te) {
    val = te.val;
  }
  /// Affectation operator .
  const ThisEval &operator=(const ThisEval &t1) {
    copy(t1);
    return *this;
  }
  /// Pack method to serialize the Node
  virtual void Pack(Serialize &bs)  const {
    bs.Pack(&val, 1);
  }
  /// Unpack method to deserialize the Node
  virtual void UnPack(DeSerialize &bs)  {
    bs.UnPack(&val, 1);
  }

};

};
#endif

