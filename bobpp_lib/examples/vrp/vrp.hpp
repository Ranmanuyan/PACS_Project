/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    Opale Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                     Authors:  B. Le Cun F. Galea
 *                    (C) 2008 All Rights Reserved
 *
 *                              NOTICE
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * Neither the institutions (Versailles University, PRiSM Laboratory, 
 * the Opale Team), nor the Authors make any representations about the 
 * suitability of this software for any purpose.  This software is 
 * provided ``as is'' without express or implied warranty.
 *
 */

/* 
 *  File   : vrp.hpp
 *  Author : B. Le Cun.
 *  Date   : Sept 08.
 *  Comment: header file VRP Application.
 */


#include <bobpp/bobpp>
#ifdef Threaded
#  include <bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif
#include <string>
#include <vector>
#include <cmath>
#include <bobpp/ilp/bob_lpbbalgo.h>
#include <bobpp/util/bob_util.hpp>

#include "config.h"

#define ACCURACY 0.000001

class VrpNode;
class VrpGenChild;
class VrpInstance;

#include "branching.hpp"

class VrpTrait {
public:
  typedef VrpNode Node;
  typedef VrpInstance Instance;
  typedef VrpGenChild GenChild;
  typedef Bob::BBAlgo<VrpTrait> Algo;
  typedef Bob::BBGoalBest<VrpTrait> Goal;
  typedef Bob::BestThenDepthPri<VrpNode> PriComp;
  //typedef Bob::PQHn<Node,PriComp> PQ;
  typedef Bob::BBLPStat Stat;
  typedef Bob::LP LP;
  typedef vrp_branching_info<VrpTrait> BranchingInfo;
  typedef Bob::MostFracBranching<VrpTrait> Branching;
  //typedef Bob::StrongThenMostFracBranching<VrpTrait> Branching;
  //typedef Bob::GenericBranchingSet<VrpTrait> Branching;
};

class VrpOpt {
   public:
   static std::string fname;
};

#include "graph.hpp"
#include "node.hpp"
#include "reader.hpp"
#include "instance.hpp"
#include "genchild.hpp"

