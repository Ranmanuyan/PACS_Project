/*
 * Bobpp/src/bob_schedalgo.h
 *
 * This source include the abstract data type for the Algo level where
 * the goal is to find axect solution of a problem.
 */

#include <string>


#ifndef BOBSCHEDALGO_HEADER
#define BOBSCHEDALGO_HEADER
namespace Bob {

/*---------------------------------------------------------------------*/
/** @defgroup SchedAlgoGRP Scheduled Algorithm
 *  @ingroup UserAlgoGRP
 *  @brief Algorithm interface when the search is scheduled
 * @{
 *  When the goal of the algorithm is to perform a search in a space,
 *  the space could be explorated using different strategies. The first step is then
 *  to allow to algorithm to schedule the search i.e. to schedule the nodes.
 *  In this case the algorithm uses
 *  -# a notion of Priority associated which each node (see @ref PriGRP)
 *  -# a priority Queue (see @ref PQGRP).
 *  -# a Goal : the algorithm go through the space to find something. The "something" to find is store in the 
 *     #Bob::SchedGoal.
 *
 *  This is also the reason why we have added the sens of optimization @ref OptiSensGRP.
 */

/*---------------------------------------------------------------------*/

/** the class that defines a Node where the search is scheduled
 * This node inherits from the Bob::base_Node class.
 * The added functionality is a int to store the number
 * of solutions represented by the node.
 * if this value equal to 0 the node is a subproblem.
 */
class SchedNode : public base_Node {
protected:
  /// integer giving the number of solutions representing by the node.
  int b;
  /// boolean : recursive node or node.
  int r;
public :
  /// Constructor
  SchedNode(bool sol=false) : base_Node(), b((sol?1:0)),r(false) {
    DBGAFF_MEM("SchedNode::SchedNode", "Constructor (no parameter)");
  }
  /** Constructor
    * @param sn a SchedNode to copy
    */
  SchedNode(SchedNode const *sn): base_Node((base_Node *)sn), b(sn->b),r(false) {
    DBGAFF_MEM("SchedNode::SchedNode", "Constructor (para Node *)");
  }
  /** Constructor
    * @param sn a SchedNode to copy
    */
  SchedNode(SchedNode const &sn): base_Node((base_Node &)sn), b(sn.b),r(false) {
    DBGAFF_MEM("SchedNode::SchedNode", "Constructor (para Node &)");
  }
  /// Destructor
  virtual ~SchedNode() {
    DBGAFF_MEM("SchedNode::~SchedNode", "Destructor");
  }
  /// copy method used by the operator=
  virtual void copy(const SchedNode &sn) {
    base_Node::copy(sn);
    b = sn.getSol();
  }
  /// return true if the node is a solution.
  bool isSol() const {
    return b != 0;
  }
  /// add a number that represents the number of solution stored by the node.
  void addSol(int _s) {
    b += _s;
  }
  /// returns a reference on the number of solution represented by the node.
  int &getSol() {
    return b;
  }
  /// returns a reference on the number of solution represented by the node.
  int getSol() const {
    return b;
  }
  /// sets the node as a solution.
  void setSol() {
    b = 1;
  }
  /// returns true if the node is a subproblem.
  bool isPb() const {
    return b == 0;
  }
  /// sets the node as a subproblem (this is the default).
  void setPb() {
    b = 0;
  }
  /** returns if the node is a recursive node or not
    */
  bool rec() { return r; }
  /** set node has a recursive node
    */
  void set_rec() { r=true; }
  /// Print the node information
  virtual ostream &Prt(ostream &os) const {
    base_Node::Prt(os);
    os << (b ? " Solution " : " Sub-Pb "); //(b?"true":"false");
    return os;
  }
  /** Fill the strbuff with node information
  * @param st the strbuff to fill.
  */
  virtual void log_space(strbuff<> &st) {
    base_Node::log_space(st);
    st << "," << (b ? "s" : "n");
  }
  /** Fill the strbuff with node information
  * @param st the strbuff to fill.
  */
  virtual void lab_log_space(strbuff<> &st) {
    base_Node::log_space(st);
    st << "," << "solution or not";
  }

