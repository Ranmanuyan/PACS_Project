/*
 * bobpp/bob_sdcalgo.h
 *
 * This source include the data type for the simple Divide and Conquer Algo level.
 */

#ifndef BOBSDCALGO_ABSTRACT
#define BOBSDCALGO_ABSTRACT

namespace Bob {

/** \defgroup SDCAlgoGRP The simple Divide and Conquer Algorithm
  * \ingroup DCAlgoGRP
  *  This module describe all classes to implement Divide
  *  and Conquer algorithms, when the goal is  to find one solution to the problem
  *  The best example of application for this algorithm is a N-Queen resolution.
  *  Have a look to the sources in the examples/TestDC directory
  *  @{
  */

/** The simple Divide and Conquer node
  * a Node is not costed we just want to find a solution.
  */
class SDCNode: public SchedNode, public PQNode<SDCNode> {
public:
  /// type of the enclosed PQNode.
  typedef PQNode<SDCNode> ThePQNode;
  /// Constructor
  SDCNode(bool sol=false): SchedNode(sol), ThePQNode() {
  }
  /** Constructor
    * @param bn a SDCNode to copy
    */
  SDCNode(SDCNode * bn): SchedNode(*bn), ThePQNode() {
  }
  /** Constructor
    * @param bn a SDCNode to copy
    */
  SDCNode(const SDCNode & bn): SchedNode(bn), ThePQNode() {
  }
  /** Constructor
    * @param bn a SDCNode to copy
    */
  SDCNode(SDCNode & bn): SchedNode(bn), ThePQNode() {
  }
  /** Destructor
    */
  virtual ~SDCNode() {}
  /** copy method
  */
  virtual void copy(const SDCNode &tn) {
    SchedNode::copy(tn);
  }
  /// Operator to compare two SDCNode.
  bool operator==(const SDCNode &t1) const {
    return true;
  }
  /// Operator to compare two SDCNode.
  bool operator>(const SDCNode &t1) const {
    return true;
  }
  /// Operator to compare two SDCNode.
  bool operator<=(const SDCNode &t1) const {
    return true;
  }
  /// Pack method to serialize the SDCNode
  virtual void Pack(Serialize &bs)  const {
    DBGAFF_PCK("SDCNode::Pack", "Packing Node");
    SchedNode::Pack(bs);
  }
  /// Unpack method to deserialize the SDCNode
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SDCNode::UnPack", "Unpacking Node");
    SchedNode::UnPack(bs);
  }
};

/** The Divide and Conquer Instance Algorithm
  */
