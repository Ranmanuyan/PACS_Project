
#ifndef BOB_ATHAENVPROG
#define BOB_ATHAENVPROG

#include<athapascan-1>
#include<bobpp/bobpp>

namespace Bob {

/**
 *  @defgroup AthaEnvProgGRP The Athapascan/Kaapi programming environment
 *  @ingroup EnvProgGRP
 *  @brief This module includes the implementation to interface Bob++
 *  with Athapascan/Kaapi.
 *  @{
 *
 * \page AthEPPage The Athapascan Programming Environment
 *
 * \section AthaEPCons Build, compiling and install
 *
 *  This module is included with the Bob++ library only if you have
 *  given the path to the Athapascan/kaapi installation as an option to
 *  the configure script :
 *  \code
 *  ./configure --with-atha=/path/to/kappi/install
 *  \endcode
 *  and then continue the different phase (see \ref InstMan).
 *
 * \section AthaEPInter The principles
 *
 *  Athpascan is a tool to virtualize the architecture of the machine.
 *  The goal is to create \em tasks using the Athapascan \em Fork.
 *
 *  For our implementation, new algorithms are forked to explore a subspace
 *  of the search space, rooted to a given node.
 *
 *  To respect the Bob++ programmning model, the fork is done in the
 *  Insert method of the Athapascan Priority Queue Bob::AthaPQ::Ins() method.
 *
 * \section AthaEPUse Using Athapascan Programing Environment
 *
 *  The Bob::AthaEnvProg class contains only static methods. The Init() and
 *  End() methods are the methods to initialize and terminate the Athapascan
 *  system.
 *
 *  The Bob::AthaBBAlgoEnvProg class is used by the user side
 *  to begin the resolution of an instance.
 *
 *  The following code shows how to use these two classes.
 *
 *  \code
    int main(int n, char ** v) {
         // Define the environment Algorithm wrapper
         Bob::AthaBBAlgoEnvProg<MyTrait> env;
         // Initialize the environment
         Bob::AthaEnvProg::init(n,v);
         // The initialization of other part of Bob++
         Bob::core::Config(n,v);

         // Initialize the instance.
         MyInstance *Instance=new MyInstance();

         // Solve the instance i.e. all the tree
         env(Instance);

         // Terminate the environment
         Bob::AthaEnvProg::end();
         // Terminate Bob++
         Bob::core::End();
         // Delete the allocated instance
         delete Instance;
    }
 *  \endcode
 *
 */

template<class Trait>class AthaAlgo;

/** Class to have the Bob identifier of an Athapascan thread...
 */
class AthaId: public EnvId {

public:
  /// Constructor
  AthaId(): EnvId() {
    DBGAFF_ENV("AthaId", "Constructeur AthaID");
  }
  /// Destructor
  virtual ~AthaId() {
    DBGAFF_ENV("AthaId", "Destructor AthaID\n");
  }
  /// the id method
  virtual long id() {
    return 0;
  }
  /// the id method
  virtual long nb() {
    return 1;
  }
};

/** @defgroup AthaIOGRP The Bob++/Atha Basic Input/Ouput classes
 *
 * The Bob++ Library defines the Pack and UnPack methods to
 * send/receive or save/restaure internal data of a Bob++ class.
 * These methods used Serialize or DeSerialize objects that represent
 * the real media.
 * The Athapascan interface on top of Kaapi use the classical operators
 * << and >> to do the same things. They use the a1::OStream and a1::IStream
 * to represent the media.
 * This module is a way to interface the Bob++ methods on the Athapascan
 * interface.
 *
 * Then we have defined the classes  Bob::AthaNode and Bob::AthaInst
 * and Bob::AthaGoal.
 * These classes implement
 * the operator<< and operator>> on a a1::OStream and a1::IStream.
 * These methods call the UnPack method on a AthaIn objects and
 * UnPack method on a AthaOut objet.
 * The AthaOut and AthaIn classes inherit respectively
 * from the Serialize and DeSerialize class. Each of them used the methods
 * to write or read datas on a a1::OStream or a a1::IStream.
 *
 * @{
 */

#if 0
/** the Macro AthaSER is used to debug the pack method.
 */
#define ATHASER(type,nb) std::cout<<"AthaOut "<<#type<<":"<<nb<<std::endl;
//size+=nb*sizeof(type);
/** the Macro AthaDER is used to debug the unpack method.
 */
#define ATHADES(type,nb) std::cout<<"AthaIn "<<#type<<":"<<nb<<std::endl;
//size+=nb*sizeof(type);
#else
/** the Macro AthaSER is used to debug the pack method.
*/
#define ATHASER(type,nb)
/** the Macro AthaDER is used to debug the unpack method.
*/
#define ATHADES(type,nb)
#endif

/** the Class AthaOut
 * this class inherits from the Serialize class, this just a wrapper
 * to call the Kaapi/Athapascan methods to pack datas but respecting the
 * Bob++ interface.
 */
struct AthaOut : public Serialize {
  /** Constructor
    * @param _out the athapascan a1::OStream used to write the datas
    */
  AthaOut(a1::OStream *_out): f(_out), size(0) { }
  /** Destructor
    */
  virtual ~AthaOut() {/*std::cout<<"AthaOut:"<<size<<std::endl;*/}
  /// Pack method for array of bool
  virtual int Pack(const bool *n, int nb) {
    ATHASER(bool, nb);
    f->write(&a1::FormatDef::Bool, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of int
  virtual int Pack(const int *n, int nb) {
    ATHASER(int, nb);
    f->write(&a1::FormatDef::Int, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of unsigned int
  virtual int Pack(const unsigned int *n, int nb) {
    ATHASER(unsigned int, nb);
    f->write(&a1::FormatDef::UInt, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of short
  virtual int Pack(const short *n, int nb) {
    ATHASER(short, nb);
    f->write(&a1::FormatDef::Short, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of unsigned short
  virtual int Pack(const unsigned short *n, int nb) {
    ATHASER(unsigned short, nb);
    f->write(&a1::FormatDef::UShort, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of unsigned long long
  virtual int Pack(const unsigned long long *n, int nb) {
    ATHASER(unsigned long long , nb);
    f->write(&a1::FormatDef::ULLong, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of long long
  virtual int Pack(const long long *n, int nb) {
    ATHASER(long long , nb);
    f->write(&a1::FormatDef::LLong, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of long
  virtual int Pack(const long *n, int nb) {
    ATHASER(long, nb);
    f->write(&a1::FormatDef::Long, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of unsigned long
  virtual int Pack(const unsigned long *n, int nb) {
    ATHASER(unsigned long, nb);
    f->write(&a1::FormatDef::ULong, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of float
  virtual int Pack(const float *n, int nb) {
    ATHASER(float, nb);
    f->write(&a1::FormatDef::Float, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of double
  virtual int Pack(const double *n, int nb) {
    ATHASER(double, nb);
    f->write(&a1::FormatDef::Double, a1::OStream::IA, n, nb);
    return nb;
  }
  /// Pack method for array of char
  virtual int Pack(const char *s, int nb) {
    ATHASER(char, nb);
    f->write(&a1::FormatDef::SChar, a1::OStream::IA, s, nb);
    return nb;
  }
  /// the athapascan/Kaapi OStream used to pack the data.
  a1::OStream *f;
  /// the packed data size.
  long size;
};

/** The Class AthaIn
 * this class inherits from the DeSerialize class, this just a wrapper
 * to call the Kaapi/Athapascan methods to unpack datas but respecting the
 * Bob++ interface.
 */
struct AthaIn : public DeSerialize {
  /** Constructor
    * @param _in the athapascan a1::IStream used to read the datas
    */
  AthaIn(a1::IStream *_in): f(_in), size(0) { }
  /** Destructor
    */
  virtual ~AthaIn() {/*std::cout<<"AthaIn:"<<size<<std::endl;*/}
  /// UnPack method for array of bool
  virtual void UnPack(bool *n, int nb) {
    ATHADES(bool, nb);
    f->read(&a1::FormatDef::Bool, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of int
  virtual void UnPack(int *n, int nb) {
    ATHADES(int, nb);
    f->read(&a1::FormatDef::Int, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of long
  virtual void UnPack(long *n, int nb) {
    ATHADES(long, nb);
    f->read(&a1::FormatDef::Long, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of unsigned long
  virtual void UnPack(unsigned long *n, int nb) {
    ATHADES(unsigned long, nb);
    f->read(&a1::FormatDef::ULong, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of int
  virtual void UnPack(unsigned int *n, int nb) {
    ATHADES(unsigned int, nb);
    f->read(&a1::FormatDef::UInt, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of short
  virtual void UnPack(short *n, int nb) {
    ATHADES(short, nb);
    f->read(&a1::FormatDef::Short, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of unsigned short
  virtual void UnPack(unsigned short *n, int nb) {
    ATHADES(unsigned short, nb);
    f->read(&a1::FormatDef::UShort, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of long long
  virtual void UnPack(long long *n, int nb) {
    ATHADES(long long , nb);
    f->read(&a1::FormatDef::LLong, a1::OStream::IA, n, nb);
  }
  /// Pack method for array of long long
  virtual void UnPack(unsigned long long *n, int nb) {
    ATHADES(unsigned long long , nb);
    f->read(&a1::FormatDef::ULLong, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of float
  virtual void UnPack(float *n, int nb) {
    ATHADES(float, nb);
    f->read(&a1::FormatDef::Float, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of double
  virtual void UnPack(double *n, int nb) {
    ATHADES(double, nb);
    f->read(&a1::FormatDef::Double, a1::OStream::IA, n, nb);
  }
  /// UnPack method for array of char
  virtual void UnPack(char *s, int nb) {
    ATHADES(char, nb);
    f->read(&a1::FormatDef::SChar, a1::OStream::IA, s, nb);
  }
  /// the athapascan/Kaapi IStream used to unpack the data.
  a1::IStream *f;
  /// the unpacked data size.
  long size;
};

/**
 * @}
 */

/** @defgroup the Class used as Shared
 * @{
 */


/** The AthaNode class is a wrapper on a Node to implement the
 * operator<< and operator>> for the Athapascan framework.
 */
template<class Trait>
class AthaNode : public Trait::Node {
public:
  /// Constructor

  /// Destructor
  virtual ~AthaNode() {}
  /// Operator= needed by Athapascan/kaapi
  AthaNode<Trait> &operator=(const AthaNode<Trait> &n) {
    Trait::Node::copy(n);
    return *this;
  }
  /// operator<< used by the athapascan framework
  friend a1::OStream &operator<< (a1::OStream & s_out, const AthaNode<Trait>& b) {
    AthaOut ao(&s_out);
    DBGAFF_ENV("AthaNode::operator<<", "Pack a node");
    //std::cout<<"Pack Node\n";
    //b.Prt(std::cout);
    //std::cout<<"---------\n";
    b.Pack(ao);
    return s_out;
  }
  /// operator>> used by the athapascan framework
  friend a1::IStream &operator>> (a1::IStream & s_in, AthaNode<Trait>& b) {
    AthaIn ai(&s_in);
    DBGAFF_ENV("AthaNode::operator>>", "Unpack a node");
    b.UnPack(ai);
    //std::cout<<"Un Pack Node\n";
    //b.Prt(std::cout);
    //std::cout<<"------------\n";
    return s_in;
  }
};

/**
 * @}
 */

/** @defgroup ATHAGDS The Classes stored in the MonotonicBound
 *
 * The principle of Bob++ is to handle the parallelism inside the data
 * structures used by the Bob++ algorithms. Then to interface Bob++
 * with Athapascan/Kaapi, we redefine a Bob++ Priority Queue i.e. Bob::AthaPQ
 * and a Bob++ goal i.e. Bob::AthaGoal. These data structures use the
 * Athapascan/Kaapi programming interface to create parallelism.
 *
 * @{
 */

/** The AthaInst class stores a pointer on a Instance to implement the
 * operator<< and operator>> for the Athapascan framework.
 */
template<class Trait>
class AthaInst : public Trait::Instance {
public:
/// type for the user side Instance
  typedef typename Trait::Instance TheInstance;
protected:
/// boolean for the update method.
public:
  bool update;
  /// Constructor
  AthaInst(): Trait::Instance(), update(false) {
    DBGAFF_ENV("AthaInst::AthaInst()", "constructor no parameter");
  }
  /// Constructor
  AthaInst(TheInstance *_i): Trait::Instance(*_i), update(false) {
    DBGAFF_ENV("AthaInst::AthaInst()", "constructor with instance");
  }
  /// Constructor
  AthaInst(AthaInst<Trait> const &ai): Trait::Instance(ai), update(false) {
    DBGAFF_ENV("AthaInst::AthaInst()", "copy const");
  }
  /// Constructor
  AthaInst(AthaInst<Trait> &ai): Trait::Instance(ai), update(false) {
    DBGAFF_ENV("AthaInst::AthaInst()", "copy non const");
  }
  /// Destructor
  virtual ~AthaInst() {}
  /// Operator= needed by Athapascan/kaapi
  AthaInst<Trait> &operator=(const AthaInst<Trait> &n) {
    Trait::Instance::copy(n);
    return *this;
  }
  /// Acess method
  TheInstance *Inst() {
    return this;
  }
  /// Acess method
  const TheInstance *Inst() const {
    return this;
  }
  /// operator<< used by the athapascan framework
  friend a1::OStream &operator<< (a1::OStream & s_out, const AthaInst<Trait>& b) {
    AthaOut ao(&s_out);
    DBGAFF_ENV("AthaInst::operator<<", "Pack an instance");
    b.Pack(ao);
    return s_out;
  }
  /// operator>> used by the athapascan framework
  friend a1::IStream &operator>> (a1::IStream & s_in, AthaInst<Trait>& b) {
    AthaIn ai(&s_in);
    DBGAFF_ENV("AthaInst::operator>>", "Unpack an instance");
    b.UnPack(ai);
    return s_in;
  }
};

/** Class to update the instance stored in the Monotonic bound.
 */
template <class OneClass>
struct OneUpdate {
  /** functor used by th a1::MonotonicBound to update its internal value.
    * the goal here is to update a "global object" only once.
    * It is just use to broadcast data store in object on all the process.
    * @param ag the object to update
    * @param agt the object use to update
    * @return bool returns true if the ag has been updated or not.
    */
  bool operator()(OneClass& ag, const OneClass& agt) {
    std::cout << "TRY Update the Instance : ";
    if (ag.update) {
      std::cout << "----NO------ \n";
      return false;
    }
    ag = agt;
    std::cout << "------YES-------\n";
    ag.Prt(std::cout);
    ag.update = true;
    return true;
  }
};


/** @defgroup ATHAGAOL The Athapascan Goal
 * To handle the Goal of the Bob++ framework, the Athapascan's
 * Monotonic bound with global pointer is used.
 * @{
 */

/** The comparison class operator to update a Goal stored in a
 * a1::MonotonicBound
 */
template <class AtGoal>
struct Update {
  /** functor used by th a1::MonotonicBound to update its internal value.
    * here it use to execute the final_merge on a goal.
    * @param ag the object to update
    * @param agt the object use to update
    * @return bool returns true if the ag has been updated or not.
    */

  bool operator()(AtGoal& ag, const AtGoal& agt) {
    return ag.final_merge(&agt);
  }
};



/** The Goal class for the athapascan environment
 *  This class is just a wrapper on the real goal used
 *  by the application and given by a typedef in the Trait class.
 *  The a1::operator<< and a1::operator>> have to be mapped on the
 *  Pack and UnPack Bob++ methods.
 */
template<class Trait>
class AthaGoal: public Trait::Goal {
public :
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Instance
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Statistics
  typedef typename Trait::Stat TheStat;
protected:
public:
  /// Constructor
  AthaGoal() : TheGoal() {
    DBGAFF_MEM("AthaGoal::AthaGoal()", "constructor no parameter");
  }
  /// Constructor
  AthaGoal(const Id &id,bool l=true) : TheGoal(id,l) {
    DBGAFF_MEM("AthaGoal::AthaGoal()", "constructor parameter id bool ");
  }
  /// Constructor
  AthaGoal(AthaGoal<Trait> &ai) : TheGoal(ai) {
    DBGAFF_MEM("AthaGoal::AthaGoal()", "copy non const");
  }
  /// Constructor
  AthaGoal(const AthaGoal<Trait> &ai) : TheGoal(ai) {
    DBGAFF_MEM("AthaGoal::AthaGoal()", "copy const");
  }
  /// Destructor
  virtual ~AthaGoal() {
    DBGAFF_MEM("AthaGoal::~AthaGoal()", "Destructor");
  }
  /** Intialiaze the Goal.
     * @param i Instance used to initialize the Goal
     */
  void Init(TheInstance *i) {
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
  virtual bool update(TheNode *n) {
    bool b;
    if (n == 0) return false;
    b = TheGoal::update(n);
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *g) {
    bool b;
    b = TheGoal::final_merge(g);
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal * g) {
    bool b;
    b = TheGoal::current_merge(g);
    return b;
  }

  /** assignment operator
   */
  AthaGoal<Trait> &operator=(const AthaGoal<Trait> &ag) {
    final_merge(&ag);
    return *this;
  }
  /// operator<< used by the athapascan framework
  friend a1::OStream &operator<< (a1::OStream & s_out, const AthaGoal<Trait>& b) {
    AthaOut ao(&s_out);
    DBGAFF_ENV("AthaGoal::operator<<", "Pack an Goal");
    b.Pack(ao);
    return s_out;
  }
  /// operator>> used by the athapascan framework
  friend a1::IStream &operator>> (a1::IStream & s_in, AthaGoal<Trait>& b) {
    AthaIn ai(&s_in);
    DBGAFF_ENV("AthaGoal::operator>>", "Unpack an Goal");
    b.UnPack(ai);
    return s_in;
  }

};

/**
 * @}
 */
/**
 * @}
 */



/** @defgroup ATHACTRL Generate the Parallelism and Control the granularity
 *
 * The goal is to test if the exploration of a node is forked
 * or if it is simply inserted in the local priority queue of
 * the current algorithm
 * @{
 */

/** struct IsFork
 * It is just the basic class all the nodes are forked
 */
struct IsFork {
  /// Constructor
  IsFork() {}
  /// Destructor
  virtual ~IsFork() {}
  /** Method to test if the exploration
    * of a node is forked or not
    * Here the method always returns true.
    */
  virtual bool fork(int d) {
    return true;
  }
};

/** struct ForkFixe
 * A node is forked if the number of local insertion
 * is more than a fixed number (give in parameter) of the constructor.
 */
struct Fork_Fixe : public IsFork {
  /// the current number of node inserted
  int c;
  /// the maximum number of node inserted before a fork
  int nb;
  /** Constructor
    * @param n the number of insertion before to fork one node
    */
  Fork_Fixe(int n = 5): IsFork(), c(0), nb(n) {}
  /// Destructor
  virtual ~Fork_Fixe() {}
  /// Method to test if a node is forked or not
  virtual bool fork(int d) {
    c++;
    if (c <= nb) {
      return false;
    }
    c = 0;
    return true;
  }
};


/** Class used as priority queue in the athapascan environment
 * This class is used by the algorithms to store new nodes but also to spawn
 * new algorithms using the athapascan call \em Fork.
 *
 * This class use an Bob::IsFork class to control if a node will be
 * root node of new spawned algorithm or simply inserted in its local priority
 * queue.
 * The class Bob::IsFork is used to control the granularity of
 * the search i.e. the size of the subtree explorated by one Algorithm.
 */
template<class Trait>
class AthaPQ : public PQSkew<typename Trait::Node, typename Trait::PriComp,typename Trait::Goal> {
public:
  /// type for the node
  typedef typename Trait::Node TheNode;
  /// type for the Priority Comparator
  typedef typename Trait::PriComp ThePriComp;
  /// type for the Priority Comparator
  typedef typename Trait::Goal TheGoal;
  /// type for the Priority Queue
  typedef PQSkew<TheNode, ThePriComp,TheGoal> TheLPQ;
  /// type for the Instance
  typedef typename Trait::Instance TheInstance;
  /// type for the Athapascan Node with the operator to serialize the node
  typedef AthaNode<Trait> ANode;
  /// type for the Shared Athapascan Node
  typedef a1::Shared< ANode > AANode;
  /// type for the Athapascan Instance with the operator to serialize the Instance
  typedef AthaInst<Trait> AInstance;
  /// type for the Athapascan compliant Bob++ Goal
  typedef AthaGoal<Trait> AGoal;
  /// type for the Shared Sync
  typedef a1::Shared_w<bool> Sync;
protected:
  /// Object used to control the ganularity of an Athapascan Task
  IsFork *isf;
  /// The Sync use to synchronize the main task to finsh after all the other ones
  Sync *sync;
public:
  /// Constructor
  AthaPQ(const Id &id,Sync *s, IsFork *_isf) : TheLPQ(id), isf(_isf), sync(s) {
    TheLPQ::getStat()->add_counter('f', "Forked");
  }
  /// Destructor
  virtual ~AthaPQ() { }
  /// Insertion in the local priority queue
  virtual void InsLoc(TheNode *n) { TheLPQ::Ins(n); }

  /// Insertion with the possibility of fork a task
  virtual void Ins(typename Trait::Node *n) {
    if ( isf->fork(n->dist())) {
      TheLPQ::getStat()->get_counter('f')++;
      DBGAFF_ENV("AthaPQ::Ins", "fork the search");
      a1::Shared< ANode > an(*((ANode *)n));
      if (n->isDel()) delete n;
      //std::cout << "Fork a node "<<std::endl;
      a1::Fork< AthaAlgo<Trait> >()(*sync, an);
    } else {
      DBGAFF_ENV("AthaPQ::Ins", "local search");
      TheLPQ::Ins(n);
    }
  }
  /// Prints the statistics and contents
  ostream &Prt(ostream &os = std::cout) const {
    TheLPQ::Prt(os);
    return os;
  }
};
/**
 * @}
 */



/** The Athapascan Environment (static class)
 */
class AthaEnvProg {
  static a1::Community *com ;
  static int nb;
  static std::vector<void *> v_inst;
  static std::vector<void *> v_goal;
  static stat_set v_stat;
public:
  /// Constructor
  AthaEnvProg() { }
  /// The init method of the environment
  static void init(int &argc, char **argv) {
    Bob::core::opt().add(std::string("--atha"), Bob::Property("-f", "fork fixed", 0));

    com = new a1::Community(a1::System::initialize_community(argc, argv));
  }

  /** a commit method
   */
  static void commit() {
    com->commit();
  }


  /// the End methods
  static void end() {
    com->leave();
    a1::System::terminate();
    delete com;
  }
  /** Set the pointer on the a1::SingleAssignment of the ith Instance.
    */
  static void setInst(int i, void *in) {
    v_inst[i] = in;
  }
  /** Set the pointer on the a1::monotonicBound of the ith Goal.
    */
  static void setGoal(int i, void *g) {
    v_goal[i] = g;
  }
  /** Set the statistics of the ith algorithms.
    */
  static void setAlStat(int i, stat *s) {
     v_stat.get_algo_stat(i)=(*s);
  }
  /** merge a statistic of the ith algorithms with the static one.
    */
  static void mergeAlStat(int i, stat &s) {
    v_stat.merge_algo_stat(s,i);
  }
  /** Get the statistics of the ith algo.
    */
  static stat *getAlStat(int i) {
    return &v_stat.get_algo_stat(i);
  }
  /** Set the statistics of the ith PQ.
    */
  static void setPQStat(int i, stat *s) {
     v_stat.get_pq_stat(i)=(*s);
  }
  /** merge a statistic of the ith PQ with the static one.
    */
  static void mergePQStat(int i, stat &s) {
    v_stat.merge_pq_stat(s,i);
  }
  /** Get the statistics of the ith PQ.
    */
  static stat *getPQStat(int i) {
    return &v_stat.get_pq_stat(i);
  }

  /** Get the pointer on the a1::SingleAssignment of the ith Instance.
    */
  static void *getInst(int i) {
    return v_inst[i];
  }
  /** Get the pointer on the a1::monotonicBound of the ith Goal.
    */
  static void *getGoal(int i) {
    return v_goal[i];
  }
  /** return true if the process is the leader process.
    * the com->is_leader() method is called.
    */
  static bool is_leader() {
    return com->is_leader();
  }
};

/** @defgroup ATHATASKS Types used to represent the athapascan/kaapi tasks
 * @{
 */

/** This class is a Wrapper on an Bob++ Algorithm.
 * This class is the Athapascan/Kaapi task
 */
template<typename Trait>
class AthaAlgo {
public:
  /// type of a node.
  typedef typename Trait::Node TheNode;
  /// type of a athapascan node.
  typedef AthaNode<Trait> ANode;
  /// type of a Shared node.
  typedef a1::Shared< ANode > AANode;
  /// type of the instance.
  typedef typename Trait::Instance TheInstance;
  /// type of the Athapascan instance.
  typedef AthaInst<Trait> AInstance;
  /// type of the Shared instance.
  typedef a1::SingleAssignment<AInstance> MBInstance;
  /// type of the Athapascan Goal.
  typedef AthaGoal<Trait> AGoal;
  /// type of the Shared Goal.
  typedef a1::MonotonicBound<AGoal, Update<AGoal> > MBGoal;
  /// type of the algorithm.
  typedef typename Trait::Algo TheAlgo;
  /// type of the priority.
  typedef typename Trait::PriComp ThePriComp;
  /// type of the Goal.
  typedef typename Trait::Goal TheGoal;
  /// type of the statistics.
  typedef typename Trait::Stat TheStat;
  /// type for the Shared Sync
  typedef a1::Shared_w<bool> Sync;
  /** Constructor */
  AthaAlgo() { }
  /** Destructor */
  ~AthaAlgo() { }
  /** operator() the operator of a athapascan/Kaapi Task
    */
  void operator()(Sync sync,a1::Shared_r<ANode> n) {
    MBInstance *mb_ai = (MBInstance *)AthaEnvProg::getInst(0);
    MBGoal *mb_ag = (MBGoal *)AthaEnvProg::getGoal(0);
    DBGAFF_ENV("AthaAlgo::operator()", "-------- ENTER ---------");

//std::cout<<"Atha Algo\n";
    //mb_ai->acquire();
    const AInstance &ti = mb_ai->read();
//            std::cout << "\nPRINT INSTANCE" << std::endl;
//            ti.Inst()->print();
    //mb_ai->release();

    ANode nd(n.read());
    Fork_Fixe ff(Bob::core::opt().NVal("--atha", "-f"));
    mb_ag->acquire();
    AGoal agoal;
    agoal.current_merge(&(mb_ag->read()));
    mb_ag->release();
    AthaPQ<Trait> apq(*(ti.id()),&sync, &ff);
    TheStat st(*(ti.id()));
    TheAlgo al(&ti, &apq, &agoal, &st);
    nd.addRef();
    apq.InsLoc(&nd);
    DBGAFF_ENV("AthaAlgo::operator()", "begin the algo");
    al();
    //st.PrtL(std::cout);
    //st.PrtD(std::cout);
    mb_ag->acquire();
    mb_ag->update(agoal);
    mb_ag->release();
    AthaEnvProg::mergeAlStat(0, st);
    AthaEnvProg::mergePQStat(0,*(apq.getStat()));
    DBGAFF_ENV("AthaAlgo::operator()", "-------- END ---------");
  }
};

/** The Print is an athapscan Task use to display all the results
 * at the end of the execution of the all tasks.
 */
template<class Trait>
struct print {
  /// type of the AGoal.
  typedef AthaGoal<Trait> AGoal;
  /// type of th Goal staored by an a1::MonotonicBound
  typedef a1::MonotonicBound<AGoal, Update<AGoal> > MBGoal;
  /// the operator use by kaapi to execute the task.
  void operator()(a1::Shared_r<bool> r) {

    
    std::cout << "------------------ " << 0 << "th  Algorithm ---------------\n";
    AthaEnvProg::getAlStat(0)->display_label(std::cout);
    AthaEnvProg::getAlStat(0)->display_data(std::cout);
    AthaEnvProg::getPQStat(0)->display_label(std::cout);
    AthaEnvProg::getPQStat(0)->display_data(std::cout);
    MBGoal *mb_ag = (MBGoal *)AthaEnvProg::getGoal(0);
    mb_ag->acquire();
    AGoal agoal(mb_ag->read());
    mb_ag->release();
    agoal.getStat()->display_label(std::cout);
    agoal.getStat()->display_data(std::cout);
    agoal.Prt(std::cout);
  }
};

/** The Go is an athapscan Task use to initalize the Search
 */
template<class Trait>
struct Go {
  /// type of an Instance
  typedef typename Trait::Instance TheInstance;
  /// type of the Ainstance.
  typedef AthaInst<Trait> AInstance;
  /// type of the MBAinstance.
  typedef a1::SingleAssignment<AInstance> MBInstance;
  ///type for a Node
  typedef typename Trait::Node TheNode;
  /// type of an Athapascan  node.
  typedef AthaNode<Trait> ANode;
  /// type of an Athapascan shared  node.
  typedef a1::Shared< ANode > AANode;
  /// type of the AGoal.
  typedef AthaGoal<Trait> AGoal;
  /// type of the MBGoal.
  typedef a1::MonotonicBound<AGoal, Update<AGoal> > MBGoal;
  /// type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// type of the stat
  typedef typename Trait::Stat TheStat;
  /// type of the search priority
  typedef typename Trait::PriComp ThePriComp;
  /// type of the search priority
  typedef typename Trait::Goal TheGoal;
  /// type for the Shared Sync
  typedef a1::Shared<bool> DefSync;
  /// type for the Shared_r Sync
  typedef a1::Shared_r<bool> RSync;
  /// type for the priority Queue
  typedef PQSkew<TheNode, ThePriComp,TheGoal>  ThePQ;

  /** Functor executed by the Athapascan system
    * Here only the sync is used since the root node is given by the instance
    */
  void operator()(a1::Shared_w<bool> sync) {
    MBInstance *mb_ai = (MBInstance *)AthaEnvProg::getInst(0);
    MBGoal *mb_ag = (MBGoal *)AthaEnvProg::getGoal(0);
    DBGAFF_ENV("GO::operator()", "-------- ENTER ---------");

    std::cout << "Go the computation\n";
    //mb_ai->acquire();
    const AInstance &ti = mb_ai->read();
    //mb_ai->release();
    mb_ag->acquire();
    AGoal agoal(*(ti.id()));
    agoal.current_merge(&(mb_ag->read()));
    mb_ag->release();

    Fork_Fixe ff(Bob::core::opt().NVal("--atha", "-f"));
    AthaPQ<Trait> apq(*(ti.id()),&sync, &ff);
    TheStat st(*(ti.id()));
    TheAlgo pal(&ti, &apq, &agoal, &st);
    DBGAFF_ENV("GO::operator()", "leader starts the algo");
    ti.InitAlgo(&pal);
    pal();
    //st.PrtL(std::cout);
    //st.PrtD(std::cout);
    mb_ag->acquire();
    mb_ag->update(agoal);
    mb_ag->release();
    AthaEnvProg::mergeAlStat(0, st);
    AthaEnvProg::mergePQStat(0,*(apq.getStat()));
    DBGAFF_ENV("AthaAlgoEnvProg::operator()", "leader finishs the algo");

  }
};
/**
 * @}
 */

/** The Athapascan EnvProg Wrapper for Branch and Bound.
 */
template<class Trait>
class AthaAlgoEnvProg : public AlgoEnvProg<Trait> {
  /// type of an Instance
  typedef typename Trait::Instance TheInstance;
  /// type of the Ainstance.
  typedef AthaInst<Trait> AInstance;
  /// type of the MBAinstance.
  typedef a1::SingleAssignment<AInstance> MBInstance;
  /// type for a Node
  typedef typename Trait::Node TheNode;
  /// type of an Athapascan  node.
  typedef AthaNode<Trait> ANode;
  /// type of an Athapascan shared  node.
  typedef a1::Shared< ANode > AANode;
  /// type of the user Goal.
  typedef typename Trait::Goal TheGoal;
  /// type of the Athapascan Goal.
  typedef AthaGoal<Trait> AGoal;
  /// type of the athapascan Goal enclosed in the monotonic bound.
  typedef a1::MonotonicBound<AGoal, Update<AGoal> > MBGoal;
  ///  type of the Algo
  typedef typename Trait::Algo TheAlgo;
  ///  type of the stat
  typedef typename Trait::Stat TheStat;
  ///  type of the search priority
  typedef typename Trait::PriComp ThePriComp;
  /// type for the Shared Sync
  typedef a1::Shared<bool> DefSync;
  /// type for the priority Queue
  typedef PQSkew<TheNode, ThePriComp,TheGoal>  ThePQ;
private:

public:
  /// the constructor
  AthaAlgoEnvProg() : AlgoEnvProg<Trait>() {
    std::cout << "Alloc AthaAlgoEnvProg" << std::endl;
  }
  /// the destructor
  virtual ~AthaAlgoEnvProg() {
  }
  /** solve the problem sequentially
   * @param inst the instance to solve
   */
  virtual void operator()(TheInstance *inst) {
    doit(0, inst);
  }
  /** solve the problem from the node nd sequentially
   * @param inst the instance to solve
   * @param nd the root node to solve
   */
  virtual void operator()(TheNode *nd, TheInstance *inst) {
    std::cerr << "Call the resolution with a node does not work with the athapascan environment\n";
    exit(1);
    doit(nd, inst);
  }
  /** get the goal after a resolution
   * @return the goal with the solution.
   */
  TheGoal *goal() {
    return 0;
  }
  /** get the stat after a resolution
   * @return the stat.
   */
  TheStat *stat() {
    return (TheStat *)AthaEnvProg::getAlStat(0);
  }
  /** Do the job
   */
  void doit(TheNode *nd, TheInstance *inst) {
    MBInstance  *mb_ai;
    MBGoal *mb_ag;
    TheStat *st;
    st = new TheStat(*(inst->id()));
    mb_ai = new MBInstance();
    mb_ai->initialize("MyInstance");
    mb_ag = new MBGoal();
    AGoal *ag= new AGoal(*(inst->id()));
    mb_ag->initialize("MyGoal", ag);
    AthaEnvProg::setInst(0, (void *)mb_ai);
    AthaEnvProg::setGoal(0, (void *)mb_ag);
    AthaEnvProg::setAlStat(0,st);
    ThePQ pqs(*(inst->id()));
    AthaEnvProg::setPQStat(0,pqs.getStat());
    //AthaEnvProg::getStat(0)->get_pq_stat(0)->add_counter('f', "Forked");

    /* commit to Kaapi the creation of the global variables: start threads */
    AthaEnvProg::commit();

    if (AthaEnvProg::is_leader()) {
      DefSync ds;
      inst->Init();
      AInstance *ai = new AInstance(inst);
      std::cout << "AthaAlgoEnvProg::operator()" << "I am the Master" << std::endl;
      mb_ai->write(*ai);
      //mb_ai->update(ai);
      //mb_ai->release();

      AGoal agoal;
      agoal.Init(inst);
      agoal.set_verbosity(BaseAlgoEnvProg::isdisp_goal());
      mb_ag->acquire();
      mb_ag->update(agoal);
      mb_ag->release();
      std::cout << "Go AthaEnvProg the computation\n";
      a1::Fork<Go<Trait> >()(ds);
      a1::Sync();

      a1::Fork<print<Trait> >()(ds);
    } else {
      //AGoal agoal;
      //agoal.Init(inst);
      //mb_ag->acquire();
      //mb_ag->update(agoal);
      //mb_ag->release();
      std::cout << "AthaAlgoEnvProg::operator()" << "I am a slave" << std::endl;
    }
    // Could delete the instance and the Goal now because
    // i do not know to delete a Monotonic bound...
    /*delete mb_ai;
    delete mb_ag;*/
  }
  void display_goal() {}
  void display_stat() {}
  void dump() {
    std::cerr << " Not implemented yet\n";
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

};


/** The Athapascan Algorithms Wrapper for Branch and Bound.
 */
template<class Trait>
class AthaBBAlgoEnvProg : public AthaAlgoEnvProg<Trait> {
public:
  /// the constructor
  AthaBBAlgoEnvProg() : AthaAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~AthaBBAlgoEnvProg() {}
};

/** The Athapascan Algorithms Wrapper for simple Divide and Conquer.
 */
template<class Trait>
class AthaSDCAlgoEnvProg : public AthaAlgoEnvProg<Trait> {
public:
  /// the constructor
  AthaSDCAlgoEnvProg() : AthaAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~AthaSDCAlgoEnvProg() {}
};

/** The Athapascan Algorithms Wrapper for Costed Divide and Conquer.
 */
template<class Trait>
class AthaCDCAlgoEnvProg : public AthaAlgoEnvProg<Trait> {
public:
  /// the constructor
  AthaCDCAlgoEnvProg() : AthaAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~AthaCDCAlgoEnvProg() {}
};

/** The Athapascan Algorithms Wrapper for iterated algorithm.
 */
template<class Trait>
class AthaStateAlgoEnvProg : public AthaAlgoEnvProg<Trait> {
public:
  /// the constructor
  AthaStateAlgoEnvProg() : AthaAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~AthaStateAlgoEnvProg() {}
};

/**
 * @}
 */

} // End of the namspace
#endif

