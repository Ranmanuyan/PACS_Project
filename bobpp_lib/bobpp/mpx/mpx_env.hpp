
#ifndef BOB_MPXENVENVPROG
#define BOB_MPXENVENVPROG

namespace Bob {
namespace mpx {

/**
@defgroup MPEnvEnvProgGRP The message Passing programming environment
@brief The different environment classes for message Passing programming environment
@ingroup MPEnvProgGRP
@{
\page MEPpage Environment classes of the message pasing environment


*/

/** The MP Algorithms Wrapper 
 */
template<class Trait, template<class TraitB,class Topos, class PQ, class Goal,class Loads> class Strat, class Topo,class Load, class ThePriLB,template <class Node,class PriComp,class PriLB, class Goal> class SPQ>
class AlgoEnvProg : public Bob::ThrAlgoEnvProg<Trait,ThePriLB,SPQ> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  //typedef typename TraitMP::Topo TheTopo;
  typedef SPQ<TheNode, ThePriComp, ThePriLB, TheGoal> IntIntPQ;
  typedef ThrPQ<TheNode,ThePriLB,TheGoal,IntIntPQ> IntPQ;
  typedef MPPQ<Trait,IntPQ>  TheMPPQ;
  typedef Bob::ThrAlgoEnvProg<Trait,ThePriLB,SPQ> TheThrAlgoEnvProg;
  typedef Strat<Trait,Topo,TheMPPQ,MPGoal<Trait>,Load > TheStrat;
private:
  //TheAlgo *pal;
  TheStat *st;
  // The local priority queue
  TheMPPQ *pq;
  MPGoal<Trait> *mpgoal;
  TheStrat *strat;
  /** Allocating the private structures
    */
  void Alloc(TheInstance *inst) {
    mpgoal = new MPGoal<Trait>(*(inst->id()),true);
    pq = new TheMPPQ(*(inst->id()),true);
    st = new TheStat(*(inst->id()));
    //pal = new TheAlgo(inst, (PQInterface<TheNode,ThePriComp,TheGoal> *)pq, mpgoal, st);
    strat=new TheStrat(pq,mpgoal);
  }

  /** Free the local data structures
    */
  virtual void Free() {
    TheThrAlgoEnvProg::Free();
    DBG_COMM("In MP Free");
    //delete pal;
    delete pq;
    DBG_COMM("Out MP Free");
  }

  /** Initialize the instance
    * @param inst the instance to initialize.
    */
  virtual void init_instance(TheInstance *inst)=0;

