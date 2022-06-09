#ifndef BOB_MPXENVPROG
#define BOB_MPXENVPROG

#include <string>
#include <vector>
#include <bobpp/bobpp>
#include <bobpp/thr/thr.h>
#include <math.h>

#include <mpi.h>
#define BUFFER_SIZE 0x40000  /* 256 Ko */
#define MAX_GRANULARITY 16

//#define DEBUG_STRAT
//#define DEBUG_COMM
//#define DEBUG_ALGO
//#define DEBUG_MEMORY
//#define DEBUG_ERROR
//#define DEBUG_ERR

#ifdef DEBUG_STRAT
#define DBG_STRAT(msg) std::cout << EnvProg::rank() << ": " << msg << std::endl
#else
#define DBG_STRAT(msg)
#endif

#ifdef DEBUG_COMM
#define DBG_COMM(msg) std::cout << EnvProg::rank() << " " << msg << std::endl
#else
#define DBG_COMM(msg)
#endif

#ifdef DEBUG_ALGO
#define DBG_ALG(msg) std::cout << EnvProg::rank() << " " << msg << std::endl
#else
#define DBG_ALG(msg)
#endif

#ifdef DEBUG_MEMORY
#define DBG_MEM(msg) std::cout << EnvProg::rank() << " " << msg << std::endl
#else
#define DBG_MEM(msg)
#endif

#ifdef DEBUG_ERROR
#define DBG_ERR(st,func) \
  if ((st).MPI_ERROR != MPI_SUCCESS) { \
    std::cout << EnvProg::rank() << " " << func << ": returned code = " \
    << (st).MPI_ERROR << std::endl; \
  }
#else
#define DBG_ERR(st,func)
#endif


namespace Bob {

/**
@defgroup MPEnvProgGRP The message Passing programming environment
@brief The message Passing programming environment for the Bob++ algorithms
@ingroup EnvProgGRP
@{
\page MEPpage The message Passing programming Environment

*/


/*On a un envprog qui accessible fait les initialisations (methode init), il initialise les id et rank MPI.
Puis la méthode start lance la methode start thr.
La méthode end(), termine l'env thr et appelle MPI_Finalize.

Le but est ici de gérer l'equilibrage de charge des noeuds entre les files de priorité et de maintenir 
la cohérence du goal. Même en environnement distribué, on a toujours des processeurs multi coeurs, nous avons donc fait le choix de toujours utiliser l'environnement de threads sous l'environnement mpx.
Dans ce contexte, un thread supplémentaire est dédié à la gestion des communications.
Il n'a pas de processeurs dédiés, le système s'occupe de lui allouer du temps en fonction des demandes.

Un algorithme d'équilibrage de charge se fait selon une topologie définie
Tous les processus lourds sont organisés en anneau
 */
namespace mpx {

/** The static message Passing Environment class
  * This class is the classical EnvProg class.
 */
class EnvProg {
  static int _nproc;
  static int _rank;

public:
  /// Constructor
  EnvProg() { }
  /// the init method of the environment
  static void init(int &argc, char **&argv);
  /// the end method of the environment
  static void end();
  /// start the environment
  static void start();
  /// Search the rank of a process
  static int rank() {
    return _rank;
  }
  /// Search the total number of processes
  static int nproc() {
    return _nproc;
  }
  /// Display the contents of a packed set of stuff
  static void DisplayPackedBuff(const void * buf, int count,
                                MPI_Datatype datatype);
};




/** Class to have the Bob identifier of a processor
 */
class MPId: public EnvId {
public:
  /// Constructor
  MPId(): EnvId() { }
  /// Destructor
  virtual ~MPId() {}
  /// the id() method
  virtual long id() { return EnvProg::rank(); }
  /// the nb() method
  virtual long nb() { return EnvProg::nproc(); }
};


/** The solution data structure for the MP environment
 */
template<class Trait>
class MPGoal: public ThrGoal<Trait> {
public :
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef ThrGoal<Trait> TheGoal;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
protected:
  /// State of the Goal.
  int state;
public:
  /// Constructor
  MPGoal() : TheGoal(true), state(0) {
    //std::cout << "MPGoal()\n";
  }
  /// Constructor
  MPGoal(const Id &id, bool b) : TheGoal(id,b), state(0) {
    //std::cout << "MPGoal()\n";
  }
  /// Destructor
  virtual ~MPGoal() { }
  ///Init method
  /** Intialiaze the Goal.
     * @param i Instance used to initialize the Goal
     */
  virtual void Init(TheInstance *i) {
    //std::cout << "MPGoal::init\n";
    TheGoal::Init(i);
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode *n) {
    bool b;
    b = TheGoal::is4Search(n);
    return b;
  }
  /** Method called when a node is a solution
    * @param n the solution node
    */
  virtual bool update(TheNode *n,int u=0) {
    bool b;
    b = TheGoal::update(n,u);
    if (b) { state = 1; }
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *g) {
    bool b;
    b = TheGoal::current_merge(g);
    // if (b) state = 1; // ATTENTION, MPI seulement en all-to-all (pour le moment), donc pas nécessaire de transmettre l'info
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *g) {
    bool b;
    b = TheGoal::final_merge(g);
    if (b) state = 1;
    return b;
  }
  /** Reset the state parameter.
    */
  void reset_state() {
    state = 0;
  }
  /** Pack method to Serialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs)  {
    DBGAFF_ENV("MPGoal::Pack", "Packing Goal");
    //bs.Pack(&state,1);
    state = 0;
    TheGoal::Pack(bs);
  }
  /** Unpack method to deserialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_ENV("MPGoal::Pack", "UnPacking Goal");
    //bs.UnPack(&state,1);
    state = 0;
    TheGoal::UnPack(bs);
  }
  /**  get the state of the goal
    */
  int getState() {
    return state;
  }
  /**  return true if goal is updated by threads
    */
  bool is_updated() {
    return (state == 1);
  }
};

/** Class to represent the priority queue use by several processors
 */
template<class Trait,class TheSPQ>
class MPPQ : public TheSPQ {
public:
  /// typedef to define the type of the node.
  typedef typename Trait::Node TheNode;
  /// typedef to define the type of the priority.
  typedef typename Trait::PriComp ThePriComp;
  /// typedef to define the type of the priority.
  typedef typename Trait::Goal TheGoal;
private:
public:

