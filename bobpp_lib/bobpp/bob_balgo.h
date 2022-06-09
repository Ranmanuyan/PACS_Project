/*
 * bobpp/bobpp/bob_balgo.h
 *
 * This source include the abstract data type for the Algo level and the
 * Associated BobNode.
 */

#ifndef BOBBALGO
#define BOBBALGO

#include <string>
#include <time.h>

namespace Bob {

/** @defgroup BaseAlgoGRP Basic Algorithm module
 *  @brief This module describes the basic level Bob algorithm.
 *  @ingroup UserAlgoGRP
 *
 *	As said in @ref UserAlgoGRP, a Bob++ algorithm includes the Node, GenChild,
 *	Instance, and Algo classes. Each algorithm proposed in the Bob++ hierarchy
 *	includes one of this.
 *
 *	The basic algorithm presents here is the lowest level of Bob++ Algorithms.
 *	The classes here are not templatized.
 *
 *	Two additional classes are also used in a Bob++ Algorithm :
 *	-# The Stat class that stores all the statistics collected during the execution
 *	   of an algorithm (@see base_Stat TimeStat).
 *	-# The Problem class which is not really used at this time. but which is present
 *	for future features that will be implemented in the Bob++ Library.
 *@{
 */

/**
 * The base_Node represents a node of the search space.
 * The base_Node proposes different features
 *    - the reference counter stores the number of times that a node is in a Data Structure,
 *      the goal is toautomaticaly know if the node could be deleted or not.
 *    - the level of the node, mainly the distance between the node and the root node.
 *    - a node identifer (a unsigned long long int).
 */
class base_Node  {
protected:
/// Number of references.
  int _ref;
/// level of the node since the root node.
  int d;
/// the identifier of the node
  node_id nid;

public:
  /** Constructor.
    */
  base_Node() : _ref(0), d(0), nid(0) {
    DBGAFF_MEM("base_Node::base_Node", "Constructor ()");
  }
  /** Constructor.
    * @param _bn the node to copy
    */
  base_Node(base_Node const &_bn): _ref(0), d(_bn.dist()), nid(_bn.nid) {
    DBGAFF_MEM("base_Node::base_Node", "Constructor (base_Node const)");
  }
  /** Constructor.
    * @param _bn the node to copy
    */
  base_Node(base_Node const *_bn): _ref(0), d(_bn->dist()), nid(_bn->nid) {
    DBGAFF_MEM("base_Node::base_Node", "Constructor (base_Node *)");
  }
  /// Destructor
  virtual ~base_Node() {
    DBGAFF_MEM("base_Node::~base_Node", "Destructor");
  }
  /** copy method used by the operator=
    * @param bn the source node.
    */
  virtual void copy(const base_Node &bn) {
    _ref = 0;
    d = bn.d;
    nid = bn.nid;
  }

  /** Atomically increments the reference count on a node
    * @return the new reference count
    */
  int addRef() {
    /* return ++_ref; */
    return __sync_add_and_fetch(&_ref, 1);
  }
  /** Atomically decrements the reference count on a node
    * @return the new reference count
    */
  int remRef() {    
    /* return --_ref; */
    return __sync_sub_and_fetch(&_ref, 1);
  }
  /** Checks whether the Node may be deleted or not
    * This method uses the reference counter of the node. If this counter 
    * is greater than 0, the node could not be deleted, it can be deleted otherwise.
    * @return true of the node can be deleted
    */
  bool isDel() {
    return _ref == 0;
  }
  /** The distance of the node from the root.
    * The distance means the number of levels in the tree or the number
    * of nodes from the root to the the node for a graph.
    * @return the number of levels
    */
  int dist() const {
    return d;
  }
  /** The number of References on the Node i.e.
    * the number of different data structures that stores the node.
    * @return the number of references on the node.
    */
  int ref() {
    return _ref;
  }
  /** The distance of the node from the root.
    * The distance means the number of levels in the tree or the number
    * of nodes from the root to the the node for a graph.
    * @return a reference on the number of levels
    */
  int &dist() {
    return d;
  }
  /** Print the node information
    * @param os the ostream to use to display the node
    * @return the modified ostream
    */
  virtual std::ostream &Prt(std::ostream &os) const {
    os << "Node Ref:" << _ref << " Dist:" << d ;
    if ( core::space_search() ) { os << " Id: "<<nid; }
    return os;
  }
  /** Get the identifier of the node
    * @return the identifier of the node in a long
    * This method is used by the stat and the log to identify a node.
    */
  node_id &id() {
    return nid;
  }
  /** Fill the strbuff with node information
    * @param st the strbuff to fill.
    */
  virtual void log_space(strbuff<> &st) {
    if ( core::space_search() ) { st << nid<<","<<d; }
  }
  /** Fill the strbuff with node information label
    * @param st the strbuff to fill.
    */
  virtual void lab_log_space(strbuff<> &st) {
    if ( core::space_search() ) { st << "Id,level"; }
  }

