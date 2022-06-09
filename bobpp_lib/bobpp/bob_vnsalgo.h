/*
 * VNSAlgo.h
 *
 * This source include the data type for the Variable Neighborhood Search Algo level.
 * This the unit of task scheduled by the Kernel class.
 */

#ifndef BOVNSBALGO_ABSTRACT
#define BOVNSBALGO_ABSTRACT

namespace  Bob {

/** @defgroup VNSAlgoGRP The Variable Neighborhood Search Algorithm.
  *  @brief This module describe all classes to implement a Variable Neighborhood Search algorithm.
  *  @ingroup UserAlgoGRP
  *  @{

The VNS algorithm implemented in the Bobpp framwork is the following :

 -# x = initial solution; k = 0
 -# Randomly choose a solution y in N_k(x)
 -# Apply a local search starting with y to obtain y’
 -# If y’ improves upon x then x = y’ and k = 0; otherwise k = k + 1
 -# If k >= K, stop
 .

In a multi threaded environment, the search is a bit different.
several searches are spawned. The thread picks up the next solution to be explored in the "global priority queue".
In fact, the number of search may be equal to the number of thread but may be also different.

Each classes use to implements Variable Neighborhood Search inherits from their
associated classes of the module @ref BaseAlgoGRP.


*/

/** The Variable Neighborhood Search node
    a Node of a Variable Neighborhood Search is 
       - scheduled then it inherits form the base_Node.
       - costed then it inherits form the Cost class. the Cost class is templatized with the type
         of the cost and the sens of comparison (Min or Max).
       - a PQNode because it could be inserted in a priority queue.
 */
template <class T, class Compare>
class VNSNode : public Cost<T, Compare> , public base_Node ,
    public PQNode<VNSNode<T, Compare> > {
public :
  /// Type of the enclosed PQNode.
  typedef PQNode<VNSNode<T, Compare> > ThePQNode;
  /// Constructor
  VNSNode() : Cost<T, Compare>() , base_Node(), ThePQNode(),walk(0),k(0),local_search(0) { }
  /// Constructor
  VNSNode(T v) : Cost<T, Compare>(v) , base_Node(), ThePQNode(),walk(0),k(0),local_search(0) { }
  /** Constructor
    * @param bn the VNSNode used to copy
    */
  VNSNode(VNSNode<T, Compare> *bn) : Cost<T, Compare>(bn->getCost()),
      base_Node(*bn), ThePQNode(),walk(bn->walk),k(bn->k),local_search(bn->local_search) { }
  /** Constructor
    * @param bn the VNSNode used to copy
    */
  VNSNode(VNSNode<T, Compare> &bn) : Cost<T, Compare>(bn.getCost()) ,
      base_Node(bn), ThePQNode(),walk(bn.walk),k(bn.k),local_search(bn.local_search) { }
  /** Constructor
    * @param bn the VNSNode used to copy
    */
  VNSNode(const VNSNode<T, Compare> &bn) : Cost<T, Compare>(bn.getCost()) ,
      base_Node(bn), ThePQNode(),walk(bn.walk),k(bn.k),local_search(bn.local_search) { }
  /// Destructor
  virtual ~VNSNode() {}
  /** copy method
    */
  virtual void copy(const VNSNode<T, Compare> &tn) {
    base_Node::copy(tn);
    Cost<T, Compare>::copy(tn);
    set(tn);
  }
  /** print the node information
    * @param os the ostream to display the node information
    */
  virtual std::ostream &Prt(std::ostream &os) const {
    base_Node::Prt(os);
    os << "Cost:" << Cost<T, Compare>::getCost() <<" Walk:"<<walk<<" K:"<<k<<" LS:"<<(is_ls()?"Yes":"No ")<<std::endl;
    return os;
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void log_space(strbuff<> &st) {
    base_Node::log_space(st);
    st << "," << Cost<T, Compare>::getCost();
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void lab_log_space(strbuff<> &st) {
    base_Node::log_space(st);
    st << "," << "evaluation";
  }

  /** Pack method to serialize the Node
    * @param bs the Serialize object to serialize the VNSNode.
    */
  virtual void Pack(Serialize &bs)  const {
    base_Node::Pack(bs);
    Cost<T, Compare>::Pack(bs);
  }
  /** Unpack method to deserialize the Node
    * @param bs the DeSerialize object to deserialize the VNSNode.
    */
  virtual void UnPack(DeSerialize &bs)  {
    base_Node::UnPack(bs);
    Cost<T, Compare>::UnPack(bs);
  }
  /** get the walk index of the node
    */
  int get_walk()  const{ return walk; }
  /** set the walk index of the node
    * @param _w the walk index to set.
    */
  void set_walk(int _w) { walk=_w; }
  /** get the k value of the node
    */
  int getk()  const{ return k; }
  /** set the k value of the node
    * @param _k the new k value
    */
  int setk(int _k) { return k=_k; }
  /** get the state of the node i.e. is in local search or not
    */
  int get_ls() const { return local_search; }
  /** set the node state (local search or not)
    * @param _ls if true the node is local search.
    */
  int set_ls(int _ls) { return local_search=_ls; }
  /** test if the node is in local search mode
    */
  bool is_ls()  const{ return local_search!=0; }
  /** set method
    */
  virtual void set(const VNSNode<T, Compare> &tn) {
    walk = tn.walk;
    k = tn.k;
    local_search = tn.local_search;
  }
  /** set method
    * @param tn the source node to copy.
    */
  virtual void set(const VNSNode<T, Compare> *tn) {
    set(*tn);
  }

protected:
  int walk; ///< index of walk.
  int k;    ///< the k variable of the node
  int local_search; ///< state of the node (local search or not).
};

/** The Variable Neighborhood Search Instance Algorithm
 * The VNSInstance class inherits from the base_Instance, it defines two abstract methods,
 *    - the InitAlgo() method that inserts the initial root nodes (one for each walks).
 *    - the getSol() method that must returns the initial solution, the incubent.
 */
template <class Trait>
class VNSInstance : public base_Instance  {
public :
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// type of the algorithm.
  typedef typename Trait::Instance TheInstance;
  /// type of evaluation for the node.
  typedef typename TheNode::CostType CostType;
  /** Constructor
    * @param _n an std::string that stores the name of the instance
    * @param nbw the number of walk to initialize
    */
  VNSInstance(std::string &_n,int nbw=1) : base_Instance(_n),nb_walk(nbw) {
  }
  /** Constructor
    * @param _n a char * that stores the name of the instance
    * @param nbw the number of walk to initialize
    */
  VNSInstance(const char *_n,int nbw=1) : base_Instance(_n),nb_walk(nbw) {
  }
  /** Constructor
    * with no parameter the name is "??"
    */
  VNSInstance() : base_Instance("??") {
  }
  /** Constructor with copy of a VNSInstance
    * @param bbi the VNSInstance to copy
    */
  VNSInstance(const VNSInstance &bbi) : base_Instance(bbi) {
  }
  /// Destructor
  virtual ~VNSInstance() {}
  /// Get the solution
  virtual TheNode *getSol() = 0;
  /** Initialize the given algorithm, to prepare the search.
    * This method must insert the walk root nodes.
    * @param al the algorithm to initialize
    */
  virtual void InitAlgo(TheAlgo *al) const = 0;
  /** set the number of walk
    * @param nw the number of walk
    */
  void set_nbwalk(int nw) { nb_walk=nw; }
  /** get the number of walk
    */
  int get_nbwalk() const { return nb_walk; }
  /// Pack method to serialize the Node
  virtual void Pack(Serialize &bs) const {
    base_Instance::Pack(bs);
  }
  /// Unpack method to deserialize the Node
  virtual void UnPack(DeSerialize &bs)  {
    base_Instance::UnPack(bs);
  }
protected:
  /// the number of walks
  int nb_walk;
};



/** @defgroup VNSGoal The Different Variable Neighborhood Search Goals
 * A Bob++ Algo coud be instancied with one goal or another.
 * The goal must be defined in the Trait class
 * @{
 */

/** The Classical Variable Neighborhood Search Goal.
 * This goal is the classical Variable Neighborhood Search goal where the aim of the
 * search is to get the best solution.
 */
template<class Trait>
class VNSGoalBest  {
public :
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of a Node
  typedef typename TheNode::CostType  TheType;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
protected:
  /** the Statistics of the Goal
    */
  SchedGoalStat st;
  /** boolean to know if a solution must be displayed at each update
    */
  bool verb;
  /// the solution
  TheNode *sol;
  /** array of node to store solutions of each walk.
    */
  std::vector<TheNode *> walk;
public:
  /** Constructor
    * @param l a bool if true the stat of the goal are logged.
    */
  //VNSGoalBest(bool l=false):  st(l),verb(false),sol(0),walk(1,(TheNode *)0) { }
  /** Constructor
    * @param id the algorithm identifier
    * @param l a bool if true the stat of the goal are logged.
    */
  VNSGoalBest(const Id &id,bool l=false): st(id,l),verb(false),sol(0),walk(1,(TheNode *)0) { }
  /** Constructor
    * @param bb the source goal
    */
  VNSGoalBest(VNSGoalBest<Trait> &bb): st(bb), verb(bb.verb), sol(0),walk(bb.walk) {
    //std::cout << "VNSGoalBest Copy &bb\n";
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
    for (int i=0;i<walk.size();i++ ) {
      if ( walk[i]!=0 ) { walk[i]->addRef(); }
    }
  }
  /** Constructor
    * @param bb the source goal
    */
  VNSGoalBest(const VNSGoalBest<Trait> &bb): sol(0),walk(bb.walk) {
    //std::cout << "VNSGoalBest Copy const &bb\n";
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
    for (int i=0;i<walk.size();i++ ) {
      if ( walk[i]!=0 ) { walk[i]->addRef(); }
    }

  }
  /// Destructor
  virtual ~VNSGoalBest() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
    for (unsigned int i=0;i<walk.size();i++ ) {
      if ( walk[i]!=0 ) { 
          if ( walk[i]->remRef()== 0 )  { delete walk[i];walk[i]=0; }
        
      }
    }
  }
  /** get the value of the best solution
    * @return the cost of the incubent.
    */
  TheType getBest() {
    if (sol == 0) return 0;
    return sol->getCost();
  }
  /** Intialize the Goal with the given instance.
    * @param ti the instance.
    * Mainly this method call the VNSInstance::getSol() method to get the incubent.
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    if ( sol!=0 ) {
       std::cout << "Node Sol:"<<*sol<<std::endl;
       sol->addRef();
       sol->addRef();
       walk[sol->getk()] = sol;
    } else 
       std::cout << "Node Sol:is null"<<std::endl;
    walk.resize(ti->get_nbwalk());
  }
  /** Method called far all node
    * @param n the solution node
    * @param u =0 the solution may be displayed
    */
  virtual bool update(TheNode *n, int u = 0) {
    DBGAFF_ALGO("VNSGoalBest::update()", "Enter");
    if (n == 0) return false;
    if (sol == 0 || (*n) > (*sol) ) {
      std::cout<<" Mise à jour global\n";
      DBGAFF_ALGO("VNSGoalBest::updsol()", "A Better Solution is found");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      //if ( u==0 ) getStat()->upd(n);
      sol = n;
      n->setk(0);
      sol->addRef();
      if (core::dispBestVal() && u == 0 && verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
    }
    if ( !n->is_ls()  || walk[n->get_walk()]==0 || (*n) > (*walk[n->get_walk()]) ) {
      if ( walk[n->get_walk()]!=0 &&  walk[n->get_walk()]->remRef()==0 ) 
         delete walk[n->get_walk()];
      std::cout<<" Mise à jour du walk"<<n->get_walk()<<"\n";
      walk[n->get_walk()] = n;
      n->addRef();
      return true;
    }
    if ( n==walk[n->get_walk()] ) return true;
    return false;
  }

  /** Method to test the value of a solution according to the walk.
    */
  bool is4search(int w,long c) {
    if ( walk[w]==0 || c >= (walk[w]->getCost()) ) {
      return true;
    }
    return false;
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode *n) {
    DBGAFF_ALGO("VNSGoalBest::is4search()", "Enter");
    /*if ( walk[n->getk()]==0 ) 
        std::cout << "Do is4search :null with node:"<<n->getCost()<<std::endl;
    else 
        std::cout << "Do is4search :"<<walk[n->getk()]->getCost()<<" with node:"<<n->getCost()<<std::endl;*/
    return TheGoal::update(n,0);
  }
  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol, 1);
  }

  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol, 1);
  }

  /** operator= for the Goal.
    */
  TheGoal &operator=(TheGoal &_tg) {
    final_merge(_tg);
    return this;
  }

  /** Return the best known solution */
  virtual const TheNode * best() {
    return sol;
  }

  /** Return the worst known solution */
  virtual const TheNode * worst() {
    return sol;
  }
  /** display the stat of the goal
    */
  virtual void display_stat(std::ostream &os=std::cout) { 
    st.display(os);
  }

  /** dump the stat of the goal (use in the collect)
    */
  virtual void stat_dump(strbuff<> &s) { 
    st.dump(s);
  }

  /** write the stat header on the log
    */
  virtual void log_header() { /*getStat()->log_header();*/ }
  /** Set the verbosity of the goal
    */
  virtual void set_verbosity(bool _v=true) { verb=_v; }
  /** returns the verbosity of the goal
    */
  virtual bool verbosity() { return verb; }
  /** Prt Method display the Solution data structure
    * @param os the ostream used to display the goal.
    */
  virtual ostream &Prt(ostream &os) const {
    os << "Solution-------------";
    if (sol == 0) {
      os << "NULL\n";
      return os;
    }
    return sol->Prt(os);
  }
  /** Pack method to Serialize the object
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("VNSGoalBest::Pack", "Packing Node");
    //st.Pack(bs);
    bs.Pack(&verb,1);
    if (sol == 0) {
      //std::cout <<" Pack no sol\n";
      bs.Pack(&i, 1);
      return;
    }
    i = 1;
    bs.Pack(&i, 1);
    sol->Pack(bs);
    sol->Prt(std::cout);
  }
  /** Unpack method to deserialize the
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    int i;
    DBGAFF_PCK("VNSGoalBest::UnPack", "Unpacking Node");
    //st.UnPack(bs);
    bs.UnPack(&verb,1);
    bs.UnPack(&i, 1);
    if (i == 0) {
      sol = 0;
      //std::cout <<" UnPack no sol\n";
      return;
    }
    sol = new TheNode();
    sol->UnPack(bs);
    sol->Prt(std::cout);
  }

};

/**
 * @}
 */
