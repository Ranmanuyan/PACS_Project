/*
 * Bobpp/src/bob_statealgo.h
 *
 * This source include the abstract data type for the Algo level where
 * the goal is to find axect solution of a problem.
 */

#include <string>
#include <list>



#ifndef BOBSTATEALGO_HEADER
#define BOBSTATEALGO_HEADER
namespace Bob {

/*---------------------------------------------------------------------*/
/** @defgroup SimSpAlgoGRP Simulation of entities in a 2D or 3D space.
 *  @ingroup UserAlgoGRP
 *  @brief Algorithm interface to simulate entities that are stored in nodes.
 * @{
 *  The goal of this algorithm is to be able to simulate Entities, that can move or 
 *  communicate in a space.
 *  
 *  The node here represents a piece of the space, it stores several Entities.
 *  Nodes could communicate using by sending message link.
 *  The link proposed here, could store tagged messages. The messages are stored as FIFO,
 *  meaning that we can not pop a message before an older one.
 *
 *  The node and by consequence the Entities are computed iteratively . At each step 
 *  a node is pickup out of the priority queue, then is is given to the genchild::operator()
 *  and then reinserted in the queue.
 *  The step of a node is given by the dist() method.
 *  If several node are in the queue, the highest one has the best priority.
 */

/** @defgroup SimSpSpaceGRP Space structs  
 *  @ingroup SimSpAlgoGRP
 *  @brief Space class to easily handle 2D or 3D structs.
 * @{
 * When the simulation must handle a Space 2D or 3D, one have to handle 
 * the distribution of the node in the space. Wich node is the neighbour 
 * of another one. Then we propose the 2 structures, that are a simple 
 * response to this problem. You can use it but also define your link 
 * as you like.
 */

/** 2D Space
  */
struct Space2D {
  /** Enumeration for the possible directions
    */
  enum direct {PREVX,/*!< Previous node in the x direction. */
               PREVY,/*!< Previous node in the y direction. */
               NEXTX,/*!< Next node in the x direction. */
               NEXTY,/*!< Next node in the y direction. */
               NBDIRECT /*!< Number of direction. */
  };
  
  long x,  /*!< x size  */
       y,  /*!< x size  */
       xy; /*!< xy size */
  /** Constructor
    */
  Space2D() : x(0),y(0),xy(0) { }
  /** Constructor
    */
  Space2D(long _x,long _y) : x(_x),y(_y),xy(_x*_y) {}
  /** Destructor
    */
  ~Space2D() { }
  /** Returns the number of neighbour
    */
  long nb_neighbour() { return NBDIRECT; }
  /** initialize the number of subspace in each direction.
    */
  void init(long _x,long _y) { x=_x;y=_y;xy=_x*_y; }
  /** returns if the node identified with id has a neighbour in the direction d
    */
  bool has_neighbour(long id,direct d) {
     switch(d) {
        case PREVX: return (id < y);
        case PREVY: return (id % y)!=0;
        case NEXTX: return (id+y)<(xy);
        case NEXTY: return ((id+1)%y)!=0;
        default : std::cerr << "Error bad direction in 2D space :"<<d<<"\n";exit(1);
      }
      return false;
  }
  /** returns identifier of the neighbour of the node identified with id
    */
  long neighbour(long id,direct d) {
     if ( !has_neighbour(id,d)) return -1;
     switch(d) {
        case PREVX: return id-y;
        case PREVY: return id -1;
        case NEXTX: return id+y;
        case NEXTY: return id+1;
        default : std::cerr << "Error bad direction in 2D space :"<<d<<"\n";exit(1);
      }
      return -1;
  }
};

/** 3D Space
  */
struct Space3D {
  /** Enumeration for the possible directions
    */
  enum direct {PREVX,/*!< Previous node in the x direction. */
               PREVY,/*!< Previous node in the y direction. */
               PREVZ,/*!< Previous node in the z direction. */
               NEXTX,/*!< Next node in the x direction. */
               NEXTY,/*!< Next node in the y direction. */
               NEXTZ,/*!< Next node in the z direction. */
               NBDIRECT /*!< Number of direction. */
  };
  long x,  /*!< x size */
       y,  /*!< y size */
       z,  /*!< z size */
       yz, /*!< yz size */
       xyz;/*!< xyz size */