  /** send the initial value of the instance and the goal.
    * @param inst the instance to send
    * @param n the initial node
    */
  void send_init(TheInstance *inst, TheNode *n) {
      int size;
      MPOut out;
      // Initialize the non-blocking receives

      // Initiation of the search and 
      /*if (n == 0) {
        init_instance(inst);
        inst->InitAlgo(pal);
      } else {<
        pq->Ins(n);
      }*/
      mpgoal->Init(inst);
      // Propagation of the Instance and the goal

      inst->Pack(out);
      mpgoal->Pack(out);
      MPI_Pack_size(out.position, MPI_PACKED, out.comm, &size);
      MPI_Bcast(&size, 1, MPI_INT, 0, out.comm);
      MPI_Bcast(out.buff, size, MPI_PACKED, 0, out.comm);
      out.reset();
      DBG_COMM("--------------------Master has sent instance and goal: "<<size);
  }
  /** receive the initial value of the instance and the goal.
    */
  void recv_init(TheInstance *inst) {
      MPIn in;
      int size;
      MPI_Bcast(&size, 1, MPI_INT, 0, in.comm);
      in.alloc(size);
      MPI_Bcast(in.buff, size, MPI_PACKED, 0, in.comm);
      inst->UnPack(in);
      mpgoal->UnPack(in);
      DBG_COMM("--------------------Slave has received instance and goal: "<<size);
  }
  void notify_end_algo() {
      MPOut out;
      int i,h;
      h = 0;
      out.Pack(&h);
      for (i = 1; i < EnvProg::nproc();i++) {
        //in_[i].cancel(); // need if we use non blocking send/receive
        out.send(i,"Notify End");
      }
  }
  /** Main method which initiate the search
    */
  void go(TheInstance *inst, TheNode *n) {
    //int size,h,i;
    Alloc(inst);
    if (EnvProg::rank() == 0) {
      // I am a master process
      // First initiate the ThrAlgoEnvProg
      TheThrAlgoEnvProg::init(n,inst,true,(PQInterface<TheNode,ThePriComp,TheGoal> *)pq,(ThrGoal<Trait> *)mpgoal);
      // Send the instance to the slaves.
      send_init(inst,n);
      pq->addRef();
      stat()->start();
      DBG_COMM("The master process begins his work ("<< pq->Load()<<")");
      TheThrAlgoEnvProg::run();
      (*strat)();
      TheThrAlgoEnvProg::wait_master();
      stat()->end();
    } else {
      // I am a slave process
      // First initiate the ThrAlgoEnvProg
      TheThrAlgoEnvProg::init(n,inst,false,(PQInterface<TheNode,ThePriComp,TheGoal> *)pq,(ThrGoal<Trait> *)mpgoal);
      // Receive the instance from the master.
      recv_init(inst);
      pq->addRef();
      DBG_COMM("The slave process begins his work ("<< pq->Load()<<")");
      // Slave main loop
      stat()->start();
      TheThrAlgoEnvProg::run();
      DBG_COMM("The Slave process : master thread executes strategy");
      (*strat)();
      TheThrAlgoEnvProg::wait_master();
      stat()->end();
    }
    DBG_COMM("Before display Stat");
    //AlgoEnvProg<Trait>::display();
    TheThrAlgoEnvProg::end(inst);
    DBG_COMM("Before MP Free");
    Free();
    DBG_COMM("Out MPAlgoEnvProg::go");
  }
protected:
  /** dump the all the stat.
    */
  virtual void dump() {
     strbuff<> s;
     s << core::Time()<<" { ";
     core::opt().dump(s);
     s << " } ";
     st->dump(s);  
     pq->stat_dump(s);
     mpgoal->stat_dump(s);
     core::docollect(s);
  }

public:
  /** Constructor
    * @param _v the default display configuration
    * @param _l  the level of the algorithm (default :0).
    */
  AlgoEnvProg(int _v=BaseAlgoEnvProg::disp_default(),int _l=0) : TheThrAlgoEnvProg(_v,_l) {}
  /** Destructor
    */
  ~AlgoEnvProg() {
    delete mpgoal;
    delete st;
  }
  /** Displaying the statistics
     */
  virtual void display_stat() {
    TheStat **sta;
    DBG_COMM("Display thr stats\n");
    TheThrAlgoEnvProg::display_stat();
    DBG_COMM("Display stats\n");
    if (EnvProg::rank() == 0) {
      MPIn in(BUFFER_SIZE);
      std::cout<<"        ";
      st->display_label(std::cout);
      sta = TheThrAlgoEnvProg::stat_array();
      for (int j=0;j<ThrEnvProg::n_algo_thread() ;j++) { 
         std::cout<<"  0,";
         std::cout.width(3);
         std::cout<<j<<"|";
         sta[j]->display_data(std::cout);
      }
      for (int i=1;i<EnvProg::nproc();i++) {
        int nbst;
        DBG_COMM("Master waits for receiving stat "<<i<<"/"<<EnvProg::nproc());
        in.recv(i,123);
        in.UnPack(&nbst,1);
        DBG_COMM("Master has received stats for "<< nbst<<" threads");
        for (int j=0;j<nbst;j++) {
           st->UnPack(in);
           std::cout.width(3);
           std::cout<<i<<",";
           std::cout.width(3);
           std::cout<<j<<"|";
           st->display_data(std::cout);
        }
      }
      mpgoal->display_stat(std::cout);
    }
    else {
      MPOut out(BUFFER_SIZE);
      DBG_COMM("Slave sends its stats\n");
      TheThrAlgoEnvProg::Pack_Stat(out);
      out.send(0,"Stat",123);
      DBG_COMM("Slave has sent its stats\n");
    }
  }
  /** stop the associated algorithms
   */
  virtual void stop() {
    std::cerr << " Not implemented yet\n";
  }