/**
 * The Statistics for Variable Neighborhood Search algorithm
 * The Variable Neighborhood Search statistics adds a timer to measure the evaluation time of the nodes.
 */
class VNSStat : public base_Stat {
public:
  /// Constructor
  VNSStat(const Id &s) : base_Stat('B', s) {
    add_counter('i', "Inserted Nodes");
    add_timer('C', "Genchild Calls");
    add_timer('G', "Node generation");
    add_timer('D', "PQ del Calls");
    add_timer('C', "time to cost a node");
    add_timer('N', "time for neihgborhood");
    add_counter('p', "Pruned Nodes");
  }
  /// Destructor
  virtual ~VNSStat() {}
  /// Counts the number of generated nodes
  void start_del() {
    get_timer('D').start();
  }
  /** Counts the number of generated nodes
    * @param bn the deleted node
    */
  void end_del(base_Node *bn) {
    if ( bn==0 ) get_timer('D').end();
    else {
       //std::cout <<"end_del "<<bn->id()<<"\n";
       get_timer('D').end(bn->id());
    }
  }
  /** Counts the number of inserted nodes
    * @param bn the inserted node
    */
  void ins(base_Node *bn) {
    if ( bn==0 ) get_counter('i').add(1);
    else {
      strbuff<> st;
      bn->log_space(st);
      get_counter('i').add(st.str(),1);
    }
  }
  /** Counts the number of prunned nodes, only one 
    * of the two parameters is used.
    * @param bn the prunned node.
    * @param n the number of prunned node.
    */
  void prun(base_Node *bn,int n = 1) {
    if ( bn==0 ) get_counter('p').add(n);
    else {
      strbuff<> st;
      bn->log_space(st);
      get_counter('p').add(st.str(),n);
    }
  }
  /** Counts the number of generated nodes
    * @param bn the node used to call the genchild
    */
  void start_gc(base_Node *bn) {
    get_timer('C').start(bn->id());
  }
  /// Counts the number of generated nodes
  void end_gc(base_Node *bn) {
    get_timer('C').end(bn->id());
  }

