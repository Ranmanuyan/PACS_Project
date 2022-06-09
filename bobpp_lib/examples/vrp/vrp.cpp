/*
 * Bob++ vrp solver
 */

#include "vrp.hpp"

std::string VrpOpt::fname="Data/E016-03m.dat";

int main(int n, char ** v) {
  typedef VrpTrait::Instance TheInstance;
  Bob::core::opt().add(std::string("--vrp"),
                       Bob::Property("-f", "vrp instance", "Data/E016-03m.dat",&VrpOpt::fname));
  VrpTrait::Branching::opt();
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<VrpTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrBBAlgoEnvProg<VrpTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::BBAlgoEnvProg<VrpTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<VrpTrait> env;
  Bob::core::Config(n, v);
#endif

  TheInstance *Instance = new TheInstance(VrpOpt::fname);
  // Initialize the instance.
  // Solve the instance i.e. all the tree
  /*std::cout << "-------- Start to solve the entire tree" << std::endl;*/
  env(Instance);

#ifdef Atha
  Bob::AthaEnvProg::end();
#elif defined(Threaded)
  Bob::ThrEnvProg::end();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::end();
#endif
  //delete Instance;
  Bob::core::End();
  return 0;
}
