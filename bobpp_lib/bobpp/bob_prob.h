
/*
 * bobpp/src/bob_prob.h
 *
 * This source include the abstract data type for the problem of the Bob++ library
 */

#ifndef BOBPROB
#define BOBPROB

namespace Bob {

/** @defgroup UserAlgoGRP User Algorithms
 *  @brief This module describes the architecture for the user algorithms.
 *
 *  The class architecture to obtain the different user algorithms is quite complex.
 *
 *  The different classes enroles in what we call an algorithm is represented by
 *  -# a Node class which represent the node of the space search
 *  -# an instance class  which stores all the datas for an instance of a Problem
 *  -# a GenChild class which compute the child node of a node.
 *  -# an Algo class  that store the main loop mainly in the operator().
 *
 *  The basic definition of these four classes are in the @ref BaseAlgoGRP.
 *  They are used to define the classes of @ref SchedAlgoGRP that are the base to 
 *  defined @ref BBAlgoGRP, @ref CDCAlgoGRP and @ref SDCAlgoGRP.
 *
 *  We have also added @ref SimSpAlgoGRP needed to parallelized a simulation of entities 
 *  that move in a 2D or 3D space. 
 * 
 *  Additionnal features are
 *  -# the problem class (that is not really used at this time)
 *  -# the statistics associated with the algorithm
 *  -# the goal of the search
 *  -# each algo is executed in a specific environment (sequential, multi-threaded, mpi, athapascan)
 *
 *  @{
 */

/** The Problem class
 * The class represent a Problem to solve. The goal is to have an instance of the Problem class
 * which is independant of the instance of the problem.
 *
 * At this time this class is almost empty * This class is present for future uses !
 *
 */
template <class Trait>
class Problem {

public:
  /// Type of the node
  typedef typename Trait::Node TheNode;
  /// Type of the info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
  /// Type of the GenChild
  typedef typename Trait::GenChild TheGenChild;

  /// An idientifier (for future use)
  int id;

  /// Default constructor
  Problem(): id(0) { }
private:
};

/* @}
 */


};
#endif