  /** start the time for evaluated node
    * @param bn the child node.
    * @param pn the parent node
    * This methods also write in the log the relation of a child node 
    * and its parent node. The goal is to be able to display the tree.
    */
  void start_create(base_Node *bn,base_Node *pn) {
    create(bn,pn);
    get_timer('G').start();
  }
  /** start the time for evaluated node
    */
  void start_create() {
    get_timer('G').start();
  }
  /** end the time for evaluated node
    * @param bn the child node
    * the end_eval method takes the child node to log the end af its evaluation.
    */
  void end_create(base_Node *bn) {
    get_timer('G').end(bn->id());
  }
  /** end the time for evaluated node
    */
  void end_create() {
    get_timer('G').end();
  }
};

/** A Move modify a solution to produce one or several ones. 
 */
template<class Trait>
class Move {
 public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;

  /** the method class  operator().
    * @param g the node to modifiy
    * @param inst the instance of the search
    * @param algo the search algorithm.
    * @param rand if rand is true, the next move is choosen randomly, otherwise several soltuion are generated.
    */
  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool rand=false)=0;
};


/** The Variable Neighborhood Search GenChild
 */
template <class Trait>
class VNSGenChild : public base_GenChild {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
protected:
  /// the algorithm used by the GenChild.
  TheAlgo *algo;
  /// the associated Instance.
  const TheInstance *inst;
  /// vector of declared moves.
  std::vector<Move<Trait> *> moves;
  /// a specific Move is used to perform local search
  Move<Trait> *ls;
public:
  /** Constructor
    * @param _inst the instace
    * @param _al the algorithm tha use this Genchild object
    */
  VNSGenChild(const TheInstance *_inst, TheAlgo*_al) : base_GenChild(),algo(_al),inst(_inst),moves(),ls(0) {  }
  /// Destructor
  virtual ~VNSGenChild() {}
  /** The functor will compute the heuristic.
    * @param g the node to explore
    * the node has the following attributs
    *   - the walk a node is in one walk.
    *   - the state (local search or not)
    *   - the k (which neighborhood has been used to generate it).
    *
    * The algorithm is has follows :
    *   - if the g node is in local search state, the move given by the set_ls() method 
    *     is used to create better new nodes
    *   - if the ls move fails to improve the walk solution, 
    *     a new node is generate using the neigborhood.
    * 
    * In all cases if a better node improve the best known solution, its associated k is set to 0.
    */
  bool operator()(TheNode *g) {
    unsigned int i;
    std::cout << "--------------- VNS Genchild ------------\n";
    std::cout << "Node step:"<<g->get_walk()<<" k:"<<g->getk()<<" ls:"<<g->get_ls()<<" dist:"<<g->dist()<< " cost:"<<g->getCost()<<std::endl;
    if ( g->is_ls() && (*ls)(g,inst,algo)>=1 )  {
      return true;
    } else {
      g->set_ls(false);
      i=g->getk();
      if ( i>=moves.size() ) return false;
      std::cout << "------- try neighbourhood "<<i<<"----- \n";
      g->dist()=0;
      g->setk(i+1);
      if ( (*moves[i])(g,inst,algo,true) ) { g->set_ls(true); return true;}
    }
    return false;
  }
  /** add a Move object for the VNS.
    * @param m the move to add
    */
  void add_move(Move<Trait> *m) {
    moves.push_back(m);
  }
  /** sets a move for the local search.
    * @param m the move used for the local search
    */
  void set_ls(Move<Trait> *m) {
    ls=m;
  }

};


