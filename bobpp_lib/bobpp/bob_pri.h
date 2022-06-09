#ifndef BOBPRI
#define BOBPRI
/*---------------------------------------------------------------------*/

namespace Bob {
/*---------------------------------------------------------------------*/
/** @defgroup PriGRP The priority module to schedule all types of nodes
 *  @brief Base priority classes to schedule all types of nodes
 *  @ingroup SchedAlgoGRP
 *  When the goal of the algorithm is to perform a search in a space,
 *  the space could traversed by different way. The first step is then
 *  to allow to algorithm to schedule the search. We define here
 *  the first abstract classes for an associated priority with node.
 * 
 *  The nodes are scheduled according to two internal data.
 *     - the depth gets by the dist() of the base_node class
 *     - then the evaluation (Eval class) or the cost (Cost class). For this 
 *       two different data, operator == and <= are defined. Then the different
 *       priorities classes use these operators to compare the nodes.
 */

/** The Simple priority just for test
  * This priority class always returns true for all 
  * of this compare methods.
  * @ingroup PriGRP
  */
template<class T>
struct SPri {
public:
  /**  is equal operator
   * @param n1 the first priority to compare
   * @param n2 the second priority to compare
   */
  static bool isEq(const T &n1, const T &n2) {
    return true;
  }
  /**  is better operator
   * @param n1 the first priority to compare
   * @param n2 the second priority to compare
   */
  static bool isBetter(const T &n1, const T &n2) {
    return true;
  }

};

/** The best first priority
 * This priority compare the evaluation or cost. 
 * The best cost or evaluation is prefered.
 * But if the cost/evaluations of two Nodes are equal, the selected 
 * Node is the deepest one.
 * @ingroup PriGRP
  */
template<class T>
class BestDPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1 == e2 && e1.dist() == e2.dist();
  }
  /**  is better operator
   * @param e1 the first priority to compare
   * @param e2 the second priority to compare
   * @return 1 if e1 is better than e2, 0 otherwise.
   */
  static bool isBetter(const T &e1, const T &e2) {
    return (e1 > e2 ? 1 : (e1 == e2 ? e1.dist() > e2.dist() : 0));
  }

};

/** The best first priority
 * This priority compare the evaluation or cost. 
 * The best cost or evaluation is prefered.
 * But if the cost/evaluations of two Nodes are equal, the selected 
 * Node is the shallowest one.
 * @ingroup PriGRP
  */
template<class T>
class BestEPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1 == e2 && e1.dist() == e2.dist() ;
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  * @return 1 if e1 is better than e2, 0 otherwise.
  */
  static bool isBetter(const T &e1, const T &e2) {
    return (e1 > e2 ? 1 : (e1 == e2 ? e1.dist() < e2.dist() : 0));
  }

};

/** the Depth first priority
 * The comparison of node is only on the depth of the node.
 * @ingroup PriGRP
  */
template<class T>
class DepthPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1.dist() == e2.dist();
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  * @return 1 if e1 is deeper than e2, 0 otherwise.
  */
  static bool isBetter(const T &e1, const T &e2) {
    return e1.dist() > e2.dist();
  }

};


/** the High first priority
 * The comparison of nodes is on the depth of the node.
 * the prefered node is the highest one and not the deepest one !
 * @ingroup PriGRP
  */
template<class T>
class HighPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1.dist() == e2.dist();
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  * @return true if e1 is higher than e2, false otherwise.
  */
  static bool isBetter(const T &e1, const T &e2) {
    return e1.dist() < e2.dist();
  }

};

/** Breath first Search
  * @ingroup PriGRP
  */
template<class T>
class BreathPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1.dist() == e2.dist();
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  * @return 1 if e2 is deeper than e1, 0 otherwise.
  */
  static bool isBetter(const T &e1, const T &e2) {
    return e1.dist() < e2.dist();
  }

};


/** A second Depth first priority
 * Depth First priority but when the depth of the nodes are equal, we get
 * the best one.
 * @ingroup PriGRP
  */
template<class T>
class DepthEPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    return e1.dist() == e2.dist() && e1 == e2;
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isBetter(const T &e1, const T &e2) {
    return e1.dist() > e2.dist() || (e1.dist() == e2.dist() && e1 > e2);
  }

};

/** the best first, then depth first priority
 * @ingroup PriGRP
  */
template<class T>
class BestThenDepthPri {
public:
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isEq(const T &e1, const T &e2) {
    if (e1.dist() >= core::BTDthreshold() && e2.dist() >= core::BTDthreshold())
      return e1.dist() == e2.dist() && e1 == e2;
    else
      return e1 == e2;
  }
  /**  is equal operator
  * @param e1 the first priority to compare
  * @param e2 the second priority to compare
  */
  static bool isBetter(const T &e1, const T &e2) {
    if (e1.dist() >= core::BTDthreshold() && e2.dist() >= core::BTDthreshold())
      return e1.dist() > e2.dist() || (e1.dist() == e2.dist() && e1 > e2);
    else
      return e1 > e2;
  }

};
};

#endif