  /** Constructor
    */
  Space3D() : x(0),y(0),z(0),yz(0),xyz(0) { }
  /** Constructor
    */
  Space3D(long _x,long _y,long _z) : x(_x),y(_y),z(_z),yz(_y*_z),xyz(_x*_y*_z) {}
  /** Desstructor
    */
  ~Space3D() { }
  /** Returns the number of neighbour
    */
  long nb_neighbour() { return NBDIRECT; }
  /** initialize the number of subspace in each direction.
    * @param _x the number of voxels in x direction.
    * @param _y the number of voxels in y direction.
    * @param _z the number of voxels in z direction.
    */
  void init(long _x,long _y,long _z) { x=_x;y=_y;z=_z;yz=_y*_z;xyz=_x*_y*_z; }
  /** returns if the node identified with id has a neighbour in the direction d
    * @param id the identifier of the node.
    * @param d the direction to have the neighbour.
    * @return a boolean.
    */
  bool has_neighbour(long id,direct d) {
     switch(d) {
        case PREVX: return (id >= yz);
        case PREVY: return ((id%yz) >=z  );
        case PREVZ: return ((id%z) !=0);
        case NEXTX: return (id+yz)<xyz;
        case NEXTY: return (id%yz)+z<yz;
        case NEXTZ: return ((id+1)%z)!=0;
        default : std::cerr << "Error bad direction in 3D space :"<<d<<"\n";exit(1);
      }
      return false;
  }
  /** returns identifier of the neighbour of the node identified with id
    * @param id the identifier of the node.
    * @param d the direction to have the neighbour.
    * @return the neighbour identifier.
    */
  long neighbour(long id,direct d) {
     if ( !has_neighbour(id,d)) return -1;
     switch(d) {
        case PREVX: return id-yz;
        case PREVY: return id -z;
        case PREVZ: return id -1;
        case NEXTX: return id+yz;
        case NEXTY: return id+z;
        case NEXTZ: return id+1;
        default : std::cerr << "Error bad direction in 2D space :"<<d<<"\n";exit(1);
      }
      return -1;
  }
};

/** @}
  */

/** @defgroup SimSpCommGRP Communications in a Simulation Space
 *  @ingroup SimSpAlgoGRP
 *  @brief In the simulation Space algorithm the nodes need to communicate between them.
 * @{
 *  In the simulation Space algorithm the nodes need to communicate between them. 
 *  Then we have define a Message type and a link type.
 *  A link could not be construct just using a new instruction.
 *  To be able to execute your application in a parallel environment, the 
 *  link be created using the new_link method proposed by the environment 
 *  class.
 *  In a multi-threaded environment a link access is protectd using a mutual exclusion
 *  primitives, in a distributed environment, a link must use a 
 *  communication library like MPI.
 */

/*---------------------------------------------------------------------*/
/** Simple message 
  * The class represents a message that could store a Tag and a Data.
   * The types for the Tag and Data are templatized.
  */
template <class Data=void *,class Tag=long>
class Message {
   protected : 
    /** Tag of the message */
    Tag t;
    /** Data of the message */
    Data d;
   public: 
     /** type of the message tag
       */
     typedef Tag TheTag;
     /**  type of the Data stored by the message
       */
     typedef Data TheData;
     /** Constructor
       * @param _t the tag of the message
       * @param _d the Data of the message
       */
     Message(const Tag &_t, Data _d) : t(_t),d(_d) {}
     /** Constructor copy
       * @param m the message to copy
       */
     Message(const Message<Data,Tag> &m) : t(m.t),d(m.d) {}
     /** Destructor
       */
     virtual ~Message() {}
     /** get the associated Tag
       * return the tag value associated to the message.
       */
     Tag tag() const { return t; }
     /** get the associated Data
       * return the Data value associated to the message.
       */
     Data data() const { return d; }
};

/** A link allows to communicate between two objects
  */
template <class Mess>
class Link {
  protected:
   /** the list of messages
     */
   std::list<Mess> l;
  public:
   /** The type of message
     */
   typedef Mess TheMess;
   /** The type of the Tag'message
     */
   typedef typename Mess::TheTag Tag;
   /** The type of the message
     */
   typedef typename Mess::TheData Data;
   /** Constructor
     */
   Link() : l() {}
   /** Copy Constructor
     */
   Link(Link<Mess> &_l) : l(_l) {}
   /** Destructor
     */
   virtual ~Link() {}
   /** send a message in the link
     */
   virtual void push_message(const Mess &m) { 
     l.push_back(m);
   }
   /** get a message from the link
     */
   virtual Mess pop_message() { 
     Mess m=l.front();
     l.pop_front();
     return m;
   }
   /** get a Data from the link
     */
   virtual Data pop_data() { 
     Mess m=l.front();
     l.pop_front();
     return m.data();
   }
   /** test if the link has a tagged message
     */
   virtual bool has_message(Tag t) {
     if ( l.empty() ) { /*std::cout << "link empty\n"*/ ;return false; }
     if ( l.front().tag()==t ) { return true; }
     std::cout<< "bad tag wait:"<<t<<" has "<<l.front().tag()<<std::endl;
     return false;
   }
   /** test if the link is empty or not
     */
   virtual bool empty() { return l.empty(); }
   /** dump the content of a link
     */
   virtual void dump() {
        typename std::list<Mess>::iterator it;
        std::cout << "Dump Link :";
        for ( it=l.begin(); it!=l.end(); it++ ) {
            std::cout << "("<<(*it).tag()<<","<<((*it).data()==0 ?"null":"data")<<")";
        } 
        std::cout << std::endl;
   }
};

/** Collection of links that a node could have to connect him to each others.
  */
template<class Mess>
class LinkSet {

