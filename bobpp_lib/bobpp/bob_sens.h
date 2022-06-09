/*
 * Bob++/bob_sens.h
 *
 * This source include the abstract data type for the sens of optimization
 */

#include <string>
#include <limits>


#ifndef BOBSENS_HEADER
#define BOBSENS_HEADER
namespace Bob {

/** @defgroup OptiSensGRP  Sens of optimization (Min or Max)
 *  @brief The module describes all the classes used to compare nodes according to the Optimization sens (Min and Max).
 *  You do not have to use them directly.
 *  But one of them is a type parameter for various other types.
 *  @ingroup SchedAlgoGRP
 */

/**
 * The Max class is use for Maximization.
 * @ingroup OptiSensGRP
 */

template <class Typ>
class Max {
public :
  /** Test if the two parameters are equals.
    * @param e1 the first parameter
    * @param e2 the second parameter
    * @return 1 if the two parameters are equal, 0 otherwise.
    */
  static bool isEq(const Typ &e1, const Typ &e2) {
    return e1 == e2;
  }
  /** Test if e1 is better than e2.
    * Here this is a maximization, the method returns e1 > e2.
    * @param e1 the first parameter
    * @param e2 the second parameter
    * @return 1 if e1 is better than e2, 0 otherwise.
    */
  static bool isBetter(const Typ &e1, const Typ &e2) {
    return e1 > e2;
  }
  /** return the default value for a solution.
    * Here this is a maximization, then the value returned is the 
    * std::numeric_limits<Typ>::min().
    */
  static Typ sol_value() { return std::numeric_limits<Typ>::min(); }
  /** return the default value for a node.
    * Here this is a maximization, then the value returned is the 
    * std::numeric_limits<Typ>::max().
    */
  static Typ node_value() { return std::numeric_limits<Typ>::max(); }
  /** return the type of the sens 
    * 1: Max, 0 : Min
    */
  static int Sens() { return 1; }
};

/**
 * the Min class is use for Minimization.
 * @ingroup OptiSensGRP
 */
template <class Typ>
class Min {
public :
  /** Test if the two parameters are equals.
    * @param e1 the first parameter
    * @param e2 the second parameter
    * @return 1 if the two parameters are equal, 0 otherwise.
    */
  static bool isEq(const Typ &e1, const Typ &e2) {
    return e1 == e2;
  }
  /** Test if e1 is better than e2.
    * Here this is a minimization, the method returns e1 < e2.
    * @param e1 the first parameter
    * @param e2 the second parameter
    * @return 1 if e1 is better than e2, 0 otherwise.
    */
  static bool isBetter(const Typ &e1, const Typ &e2) {
    return e1 < e2;
  }
  /** return the default value for a solution.
    * Here this is a minimization, then the value returned is the 
    * std::numeric_limits<Typ>::max().
    */
  static Typ sol_value() { return std::numeric_limits<Typ>::max(); }
  /** return the default value for a node.
    * Here this is a minimization, then the value returned is the 
    * std::numeric_limits<Typ>::min().
    */
  static Typ node_value() { return std::numeric_limits<Typ>::min(); }
  /** return the type of the sens 
    * 1: Max, 0 : Min
    */
  static int Sens() { return 0; }

};


};
#endif