/**
 * The Variable Neighborhood Search Algorithm
 */
template <class Trait>
class VNSAlgo : public base_Algo {
public:
  /// Type of the node
  typedef typename Trait::Node TheNode;
  /// Type of the info
  typedef typename Trait::Instance TheInstance;
  /// Type of the priotiy comparator
  typedef typename Trait::PriComp ThePriComp;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
  /// Type of the priotiy comparator
  typedef typename Trait::GenChild TheGenChild;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type of the pq
  typedef PQInterface<TheNode, ThePriComp,TheGoal> ThePQ;

protected:
  /// the associated Instance.
  const TheInstance *ia;
  /// Pointer to the GPQ object that stores pending nodes.
  ThePQ *gpq;
  /// Pointer to the Goal object that stores the goal to find.
  TheGoal *goal;
  /// the GenChild class
  TheGenChild *gc;
  
public:
  /** Constructor
   * @param i the instance to solve
   * @param pq the Priority queue
   * @param tg the goal of the search
   * @param st the statistics object
   */
  VNSAlgo(const TheInstance *i, ThePQ *pq, TheGoal *tg, TheStat *st) :
      base_Algo(st), ia(i), gpq(pq), goal(tg),
      gc(new TheGenChild(ia, (TheAlgo *)this)) {
    BOBASSERT(gpq != 0, NullPointerException, "VNSAlgo", "GPQ is null");
    DBGAFF_MEM("VNDAlgo::VNSAlgo", "Constructor (Instance *,PQ *,Stat *)");
  }
  /// destructor
  virtual ~VNSAlgo() {
    delete gc;
    gc = 0;
    DBGAFF_MEM("VNSAlgo::~VNSAlgo", "Destructor");
  }
  /// the run method that performs the main loop.
  virtual void operator()() {
    TheNode *n;
    BOBASSERT(base_Algo::isEtInit() || base_Algo::isEtResumed(), StateException,
              "VNSAlgo::Operator()", " This object has not been initialized ");
    base_Algo::StStart();
    base_Algo::toRun();
    GPQ()->addRef();
    DBGAFF_ALGO("VNSAlgo::operator()", "Before the main loop");
    while (base_Algo::isEtRun() && (n = GPQGet()) != 0) {
      DBGAFF_ALGO("VNSAlgo::operator()",
                  "We have a node and the state of the algo is ok");
      if (!goal->is4Search(n)) {
        DBGAFF_ALGO("VNSAlgo::operator()", "Node must be discarded");
        getStat()->prun(n,1);
        if (n->isDel())
          delete n;
        continue;
      }
      getStat()->start_gc(n);
      DBGAFF_ALGO("VNSAlgo::operator()", "Call the genchild");
      n->addRef();
      (*gc)(n);
      n->remRef();
      getStat()->end_gc(n);
      if (n->isDel())
        delete n;
    }
    if ( !base_Algo::isEtRun() ) {
      std::cout << "Algorithm has been stopped\n";
    } else base_Algo::toEnd();
    GPQ()->remRef();
    StEnd();
    DBGAFF_ALGO("VNSAlgo::operator()", "ends !");
  }
  /// Ends the algorithm
  virtual void End() {
    DBGAFF_ALGO("VNSAlgo::end()", "is called");
  }