   public:
   /** the type of the link
     */
   typedef Link<Mess> TheLink;
   /** The type of the Tag'message
     */
   typedef typename Mess::TheTag Tag;
   /** The type of the message
     */
   typedef typename Mess::TheData Data;
   /** the type that stores the collection of link.
     */
   typedef std::vector<TheLink *> CollLink;

     /** Constructor
       */
     LinkSet() : setl() { }
     /** Constructor
       * @param nb the number of link of the collection
       */
     LinkSet(int nb) : setl(nb,(TheLink *)0) { }
     /** Copy Constructor
       */
     LinkSet(const LinkSet<Mess> &ls) : setl(ls.setl) { }
     /** Destructor
       */
     virtual ~LinkSet() { }
     /** Get the link at the index il.
       * @param il the index of the link
       * @return the link.
       */
     Link<Mess> *get(int il) { return setl[il]; }
     /** Set the link at the index il
       * @param il the index to store the link.
       * @param lm the link to store.
       */
     void set(int il, TheLink *lm) { setl[il]=lm; }
     /** Send a mesage by the link at the il index.
       * @param il the link index.
       * @param m the message to send
       */
     void send(int il, Mess &m) {  setl[il]->push_back(m); }
     /** Test if a link exists at the il index.
       * @param il the link index.
       * @return boolean.
       */
     bool has_link(int il) {
        return get(il)!=0;
     }
     /** Test if the given link has a message
     */
     bool has_message(int il) {  
        return setl[il]->has_message();
     }
     /** Test if the given link has a tagged message
     */
     bool has_message(int il,Tag t) const {
        return setl[il]->has_message(t);
     }
     /** Count the number of links that stores a tagged message
     */
     int all_count_message(Tag t) const {
        int nb=0;
        for ( int i=0 ; i<setl.size() ; i++ ) {
           if ( setl[i]==0 || setl[i]->has_message(t) ) nb++;
        }
        return nb;
     }

