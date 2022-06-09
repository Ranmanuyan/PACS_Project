/*
 * Bob++ MIP solver
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

#include "config.h"

#define ACCURACY 0.000001

class TstMipTrait {
public:
  typedef Bob::BBLPNode<TstMipTrait> Node;
  typedef Bob::BBLPInstance<TstMipTrait> Instance;
  typedef Bob::BBLPGenChild<TstMipTrait> GenChild;
  typedef Bob::BBAlgo<TstMipTrait> Algo;
  typedef Bob::BBGoalBest<TstMipTrait> Goal;
  typedef Bob::BestThenDepthPri<Bob::BBLPNode<TstMipTrait> > PriComp;
  typedef Bob::BBLPStat Stat;
  typedef Bob::LP LP;
  typedef Bob::DichoBranchingInfo<TstMipTrait> BranchingInfo;
  //typedef Bob::MostFracBranching<TstMipTrait> Branching;
  //typedef Bob::StrongThenMostFracBranching<TstMipTrait> Branching;
  typedef Bob::GenericBranchingSet<TstMipTrait> Branching;
};


int main(int n, char ** v) {
  typedef TstMipTrait::Instance TheInstance;
  Bob::core::opt().add(std::string("--mip"),
                       Bob::Property("-f", "lp file name", "roy.lp"));
  TstMipTrait::Branching::opt();
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<TstMipTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrBBAlgoEnvProg<TstMipTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::BBAlgoEnvProg<TstMipTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<TstMipTrait> env;
  Bob::core::Config(n, v);
#endif

  TheInstance *Instance = new TheInstance(
    Bob::core::opt().SVal("--mip", "-f").c_str());
  // Initialize the instance.

  // Solve the instance i.e. all the tree
  std::cout << "-------- Start to solve the entire tree" << std::endl;
  env(Instance);
  /* Alloc one algo.
   * lance le Init de Instance
   * lance le () de l'algo
   */

  //MipNode *nd=new MipNode(Bob::core::opt().NVal("--bb","-lb"));
  // Initialize the node

  //std::cout<<"-------- Start to solve a subtree"<<std::endl;
  // solve only the subtree rooted by the node nd.
  //saenv(nd,Instance);
  /* Alloc one algo.
   * Recupere la meilleur solution connue
   * insere le nd
   * lance le () de l'algo
   */
#ifdef Atha
  Bob::AthaEnvProg::end();
#elif defined(Threaded)
  Bob::ThrEnvProg::end();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::end();
#endif
  delete Instance;
  Bob::core::End();
}