  /** The Search method
    * This method is called by the GenChild::operator(), for each new
    * generated nodes that are suitable to be explorated.
    * The actual code is simply an insertion of the Node in the GPQ.
    * This methods could be redefined for a specific algorithm.
    * @param n the node to search
    */
  virtual bool Search(TheNode *n) {
    n->addRef();
    /*if (base_Algo<Trait>::goal->update(n)) {
      int count = base_Algo<Trait>::gpq->DelG(*base_Algo<Trait>::goal);
      if (count)
          base_Algo<Trait>::getStat()->prun(0,count);
    }*/
    if (! goal->is4Search(n)) {
      DBGAFF_ALGO("base_Algo::Search()", "Goal wants to discard node");
      n->addRef();
      getStat()->prun(n,1);
      if (n->isDel())
        delete n;
      return false;
    }
    DBGAFF_ALGO("base_Algo::Search()", "Insert the node");
    GPQIns(n);
    return true;
  };

  /**
     @name GPQ Access methods.
  */
  //@{
  /// get the Global priority Queue
  virtual ThePQ *&GPQ() {
    return gpq;
  }
  /// inserts a Node in the GPQ
  void GPQIns(TheNode *n) {
    getStat()->ins(n);
    GPQ()->Ins(n);
  }
  /// gets  a Node in the GPQ
  TheNode *GPQGet() {
    TheNode *n;
    getStat()->start_del();
    n = (TheNode *)GPQ()->Del();
    getStat()->end_del(n);
    return  n;
  }
  //@}

