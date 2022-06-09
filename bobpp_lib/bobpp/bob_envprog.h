
#ifndef BOB_ENVPROG
#define BOB_ENVPROG


namespace Bob {

/*------------------ EnvProg Class-------------*/
/**
 *  @defgroup EnvProgGRP The Bob++ programming environment
 *  @brief The Programming Environment
 *  @{
 *  This module groups the different programming environments proposed in
 *  Bob++.
 *
 * \page ProgEnvPage The Bob++ Programming Environment
 *
 * \section AbsSect The Bob::AlgoEnvProg
 *  We try to explain here the way to add a new enviroment.
 *  The idea is to derive the abstract Bob::AlgoEnvProg and redefine
 *  the two operators void Bob::AlgoEnvProg::operator()(TheInstance *inst) and
 *  virtual void Bob::AlgoEnvProg::operator()(TheNode *nd,TheInstance *inst)
 *
 *  The first one is dedicated to the resolution of an instance given by the
 *  inst parameter.
 *  The second one is dedicated to the resolution of a sub-space rooted on
 *  the node nd for the instance inst.
 *
 *  For a given environment, one could rewrite one
 *  Bob::AlgoEnvProg class for each of the algorithms
 *  Branch and Bound, Divide and Conquer....
 *
 * \section StaticSect Need of static environment class
 *  A specific environment could need a static class to call specific
 *  methods or function. For example in @ref ThrEnvProgGRP, @ref MPEnvProgGRP or the @ref AthaEnvProgGRP, 
 *  it has been necessary to add a specific class
 *  with static methods. For example, in the @ref ThrEnvProgGRP, the ThrEnvProg class implements the init(), start() and 
 *  end() methods, to respectively initialize the internal data, to start the POSIX threads end finally to stop the 
 *  threads.
 *
 * \section DataSect The Data structures
 *
 * Now for a given parallelization, several Bob++ algorithms are spawned
 * on different processors or cores to solve the same instance. The different algorithms
 * and the different processor or cores need to share information or to communicate
 * information.
 * The way to do this in Bob++ is to derive the different data structures
 * used by the algorithms i.e. the Priority Queue Bob::PQ and the
 * Goal data structure which depends from each algorithm.
 *
 * For example, in the threaded environment (@ref ThrEnvProgGRP), to parallelize
 * a Branch and Bound, several Bob::BBAlgo are spawned in the different threads (@ref Thread)
 * These Bob::BBAlgo use \em wrapper on top of the Priority queue
 * and the solution data structure.
 * These \em wrappers have the same interfaces as the data structures,
 * but they use mutex to insure the mutual exculsion of
 * the operations.
 *
 * The Priority queue and the solution could be seen as high level
 * communication and/or synchronization tools.
 *
 * For the athapascan/kaapi environment (@ref AthaEnvProgGRP), the idea is
 * quite similar, in the sens that we also use the Priority queue to
 * generate parallelism. But in this case, the priority queue \em wrapper,
 * could insert a node on a "sequential" priority queue, or spawn a new
 * algorithm with this node as a new root.
 *
 */

/**
 *  @defgroup AbsEnvProgGRP The abstract class for the Programming environment
 *  @brief The base class for the programming environment for the Bob++ algorithms
 *  @{
 */

/** the basic AlgoEnvProg
*/
class BaseAlgoEnvProg {
   int level;
   int state; 
   enum envprog_state { epst_running=1, epst_stop=2, epst_save=4 };
   int verbosity;
   enum verb { nothing=0,goal=1,stat=2,start_end=4 };
public:
  /** constructor
   */
  BaseAlgoEnvProg() : level(0),state(0),verbosity(0) { register_env(); }
  /** constructor
   * @param v the verbosity of the programming environment
   * @param l the level of the programming environment
   */
  BaseAlgoEnvProg(int v,int l=0) : level(l),state(0),verbosity(v) { register_env(); }
  /** destructor
   */
  virtual ~BaseAlgoEnvProg() {}
  /** Method to display the level of the programming environment
    * a Bobpp algorithm could be used by another one, 
    * then an algorithm and thus the environment has an associated level.
    */
  int Level() { return level; }
  /** stop the associated algorithms
   */
  virtual void stop()=0;
  /** stop the associated algorithms and save the state of the search
   */
  virtual void stop_and_save()=0;
  /** print the stat according to the environment
   */
  virtual void display_stat() = 0;
  /** print the stat according to the environment
   */
  virtual void display_goal() = 0;
  /** print the stat according to the environment
   */
  virtual void display() {
    if ( isdisp_stat() ) display_stat();
    if ( isdisp_goal() ) display_goal();
    if ( isdisp_stat() && core::iscollect() ) {
       dump();
    }
  }
  /** returns the verbosity of the algorithm
    */
  int isdisp() { return verbosity; }
  /** test if the goal could be displayed
   */
  bool isdisp_goal() { return (verbosity&goal)==goal; }
  /** test if the statistics could be displayed
   */
  bool isdisp_stat() { return (verbosity&stat)==stat; }
  /** test if the start and end information could be displayed
   */
  bool isdisp_start_end() { return (verbosity&start_end)==start_end; }
  /** default value
   */
  static int disp_default() { return goal|stat; }
  /** register the environment in the core
   */
  void register_env() {
      if ( level==0 ) core::register_env_prog(this);
  }
 protected:
  /** method to set the state of the EnvProg
    */
  void state_set(int _state) { state |= _state; }
  /** method to unset the state of the EnvProg
    */
  void state_unset(int _state) { state &= ~_state; }
  /** method to test if the parameter is set.
    */
  bool state_is_set(int _state) { return (state&_state)!=0; }
  /** method to set the algoEnvProg state to running
    */
  void state_running() { state_set(epst_running); }
  /** method to set the algoEnvProg state to stop
    */
  void state_stop() { state_unset(epst_running);state_set(epst_stop); }
  /** method to set the algoEnvProg state to stop_and_save
    */
  void state_stop_and_save() { state_unset(epst_running);state_set(epst_stop|epst_save); }
  /** method to test if the state of the algoEnvProg is "running" 
    * @returns bool true if the algoEnvProg is running.
    */
  bool state_is_running() { return state_is_set(epst_running); }
  /** method to test if the state of the algoEnvProg is "stop" 
    * @returns bool true if the algoEnvProg is stop.
    */
  bool state_is_stop() { return state_is_set(epst_stop); }
  /** method to test if the state of the algoEnvProg is "save" 
    * @returns bool true if the algoEnvProg is save.
    */
  bool state_is_save() { return state_is_set(epst_save); }
  