  /** stop the associated algorithms and save the state of the search
   */
  virtual void stop_and_save() {
    std::cerr << " Not implemented yet\n";
  }

  /** Display the value of the goal.
    */
  virtual void display_goal() {
      mpgoal->Prt(std::cout);
   }

  /** solve the problem
   * @param inst the instance to solve
   */
  virtual void operator()(TheInstance *inst) {
    go(inst, 0);
  }
  /** solve the problem from the node \a nd
   * @param inst the instance to solve
   * @param nd the root node to solve
   */
  virtual void operator()(TheNode *nd, TheInstance *inst) {
    go(inst, nd);
  }
  virtual TheStat *stat() {
    return TheThrAlgoEnvProg::stat();
  }
  /** get the goal after a resolution
   * @return the goal with the solution.
   */
  virtual TheGoal *goal() {
    return mpgoal;
  }
};

/** The MP Algorithms Wrapper for Branch and Bound.
 */
template<class TraitA,template<class TraitB,class Topos, class PQ, class Goal,class Loads> class Strat, class Topo,class Load>
class BBAlgoEnvProg : public AlgoEnvProg<TraitA,Strat, Topo, Load,BestEPri<typename TraitA::Node>,PQLB> {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename TraitA::Instance TheInstance;
  /// the constructor
  BBAlgoEnvProg() : AlgoEnvProg<TraitA,Strat, Topo,Load,BestEPri<typename TraitA::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~BBAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};


/** The MP Algorithms Wrapper for simple Divide and Conquer.
 */
template<class TraitA,template<class TraitB,class Topos, class PQ, class Goal,class Loads> class Strat, class Topo,class Load>
class SDCAlgoEnvProg : public AlgoEnvProg<TraitA,Strat,Topo,Load,BreathPri<typename TraitA::Node>,PQLB> {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename TraitA::Instance TheInstance;
  /// the constructor
  SDCAlgoEnvProg() : AlgoEnvProg<TraitA,Strat, Topo,Load,BreathPri<typename TraitA::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~SDCAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }

};

/** The MP Algorithms Wrapper for Costed Divide and Conquer.
 */
template<class TraitA,template<class TraitB,class Topos, class PQ, class Goal,class Loads> class Strat, class Topo,class Load>
class CDCAlgoEnvProg : public AlgoEnvProg<TraitA,Strat, Topo,Load,BestEPri<typename TraitA::Node>, PQLB> {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename TraitA::Instance TheInstance;
  /// the constructor
  CDCAlgoEnvProg() : AlgoEnvProg<TraitA,Strat,Topo,Load,BestEPri<typename TraitA::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~CDCAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/** The MP Algorithms Wrapper for SimSp Algorithms
 */
template<class TraitA,template<class TraitB,class Topos, class PQ, class Goal,class Loads> class Strat, class Topo,class Load>
class SimSpAlgoEnvProg : public AlgoEnvProg<TraitA,Strat,Topo,Load,HighPri<typename TraitA::Node>,PQSP >, public SimSpEnvProg<TraitA>  {
  typedef typename TraitA::Instance TheInstance;
  typedef typename TraitA::Node TheNode;
  typedef typename TraitA::Algo TheAlgo;
  typedef typename TraitA::Goal TheGoal;
  typedef typename TraitA::Stat TheStat;
  typedef typename TraitA::Link TheLink;
  typedef typename TraitA::PriComp ThePriComp;
  typedef typename TraitA::Key TheKey;

public:
  /// the constructor
  SimSpAlgoEnvProg() : AlgoEnvProg<TraitA,Strat,Topo,Load,HighPri<typename TraitA::Node>,PQSP >(),SimSpEnvProg<TraitA>()  {
  }
  /// Destructor
  virtual ~SimSpAlgoEnvProg() {}
protected:
  /** Get a new link according to the environment
    */
  virtual TheLink *new_env_link(const TheKey &Key) { 
      return 0; //new MPLink<TheLink>();
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(this); }


};

/** @}
  */

}; // end of namespace mpx
}; // end of namespace bobpp
#endif