     /** Test if all links have a tagged message
     */
     bool all_has_message(Tag t) const {
        for ( int i=0 ; i<setl.size() ; i++ ) {
           if ( setl[i]!=0 && !setl[i]->has_message(t) ) return false;
        }
        return  true;
     }
     /** dump the set link
     */
     void dump() {
        for (int i=0;i<6;i++) {
           if ( has_link(i)  )  {
              get(i)->dump();
           } else {
              std::cout << i <<": no link, no dump\n";
           }
        }
     }
  protected:
   /** the collection of link
     */
   CollLink setl;

};

/**@}
  */


/** The SimSpd Programming environment
 * This class adds to the classical EnvProg, the possibility to share a collection of links.
 */
template<class Trait>
class SimSpEnvProg {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::Link TheLink;
  typedef typename Trait::PriComp ThePriComp;
  typedef typename Trait::Key TheKey;

public:
  /// Constructor
  SimSpEnvProg():vl(10) { }
  /// Denstructor
  virtual ~SimSpEnvProg() {}
  /** Get a new link
    * @param key the key associated to the link.
    * @return the new link.
    */
  virtual TheLink *new_link(const TheKey &key) { 
      long kl = key.to_long();
      if ( kl>vl.size() ) vl.resize(kl+100,(TheLink *)0);
      if ( vl[kl]==0 ) {
        vl[kl] = new_env_link(key);
      }
      return vl[kl]; 
  }

protected:
  /** get a new link according to the environment
    * @param key the key associated to the link.
    * @return the new link.
    */
  virtual TheLink *new_env_link(const TheKey &key)=0;
  /** vector that stores all the links
    */
  std::vector<TheLink *> vl;
};


/** The class that defines a Node
 * This node inherits from the base_node class, the PQNode class and the Key class.
 */
template<class Key>
class SimSpNode : public base_Node, public PQNode<SimSpNode<Key> >,public Key  {
protected:
public :
  /// Type of the enclosed PQNode.
  typedef PQNode<SimSpNode<Key> > ThePQNode;
  /// Constructor
  SimSpNode() : base_Node(),Key() {
    DBGAFF_MEM("SimSpNode::SimSpNode", "Constructor (no parameter)");
  }
  /** Constructor
    * @param sn a SimSpNode to copy
    */
  SimSpNode(SimSpNode<Key> const *sn): base_Node((base_Node *)sn),ThePQNode(),Key((Key *)sn) {
    DBGAFF_MEM("SimSpNode::SimSpNode", "Constructor (para Node *)");
  }
  /** Constructor
    * @param sn a SimSpNode to copy
    */
  SimSpNode(SimSpNode<Key> const &sn): base_Node((base_Node &)sn),ThePQNode(), Key(sn) {
    DBGAFF_MEM("SimSpNode::SimSpNode", "Constructor (para Node &)");
  }
  /// Destructor
  virtual ~SimSpNode() {
    DBGAFF_MEM("SimSpNode::~SimSpNode", "Destructor");
  }
  /// copy method used by the operator=
  virtual void copy(const SimSpNode<Key> &sn) {
    base_Node::copy(sn);
    Key::copy(sn);
  }
  /// Print the node information
  virtual ostream &Prt(ostream &os) const {
    base_Node::Prt(os);
    return os;
  }
  /** Fill the strbuff with node information
  * @param st the strbuff to fill.
  */
  virtual void log_space(strbuff<> &st) {
    base_Node::log_space(st);
  }
  /** Fill the strbuff with node information
  * @param st the strbuff to fill.
  */
  virtual void lab_log_space(strbuff<> &st) {
    base_Node::log_space(st);
  }