  /** dump the all the stat.
    */
  virtual void dump()=0;
};

/** the templatized AlgoEnvProg
 */
template<class Trait>
class AlgoEnvProg : public BaseAlgoEnvProg{
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
public:
  /// Constructor
  AlgoEnvProg():BaseAlgoEnvProg() {
  }
  /** constructor
   * @param v the verbosity of the programming environment
   * @param l the level of the programming environment
   */
  AlgoEnvProg(int v,int l):BaseAlgoEnvProg(v,l) {
  }
  /// Destructor
  virtual ~AlgoEnvProg() {
  }
  /** solve the problem, given by the instance.
   * @param inst the Instance to solve
   */
  virtual void operator()(TheInstance *inst) = 0;
  /** solve the problem for the given instance with the root node nd
   * @param inst the Instance to solve
   * @param nd the root node to solve
   */
  virtual void operator()(TheNode *nd, TheInstance *inst) = 0;
  /** get the goal after a resolution
   * @return the goal with the solution.
   */
  virtual TheGoal *goal() = 0;
  /** get the stat after a resolution
   * @return the stat.
   */
  virtual TheStat *stat() = 0;
protected:
};
/**
 * @}
 */

/**
 *  @defgroup SeqEnvProgGRP The sequential Programming environment
 *  @brief The base class for sequential programming environment for the Bob++ algorithms
 *  @{
 */


/** The Sequential AlgoEnvProg
 */
template<class Trait>
class SeqAlgoEnvProg : public AlgoEnvProg<Trait> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  //typedef PQSkew<TheNode, ThePriComp, TheGoal> ThePQ;
  //typedef PQHn<TheNode, ThePriComp, TheGoal> ThePQ;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  //typedef PQLB<TheNode, ThePriComp, BestEPri<TheNode>, TheGoal> ThePQ;
  typedef AlgoEnvProg<Trait> aep;

public:
  /** constructor
   * @param _v the verbosity of the programming environment
   * @param _l the level of the programming environment
   */
  SeqAlgoEnvProg(int _v,int _l=0) : AlgoEnvProg<Trait>(_v,_l), tg(0), st(0), pq(0),al(0) {
  }
  /// Destructor
  virtual ~SeqAlgoEnvProg() {
    if (tg != 0) delete tg;
    if (pq != 0) delete pq;
    if (st != 0) delete st;
  }
  /** solve the problem
    * @param inst the instance to solve
    */
  virtual void operator()(TheInstance *inst) {
    doit(0, inst);
  }
  /** solve the problem from the node \a nd
    * @param inst the instance to solve
    * @param nd the root node to solve
    */
  virtual void operator()(TheNode *nd, TheInstance *inst) {
    doit(nd, inst);
  }
  /** Return the goal after a resolution
   * @return the goal with the solution.
   */
  TheGoal *goal() {
    return tg;
  }
  /** Return the stat after a resolution
   * @return the stat.
   */
  TheStat *stat() {
    return st;
  }
  /** Display the stat according to the environment
   */
  virtual void display_stat() {
     st->display(std::cout);
     pq->display_stat(std::cout);
     tg->display_stat(std::cout);
  }
  /** Display the Goal according to the environment
   */
  virtual void display_goal() {
     tg->Prt(std::cout);
  }
  /** Stop the associated algorithms
   */
  virtual void stop() {
    AlgoEnvProg<Trait>::state_stop();
    if ( al!=0 ) al->toStop(); 
  }