  /// gets the Instance
  const TheInstance *getInstance() {
    return ia;
  }
  /// gets the Instance
  TheGoal *getGoal() {
    return goal;
  }
  /// gets the GenChild
  TheGenChild *getGC() {
    return gc;
  }

  /**
     @name Statistics methods.
  */
  //@{
  /** return the statistics with the right types
    * @return TheStat pointer on the internal algorithm statistics.
    */
  TheStat *getStat() const {
    return (TheStat *)base_Algo::getBStat();
  }
  //@}
  /**
   @name additional Variable Neighborhood Search Statistics
  */
  //@{
  /// start the timer for nodes
  void start_cost(base_Node *bn,base_Node *pn) {
    getStat()->start_eval(bn,pn);
  }
  /// end the timer for evaluated nodes
  void end_cost(base_Node *bn) {
    getStat()->end_eval(bn);
  }
  /// start the timer for evaluated nodes
  void start_cost() {
    getStat()->start_eval();
  }
  /// end the timer for evaluated nodes
  void end_cost() {
    getStat()->end_eval();
  }

  //@}

};

/** @defgroup VNSAlgoSHGRP The shortcuts definition for Variable Neighborhood Search Algorithm
    @brief This modules presents shortcuts definition of the Instance/Algo/Node to
   implement a Variable Neighborhood Search
 */
//@{
/// @name Minimization and integer evaluation
//@{
typedef VNSNode<int, Min<int> > VNSIntMinNode;
//@}
/// @name Maximisation and integer evaluation
//@{
/// Node
typedef VNSNode<int, Max<int> > VNSIntMaxNode;
//@}
/// @name Minimization and long evaluation
//@{
/// Node
typedef VNSNode<long, Min<long> > VNSLongMinNode;
//@}
/// @name Maximisation and long evaluation
//@{
/// Node
typedef VNSNode<long, Max<long> > VNSLongMaxNode;
//@}
/// @name Minimization and double evaluation
//@{
/// Node
typedef VNSNode<double, Min<double> > VNSDoubleMinNode;
//@}
/// @name Maximisation and double evaluation
//@{
/// Node
typedef VNSNode<double, Max<double> > VNSDoubleMaxNode;
//@}
/// @name Minimization and float evaluation
//@{
/// Node
typedef VNSNode<float, Min<float> > VNSFloatMinNode;
//@}
/// @name Maximisation and float evaluation
//@{
/// Node
typedef VNSNode<float, Max<float> > VNSFloatMaxNode;
//@}

//@}
//@}
//
};
#endif