  /** Pack method to Serialize the SimSpNode
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs)  const {
    DBGAFF_PCK("SimSpNode::Pack", "Packing Node");
    base_Node::Pack(bs);
    Key::Pack(bs);
  }
  /** Unpack method to deserialize the SimSpNode
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SimSpNode::UnPack", "Unpacking Node");
    base_Node::UnPack(bs);
    Key::UnPack(bs);
  }
};


/** The SimSpInstance only adds a method to obtain the root node of the search space.
 * The SimSpInstance class for a scheduled search inherits from the basic Instance.
 * the functionality that has been added is the SimSpInstance::GetRoot() method
 * to obtain the root node of the space search.
 *
  */
template <class Trait>
class SimSpInstance : public base_Instance {
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
  SimSpInstance(std::string &_n) : base_Instance(_n) {
    DBGAFF_MEM("SimSpInstance::SimSpInstance", "Constructor (para string)");
  }
  /** Constructor
    * @param _n a char * for the identifier
    */
  SimSpInstance(const char *_n) : base_Instance(_n) {
    DBGAFF_MEM("SimSpInstance::SimSpInstance", "Constructor (para const char *)");
  }
  /** Constructor
    * @param ia a SimSpInstance to copy
    */
  SimSpInstance(SimSpInstance &ia) : base_Instance(ia) {
    DBGAFF_MEM("SimSpInstance::SimSpInstance", "Constructor (para Instance &)");
  }
  /** Constructor
    * @param ia a SimSpInstance to copy
    */
  SimSpInstance(const SimSpInstance &ia) : base_Instance(ia) {
    DBGAFF_MEM("SimSpInstance::SimSpInstance", "Constructor (para const Instance &)");
  }
  /// Destructor
  virtual ~SimSpInstance()  {
    DBGAFF_MEM("SimSpInstance::~SimSpInstance", "Destructor");
  }
  /** copy method used by the operator=
    */
  virtual void copy(const SimSpInstance &si) {
    base_Instance::copy(si);
  }
  /** Operator= for the instance.
    * this operator call the virtual copy method
    */
  const TheInstance &operator=(const TheInstance &ti) {
    copy(ti);
  }