  /** Stop the associated algorithms and save the state of the search
   */
  virtual void stop_and_save() {
    AlgoEnvProg<Trait>::state_stop_and_save();
    if ( al!=0 ) al->toStop(); 
  }
  /** Dump all the stat.
    */
  virtual void dump() {
     strbuff<> s;
     s << core::dTime()<<" { ";
     core::opt().dump(s);
     s << " } ";
     st->dump(s);  
     pq->stat_dump(s);
     tg->stat_dump(s);
     s << " t:1," << core::dTime();
     core::docollect(s);
  }

protected:
  /** Method to allocate a new priotiy queue.
    * @param id the identifier of the instance.
    * @return the new allocated priority queue.
    */
  virtual ThePQ *alloc_pq(const Id &id)=0;
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst)=0;
  /** Do the job for the two interfaces
   * @param nd the node to search from.
   * @param inst the instance used to perform the search.
   */
  virtual void doit(TheNode *nd, TheInstance *inst) {
    pq = alloc_pq(*(inst->id()));
    tg = new TheGoal(*(inst->id()));
    st = new TheStat(*(inst->id()));
    AlgoEnvProg<Trait>::state_running();
#ifdef BOBPP_ENABLE_FT
    if ( core::opt().BVal("--","-ft_rs") && AlgoEnvProg<Trait>::Level()==0 ) {
      std::cout << "EnvProg : Restoring Data\n"; 
      DeSerialFile fti(core::opt().SVal("--","-ft_file"));
      if ( !fti.is_open() ) {
         std::cout << "EnvProg : File "<<core::opt().SVal("--","-ft_file")<<" not found (exit)\n"; 
         exit(1);
      }
      inst->UnPack(fti);
      tg->UnPack(fti);
      pq->UnPack(fti);
      st->UnPack(fti);
      fti.close();
      tg->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
      core::log().start_header();
      st->log_header();
      pq->log_header();
      tg->log_header();
      core::log().end_header();
      al = new TheAlgo(inst, pq, tg, st);
      std::cout << "EnvProg : Data are restored\n"; 
    }
    else { 
#endif
      tg->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
      core::log().start_header();
      st->log_header();
      pq->log_header();
      tg->log_header();
      core::log().end_header();
      init_instance(inst); //inst->Init();
      tg->Init(inst);
      al = new TheAlgo(inst, pq, tg, st);
      if (nd == 0) inst->InitAlgo(al);
      else al->GPQIns(nd);
#ifdef BOBPP_ENABLE_FT
    }
#endif
    (*al)();
#ifdef BOBPP_ENABLE_FT
    if (al->isEtStop() && AlgoEnvProg<Trait>::state_is_save() && AlgoEnvProg<Trait>::Level()==0 ) {
      std::cout << "EnvProg : algo is stopped, save data\n"; 
      SerialFile fto(core::opt().SVal("--","-ft_file"));
      inst->Pack(fto);
      tg->Pack(fto);
      pq->Pack(fto);
      st->Pack(fto);
      fto.close();
      std::cout << "EnvProg : Data are saved\n"; 
    }
#endif
    aep::display();
    delete al;
  }