  /** Pack method to serialize the Node
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs)  const {
    bs.Pack(&_ref, 1);
    bs.Pack(&d, 1);
    bs.Pack(&nid, 1);
  }
  /** Unpack method to deserialize the Node
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    bs.UnPack(&_ref, 1);
    bs.UnPack(&d, 1);
    bs.UnPack(&nid, 1);
  }
};

/**
 * The base_Instance class
 */
class base_Instance {
protected:
  /// The Identifier of the Instance
  SId in;
public:
  /** Constructor
    * @param _n a std::string for the identifier
    */
  base_Instance(std::string &_n): in(_n) {
    DBGAFF_MEM("base_Instance::base_Instance", "Constructor (para string)");
  }
  /** Constructor
    * @param _n a char * for the identifier
    */
  base_Instance(const char *_n): in(_n) {
    DBGAFF_MEM("base_Instance::base_Instance", "Constructor (para char *)");
  }
  /** Constructor
    * @param _ia a base_Instance to copy
    */
  base_Instance(base_Instance &_ia): in(_ia.id()->str()) {
    DBGAFF_MEM("base_Instance::base_Instance", "Constructor (base_instance &)");
  }
  /** Constructor
    * @param _ia a base_Instance to copy
    */
  base_Instance(const base_Instance &_ia): in(_ia.id()->str()) {
    DBGAFF_MEM("base_Instance::base_Instance", "Constructor (base_instance const )");
  }
  /// Destructor
  virtual ~base_Instance() {
    DBGAFF_MEM("base_instance::~base_instance", "Destructor");
  }
  /** copy method used by the operator=
    */
  virtual void copy(const base_Instance &bi) {
    in = bi.in;
  }
  /** Gets the id of te instance.
    * @return a const pointer on the id of the Instance
    */
  const Id *id() const {
    return &in;
  }
  /** Gets the id of te instance.
    * @return a pointer on the id of the Instance
    */
  Id *id() {
    return &in;
  }
  /** Print the instance information
    * @param os the ostream to use to display the node
    * @return the modified ostream
    */
  virtual std::ostream &Prt(std::ostream &os) {
    os << "base_Instance" << in.str() << "\n";
    return os;
  }
  /** Pack method to serialize the SchedInstance
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs)  const {
    DBGAFF_PCK("base_Instance::Pack", "Packing Instance");
    in.Pack(bs);
  }
  /** Unpack method to deserialize the SchedInstance
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("base_Instance::UnPack", "Unpacking Instance");
    in.UnPack(bs);
  }
private:
};

/**
 * The StatAlgo class is the basic class that stores the statitics
 * associated with the algorithm at this stage of the definition of
 * the hierarchy of class the statistics only stores the time
 * to execute the algorithm.
 */
class base_Stat : public stat {
public:
  /** Constructor
     * @param n the name of the algorithm
     * @param i the Id of the associated algorithm
     */
  base_Stat(char n, const Id &i) : stat(n, i) {
    add_timer('T', "Algorithm Time");
    add_counter('c', "created node");
  }
  /// Destructor
  virtual ~base_Stat() { }
  /// Starts the time execution
  void start() {
    get_timer('T').start();
  }
  /// Ends the time execution
  void end() {
    get_timer('T').end();
  }
  /** method to log a node creation.
    * @param bn the created node
    * @param pn the parent node of bn
    */
  void create(base_Node *bn,base_Node *pn) {
    stat_counter &stc = get_counter('c');
    if ( core::space_search() ) {
      strbuff<> st;
      bn->id() = core::get_new_id();
      st <<bn->id()<<":"<<pn->id();
      stc.add(st.str(),1);
    } else {
      stc.add(1);
    }
  }
};


/** The base_GenChild class Generation of the child nodes
 */
class base_GenChild {
public:
  /// Constructor
  base_GenChild() { }
};


/**
 * The base_Algo class is the basic class that represents an algorithm
 * This class could not be executed.
 * Reals algorithms inherit of this class.
 * In a parallel environment, an instance of this class is executed on each
 * processor. Data are distributed or shared by the Global Data Structures.
 * The present fonctionnalities are:
 * - managing methods for the Instance
 
 *
 * \todo Assertion on the Instance pointer and on the Log pointer pack and unpack of Stat.
 */
class base_Algo {
protected:
/// the associated Instance.
  base_Stat *sa;
/// enumeration of the state of a thread.
  enum EtState {EtAlloc, /*!< The algo has been allocated */
                EtInit, /*!< The algo has been initialized */
                EtResumed, /*!< The algo has been resumed */
                EtRun, /*!< The algo is running */
                EtToStop, /*!< The algo will stop */
                EtStop, /*!< The algo has been stopped */
                EtEnd /*!< The algo has terminate */
               };
/// the state of the algorithm (not really supported yet).
  EtState State;

public:
  /** Constructor
    * @param _sa the statistics object
    */
  base_Algo(base_Stat *_sa = 0) {
    sa = _sa;
    State = EtAlloc;
  }
  /// Destructor
  virtual ~base_Algo() {  }
  /// Gets the Instance
  base_Stat *getBStat() const {
    return sa;
  }
  /// Gets the Instance
  void setStat(base_Stat *_sa) {
    sa = _sa;
  }
  /** @name Running methods
  */
  //@{
  /** Runs the  algorithm.
    */
  virtual void operator()()=0;
  /// Stop the algorithm (meaning that the algorithm could be resumed after)
  virtual void Stop() {
    State = EtToStop;
  }
  /// Resumes the algorithm (meaning that the algorithm has been stopped before)
  virtual void Resume() {
    DBGAFF_ALGO("base_Algo::Resume()", "base_Algo::Resume do nothing");
  }
  /// Ends the algorithm
  virtual void End() {
    DBGAFF_ALGO("base_Algo::End()", "base_Algo::End do nothing");
  }
  //@}
  /// Pack method
  virtual void Pack() { /*sa->Pack();*/ }
  /// UnPack method
  virtual void UnPack() { /*sa->UnPack();*/ }
  /** @name State methods (not really supported yet)
  */
  //@{
  virtual void toInit() {
    State = EtInit;
  }
  virtual bool isEtInit() {
    return State == EtInit;
  }
  virtual void toRun() {
    State = EtRun;
  }
  virtual bool isEtRun() {
    return State == EtRun;
  }
  virtual void toResumed() {
    State = EtResumed;
  }
  virtual bool isEtResumed() {
    return State == EtResumed;
  }
  virtual void totoStop() {
    State = EtToStop;
  }
  virtual bool isEtToStop() {
    return State == EtToStop;
  }
  virtual void toStop() {
    State = EtStop;
  }
  virtual bool isEtStop() {
    return State == EtStop;
  }
  virtual void toEnd() {
    State = EtEnd;
  }
  virtual bool isEtEnd() {
    return State == EtEnd;
  }
  //@}
  /** @name Statistics methods
  */
  //@{
  /// Statistic Start method the Algo
  virtual void StStart() {
    sa->start();
  }
  /// Statistic End method of the Algo
  virtual void StEnd() {
    sa->end();
  }
  //@}

  /// the Prt Method
  virtual std::ostream &Prt(std::ostream &os) const {
    os << "Base Algo :";
    if (sa != 0) {
      os << "Stats:";
      os << *sa;
    } else os << "No Stats, humm strange !" << std::endl;
    return os;
  }
};


/** Operator << for a refence on a BobNode
 * @ingroup  BaseAlgoGRP
 */
inline std::ostream &operator<<(std::ostream &o, const base_Node &n) {
  return n.Prt(o);
}

/** Operator << for a pointer on a Node
 * @ingroup  BaseAlgoGRP
 */
inline std::ostream &operator<<(std::ostream &o, const base_Node *n) {
  return n->Prt(o);
}

/** Operator << for a refence on a Algo
 * @ingroup  BaseAlgoGRP
 */
inline std::ostream &operator<<(std::ostream &o, const base_Algo &n) {
  return n.Prt(o);
}

/** Operator << for a pointer on a Algo
 * @ingroup  BaseAlgoGRP
 */
inline std::ostream &operator<<(std::ostream &o, const base_Algo *n) {
  return n->Prt(o);
}



/** @}
 */

};
#endif


