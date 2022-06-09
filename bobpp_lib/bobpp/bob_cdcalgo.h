/*
 * bobpp/bob_cdcalgo.h
 *
 * This source include the data type for the Divide and Conquer Algo level.
 * This the unit of task scheduled by the Kernel class.
 */

#ifndef BOBCDCALGO_ABSTRACT
#define BOBCDCALGO_ABSTRACT

namespace Bob {

/** @defgroup CDCAlgoGRP The Costed Divide and Conquer Algorithm
    @ingroup DCAlgoGRP
    brief This module describe all classes to implement Divide
           and Conquer algorithms, when the goal is a Costed solution.

In Costed Divide and Conquer algorithm,  we find the minimum or the
maximum value for a solution for a given problem.
The node of the search are constructed recursively, specifing a little
at each stage the cost of the sub-problem.
If a the goal is to find just one feasible solution to a problem have
a look to @ref SDCAlgoGRP

For example, we could solve a Problem CPB using this method,
we give step by step the specific classes and their associated
methods and attributs.

  -# Define the CPBTrait class with the forward definition of the classes.
     \verbatim
     class CPBNode;
     class CPBInstance;
     class CPBGenChild;

     class CPBTrait {
     public:
        typedef CPBNode Node;
        typedef CPBInstance Instance;
        typedef CPBGenChild GenChild;
        typedef Bob::DepthPri<KSNode> PriComp;
        typedef CDCGoalBest<CPBTrait> Goal;
        typedef CDCAlgo<CPBTrait> Algo;
        typedef CDCStat stat;
     };
     \endverbatim
  -# define the CPBNode class
     \verbatim
      /// the SPB Node
      class SPBNode : public CDCIntMaxNode {
        protected:
        ... // Adds the attribut to represent your own problem
        public :
        /// Constructor
        SPBNode() : CDCIntMaxNode(){}
        /// Constructor
        SPBNode(int size) : CDCIntMaxNode(0) {
            /// the parameter of the CDCIntMaxNode is the default cost.
        }
        /// Constructor
        SPBNode(SPBNode &qn) : CDCIntMaxNode(qn){... }
        /// Constructor
        SPBNode(SPBNode *qn) : CDCIntMaxNode(qn){...}
        /// Display a QAP Node
        virtual ostream &Prt(ostream &os) const {
           ...
           return os;
        }
      };
     \endverbatim
     To fixe the cost of a node you could use the methods for the Cost in the
     Cost class (setCost(T t) and T getCost()).
     This CPBNode inherits form the CDCIntMaxNode, but could also inherits
     from the CDCLongMinNode, etc... @ref CCDCAlgoSHGRP
     You could also inherit from the CDCNode <T,Compare> where T is a type
     for a cost and Compare is a class to Compare these cost and could
     be Min or Max.
  -# defines the CPBGenChild
     \verbatim
      class CPBGenChild : public GenChild<CPBTrait>  {
        public:
          /// Constructor
          CPBGenChild(const CPBInstance *_inst, CPBTrait::Algo *_sch) :
                 GenChild<CPBTrait>(_inst,_sch) {}

          /// the operator() that generate the child nodes.
          virtual void operator()(CPBNode *n)  {
              ...
              //for each New CBPNode *fn generated that are feasible
                  algo->Search(fn);
              // could be a solution or not a solution.
          }
      };
     \endverbatim
     When you call a Search(n) with CDCNode::IsSol() that returns true, n is not
     reinserted in the GPQ.
     This semantic could be a problem, for some application. For example
     when solving a Knapsac problem using a Costed divide and Conquer
     algorithms, one algorithm is at the level i to create two child nodes,
     one where the object i has been added in the bag and one where the
     object i is not added in the bag.
     Thus a node could is a solution and a subproblem at the same time.
     you could see the sources of the Bob++ knapsac application to see
     an example to solve this problem.
  -# defines the CPBInstance
     \verbatim
      /// the Instance Algo class for the CPB.
      class CPBInstance : public CDCInstance<CPBTrait> {
         /// Attributs  : global data for your problem.
      public:
         /// Constructor
         CPBInstance() : CDCInstance<CPBTrait>() {
             AddTitle("CPB ");
             ... /// Adds the option and parameters for the application
         }
         ///Intialization
         void Init() {
             ... /// Mainly initialize the attibuts
         }
         ///Intialization of the Algorithm
         void InitAlgo(CPBTrait::Algo *a) const {
             CPBTrait::Node *n = new CPBTrait::Node(...);
             // ... initialisation of the node
             a->GPQIns(n);
         }
         /// Get the initial best known solution
         CPBNode *getSol() {
             std::cout<<"Instance::getSol "<<0 <<"\n";
             return new CPBNode();
         }

      };
     \endverbatim

@{
*/

/** The costed Divide and Conquer node
    a Node is costed.
 */
template < class T, class Compare >
class CDCNode: public Cost < T, Compare >, public SchedNode,
    public PQNode<CDCNode<T, Compare> > {
  bool b;
public:
  /// type of the enclosed PQNode.
  typedef PQNode<CDCNode<T, Compare> > ThePQNode;
  /// Constructor
  CDCNode(bool sol=false): Cost < T, Compare > (sol), SchedNode(sol), ThePQNode() {
  }
  /** Constructor
    * @param v the cost of the node
    */
  CDCNode(T v): Cost < T, Compare > (v), SchedNode(), ThePQNode() {
  }
  /** Constructor
    * @param bn the Node to copy
    */
  CDCNode(CDCNode < T, Compare > *bn): Cost < T, Compare > (bn->getCost()),
      SchedNode(*bn), ThePQNode() {
  }
  /** Constructor
    * @param bn the Node to copy
    */
  CDCNode(const CDCNode < T, Compare > &bn): Cost < T, Compare > (bn.getCost()),
      SchedNode(bn), ThePQNode() {
  }
  /** Constructor
    * @param bn the Node to copy
    */
  CDCNode(CDCNode < T, Compare > &bn): Cost < T, Compare > (bn.getCost()),
      SchedNode(bn), ThePQNode() {
  }
  /**Destructor
    */
  ~CDCNode() { }  
  /** copy method
  */
  virtual void copy(const CDCNode<T, Compare> &tn) {
    SchedNode::copy(tn);
    Cost<T, Compare>::copy(tn);
  }

  /// Print the node information
  virtual std::ostream & Prt(std::ostream & os) const {
    SchedNode::Prt(os);
    os << "C:"<<Cost<T, Compare>::getCost();
    return os;
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void log_space(strbuff<> &st) {
    SchedNode::log_space(st);
    st << "," << Cost<T, Compare>::getCost();
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void lab_log_space(strbuff<> &st) {
    SchedNode::log_space(st);
    st << "," << "cost";
  }
  /** Pack method to serialize the Node
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize & bs) const {
    SchedNode::Pack(bs);
    Cost < T, Compare >::Pack(bs);
    DBGAFF_PCK("CDCNode::Pack", "Packing the Node");
  }
  /** UnPack method to deserialize the Node
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize & bs) {
    SchedNode::UnPack(bs);
    Cost < T, Compare >::UnPack(bs);
    DBGAFF_PCK("CDCNode::UnPack", "Unpacking the Node");
  }

};

/** The Divide and Conquer Instance
 */
template < class Trait >
class CDCInstance: public SchedInstance<Trait> {
public:
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// type of the algorithm.
  typedef typename Trait::Instance TheInstance;
  /** Constructor
    * The default identifier of the Instance is ??
    */
  CDCInstance(): SchedInstance < Trait > ("??") {
    DBGAFF_MEM("CDCInstance::CDCInstance", "Constructor");
  }
  /** Constructor
    * @param _n a std::string for the identifier
    */
  CDCInstance(std::string &_n): SchedInstance < Trait > (_n) {
    DBGAFF_MEM("CDCInstance::CDCInstance", "Constructor (para string)");
  }
  /** Constructor
    * @param _n a char * for the identifier
    */
  CDCInstance(const char *_n): SchedInstance < Trait > (_n) {
    DBGAFF_MEM("CDCInstance::CDCInstance", "Constructor (para char *)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  CDCInstance(TheInstance &ia) : SchedInstance<Trait>(ia) {
    DBGAFF_MEM("CDCInstance::CDCInstance", "Constructor (para Instance &)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  CDCInstance(const TheInstance &ia) : SchedInstance<Trait>(ia) {
    DBGAFF_MEM("CDCInstance::CDCInstance", "Constructor (para const Instance &)");
  }
  /** Initialize the Instance.
    */
  virtual void Init() = 0;
  /** Pack method to serialize the CDCInstance
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize & bs) const {
    SchedInstance < Trait >::Pack(bs);
  }
  /** Unpack method to deserialize the CDCInstance
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize & bs) {
    SchedInstance < Trait >::UnPack(bs);
  }

};

/**
  * The Statistics for Costed Divide and Conquer algorithm
  */
class CDCStat: public SchedStat {
public:
  /// Constructor
  CDCStat(const Id &s): SchedStat('C', s) {
    add_counter('t', "Costed Nodes");
  }
  /// Counts the number of costed nodes
  void Cost(base_Node *n,base_Node *p) {
    create(n,p);
    get_counter('t')++;
  }
};

/** The Divide and Conquer GenChild
  */
template < class Trait >
class CDCGenChild: public SchedGenChild < Trait > {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /** Constructor
    * @param _inst The instance to solve
    * @param _al The associated algorithm
    */
  CDCGenChild(const TheInstance * _inst, TheAlgo * _al):
      SchedGenChild < Trait > (_inst, _al) {
    DBGAFF_MEM("CDCGenChild::CDCGenChild", "Constructor (Instance *,Algo *)");

  }
  /// Destructor
  virtual ~CDCGenChild() {
    DBGAFF_MEM("CDCGenChild::~CDCGenChild", "Destructor");
  }
};

/**  @defgroup CDCGoal The Different Costed Divide and Conquer Goals
 * A Bob++ Algo coud be instancied with one goal or another.
 * The goal must be defined in the Trait class
 * @{
 */

/** The Classical Costed Divide and Conquer Goal.
 * This goal is the classical Divide and Conquer goal where the aim of the
 * search is to get the best solution.
 */

template<class Trait>
class CDCGoalBest : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;

protected:
  /// The Solution object
  TheNode *sol;
public:
  /// the type of the Evaluation (int, long, float, ...)
  typedef typename TheNode::CostType CostType;
  /// Constructor
  CDCGoalBest(bool l=false): SchedGoal<Trait>(l), sol(0) {
  }
  /// Constructor
  CDCGoalBest(const Id &id,bool l=false): SchedGoal<Trait>(id,l), sol(0) {
  }
  /// Constructor
  CDCGoalBest(CDCGoalBest<Trait> &cg): SchedGoal<Trait>(cg), sol(0) {
    if (cg.sol != 0) {
      sol = cg.sol;
      sol->addRef();
    }
    //std::cout << "CDCGoalBest Constructeur Copy\n";
  }
  /// Constructor
  CDCGoalBest(const CDCGoalBest<Trait> &cg): SchedGoal<Trait>(cg), sol(0) {
    if (cg.sol != 0) {
      sol = cg.sol;
      sol->addRef();
    }
    //std::cout << "CDCGoalBest Constructeur Copy const\n";
  }
  /// Destructor
  virtual ~CDCGoalBest() {
    if (sol != 0 && sol->remRef()) {
      delete sol;
      sol = 0;
    }
    //std::cout << "CDCGoalBest Destructeur\n";
  }
  /** Intialize the Goal.
    * @param ti the Instance used for the initialization
     * @todo Warning when the Instance::getSol() return null solution
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    if (sol != 0) {
      sol->addRef();
    } else {
      std::cerr << " Warning : Init return a null solution\n";
    }
  }
  /** Method called when a node is a solution
    * This method is called directly by an algorithm to try to update the base solution. 
    * But it is also used by the merge operations (current_merge and final_merge). 
    * In these last cases the stat is not called.
    *
    * @param n the solution node
    * @param u if equal to 0, (default value) the stat is updated.
    */
  virtual bool update(TheNode *n,int u=0) {
    if (n == 0) return false;
    BOBASSERT(!n->isSol(), Exception, "Goal", "update called with a non solution node");
    DBGAFF_ALGO("CDCGoalBest::updsol()", "Node is a solution");
    if (sol == 0 || (*n) > (*sol)) {
      DBGAFF_ALGO("CDCGoalBest::update()", "A Better Solution is found");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      if ( u==0 ) SchedGoal<Trait>::stat_upd(n);
      sol = n;
      sol->addRef();
      if (u==0 && core::dispBestVal())
        std::cout << "Update the incubent : " << n << std::flush;
      return true;
    }
    return false;
  }

  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) const {
    return ( n->Sens()==1 || sol==0 || (*n)>(*sol) );
    //return true;
  }

  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    //std::cout << "CDCGoalBest current_merge\n";
    if (_tg == 0) return false;
    if (_tg->sol == 0) return false;
    return update(_tg->sol,1);
  }
  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    //std::cout << "CDCGoalBest final_merge\n ";
    if (_tg == 0) return false;
    if (_tg->sol == 0) return false;
    return update(_tg->sol,1);
  }

  /** Return the best known solution */
  virtual const TheNode * best() {
    return sol;
  }

  /** Return the worst known solution */
  virtual const TheNode * worst() {
    return sol;
  }

  /// Prt Method display the Solution data structure
  virtual ostream &Prt(ostream &os) const {
    os << "Solution-------------";
    if (sol == 0) {
      os << "NULL\n";
      return os;
    }
    return sol->Prt(os);
  }
  /** Pack method to Serialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("CDCGoalBest::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
    if (sol == 0) {
      //std::cout <<" Pack no sol\n";
      bs.Pack(&i, 1);
      return;
    }
    i = 1;
    bs.Pack(&i, 1);
    sol->Pack(bs);
  }
  /** Unpack method to deserialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    int i;
    DBGAFF_PCK("CDCGoalBest::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
    bs.UnPack(&i, 1);
    if (i == 0) {
      sol = 0;
      //std::cout <<" UnPack no sol\n";
      return;
    }
    sol = new TheNode();
    sol->UnPack(bs);
    sol->addRef();
  }

};

/** The Goal that counts the number of best solution.
 * This goal is quite different. The aim of the search is to count
 * the number of solution with the best cost.
 */
template<class Trait>
class CDCGoalCount : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Stat
  typedef typename Trait::Stat TheStat;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
protected:
  /// The Solution object
  TheNode *sol;
  /// The number of solution
  int nbsol;
public:
  /// Constructor
  CDCGoalCount(bool l=false) : SchedGoal<Trait>(l), sol(0), nbsol(0) {
    //std::cout << "CDCGoal Constructeur\n";
  }
  /// Constructor
  CDCGoalCount(const Id &id,bool l=false) : SchedGoal<Trait>(id,l), sol(0), nbsol(0) {
    //std::cout << "CDCGoal Constructeur\n";
  }
  /// Constructor
  CDCGoalCount(CDCGoalCount<Trait> &cg): SchedGoal<Trait>(cg), sol(0), nbsol(cg.nbsol) {
    if (cg.sol != 0) {
      sol = cg.sol;
      sol->addRef();
    }
    //std::cout << "CDCGoal Constructeur Copy\n";
  }
  /// Constructor
  CDCGoalCount(const CDCGoalCount<Trait> &cg): SchedGoal<Trait>(cg), sol(0), nbsol(cg.nbsol) {
    if (cg.sol != 0) {
      sol = cg.sol;
      sol->addRef();
    }
    //std::cout << "CDCGoal Constructeur Copy\n";
  }
  /// Destructor
  virtual ~CDCGoalCount() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
    //std::cout << "CDCGoalCount Destructeur\n";
  }
  /** Intialiaze the Goal.
    * @param ti Instance used to initialize the Goal
    * @todo Warning when the Instance::getSol() return null solution
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    if (sol != 0) {
      sol->addRef();
      nbsol = 0;
    } else {
      std::cerr << " Warning : Init return a null solution\n";
    }
  }

  /** Method called when a node is a solution
    * This method is called directly by an algorithm to try to update the base solution. 
    * But it is also used by the merge operations (current_merge and final_merge). 
    * In these last cases the stat is not called.
    *
    * @param n the solution node
    * @param u if equal to 0, (default value) the stat is updated.
    */
  virtual bool update(TheNode *n, int u=0) {
    if (n == 0) return false;
    BOBASSERT(!n->isSol(), Exception, "Goal", "update called with a non solution node");
    //std::cout << "CDCGoalCount Update with";
    //n->Prt(std::cout);
    DBGAFF_ALGO("CDCGoal::update()", "Node is a solution");
    if (sol == 0 || (*n) > (*sol)) {
      DBGAFF_ALGO("CDCGoal::upSol()", "A Better Solution is found");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      if ( u==0 ) SchedGoal<Trait>::stat_upd(n);
      sol = n;
      sol->addRef();
      nbsol = sol->getSol();
      if (u==0 && core::dispBestVal())
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      return true;
    }
    if ((*n) == (*sol)) {
      DBGAFF_ALGO("CDCGoal::is4Search()", "An equal Solution is found");
      if ( core::dispBestVal() ) {
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      }
      if ( u==0 ) SchedGoal<Trait>::stat_upd(n);
      nbsol += n->getSol();
    }
    return false;
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) const {
    return ( n->Sens()==1 || sol==0 || (*n)>(*sol) );
    //return true;
  }
  /** The merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    //std::cout << "CDCGoalBest final_merge\n";
    if (_tg->sol == 0) return false;
    if (sol == 0) {
      sol = _tg->sol;
      sol->addRef();
      nbsol = _tg->nbsol;
      return true;
    }
    if (*(_tg->sol) == (*sol)) {
      nbsol += _tg->nbsol;
      return true;
    }
    if (*(_tg->sol) > (*sol)) {
      nbsol = _tg->nbsol;
      sol = _tg->sol;
      sol->addRef();
      return true;
    }
    return false;
  }
  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    //std::cout << "CDCGoalBest current_merge\n";
    if (_tg->sol == 0) return false;
    if (sol == 0) {
      sol = _tg->sol;
      sol->addRef();
      nbsol = 0;
      return true;
    }
    if (*(_tg->sol) > (*sol)) {
      nbsol = 0;
      sol = _tg->sol;
      sol->addRef();
      return true;
    }
    return false;
  }

  /** Return the best known solution */
  virtual const TheNode * best() {
    return sol;
  }

  /** Return the worst known solution */
  virtual const TheNode * worst() {
    return sol;
  }

  /** Pack method to Serialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("CDCGoalCount::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
    if (sol == 0) {
      bs.Pack(&i, 1);
      return;
    }
    i = 1;
    bs.Pack(&i, 1);
    bs.Pack(&nbsol, 1);
    sol->Pack(bs);
  }
  /** Unpack method to deserialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    int i;
    DBGAFF_PCK("CDCGoalCount::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
    bs.UnPack(&i, 1);
    if (i == 0) sol = 0;
    return;
    bs.UnPack(&nbsol, 1);
    sol = new TheNode();
    sol->UnPack(bs);
  }

  /// Prt Method display the Solution data structure
  virtual ostream &Prt(ostream &os) const {
    if (sol == 0)
      os << "Solution------------- NULL#:" << nbsol << "\n";
    else
      os << "Solution-------------" << sol->getCost() << "#:" << nbsol << "\n";
    return os;
  }
};

/**
 * @}
 */

/**
 * The Divide and Conquer Algorithm
 */
template < class Trait >
class CDCAlgo: public SchedAlgo<Trait> {
public:
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of the algorithm.
  typedef typename Trait::Instance TheInstance;
  /// Type of the priotiy comparator
  typedef typename Trait::PriComp ThePriComp;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
  /// type of the algorithm.
  typedef typename Trait::GenChild TheGenChild;
  /// type of the goal use for the algorithm.
  typedef typename Trait::Goal TheGoal;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// Type of the pq
  typedef PQInterface <TheNode, ThePriComp,TheGoal> ThePQ;

  /** Constructor
     * @param i the instance to solve
     * @param pq the Priority queue
     * @param tg the goal of the search
     * @param st the statistics object
     */
  CDCAlgo(const TheInstance *i, ThePQ *pq, TheGoal *tg, TheStat *st): SchedAlgo <Trait> (i, pq, tg, st) {
    DBGAFF_MEM("CDCAlgo::CDCAlgo", "Constructor (Instance *,PQ *,tg *,Stat *)");
  }
  /// Destructor
  virtual ~CDCAlgo() {
    DBGAFF_MEM("CDCAlgo::~CDCAlgo", "Destructor");
  }
  /**
     @name additional Divide and Conquer Statistics
  */
  //@{
  /// statisitics for evaluated nodes
  void StCost(base_Node *n,base_Node* p) {
    (SchedAlgo<Trait>::getStat())->Cost(n,p);
  }
  //@}

};

/** @defgroup CCDCAlgoSHGRP The shortcuts definition for Divide and Conquer Algorithm
  * @brief This modules presents shortcuts definition of the Instance/Algo/Node to
  * implement a Divide and Conquer
  */
//@{
/// @name Minimization and integer cost
// @ingroup CDCAlgoSHGRP
//@{
typedef CDCNode < int, Min < int > >CDCIntMinNode;
//@}
/// @name Maximisation and integer cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < int, Max < int > >CDCIntMaxNode;
//@}
/// @name Minimization and long cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < long, Min < long > >CDCLongMinNode;
//@}
/// @name Maximisation and long cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < long, Max < long > >CDCLongMaxNode;
//@}
/// @name Minimization and double cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < double, Min < double > >CDCDoubleMinNode;
//@}
/// @name Maximisation and double cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < double, Max < double > >CDCDoubleMaxNode;
//@}
/// @name Minimization and float cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < float, Min < float > >CDCFloatMinNode;
//@}
/// @name Maximisation and float cost
// @ingroup CDCAlgoSHGRP
//@{
/// Node
typedef CDCNode < float, Max < float > >CDCFloatMaxNode;
//@}

//@}
//@}

};
#endif