template < class Trait >
class SDCInstance: public SchedInstance < Trait > {
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
  SDCInstance(): SchedInstance < Trait > ("??") {
    DBGAFF_MEM("SDCInstance::SDCInstance", "Constructor");
  }
  /** Constructor
    * @param _n a std::string for the identifier
    */
  SDCInstance(std::string &_n): SchedInstance < Trait > (_n) {
    DBGAFF_MEM("SDCInstance::SDCInstance", "Constructor (para string)");
  }
  /** Constructor
    * @param _n a char * for the identifier
    */
  SDCInstance(const char *_n): SchedInstance < Trait > (_n) {
    DBGAFF_MEM("SDCInstance::SDCInstance", "Constructor (para char *)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  SDCInstance(TheInstance &ia) : SchedInstance<Trait>(ia) {
    DBGAFF_MEM("SDCInstance::SDCInstance", "Constructor (para Instance &)");
  }
  /** Constructor
    * @param ia a SchedInstance to copy
    */
  SDCInstance(const TheInstance &ia) : SchedInstance<Trait>(ia) {
    DBGAFF_MEM("SDCInstance::SDCInstance", "Constructor (para const Instance &)");
  }
  /// Destructor
  virtual ~SDCInstance() {}
  /** Initialize the Instance.
    */
  virtual void Init() = 0;

  /** Pack method to serialize the SchedInstance
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const  {
    DBGAFF_PCK("SDCIntance::Pack", "Packing Instance");
    SchedInstance<Trait>::Pack(bs);
  }
  /** Unpack method to deserialize the SchedInstance
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SDCInstance::UnPack", "Unpacking Instance");
    SchedInstance<Trait>::UnPack(bs);
  }
};

/**
  * The Statistics for Simple Divide and Conquer algorithm
  */
class SDCStat: public SchedStat {
public:
  /// Constructor
  SDCStat(const Id &s):
      SchedStat('S', s) {
    add_counter('x', "Explored Nodes");

  }
  /// Counts the number of explorated nodes
  void expl() {
    get_counter('x')++;
  }
};

/** The Divide and Conquer GenChild
  */
template < class Trait >
class SDCGenChild: public SchedGenChild < Trait > {
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
  SDCGenChild(const TheInstance * _inst, TheAlgo * _al):
      SchedGenChild < Trait > (_inst, _al) {
    DBGAFF_MEM("SDCGenChild::SDCGenChild", "Constructor (Instance *,Algo *)");
  }
  /// Destructor
  virtual ~SDCGenChild() {
    DBGAFF_MEM("SDCGenChild::~SchedGenChild", "Destructor");
  }

};

/** The Goal of the search Group
 * A Bob++ Algo coud be instancied with one goal or another.
 * @{
 */

/** Search one Solution
 */
template<class Trait>
class SDCGoalOne : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Stat
  typedef typename Trait::Stat TheStat;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
protected:
  /// The object that stores the solutions (this structure could stores several solutions).
  TheNode *sol;
public:
  /// Constructor
  SDCGoalOne(bool l = true) : SchedGoal<Trait>(l), sol(0) {   }
  /// Constructor
  SDCGoalOne(const Id &id,bool l = true) : SchedGoal<Trait>(id,l), sol(0) {   }
  /// Constructor
  SDCGoalOne(SDCGoalOne<Trait> &sg) : SchedGoal<Trait>(sg), sol(0) {
    if (sg.sol != 0) {
      sol = sg.sol;
      sol->addRef();
    }
  }
  /// Constructor
  SDCGoalOne(const SDCGoalOne<Trait> &sg) : SchedGoal<Trait>(sg), sol(0) {
    if (sg.sol != 0) {
      sol = sg.sol;
      sol->addRef();
    }
  }
  /// Destructor
  virtual ~SDCGoalOne() {
    if (sol != 0 && sol->remRef() == 0) {
      delete sol;
      sol = 0;
    }
  }
  /// Intialiaze the Goal.
  virtual void Init(TheInstance *ti) { }

  /** Method called when a node is a solution
    * This method is called directly by an algorithm to try to update the base solution. 
    * But it is also used by the merge operations (current_merge and final_merge). 
    * In these last cases the stat is not called.
    *
    * @param n the solution node
    * @param u if equal to 0, (default value) the stat is updated.
    *
    */
  virtual bool update(TheNode *n,int u=0) {
    if (n == 0) return false;
    BOBASSERT(!n->isSol(), Exception, "Goal", "update called with a non solution node");
    DBGAFF_ALGO("SDCGoalOne::is4Search()", "Node is a solution");
    if (sol == 0 && n != 0) {
      DBGAFF_ALGO("SDCGoalOne::is4Search()", "A Better Solution is found");
      n->addRef();
      sol = n;
      if ( u==0 ) SchedGoal<Trait>::st.upd(n);
      return true;
    }
    DBGAFF_ALGO("SDCGoalOne::is4Search()", "Another solution is found");
    return false;
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) const {
    if (sol != 0) return false;
    return true;
  }
  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol,1);
  }
  /** the merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal * _tg) {
    if (_tg->sol == 0) return false;
    return TheGoal::update(_tg->sol,1);
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
    DBGAFF_PCK("SDCGoalBest::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
    if (sol == 0) {
      std::cout << " Pack no sol\n";
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
    DBGAFF_PCK("SDCGoalBest::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
    bs.UnPack(&i, 1);
    if (i == 0) {
      sol = 0;
      std::cout << " UnPack no sol\n";
      return;
    }
    sol = new TheNode();
    sol->UnPack(bs);
  }

};

/** Count the number of Solution
 */