protected:
  /// The Goal associated with the algorithm
  TheGoal *tg;
  /// The Stat associated with the algorithm
  TheStat *st;
  /// The priority Queue used in the algorithm
  ThePQ *pq;
  /// The algorithm
  TheAlgo *al;
};

/** The Sequential Branch and Bound AlgoEnvProg
 */
template<class Trait>
class SeqBBAlgoEnvProg : public SeqAlgoEnvProg<Trait>{
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  typedef PQHn<TheNode,ThePriComp,TheGoal> TheRPQ;

public:
  /// Constructor
  SeqBBAlgoEnvProg(int _v=BaseAlgoEnvProg::disp_default(),int _l=0) : SeqAlgoEnvProg<Trait>(_v,_l) {
  }
  /// Destructor
  virtual ~SeqBBAlgoEnvProg() {}
protected:
  /** Allocate the real Priority Queue
    */
  virtual ThePQ *alloc_pq(const Id &id) { 
       return new TheRPQ(id);
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/** The Sequential Simple Divide and Conquer AlgoEnvProg
 */
template<class Trait>
class SeqSDCAlgoEnvProg : public SeqAlgoEnvProg<Trait> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  typedef PQHn<TheNode,ThePriComp,TheGoal> TheRPQ;

public:
  /// Constructor
  SeqSDCAlgoEnvProg(int _v = BaseAlgoEnvProg::disp_default(),int _l=0) : SeqAlgoEnvProg<Trait>(_v,_l) {
  }
  /// Destructor
  virtual ~SeqSDCAlgoEnvProg() {}
protected:
  /** Allocate the real Priority Queue
    */
  virtual ThePQ *alloc_pq(const Id &id) { 
       return new TheRPQ(id);
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};


/** The Sequential Costed Divide and Conquer AlgoEnvProg
 */
template<class Trait>
class SeqCDCAlgoEnvProg : public SeqAlgoEnvProg<Trait> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  typedef PQHn<TheNode,ThePriComp,TheGoal> TheRPQ;

public:
  /// Constructor
  SeqCDCAlgoEnvProg(int _v = BaseAlgoEnvProg::disp_default(),int _l=0) : SeqAlgoEnvProg<Trait>(_v,_l) {
  }
  /// Denstructor
  virtual ~SeqCDCAlgoEnvProg() {}
protected:
  /** Allocate the real Priority Queue
    */
  virtual ThePQ *alloc_pq(const Id &id) { 
       return new TheRPQ(id);
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/** The Sequential Stated AlgoEnvProg
 */
template<class Trait>
class SeqSimSpAlgoEnvProg : public SeqAlgoEnvProg<Trait>, public SimSpEnvProg<Trait> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::Link TheLink;
  typedef typename Trait::PriComp ThePriComp;
  typedef typename Trait::Key TheKey;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  typedef PQSP<TheNode,ThePriComp,ThePriComp,TheGoal> TheRPQ;

public:
  /// Constructor
  SeqSimSpAlgoEnvProg(int _v = BaseAlgoEnvProg::disp_default(),int _l=0) : SeqAlgoEnvProg<Trait>(_v,_l),SimSpEnvProg<Trait>()  {
  }
  /// Denstructor
  virtual ~SeqSimSpAlgoEnvProg() {}
protected:
  /** Allocate the real Priority Queue
    */
  virtual ThePQ *alloc_pq(const Id &id) { 
       return new TheRPQ(id);
  }
  /** Get a new link according to the environment
    * This method is only for internal use. Only the environment knows the real type of the link.
    */
  virtual TheLink *new_env_link(const TheKey &key) { 
      return new TheLink();
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(this); }

};

/** The Sequential Variable Neighboorhood Search AlgoEnvProg
 */
template<class Trait>
class SeqVNSAlgoEnvProg : public SeqAlgoEnvProg<Trait>{
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  typedef PQInterface<TheNode,ThePriComp,TheGoal> ThePQ;
  typedef PQHn<TheNode,ThePriComp,TheGoal> TheRPQ;

public:
  /// Constructor
  SeqVNSAlgoEnvProg(int _v=BaseAlgoEnvProg::disp_default(),int _l=0) : SeqAlgoEnvProg<Trait>(_v,_l) {
  }
  /// Destructor
  virtual ~SeqVNSAlgoEnvProg() {}
protected:
  /** Allocate the real Priority Queue
    */
  virtual ThePQ *alloc_pq(const Id &id) { 
       return new TheRPQ(id);
  }
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/**
 * @}
 */

/**
 * @}
 */

}
#endif