  /** Pack method to Serialize the SchedNode
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs)  const {
    DBGAFF_PCK("SchedNode::Pack", "Packing Node");
    base_Node::Pack(bs);
    bs.Pack(&b, 1);
  }
  /** Unpack method to deserialize the SchedNode
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SchedNode::UnPack", "Unpacking Node");
    base_Node::UnPack(bs);
    bs.UnPack(&b, 1);
  }
};


/** The SchedInstance only adds a method to obtain the root node of the search space.
 * The SchedInstance class for a scheduled search inherits from the basic Instance.
 * the functionality that has been added is the SchedInstance::GetRoot() method
 * to obtain the root node of the space search.
 *
  */
template <class Trait>
class SchedInstance : public base_Instance {
public :
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type of a Stat
  typedef typename Trait::Stat TheStat;
  /** Constructor
    * @param _n a std::string for the identifier
    */
  SchedInstance(std::string &_n) : base_Instance(_n) {
    DBGAFF_MEM("SchedInstance::SchedInstance", "Constructor (para string)");
  }
  /** Constructor
    * @param _n a char * for the identifier
    */
  SchedInstance(const char *_n) : base_Instance(_n) {
    DBGAFF_MEM("SchedInstance::SchedInstance", "Constructor (para const char *)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  SchedInstance(SchedInstance &ia) : base_Instance(ia) {
    DBGAFF_MEM("SchedInstance::SchedInstance", "Constructor (para Instance &)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  SchedInstance(const SchedInstance &ia) : base_Instance(ia) {
    DBGAFF_MEM("SchedInstance::SchedInstance", "Constructor (para const Instance &)");
  }
  /// Destructor
  virtual ~SchedInstance()  {
    DBGAFF_MEM("SchedInstance::~SchedInstance", "Destructor");
  }
  /** copy method used by the operator=
    */
  virtual void copy(const SchedInstance &si) {
    base_Instance::copy(si);
  }
  /** Operator= for the instance.
    * this operator call the virtual copy method
    */
  const TheInstance &operator=(const TheInstance &ti) {
    copy(ti);
  }
  /** Initialize the search for the algorithm _al.
    * @param _al the algorithm to initialize
    */
  virtual void InitAlgo(TheAlgo *_al) const = 0;
  /** Pack method to serialize the SchedInstance
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const  {
    DBGAFF_PCK("SchedInstance::Pack", "Packing Instance");
    base_Instance::Pack(bs);
  }
  /** Unpack method to deserialize the SchedInstance
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SchedInstance::UnPack", "Unpacking Instance");
    base_Instance::UnPack(bs);
  }
};


/** The GenChild class Generation of the child nodes
  * The operator() must be rewritten by the user to generated the child nodes form the parent node.
  * the operator could ressemble to the following one
  *  \verbatim
       void operator()(MyNode *n) {
          int nbs=inst->getnbson();
          for (int i=0;i<nbs;i++ ) {
              MyNode *ns=new MyNode(n); // create a new node from the parent one
              algo->start_eval(ns,n);   // start the timer for the evaluation
              /// construct and do the evaluation of the node here using ns->setEval(xx)
              algo->end_eval(ns);       // end the evaluation timer
              if ( some test is ok ) ns->setSol(); // the child node is a solution
              Search(ns);  // Call the Search method that inserts the node to the priority queue 
                           // or update the goal if the node is a solution.
          }
       }
    \endverbatim
   * Due to poor performance obtained if too many allocations are called during the search in 
   * the threaded environment, we also propose a second interface in order to perform recursive search. 
   * The idea is to store nodes in an array according to a depth. Than at each time that the GenChild needs a node
   * at the index i, it gets the already allocated node at the index. then the search continue, calling the GenChild::operator()
   * on this node.
   * We propose 2 interfaces in order to perform recursive search. The First one , the simplest performs recursive search
   * when a node depth is greater than a constant stored in the core::depth_rec variable (this value could be changed using the -dr option of of Bobpp application).
   * The simplest interface is as follows :
   * \verbatim
       void operator()(MyNode *n) {
          int nbs=inst->getnbson();
          for (int i=0;i<nbs;i++ ) {
              MyNode *ns=get_node(n); // create a new node or get a recursive node from the parent one
              algo->start_eval(ns,n);   // start the timer for the evaluation
              /// construct and do the evaluation of the node here using ns->setEval(xx)
              algo->end_eval(ns);       // end the evaluation timer
              if ( some test is ok ) ns->setSol(); // the child node is a solution
              Search(ns);  // Call the Search method that inserts the node to the priority queue 
                           // or update the goal if the node is a solution.
          }
       }
    \endverbatim
   * Another interface that is more tricky let the choice to the user to control when the 
   * recursive search is done.
   * \verbatim
       void operator()(MyNode *n) {
         int nbs=inst->getnbson();
         TheNode *ns;                 // child node
         for (int i=0;i<nbs;i++ ) {
           if ( decide to switch in recursive search ) {
             ir = <index of the recursive node>
             ns = get_rec_node(ir);   // try to get a child node from the index ir.
             if ( ns==0 ) {           // this is the first time then no node at the index ir
               ns = new TheNode(n);   // allocat a new node
               declare_as_rec(ns,ir); // declare it as a recursive node
             }  
             ns->copy(n);             // copy data from the parent node
             ns->addRef();            // add ref to forbid the delete of the node
           } else {
             ns = new TheNode(n);     // otherwise no recursive search, the node is allocated.
           }
           algo->start_eval(ns,n);   // start the timer for the evaluation
           /// construct and do the evaluation of the node here using ns->setEval(xx)
           algo->end_eval(ns);       // end the evaluation timer
           if ( some test is ok ) ns->setSol(); // the child node is a solution
           Search(ns);  // Call the Search method that inserts the node to the priority queue 
                        // or update the goal if the node is a solution.
         }
       }
    \endverbatim
    We can also notice that if your application performs a search on a huge tree, that stores many nodes, 
    you get more performance using recursive search. The number of allocated nodes is linear and no more exponential...
  */
template <class Trait>
class SchedGenChild : public base_GenChild {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
protected:
  /// the algorithm used by the GenChild.
  TheAlgo *algo;
  /// the associated Instance.
  const TheInstance *inst;
  /// The  array of node used to perform recursive search
  std::vector<TheNode *> dn;
public:
  /** Constructor
  * @param _inst The instance to solve
  * @param _al The associated algorithm
  */
  SchedGenChild(const TheInstance *_inst, TheAlgo*_al): base_GenChild(),
      algo(_al), inst(_inst), dn(50,(TheNode *)0) {
    DBGAFF_MEM("SchedGenChild::SchedGenChild", "Constructor (Instance *,Algo *)");
  }
  /// Destructor
  virtual ~SchedGenChild() {
    DBGAFF_MEM("SchedGenChild::~SchedGenChild", "Destructor");
  }
  /** operator() which generate the new child nodes and call
  * the Search(nn) method on each of them.
  * @param n the node to explore
  * @return true if n has generated feasible and interesting child nodes
  */
  virtual bool operator()(TheNode *n) = 0;
  /** method to declare that the node is suitable to be search later.
    * @param the node
    */
  virtual bool Search(TheNode *n) {
    if ( n->rec() && !n->isSol() ) 
      return (*this)(n);
    else if ( n->isSol() ) 
      return algo->Search(new TheNode(n));
    else
      return algo->Search(n);
  }
  /** get a recursive node
    * @return an already allocated node
    * be carefull this node pointer could be null !
    */
  TheNode *get_rec_node(int ind) {
    return dn[ind];
  }
  /** Declare a node as a recursive node.
    */
  void declare_as_rec(TheNode *n,unsigned int ind) {
     if ( ind<0 ) { std::cerr <<" Error bad index for recursiv search \n";return; }
     if ( dn[ind]!=0 && dn[ind]!=n ) { 
       std::cerr << "Already exists a node on the recursive array of node \n";
       return;
     }
     if ( ind >dn.size() ) dn.resize(dn.size()+20,(TheNode *)0);
     n->set_rec();
     n->addRef();
     n->addRef();
     dn[ind] = n;
  }
  /** get a node : recursive or not.
    * According to the recursive search strategy this function could return a new allocated node or a recursive node 
    * (already allocated).
    * @param p the parent node used to copy the data.
    * @return a TheNode that could be a recursive node or a new allocated node
    */
  TheNode *get_node(TheNode *p) {
    int ir = p->dist()+1-Bob::core::get_depth_rec();
    TheNode *child;
    if ( ir>=0 ) {
      //std::cout<<pthread_self()<<"   Rec node "<<p->dist()+1<<":"<<ir<<"\n";
      child = get_rec_node(ir);
      if ( child==0 ) {
        child = new TheNode(p); 
        declare_as_rec(child,ir);
        //std::cout<<pthread_self()<<"  Declare node:"<<ir<<"\n";
      }  
      child->copy(p);
      child->addRef();
    } else {
      child = new TheNode(p); 
    }
    return child;
  }
};


/** The Goal Stat
 */
class SchedGoalStat : public stat {
public:
  /// Constructor 
  SchedGoalStat() : stat('G',CId('?'),"Goal Stat",false) {}
  /// Constructor
  SchedGoalStat(const Id &id,bool l=false): stat('G',id,"Goal Stat",l) {
    add_counter('s', "Generated Solution",0);
    add_counter('u', "Updated Solution",1);
    add_counter('m', "Merge Goal",0);
  }
  /// Destructor
  virtual ~SchedGoalStat() {}
  /// counts the number of solutions found
  virtual void sol(long l = 1) {
    get_counter('s')+=l;
  }
  /// counts the number of update of the solution
  virtual void upd(base_Node *bn=0) {
    if ( bn==0 ) get_counter('u').add(1);
    else {
      strbuff<> st;
      bn->log_space(st);
      get_counter('u').add(st.str(),1);
    }
  }
  /// counts the number of calls to the merge method
  virtual void merge(long l = 0) {
    get_counter('m')++;
  };

};


/** The abstract Goal for a SchedAlgo
 *  The class is reimplemented for the concrete algorithm in order to control
 *  the search.
 *  This class could have different implementation according to the aim of
 *  the search. For example for a Branch and Bound, one want to have the
 *  optimal solution, to count the number of optimal solution, to have
 *  the n best soltuions, etc...
 *  The goal of the search must be defined in the trait class for a
 *  specific problem code.
 *  The main methods are
 *    - is4Search(Node *n) that tests if a node is suitable to be explored.
 *    - update(Node *n) that tests if a node (that must be a solution) is
 *      a candidate to update the best solution known.
 *    - final_merge(Goal *g) performs the final merge.
 *      This method is used at the end of the resolution.
 *      This method is only used in parallel environment.
 *    - current_merge(Goal *g) that performs a merge from a Goal given in
 *      parameter, this method is used during the resoltuion. This method is
 *      only used in parallel environment.
 *    - best() returns the best known solution
 *    - worst() returns the worst solution retained by the Goal. Its evaluation
 *      is different from the one from best() only in the case where the Goal
 *      stores solutions with different evaluations.
 */
template<class Trait>
class SchedGoal {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type of the Algo
  typedef typename Trait::Stat TheStat;
  /// Type of the priority
  typedef typename Trait::PriComp ThePriComp;
  /// Type of the GenChild
  typedef typename Trait::GenChild TheGenChild;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Priority Queue
  typedef PQInterface<TheNode, ThePriComp,TheGoal> ThePQ;
  /** Constructor
    */
  SchedGoal(bool l=false): st(),verb(false) {
  }
  /** Constructor
    */
  SchedGoal(const Id &id,bool l=false): st(id,l),verb(false) {
  }
  /// Destructor
  virtual ~SchedGoal() {
  }
  /** Init method
    * @param ti an Instance object
    */
  virtual void Init(TheInstance *ti) = 0;
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
  virtual void log_header() { st.log_header(); }
  /** handleSol method is called for a solution node
    */
  virtual bool update(TheNode *n) {
    std::cout << "Warning : Call SchedGoal update() always return false\n";
    return false;
  }
  /** is4Search method
    * @param _n the node to test
    */
  virtual bool is4Search(TheNode *_n) const {
    std::cout << "Warning : Call SchedGoal i4search() always return true\n";
    return true;
  }
  /** the current merging method is used to update the solution
    * with another one, during the execution of algorithms.
    * this method is used in distributed environment.
    * @param _tg the Goal to merge
    */
  virtual bool current_merge(TheGoal *_tg) {
    std::cout << "Warning : Call SchedGoal current_merge() do nothing\n";
    return true;
  }
  /** the final merging method is used to update the solution
    * with another one, but at the end of the execution of the algorithm.
    * this method is used in distributed environment.
    * @param _tg the Goal to merge
    */
  virtual bool final_merge(TheGoal *_tg) {
    std::cout << "Warning : Call SchedGoal final_merge() do nothing\n";
    return true;
  }
  /** operator= for the Goal.
    */
  TheGoal &operator=(TheGoal &_tg) {
    final_merge(_tg);
    return this;
  }
  /** Set the verbosity of the goal
    */
  virtual void set_verbosity(bool _v=true) { verb=_v; }
  /** returns the verbosity of the goal
    */
  virtual bool verbosity() { return verb; }
  /** Return the best known solution */
  virtual const TheNode * best() = 0;
  /** Return the worst known solution */
  virtual const TheNode * worst() = 0;

  /* counts the number of solutions found
   */
  virtual void stat_sol(long l = 1) { st.sol(l); }
  /// counts the number of update of the solution
  virtual void stat_upd(base_Node *bn=0) {
    st.upd(bn);
  }
  /// counts the number of calls to the merge method
  virtual void stat_merge(long l = 0) {
    st.merge(l);
  }
  /** Pack method to Serialize the SchedNode
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("SchedGoal::Pack", "Packing Node");
    st.Pack(bs);
    bs.Pack(&verb,1);
  }
  /** Unpack method to deserialize the SchedNode
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SchedGoal::UnPack", "Unpacking Node");
    st.UnPack(bs);
    bs.UnPack(&verb,1);
  }

protected:
  /** the Statistics of the Goal
    */
  SchedGoalStat st;
  /** boolean to know if a solution must be displayed at each update
    */
  bool verb;
};

/**
 * The SchedStatAlgo represents the statistics for the SchedAlgo
 * it inherits from base_stat.
 */
class SchedStat : public base_Stat {
public:
  /** Constructor
   * @param n the identifier of the this algorithm
   * @param _id the parent identifier
   */
  SchedStat(char n, const Id &_id) : base_Stat(n, _id) {
    add_counter('i', "Inserted Nodes");
    add_counter('n', "Nonfeasible Nodes");
    add_counter('p', "Pruned Nodes");
    add_timer('C', "Genchild Calls");
    add_timer('D', "PQ del Calls");
  }
  /// destructor
  virtual ~SchedStat() {
  }
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
  /** Counts the number of non feasible nodes
    * @param bn the nonfeasible node
    */
  void nofeas(base_Node *bn) {
    if ( bn==0 || !core::space_search()  ) get_counter('n').add(1);
    else  {
      strbuff<> st;
      bn->log_space(st);
      get_counter('n').add(st.str(),1);
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
};


/** The SchedAlgo class
  *
  * This class is used when the search in the space is scheduled.
  *
  * The scheduling could be defined according to a priority (@ref PriGRP),
  * the class has parametrized type called PriAllocator that has a methods GetPri(Node *)
  * to obtain a Pri object associated with the Node gien by parameter.
  *
  * The SchedAlgo class define several functionalities
  *   - has the Traitclass as parametrized type
  *   - a GPQ is used to store the pending nodes.
  *   - A PriAllocator (as parametrized type) to create a priority object associated
  *     to each pending node
  *   - create and use a GenChild object call <b>gc</b> (the exact type is given by Trait class defined as parametrized type)
  *   - define the Generate() method (which is abstract in this class)
  *   - define the Search() method (which is abstract in this class)
  *   - the funtion class (operator()) which is a template algorithm,  which is mainly a loop
  *     \verbatim
  *         while the GPQ is not empty
  *            Get the best node n from the GPQ.
  *            Call the Generate method on n to know if n is suitable to be generated
  *            if ok call the gc(n) (the function class associated to the GenChild object gc)
  *         end while.
  *     \endverbatim
  *     Notice that the operator()(node *n) of the class GenChild must call the Search(Node *c)
  *     method of the algorithm to know what to do with new generated child node c
  *
  */
template <class Trait>
class SchedAlgo : public base_Algo {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type of the Algo
  typedef typename Trait::Stat TheStat;
  /// Type of the priority
  typedef typename Trait::PriComp ThePriComp;
  /// Type of the GenChild
  typedef typename Trait::GenChild TheGenChild;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Priority Queue
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
  // the current parent node
  //TheNode *parent;
public:
  /** Constructor
   * The Genchild used by this algorithm is internaly allocated.
   * @param bsia the instance to solve
   * @param _pq the Priority queue
   * @param tg the Goal of the Search
   * @param st the statistics object
   */
  SchedAlgo(const TheInstance *bsia, ThePQ *_pq, TheGoal *tg, TheStat *st) :
      base_Algo(st), ia(bsia), gpq(_pq), goal(tg),
      gc(new TheGenChild(ia, (TheAlgo *)this))/*, parent(0)*/ {
    BOBASSERT(gpq != 0, NullPointerException, "SchedAlgo", "GPQ is null");
    DBGAFF_MEM("SchedAlgo::SchedAlgo", "Constructor (Instance *,PQ *,Stat *)");
      //std::cout << "Schedalgo:Goal "<<typeid(*goal).name()<<"\n";
      //std::cout << "Schedalgo:PQ : "<<typeid(*gpq).name()<<"\n";

  }
  /** Destructor
    * The Genchild is deleted
    */
  virtual ~SchedAlgo() {
    delete gc;
    gc = 0;
    DBGAFF_MEM("SchedAlgo::~SchedAlgo", "Destructor");
  }
  /// the run method that performs the main loop.
  virtual void operator()() {
    TheNode *n;
    BOBASSERT(base_Algo::isEtInit() || base_Algo::isEtResumed(), StateException,
              "SchedAlgo::Operator()", " This object has not been initialized ");
    base_Algo::StStart();
    base_Algo::toRun();
    GPQ()->addRef();
    DBGAFF_ALGO("SchedAlgo::operator()", "Before the main loop");
    while (base_Algo::isEtRun() && (n = GPQGet()) != 0) {
      DBGAFF_ALGO("SchedAlgo::operator()",
                  "We have a node and the state of the algo is ok");
      if (!goal->is4Search(n)) {
        DBGAFF_ALGO("SchedAlgo::operator()", "Node must be discarded");
        getStat()->prun(n,1);
        if (n->isDel())
          delete n;
        continue;
      }
      //parent = n;
      getStat()->start_gc(n);
      DBGAFF_ALGO("SchedAlgo::operator()", "Call the genchild");
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
    base_Algo::StEnd();
    DBGAFF_ALGO("SchedAlgo::operator()", "ends !");
  }
  /// Ends the algorithm
  virtual void End() {
    DBGAFF_ALGO("SchedAlgo::end()", "is called");
  }
  /** The Search method
    * This method is called by the GenChild::operator(), for each new
    * generated nodes that are suitable to be explorated.
    * The actual code is simply an insertion of the Node in the GPQ.
    * This methods could be redefined for a specific algorithm.
    * @param n the node to search
    */
  virtual bool Search(TheNode *n) {
    if (n->isSol()) {
      n->addRef();
      if (goal->update(n)) {
        //std::cout << "Schedalgo::Search Goal "<<typeid(*goal).name()<<"\n";
        int count = gpq->DelG(*goal);
        if (count)
          getStat()->prun(0,count);
      }
      if (n->remRef() == 0)
        delete n;
      return false;
    }
    if (! goal->is4Search(n)) {
      DBGAFF_ALGO("SchedAlgo::Search()", "Goal wants to discard node");
      getStat()->prun(n,1);
      if (n->isDel())
        delete n;
      return false;
    }
    DBGAFF_ALGO("SchedAlgo::Search()", "Insert the node");
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
  /// return the statistics with the right types
  TheStat *getStat() const {
    return (TheStat *)base_Algo::getBStat();
  }
  /// statisitics for non feasible nodes
  void StNoFea(TheNode *n=0) {
    getStat()->nofeas(n);
  }
  /// statisitics for non feasible nodes
  void StPrun(TheNode *n=0) {
    getStat()->prun(n);
  }
  //@}
};


//@}
};
#endif