template<class Trait>
class SDCGoalCount : public SchedGoal<Trait> {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Stat
  typedef typename Trait::Stat TheStat;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;

protected:
  /// The object that stores the solutions (this structure could stores several solutions).
  int nbsol;
public:
  /// Constructor
  SDCGoalCount(bool l = true) : SchedGoal<Trait>(l), nbsol(0) {   }
  /// Constructor
  SDCGoalCount(const Id &id,bool l = true) : SchedGoal<Trait>(id,l), nbsol(0) {   }
  /// Constructor
  SDCGoalCount(SDCGoalCount<Trait> &sg) : SchedGoal<Trait>(sg), nbsol(sg.nbsol) {   }
  /// Constructor
  SDCGoalCount(const SDCGoalCount<Trait> &sg) : SchedGoal<Trait>(sg), nbsol(sg.nbsol) {   }
  /// Destructor
  virtual ~SDCGoalCount() { }
  /// Intialiaze the Goal.
  virtual void Init(TheInstance *ti) { }

  /** Method called when a node is a solution
    * This method is called directly by an algorithm to try to update the base solution. 
    * But it is also used by the merge operations (current_merge and final_merge). 
    * In these last cases the stat is not called.
    *
    * @param n the solution node
    * @param u if equal to 0, (default value) the stat is updated.
    */
  virtual bool update(TheNode *n,int u=0) {
    if (n != 0) {
      BOBASSERT(!n->isSol(), Exception, "Goal", "update called with a non solution node");
      DBGAFF_ALGO("SDCGoalCount::is4Search()", "Node is a solution");
      if ( u==0 ) SchedGoal<Trait>::st.upd(n);
      nbsol += n->getSol();
    }
    return false;
  }

  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode * n) const {
    return true;
  }

  /** The merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *_tg) {
    return false;
  }

  /** The merging method is used to update the goal with another one.
    * this method is used in distributed environment.
    * @param _tg TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *_tg) {
    nbsol += _tg->nbsol;
    return true;
  }

  /** Return the best known solution */
  virtual const TheNode * best() {
    return 0;
  }

  /** Return the worst known solution */
  virtual const TheNode * worst() {
    return 0;
  }

  /// Prt Method display the Solution data structure
  virtual ostream &Prt(ostream &os) const {
    os << "Solution-------------:\n" << nbsol << "\n";
    return os;
  }
  /** Pack method to Serialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("SDCGoalCount::Pack", "Packing Node");
    SchedGoal<Trait>::Pack(bs);
    bs.Pack(&nbsol, 1);
  }
  /** Unpack method to deserialize the Goal
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SDCGoalCount::UnPack", "Unpacking Node");
    SchedGoal<Trait>::UnPack(bs);
    bs.UnPack(&nbsol, 1);
  }
};

/**
 * @}
 */

/**
 * The simple Divide and Conquer Algorithm
 */
template <class Trait>
class SDCAlgo: public SchedAlgo <Trait> {
public:
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of the algorithm.
  typedef typename Trait::Instance TheInstance;
  /// Type of the priority
  typedef typename Trait::PriComp ThePriComp;
  /// Type of the Algo
  typedef typename Trait::Stat TheStat;
  /// type of the algorithm.
  typedef typename Trait::GenChild TheGenChild;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// Type of the Priority Queue
  typedef PQInterface<TheNode, ThePriComp,TheGoal> ThePQ;
  /** Constructor
     * The Genchild used by this algorithm is internaly allocated.
     * @param _i the instance to solve
     * @param _pq the Priority queue
     * @param _tg the Goal of the Search
     * @param _st the statistics object
     */

  SDCAlgo(const TheInstance * _i, ThePQ *_pq, TheGoal *_tg, TheStat *_st):
      SchedAlgo <Trait> (_i, _pq, _tg, _st) {
    DBGAFF_MEM("SDCAlgo::SDCAlgo", "Constructor (Instance *,PQ *,Sol *,Stat *)");
  }
  /// Destructor
  virtual ~SDCAlgo() {
    DBGAFF_MEM("SDCAlgo::~SDCAlgo", "Destructor");
  }
};

/** @}
 */

};
#endif