  /// Constructor
  MPPQ(const Id &id, bool l) : TheSPQ(id,l) {
    if (EnvProg::rank() != 0) {
    }
  }
  /// Destructor
  virtual ~MPPQ() {
  }
  ///Reset method
  virtual void Reset() {
      TheSPQ::Reset();
  }
  /// Remove a reference on the Priority Queue
  virtual void remRef() {
    TheSPQ::remRef();
  }
  /// Add a reference on the Priority Queue
  virtual void addRef() {
    TheSPQ::addRef();
  }
  /// Insertion
  virtual void Ins(TheNode *n) {
    n->addRef();
    TheSPQ::Ins(n);
    //~ DBG_COMM("Insert Node "<<nbnode<<std::endl);
  }
  /// Delete greater
  virtual int DelG(TheGoal &g) {
    int nb = TheSPQ::DelG(g);
    return nb;
  }
  virtual TheNode *Del() {
    TheNode *n =TheSPQ::Del();
    if ( n!=0 ) { 
       n->remRef();
    } 
    return n;
  }
  /// Delete the best node
  virtual TheNode *DelLB() {
    DBG_COMM("AV nb_Node : "<<TheSPQ::nb_node()<<std::endl);
    TheNode *n = TheSPQ::DelLB();
    DBG_COMM("AP nb_Node : "<<TheSPQ::nb_node()<<std::endl);
    if ( n==0 && TheSPQ::nb_node()>0 ) DBG_COMM("argleuh DelLB gets null node but nb_Node : "<<TheSPQ::nb_node()<<std::endl);
    return n;
  }
  /// Prints the statistics and contents
  ostream &Prt(ostream &os = std::cout) const {
    TheSPQ::Prt(os);
    return os;
  }
  /// return the load of the priority queue
  long Load() { return TheSPQ::nb_node(); }
};

} // End of namespace mpx

/**
 * @}
 */

} // End of the namespace bobpp
#endif

#include <bobpp/mpx/mpx_mess.hpp>
#include <bobpp/mpx/mpx_strategy.hpp>
#include <bobpp/mpx/mpx_env.hpp>