  /** Initialize the Instance.
    */
  virtual void Init(SimSpEnvProg<Trait> *) = 0;
  /** Initialize the search for the algorithm _al.
    * @param _al the algorithm to initialize
    */
  virtual void InitAlgo(TheAlgo *_al) const = 0;
  /** Pack method to serialize the SimSpInstance
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const  {
    DBGAFF_PCK("SimSpInstance::Pack", "Packing Instance");
    base_Instance::Pack(bs);
  }
  /** Unpack method to deserialize the SimSpInstance
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SimSpInstance::UnPack", "Unpacking Instance");
    base_Instance::UnPack(bs);
  }
};


/** The GenChild class Generation of the child nodes
 */
template <class Trait>
class SimSpGenChild : public base_GenChild {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
protected:
  /// the algorithm used by the GenChild.
  TheAlgo *algo;
  /// the associated Instance.
  const TheInstance *inst;
public:
  /** Constructor
  * @param _inst The instance to solve
  * @param _al The associated algorithm
  */
  SimSpGenChild(const TheInstance *_inst, TheAlgo*_al): base_GenChild(),
      algo(_al), inst(_inst) {
    DBGAFF_MEM("SimSpGenChild::SimSpGenChild", "Constructor (Instance *,Algo *)");
  }
  /// Destructor
  virtual ~SimSpGenChild() {
    DBGAFF_MEM("SimSpGenChild::~SimSpGenChild", "Destructor");
  }
  /** operator() which generate the new child nodes and call
  * the search(nn) method on each of them.
  * @param n the node to explore
  */
  virtual void operator()(TheNode *n) = 0;
};


/** The Goal Stat
 */
class SimSpGoalStat : public stat {
public:
  /// Constructor 
  SimSpGoalStat() : stat('G',CId('?'),"Goal Stat",false) {}
  /// Constructor
  SimSpGoalStat(const Id &id,bool l=false): stat('G',id,"Goal Stat",l) {
    add_counter('s', "Generated Solution",0);
    add_counter('u', "Updated Solution",1);
    add_counter('m', "Merge Goal",0);
  }
  /// Destructor
  virtual ~SimSpGoalStat() {}
  /// counts the number of calls to the update method
  virtual void sol(long l = 0) {
    get_counter('s')++;
    //get_counter("l")+=l;
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


/** The abstract Goal for a SimSpAlgo
 *  The class is reimplemented for the concrete algorithm in order to control
 *  the search.
 *  This class could have different implementation according to the aim of
 *  the search. For example for a Branch and Bound, one want to have the
 *  optimal solution, to count the number of optimal solution, to have
 *  the n best soltuions, etc...
 *  The goal of the search must be defined in the trait class for a
 *  specific problem code.
 *  The main methods are
 *    - update(Node *n) that tests if a node (that must be a solution) is
 *      a candidate to update the best solution known.
 *    - final_merge(Goal *g) performs the final merge.
 *      This method is used at the end of the resoltuion.
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
class SimSpGoal {
public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of thr Key
  typedef typename Trait::Key TheKey;
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
  SimSpGoal(bool l=false): st(),verb(false),nbn(0),tab(0) {
  }
  /** Constructor
    */
  SimSpGoal(const Id &id,bool l=false): st(id,l),verb(false),nbn(0),tab(0) {
  }
  /// Destructor
  virtual ~SimSpGoal() {
     if ( tab==0 ) return;
     for (int i=0;i<nbn;i++) {
        if ( tab[i]!=0 ) {
          tab[i]->remRef();
          if ( tab[i]->isDel() ) { delete tab[i];}
          tab[i]=0;
        }
     }
     delete[] tab;
  }
  /** Init method
    * @param ti an Instance object
    */
  virtual void Init(TheInstance *ti) { 
     nbn=ti->get_nb_states();
     tab = new TheNode *[nbn];
     for (int i=0;i<nbn;i++) {
        tab[i]=0;
     }
  }
  /** getStat method
    */
  SimSpGoalStat *getStat() {
    return &st;
  }
  /** write the stat header on the log
    */
  virtual void log_header() { getStat()->log_header(); }
  /** Update method is called to update the state space with a node
    * @todo simple assignment according to the Key.
    */
  virtual bool update(TheNode *n) {
    n->addRef();
    tab[n->to_long()]=n;
    return true;
  }
  /** is4Search method
    * @param _n the node to test
    */
  virtual bool is4Search(TheNode *_n) const {
    return true;
  }
  /** Get the node at the key k
    * @param k the node's key to get
    */
  virtual TheNode *get(const TheKey &k) { return tab[k.to_long()]; }
  /** Releases the node at the key k
    * @param k the node's key to release
    */
  virtual void release(const TheKey &k) {  }
  /** the current merging method is used to update the solution
    * with another one, during the execution of algorithms.
    * this method is used in distributed environment.
    * @param _tg the Goal to merge
    */
  virtual bool current_merge(const TheGoal *_tg) {
    std::cout << "Warning : Call SimSpGoal current_merge() do nothing\n";
    return true;
  }
  /** the final merging method is used to update the solution
    * with another one, but at the end of the execution of the algorithm.
    * this method is used in distributed environment.
    * @param _tg the Goal to merge
    */
  virtual bool final_merge(const TheGoal *_tg) {
    std::cout << "Warning : Call SimSpGoal final_merge() do nothing\n";
    return true;
  }
  /** operator= for the Goal.
    * @param _tg the Goal to assign
    */
  TheGoal &operator=(TheGoal &_tg) {
    final_merge(_tg);
    return this;
  }
  /** returns the verbosity of the goal
    * @param _v the boolean to set
    */
  virtual void set_verbosity(bool _v=true) { verb=_v; }
  /** returns the verbosity of the goal
    */
  virtual bool verbosity() { return verb; }
  /// Prt Method display the Solution data structure
  virtual ostream &Prt(ostream &os) const {
    os << "-------------";
    return os;
  }
  /** Pack method to Serialize the SimSpNode
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("SimSpGoal::Pack", "Packing Node");
  }
  /** Unpack method to deserialize the SimSpNode
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("SimSpGoal::UnPack", "Unpacking Node");
  }

protected:
  /** the Statistics of the Goal
    */
  SimSpGoalStat st;
  /** boolean to know if a solution must be displayed at each update
    */
  bool verb;
  /** Number of nodes
    */
  int nbn;
  /** array of nodes
    */
  TheNode **tab;
};

/**
 * The SimSpStatAlgo represents the statistics for the SimSpAlgo
 * it inherits from base_stat.
 */
class SimSpStat : public base_Stat {
public:
  /** Constructor
   * @param _id the parent identifier
   */
  SimSpStat(const Id &_id) : base_Stat('I', _id) {
    add_counter('i', "Inserted Nodes");
    add_timer('C', "Genchild Calls");
    add_timer('D', "PQ del Calls");
  }
  /// destructor
  virtual ~SimSpStat() {
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

  /** Starts the timer of a genchild.
    * @param bn the node used to call the genchild
    */
  void start_gc(base_Node *bn) {
    get_timer('C').start(bn->id());
  }
  /** Ends the timer of a genchild.
    * @param bn the node.
    */
  void end_gc(base_Node *bn) {
    get_timer('C').end(bn->id());
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


/** The SimSpAlgo class
  *
  * This algo is proposed to simulate entity on a space. The entities to simulate are encapsulated
  * on a node. The node are iteratively given to the GenChild which perform the simulation for one step.
  *
  * The scheduling could be defined according to a priority (@ref PriGRP),
  * the class has parametrized type called PriAllocator that has a methods GetPri(Node *)
  * to obtain a Pri object associated with the Node given by parameter.
  *
  */
template <class Trait>
class SimSpAlgo : public base_Algo {
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
  /// the current parent node
  TheNode *parent;
public:
  /** Constructor
   * The Genchild used by this algorithm is internaly allocated.
   * @param bsia the instance to solve
   * @param _pq the Priority queue
   * @param tg the Goal of the Search
   * @param st the statistics object
   */
  SimSpAlgo(const TheInstance *bsia, ThePQ *_pq, TheGoal *tg, TheStat *st) :
      base_Algo(st), ia(bsia), gpq(_pq), goal(tg),
      gc(new TheGenChild(ia, (TheAlgo *)this)), parent(0) {
    BOBASSERT(gpq != 0, NullPointerException, "SimSpAlgo", "GPQ is null");
    DBGAFF_MEM("SimSpAlgo::SimSpAlgo", "Constructor (Instance *,PQ *,Stat *)");
  }
  /** Destructor
    * The Genchild is deleted
    */
  virtual ~SimSpAlgo() {
    delete gc;
    gc = 0;
    DBGAFF_MEM("SimSpAlgo::~SimSpAlgo", "Destructor");
  }
  /// the run method that performs the main loop.
  virtual void operator()() {
    TheNode *n;
    BOBASSERT(base_Algo::isEtInit() || base_Algo::isEtResumed(), SimSpException,
              "SimSpAlgo::Operator()", " This object has not been initialized ");
    base_Algo::StStart();
    base_Algo::toRun();
    GPQ()->addRef();
    DBGAFF_ALGO("SimSpAlgo::operator()", "Before the main loop");
    while (base_Algo::isEtRun() && (n = GPQGet()) != 0) {
      DBGAFF_ALGO("SimSpAlgo::operator()",
                  "We have a node and the state of the algo is ok");
      parent = n;
      getStat()->start_gc(n);
      DBGAFF_ALGO("SimSpAlgo::operator()", "Call the genchild");
      (*gc)(n);
      getStat()->end_gc(n);
    }
    if ( !base_Algo::isEtRun() ) {
      std::cout << "Algorithm has been stopped\n";
    } else base_Algo::toEnd();
    GPQ()->remRef();
    base_Algo::StEnd();
    DBGAFF_ALGO("SimSpAlgo::operator()", "ends !");
  }
  /// Ends the algorithm
  virtual void End() {
    DBGAFF_ALGO("SimSpAlgo::end()", "is called");
  }
  /** The Search method
    * This method is called by the GenChild::operator(), for each new
    * generated nodes that are suitable to be explorated.
    * The actual code is simply an insertion of the Node in the GPQ.
    * This methods could be redefined for a specific algorithm.
    * @param n the node to search
    */
  virtual bool Search(TheNode *n) {
    DBGAFF_ALGO("SimSpAlgo::Search()", "Insert the node");
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
  //@}
};




//@}
};
#endif

