/*
 * bobpp/bob_cost.h
 *
 * This source include the abstract data type the costed notion in Bob++
 */

#include <string>


#ifndef BOBCOST_HEADER
#define BOBCOST_HEADER
namespace Bob {


/** @defgroup CostFunctGRP  Costed functionnality.
 *  @brief The module describes all the classes used when a node of the search is costed.
 *  @ingroup DCAlgoGRP
    In many case of exact algorithm, a node of the space search is costed.
    The cost means here value obtained with the part of variables that
    have been fixed. This is not an evalution. This is not an evaluation
    of the cost of the solutions that could be reach.

    Then we defined here all classes that should be used in this case.
 */

/** The class that represents the cost of a node
 * The type of the cost could be int, long float, double.
 * But also some user defined types.
 * @ingroup CostFunctGRP
 */
template < class T = int, class Compare = Min<T> >
class  Cost {
public :
/// Type declaration of a Cost with a specific Type t and comparison class Compare.
  typedef Cost<T, Compare> TheCost;
/// Type declaration of a Cost with a specific Type t and comparison class Compare.
  typedef Compare Comparator;
/// Type of the cost.
  typedef T CostType;
/// The value of the cost.
  T cost;
  /** Constructor
    * @param sol if true the cost is set with the 
    *        Compare::sol_value() (@see Min or Max).
    */
  Cost(bool sol =false) {
    cost = (sol? Compare::sol_value(): Compare::node_value());
  }
  /** Constructor with a initialization of the cost.
    * @param p the cost value
    */
  Cost(T p) {
    cost = p;
  }
  /// destructor
  virtual ~Cost() {}
  /** Method to set the cost value.
    * @param t the new cost.
    */
  void setCost(T t) {
    cost = t;
  }
  /** Method that returns the cost value.
    * @return the cost value.
    */
  T getCost() const {
    return cost;
  }
  /** Operator== to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the costs are equal
    */
  bool operator==(const TheCost&t1) const {
    return Compare::isEq(cost, t1.cost);
  }
  /** Operator> to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the internal cost is better than t1
    */
  bool operator>(const TheCost&t1) const {
    return Compare::isBetter(cost, t1.cost);
  }
  /** Operator<= to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the internal cost is worse than t1
    */
  bool operator<=(const TheCost&t1) const {
    return !Compare::isBetter(cost, t1.cost);
  }
  /** Operator== to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the costs are equal
    */
  bool operator==(const T &t1) const {
    return Compare::isEq(cost, t1);
  }
  /** Operator> to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the internal cost is better than t1
    */
  bool operator>(const T &t1) const {
    return Compare::isBetter(cost, t1);
  }
  /** Operator<= to compare two costs.
    * @param t1 the cost to compare
    * @return return true if the internal cost is worse than t1
    */
  bool operator<=(const T &t1) const {
    return !Compare::isBetter(cost, t1);
  }

  /** Copy method.
    * @param te the cost to copy
    */
  virtual void copy(const TheCost &te) {
    cost = te.cost;
  }
  /** Assignment operator.
    * @param t1 the cost to copy
    */
  const TheCost &operator=(const TheCost&t1) {
    copy(t1);
    return *this;
  }
  /** return the type of the sens 
    * 1: Max, 0 : Min
    */
  const int Sens() { return Compare::Sens(); }
  /** Pack method to serialize the cost
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const  {
    DBGAFF_PCK("Cost::Pack", "Packing Instance");
    bs.Pack(&cost, 1);
  }
  /** Unpack method to deserialize the cost
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("Cost::UnPack", "Unpacking Instance");
    bs.UnPack(&cost, 1);
  }
};


};
#endif

