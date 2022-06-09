/*
 * ++/Algo/BBAlgo.h
 *
 * This source include the data type for the Branch and Bound Algo level.
 * This the unit of task scheduled by the Kernel class.
 */

#ifndef BOBBBALGO_ABSTRACT
#define BOBBBALGO_ABSTRACT

namespace  Bob {
/*! \file bob_bbalgo.h
    \brief the file where the Branch and Bound basic classes are defined
*/ 


/** @defgroup BBAlgoGRP The Branch and Bound Algorithm.
  *  @brief This module describe all classes to implement a Branch and Bound  algorithm.
  *  @ingroup SchedAlgoGRP
  *  @{

Each classes use to implements Branch and Bound inherits from their
associated classes of the module @ref SchedAlgoGRP.


For example, we could solve My Problem  using a Branch and Bound,
We give step by step the specific classes and their associated methods and attributs.

  -# Define the MyTrait class with the forward definition of the classes.
     \verbatim
#include<bobpp/bobpp>

class MyProblem;
class MyInstance;
class MyGenChild;
class MyNode;


class MyTrait {
  public:
    typedef MyNode Node;
    typedef MyInstance Instance;
    typedef MyGenChild GenChild;
    typedef Bob::BBAlgo<MyTrait> Algo;
    typedef Bob::BBGoalBest<MyTrait> Goal;
    typedef Bob::BestEPri<MyNode> PriComp;
    typedef Bob::BBStat Stat;
};
     \endverbatim
     We have decide to group the definition of the types of the Node/Instance/GenChild/Algo/Goal/Stat classes used by an 
     algorithm in only one class called the trait class. This principle simplifies many definition classes.
  -# define the MyNode class
     \verbatim
     class MyNode : public Bob::BBIntMinNode {
        // specific datas
        public:
                MyNode() : Bob::BBIntMinNode(0) {
                }
                MyNode(int n) : Bob::BBIntMinNode(n) {
                }
                virtual ~MyNode() {
                }

     };
     \endverbatim
     To fixe the evaluation of a node you could use the methods Bob::Eval::setEval(T t) and T Bob::Eval::getEval() 
     of the class Bob::Eval 
     This MyNode inherits from the BBIntMinNode, but could also inherits
     from the BBLongMinNode, etc... @ref BBAlgoSHGRP
     You could also inherit from the Bob::BBNode <T,Compare> where T is a type for a cost
     and Compare is a class to Compare these evaluations and could be Min or Max.
     The Bob::BBNode class has no pure virtual methods.
  -# defines the MyGenChild
     \verbatim
class MyGenChild: public Bob::BBGenChild<MyTrait> {
   public:
       /// Constructor of MyGenChild
       MyGenChild(MyTrait::Instance *_inst,MyTrait::Algo *_al) :
          Bob::BBGenChild<MyTrait>(_inst,_al) {
       }
       virtual ~MyGenChild() {
       }
       /// Operator called by the algo to Generate child
       void operator()(MyNode *n) {
          int nbs=inst->getnbson();
          int step=inst->getstep();
          int i;
          for (i=0;i<nbs;i++ ) {
              MyNode *ns=new MyNode();
              algo->start_eval(ns,n);
              /// construct and do the evaluation of the node here using ns->setEval(xx)
              algo->end_eval(ns);
              algo->Search(ns);
          }
       }
};

     \endverbatim
     The Bob::BBGenChild class has only one pure virtual method. You must reimplement the Bob::BBGenChild::operator()(MyNode *n).
     The goal of this method is to generate the child node of the node parameter and to call Bob::BBAlgo::Search() method. 
     If the node is a subproblem (Bob::BBNode::isSol() returns false), and the evaluation is better than the best known 
     solution the node is inserted in the priority queue to be explored later.
     When you call a Bob::BBAlgo::Search(n) method on a solution (Bob::BBNode::IsSol() returns true), the Node n is not
     inserted in the GPQ, but is used to update the Goal.
     This semantic could be a problem, for some application. For example when solving
     a Knapsac problem, one subproblem
     at the level i is a subproblem (if the bag is not complete) but is also a solution.
     In this case you must produce 2 nodes one which is a solution (n->setSol()) and one which is a subproblem.
  -# defines the MyInstance
     \verbatim
class MyInstance : public Bob::BBInstance<MyTrait> {
  // specific datas for the instance
  public:
   /// Constructor
   MyInstance() : Bob::BBInstance<MyTrait>() {
   }
   void InitAlgo(MyTrait::Algo *a) const {
       std::cout<<"MyInstance::Init(Algo)"<<std::endl;
       MyNode *nd= new MyNode(lb);
       /// Initialize de data for the root node
       a->GPQIns(nd);
   }
   /// Get the initial best known solution
   MyNode *getSol() {
      int ub = compute_the_cost_of_a_feasible_solution()
      return new MyNode(ub);
   }
};

\endverbatim
      Comments
          -The Instance::InitAlgo method is used to initialize the search. Mainly is to insert the root node. 
           It is possible to insert several nodes, if you have several root nodes.
          -The Instance::getSol method is use to initialize th incubent, the best known solution.
   -# defines the main function. The main function given here is made for a sequential resolution.
\verbatim
int main(int n, char ** v) {

    // Eventually, call the Bob::core::opt().add() method to add command line option.
    Bob::core::opt().add(std::string("--bb"), Bob::Property("-opt", "An option", 0));
    // Create a sequential environment.
    Bob::SeqBBAlgoEnvProg<MyTrait> saenv;
    // call the Bob::core::Config to initialize the framework core and parse the command line options.
    Bob::core::Config(n, v);

    // Initialize the instance.
    MyInstance *Instance = new MyInstance();

    std::cout << "-------- Start to solve the entire tree" << std::endl;
    // Solve the instance i.e. all the tree
    saenv(Instance);

    // delete the created instance
    delete Instance;
    // call Bob::core::End() a static method to close/delete all the feature open/allocate 
    // by the Bob::core::Config(n, v) static method
    Bob::core::End();
}
\endverbatim
     You could have a look to the Example source code that comes with the Bob++ Library.
*/

/** The Branch and Bound node
    a Node of a Branch and Bound is 
       - scheduled then it inherits form the SchedNode.
       - evaluated then it inherits form the Eval class. the Eval class is templatized with the type
         of the evaluation and the sens of comparison (Min or Max).
       - a PQNode because it could be inserted in a priority queue.
 */
template <class T, class Compare>
class BBNode : public Eval<T, Compare> , public SchedNode,
    public PQNode<BBNode<T, Compare> > {
public :
  /// Type of the enclosed PQNode.
  typedef PQNode<BBNode<T, Compare> > ThePQNode;
  /// Constructor
  BBNode(bool sol=false) : Eval<T, Compare>(sol) , SchedNode(sol), ThePQNode() { }
  /// Constructor
  BBNode(T v) : Eval<T, Compare>(v) , SchedNode(), ThePQNode() { }
  /** Constructor
    * @param bn the BBNode used to copy
    */
  BBNode(BBNode<T, Compare> *bn) : Eval<T, Compare>(bn->getEval()),
      SchedNode(*bn), ThePQNode() { }
  /** Constructor
    * @param bn the BBNode used to copy
    */
  BBNode(BBNode<T, Compare> &bn) : Eval<T, Compare>(bn.getEval()) ,
      SchedNode(bn), ThePQNode() { }
  /** Constructor
    * @param bn the BBNode used to copy
    */
  BBNode(const BBNode<T, Compare> &bn) : Eval<T, Compare>(bn.getEval()) ,
      SchedNode(bn), ThePQNode() { }
  /// Destructor
  virtual ~BBNode() {}
  /** copy method
    */
  virtual void copy(const BBNode<T, Compare> &tn) {
    SchedNode::copy(tn);
    Eval<T, Compare>::copy(tn);
  }
  /** print the node information
    * @param os the ostream to display the node information
    */
  virtual std::ostream &Prt(std::ostream &os) const {
    SchedNode::Prt(os);
    os << "Eval:" << Eval<T, Compare>::getEval() << std::endl;
    return os;
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void log_space(strbuff<> &st) {
    SchedNode::log_space(st);
    st << "," << Eval<T, Compare>::getEval();
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void lab_log_space(strbuff<> &st) {
    SchedNode::log_space(st);
    st << "," << "evaluation";
  }

  /** Pack method to serialize the Node
    * @param bs the Serialize object to serialize the BBNode.
    */
  virtual void Pack(Serialize &bs)  const {
    SchedNode::Pack(bs);
    Eval<T, Compare>::Pack(bs);
  }
  /** Unpack method to deserialize the Node
    * @param bs the DeSerialize object to deserialize the BBNode.
    */
  virtual void UnPack(DeSerialize &bs)  {
    SchedNode::UnPack(bs);
    Eval<T, Compare>::UnPack(bs);
  }

};

/** The Branch and Bound Instance Algorithm
 * The BBInstance class inherits from the SchedInstance, it only defines an abstract method,
 * the getSol() method that must returns the initial solution, the incubent.
 */
template <class Trait>
class BBInstance : public SchedInstance<Trait>  {
public :
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// type of the algorithm.
  typedef typename Trait::Instance TheInstance;
  /// type of evaluation for the node.
  typedef typename TheNode::EvalType EvalType;
  /** Constructor
    * @param _n an std::string that stores the name of the instance
    */
  BBInstance(std::string &_n) : SchedInstance<Trait>(_n) {
  }
  /** Constructor
    * @param _n a char * that stores the name of the instance
    */
  BBInstance(const char *_n) : SchedInstance<Trait>(_n) {
  }
  /** Constructor
    * with no parameter the name is "??"
    */
  BBInstance() : SchedInstance<Trait>(std::string("??")) {
  }
  /** Constructor with copy of a BBInstance
    * @param bbi the BBInstance to copy
    */
  BBInstance(const BBInstance &bbi) : SchedInstance<Trait>(bbi) {
  }
  /// Destructor
  virtual ~BBInstance() {}
  /// Get the solution
  virtual TheNode *getSol() = 0;
  /** Initialize the Instance.
    */
  virtual void Init() = 0;
  /// Pack method to serialize the Node
  virtual void Pack(Serialize &bs) const {
    SchedInstance<Trait>::Pack(bs);
  }
  /// Unpack method to deserialize the Node
  virtual void UnPack(DeSerialize &bs)  {
    SchedInstance<Trait>::UnPack(bs);
  }
};


/**
 * The Statistics for Branch and Bound algorithm
 * The Branch and Bound statistics adds a timer to measure the evaluation time of the nodes.
 */
class BBStat : public SchedStat {
public:
  /// Constructor
  BBStat(const Id &s) : SchedStat('B', s) {
    add_timer('E', "time for node Evaluation");
  }
  /// Destructor
  virtual ~BBStat() {}
  /** start the time for evaluated node
    * @param bn the child node.
    * @param pn the parent node
    * This methods also write in the log the relation of a child node 
    * and its parent node to be able to display the tree.
    */
  void start_eval(base_Node *bn,base_Node *pn) {
    create(bn,pn);
    get_timer('E').start();
  }
  /** start the time for evaluated node
    */
  void start_eval() {
    get_timer('E').start();
  }
  /** start the time for evaluated node
    * @param bn the child node
    * the end_eval method takes the child node to log the end of its evaluation.
    */
  void end_eval(base_Node *bn) {
    get_timer('E').end(bn->id());
  }
  /** start the time for evaluated node
    */
  void end_eval() {
    get_timer('E').end();
  }
};

/** The Branch and Bound GenChild
  * This class is just for naming convention coherency since the BobBBGenChild class is exactly the same 
as the Bob::SchedGenChild.
 */
template <class Trait>
class BBGenChild : public SchedGenChild<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Constructor
  BBGenChild(const TheInstance *_inst, TheAlgo*_al) : SchedGenChild<Trait>(_inst, _al) { }
  /// Destructor
  virtual ~BBGenChild() {}
};

/** @defgroup BBGoal The Different Branch and Bound Goals
 * A Bob++ Algo coud be instancied with one goal or another.
 * The goal must be defined in the Trait class
 * At this time we have implement 3 different Goals.
 *   - Bob::BBGoalBest the classical Branch and Bound goal, where we want the best solution.
 *   - Bob::BBGoalCount the goal where where we want to count the number of best solutions.
 *   - Bob::BBGoalAll this goal is for debug only, you can use it when you want to search all the solution of a problem.
 * @{
 */

/** The Classical Branch and Bound Goal.
 * This goal is the classical Branch and Bound goal where the aim of the
 * search is to get the best solution.
 */
template<class Trait>
class BBGoalBest : public SchedGoal<Trait> {
public :
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of a Node
  typedef typename TheNode::EvalType  TheType;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
protected:
  /// the solution
  TheNode *sol;
public:
  /** Constructor
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalBest(bool l=false): SchedGoal<Trait>(l), sol(0) { }
  /** Constructor
    * @param id the algorithm identifier
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalBest(const Id &id,bool l=false): SchedGoal<Trait>(id,l), sol(0) { }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalBest(BBGoalBest<Trait> &bb): SchedGoal<Trait>(bb), sol(0) {
    //std::cout << "BBGoalBest Copy &bb\n";
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalBest(const BBGoalBest<Trait> &bb): SchedGoal<Trait>(bb), sol(0) {
    //std::cout << "BBGoalBest Copy const &bb\n";
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /// Destructor
  virtual ~BBGoalBest() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
  }
	/** get the value of the best solution
    * @return the cost of the incubent.
    */
  TheType getBest() {
    if (sol == 0) return 0;
    return sol->getEval();
  }
  /** Intialize the Goal with the given instance.
    * @param ti the instance.
    * Mainly this method call the BBInstance::getSol() method to get the incubent.
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    //std::cout << "Node Sol:"<<*sol<<std::endl;
    sol->addRef();
  }
  /** Method called when a node is a solution
    * @param n the solution node
    * @param u if u!=0 the solution is not displayed
    */
  virtual bool update(TheNode *n, int u = 0) {
    if (n == 0) return false;
    DBGAFF_ALGO("BBGoalBest::updsol()", "Node is a solution");
    SchedGoal<Trait>::stat_sol();
    if (sol == 0 || (*n) > (*sol)) {
      DBGAFF_ALGO("BBGoalBest::updsol()", "A Better Solution is found");
      //~ if (sol != 0 && sol->remRef() == 0)
        //~ delete sol;
      if (sol != 0 && sol->remRef() == 0) {
        delete sol;
	}
      if ( u==0 ) SchedGoal<Trait>::stat_upd(n);
      sol = n;
      sol->addRef();
      if (core::dispBestVal() && u == 0 && SchedGoal<Trait>::verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      return true;
    }
    return false;
  }

  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode *n) {
    if (sol == 0) {
      DBGAFF_ALGO("BBGoalBest::is4Search()", "Node is ok, no solution");
      return true;
    }
    if ((*n) > (*sol)) {
      DBGAFF_ALGO("BBGoalBest::is4Search()", "Node is a better");
      return true;
    }
    DBGAFF_ALGO("BBGoalBest::is4Search()", "Node must be pruned");
    return false;
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
  /** Pack method to Serialize the object
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("BBGoalBest::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
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
    DBGAFF_PCK("BBGoalBest::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
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


/** The Goal that counts the number of best solution.
 * This goal is quite different. The aim of the search is to count
 * the number of solution with the best cost.
 */
template<class Trait>
class BBGoalCount : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the evaluation of the node
  typedef typename TheNode::EvalType  TheType;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
protected:
  /// the solution
  TheNode *sol;
  /// the number of solution
  int nbsol;
public:
  /** Constructor
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalCount(bool l = false) : SchedGoal<Trait>(l), sol(0), nbsol(0) {   }
  /** Constructor
    * @param id the algorithm identifier
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalCount(const Id &id,bool l=false): SchedGoal<Trait>(id,l), sol(0),nbsol(0) { }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalCount(BBGoalCount<Trait> &bb) : SchedGoal<Trait>(bb), sol(0), nbsol(bb.nbsol) {
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalCount(const BBGoalCount<Trait> &bb) : SchedGoal<Trait>(bb), sol(0),
      nbsol(bb.nbsol) {
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /// Destructor
  virtual ~BBGoalCount() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
  }
  /** Intialize the Goal.
    * @param ti the Instance used to initialize the Goal
    *
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    nbsol = sol->getSol();
    sol->addRef();
  }
  /// get the value of the best solution
  TheType getBest() {
    if (sol == 0) return 0;
    return sol->getEval();
  }

  /** Method called when a node is a solution
    * @param n the solution node
    * @param u if =0  the solution is never displayed
    */
  virtual bool update(TheNode *n, int u = 0) {
    if (n == 0) return false;
    DBGAFF_ALGO("BBGoalCount::update()", "Node is a solution");
    if (sol == 0 || (*n) > (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "A Better Solution is found");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      sol = n;
      nbsol = sol->getSol();
      sol->addRef();
      if ( u==0 ) SchedGoal<Trait>::stat_upd(sol);
      if (core::dispBestVal() && u== 0 && SchedGoal<Trait>::verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      return true;
    }
    if ((*n) == (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "A as good Solution is found");
      if (core::dispBestVal() && u == 0 && SchedGoal<Trait>::verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      if ( u==0 ) SchedGoal<Trait>::stat_upd(sol);
      nbsol += sol->getSol();
    }
    return false;
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) {
    //std::cout<<"isSearch Test "<<(sol==0?"NULL":sol->getEval())<<" with "<<n->getEval()<<"\n";
    if (sol == 0 || (*n) > (*sol) || (*n) == (*sol)) {
      //DBGAFF_ALGO("BBGoalCount::is4Search()", "Node is ok");
      return true;
    }
    DBGAFF_ALGO("BBGoalCount::is4Search()", "Node is must be pruned");
    return false;
  }
  /** The final merging method is used to update the goal with another one.
    * but at the end of the execution of the algorithm
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    if ( sol==0 || (*_tg->sol) > (*sol)) {
      DBGAFF_ALGO("BBGoalCount::final merge()", "Merge the Goal with a better solution");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      sol = _tg->sol;
      nbsol = _tg->nbsol;
      sol->addRef();
      return true;
    }
    if ((*_tg->sol) == (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "Merge the goal with a same value of solution");
      SchedGoal<Trait>::stat_sol();
      nbsol += sol->getSol();
      return true;
    }

    return false;
  }

  /** The current merging method is used to update the goal with another
    * one, during the execution of the associated algorithm.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    if ( sol==0 || (*_tg->sol) > (*sol)) {
      DBGAFF_ALGO("BBGoalCount::final merge()", "Merge the Goal with a better solution");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      sol = _tg->sol;
      nbsol = 0;
      sol->addRef();
      return true;
    }
    if ((*_tg->sol) == (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "Merge the goal with a same value of solution");
      SchedGoal<Trait>::stat_sol();
      nbsol = 0;
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

  /** Pack method to Serialize the object
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("BBGoalCount::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
    bs.Pack(&nbsol, 1);
    if (sol == 0) {
      bs.Pack(&i, 1);
      return;
    }
    i = 1;
    bs.Pack(&i, 1);
    sol->Pack(bs);
  }
  /** Unpack method to deserialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    int i;
    DBGAFF_PCK("BBGoalCount::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
    bs.UnPack(&nbsol, 1);
    bs.UnPack(&i, 1);
    if (i == 0) {
       sol = 0;
       return;
    }
    sol = new TheNode();
    sol->UnPack(bs);
  }
  /// Prt Method display the Solution data structure
  virtual ostream &Prt(ostream &os) const {
    if (sol == 0)
      os << "Solution------------- NULL #:" << nbsol << "\n";
    else
      os << "Solution-------------" << sol->getEval() << "#:" << nbsol << "\n";
    return os;
  }
};

/** The Goal that stores all solutions
 * This goal yields to an exhaustive search
 */
template<class Trait>
class BBGoalAll : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the evaluation of the node
  typedef typename TheNode::EvalType  TheType;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
protected:
  /// the solution
  TheNode *sol;
  /// the number of solution
  int nbsol;
public:
  /** Constructor
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalAll(bool l = false) : SchedGoal<Trait>(l), sol(0), nbsol(0) {   }
  /** Constructor
    * @param id the algorithm identifier
    * @param l a bool if true the stat of the goal are logged.
    */
  BBGoalAll(const Id &id,bool l=false): SchedGoal<Trait>(id,l), sol(0),nbsol(0) { }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalAll(BBGoalAll<Trait> &bb) : SchedGoal<Trait>(bb), sol(0), nbsol(bb.nbsol) {
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /** Constructor
    * @param bb the source goal
    */
  BBGoalAll(const BBGoalAll<Trait> &bb) : SchedGoal<Trait>(bb), sol(0),
      nbsol(bb.nbsol) {
    if (bb.sol != 0) {
      sol = bb.sol;
      sol->addRef();
    }
  }
  /// Destructor
  virtual ~BBGoalAll() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
  }
  /** Intialize the Goal.
    * @param ti the Instance used to initialize the Goal
    */
  virtual void Init(TheInstance *ti) {
    sol = ti->getSol();
    nbsol = sol->getSol();
    sol->addRef();
  }
  /// get the value of the best solution
  TheType getBest() {
    if (sol == 0) return 0;
    return sol->getEval();
  }

  /** Method called when a node is a solution
    * @param n the solution node
    * @param u if u!=0 the solution is never displayed
    */
  virtual bool update(TheNode *n, int u = 0) {
    if (n == 0) return false;
    DBGAFF_ALGO("BBGoalCount::update()", "Node is a solution");
    if (sol == 0 || (*n) > (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "A Better Solution is found");
      if (sol != 0 && sol->remRef() == 0)
        delete sol;
      sol = n;
      nbsol = sol->getSol();
      sol->addRef();
      if ( u==0 ) SchedGoal<Trait>::stat_upd(sol);
      if (core::dispBestVal() && u==0 && SchedGoal<Trait>::verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      return true;
    }
    if ((*n) == (*sol)) {
      DBGAFF_ALGO("BBGoalCount::update()", "A as good Solution is found");
      if ( u==0 ) SchedGoal<Trait>::stat_upd(n);
      if (core::dispBestVal() && u==0 && SchedGoal<Trait>::verbosity() )
        std::cout << core::dTime() << " Solution: " << n << std::flush;
      nbsol += sol->getSol();
    }
    return false;
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) {
    //std::cout<<"isSearch Test "<<(sol==0?"NULL":sol->getEval())<<" with "<<n->getEval()<<"\n";
    DBGAFF_ALGO("BBGoalCount::is4Search()", "Node is a subproblem");
    if (sol == 0 || (*n) > (*sol) || (*n) == (*sol))
      return true;
    return true;
  }
  /** The final merging method is used to update the goal with another one.
    * but at the end of the execution of the algorithm
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol, 1);
  }

  /** The current merging method is used to update the goal with another
    * one, during the execution of the associated algorithm.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol, 1);
  }

  /** Return the best known solution */
  virtual const TheNode * best() {
    return sol;
  }

  /** Return the worst known solution */
  virtual const TheNode * worst() {
    return sol;
  }

  /** Pack method to Serialize the object
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const {
    int i = 0;
    DBGAFF_PCK("BBGoalCount::Pack", "Packing Node");
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
  /** Unpack method to deserialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    int i;
    DBGAFF_PCK("BBGoalCount::UnPack", "Unpacking Node");
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
      os << "Solution------------- NULL #:" << nbsol << "\n";
    else
      os << "Solution-------------" << sol->getEval() << "#:" << nbsol << "\n";
    return os;
  }
};

/**
 * @}
 */


/**
 * The Branch and Bound Algorithm
 */
template <class Trait>
class BBAlgo : public SchedAlgo<Trait> {
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
  /** Constructor
   * @param i the instance to solve
   * @param pq the Priority queue
   * @param tg the goal of the search
   * @param st the statistics object
   */
  BBAlgo(const TheInstance *i, ThePQ *pq, TheGoal *tg, TheStat *st) :
      SchedAlgo<Trait>(i, pq, tg, st) {
      //std::cout << "BBalgo:Goal "<<typeid(*tg).name()<<"\n";
      //std::cout << "BBalgo:PQ : "<<typeid(*pq).name()<<"\n";
  }
  /// destructor
  virtual ~BBAlgo() {
  }
  /**
   @name additional Branch and Bound Statistics
  */
  //@{
  /// start the timer for evaluated nodes
  void start_eval(base_Node *bn,base_Node *pn) {
    (SchedAlgo<Trait>::getStat())->start_eval(bn,pn);
  }
  /// end the timer for evaluated nodes
  void end_eval(base_Node *bn) {
    (SchedAlgo<Trait>::getStat())->end_eval(bn);
  }
  /// start the timer for evaluated nodes
  void start_eval() {
    (SchedAlgo<Trait>::getStat())->start_eval();
  }
  /// end the timer for evaluated nodes
  void end_eval() {
    (SchedAlgo<Trait>::getStat())->end_eval();
  }

  //@}

};

//@}

/** @defgroup BBAlgoSHGRP The shortcuts definition for Branch and Bound Algorithm
    @ingroup BBAlgoGRP
    @brief This modules presents shortcuts definition of the Instance/Algo/Node to
   implement a Branch and Bound
 */
//@{
/** \fn typedef BBNode BBIntMinNode 
  * shortcut for a Branch and Bound node with Minimization and integer evaluation
  */
typedef BBNode<int, Min<int> > BBIntMinNode;
/** \fn typedef BBNode BBIntMaxNode 
  * shortcut for a Branch and Bound node with Maximization and integer evaluation
  */
typedef BBNode<int, Max<int> > BBIntMaxNode;
/** \fn typedef BBNode BBLongMinNode 
  * shortcut for a Branch and Bound node with Minimization and Long evaluation
  */
typedef BBNode<long, Min<long> > BBLongMinNode;
/** \fn typedef BBNode BBLongMaxNode 
  * shortcut for a Branch and Bound node with Maximization and Long evaluation
  */
typedef BBNode<long, Max<long> > BBLongMaxNode;
/** \fn typedef BBNode BBDoubleMinNode 
  * shortcut for a Branch and Bound node with Minimization and double evaluation
  */
typedef BBNode<double, Min<double> > BBDoubleMinNode;
/** \fn typedef BBNode BBDoubleMaxNode 
  * shortcut for a Branch and Bound node with Maximization and double evaluation
  */
typedef BBNode<double, Max<double> > BBDoubleMaxNode;
/** \fn typedef BBNode BBFloatMinNode 
  * shortcut for a Branch and Bound node with Minimization and float evaluation
  */
typedef BBNode<float, Min<float> > BBFloatMinNode;
/** \fn typedef BBNode BBFloatMaxNode 
  * shortcut for a Branch and Bound node with Maximization and float evaluation
  */
typedef BBNode<float, Max<float> > BBFloatMaxNode;
//@}

//@}
//
};
#endif



