/*
 * Bobpp/src/bob_dcalgo.h
 *
 * This source include the data type for the Divide and Conquer Algo level.
 * This the unit of task scheduled by the Kernel class.
 */

#ifndef BOBDCALGO_ABSTRACT
#define BOBDCALGO_ABSTRACT

namespace Bob {

/** \defgroup DCAlgoGRP The Divide and Conquer Algorithm.
  * \ingroup SchedAlgoGRP

  * brief This module describe all classes to implement Divide and Conquer algorithms.
Various Divide and Conquer algorithms exist. The goal of some them is
only to find a feasible solution to a problem, whereas other one find
the best solution according to a Cost notion.
In this last case, a DC algorithm could be considered as a Branch and Bound
algorithm without the evaluation functionality and then the pruning capability.

This is the raison why we cut the design of Divide and Conquer in two parts,
@ref SDCAlgoGRP and @ref CDCAlgoGRP.

Each class used to implement Divide and Conquer algorithm inherits
from their associated classes of the module @ref SchedAlgoGRP .

*/

};
#endif
