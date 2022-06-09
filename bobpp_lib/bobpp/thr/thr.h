
#ifndef BOB_THRENVPROG
#define BOB_THRENVPROG

#include<pthread.h>
#ifndef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN  16384
#endif

#include<bobpp/bobpp>

namespace Bob {

/**
 *  @defgroup ThrEnvProgGRP The Multi-threaded programming environment
 *  @brief The multi threaded environment for the Bob++ algorithms
 *  @ingroup EnvProgGRP
 *  @{
 *  \page TEPpage The Multi-threaded programming Environment
 *
 * \section Using
 *
 * The Bob::ThrEnvProg is the static class to spawn the threads.
 * The number of threaded spawned are given in the command line
 * using the --thr -n option.
 *
 * the following code shows how to solve an instance of a problem
 * using the Branch and Bound Method.
 * \code
    int main(int n, char ** v) {
        // Define the Threaded environment wrapper
        Bob::ThrBBAlgoEnvProg<MyTrait> env;
        // Initialize the threaded environment
        Bob::ThrEnvProg::init();
        // The initialization of other part of Bob++
        Bob::core::Config(n,v);
        // Start the environment
        Bob::ThrEnvProg::start();

        // Initialize the instance.
        MyInstance *Instance=new MyInstance();

        // Solve the instance i.e. all the tree
        env(Instance);

        // Terminate the environment
        Bob::ThrEnvProg::end();
        // Terminate Bob++
        Bob::core::End();
        // Delete the allocated instance
        delete Instance;
}

 * \endcode
 *
 */

/** 
  * @defgroup ThrEnvProgRoutinesGRP Synchronization routines
  * @brief Synchronization routines presented here, are the Mutex, Condition variable, and a barrier.
  * 
  * The Bobpp multi-threaded environment is based on the POSIX threads implementation.
  * We propose here a simple object oriented implementation of the classical synchronization routines.
  * @{
  */
//#define USE_TIME_MUTEX

/** macro used to log the time spent in the mutex.
  * if you define USE_TIME_MUTEX, each mutex used in the thr implementation
  * uses a stat_timer to display the time spent in the mutex at the end of 
  * the execution.
  */ 
#ifdef USE_TIME_MUTEX
/// declaration of the stat_timer
#define UTM_DECL stat_timer tm;
/// initialisation of the stat_timer
#define UTM_INIT :tm()
/// begin the stat_timer
#define UTM_START tm.start()
/// end the stat_timer
#define UTM_END tm.end()
/// display the stat_timer
#define UTM_DISP os<<tm<<"\n";
#else
/// declaration of the stat_timer
#define UTM_DECL 
/// initialisation of the stat_timer
#define UTM_INIT 
/// begin the stat_timer
#define UTM_START 
/// end the stat_timer
#define UTM_END 
/// display the stat_timer
#define UTM_DISP
#endif

/** Class that represents a thread mutex
 * This class can also logs the time lost to lock the mutex.
 */
struct ThrMutex {
  /// The POSIX mutex
  pthread_mutex_t mut;
  UTM_DECL
public:
  /// Constructor
  ThrMutex() UTM_INIT {
    pthread_mutex_init(&mut, 0);
  };
  /// destructor
  virtual ~ThrMutex() {
    pthread_mutex_destroy(&mut);
  };
  /// Method to lock the mutex
  virtual void lock() {
    UTM_START;
    pthread_mutex_lock(&mut);
    UTM_END;
  }
  /// Method to unlock the mutex
  virtual void unlock() {
    pthread_mutex_unlock(&mut);
  }
  /// Method to display the lock waiting time.
  virtual void display(std::ostream &os) const {
    UTM_DISP;
  }
};

/** class to represent a condition variable
 * As a pthread condition varaible must be used with a mutex, 
 * this class inherits from the ThrMutex.
 */
struct ThrCond : public ThrMutex {
  /// the Posix condition variable.
  pthread_cond_t cond;
public:
  /// Constructor
  ThrCond():ThrMutex() {
    pthread_cond_init(&cond, 0);
  }
  /// destructor
  virtual ~ThrCond() {
    pthread_cond_destroy(&cond);
  }
  /// reset
  void reset() {
    pthread_cond_init(&cond, 0);
  }
  /// Bordcast method
  void bcast() {
    pthread_cond_broadcast(&cond);
  }
  /// wait method
  void wait() {
    UTM_START;
    pthread_cond_wait(&cond, &mut);
    UTM_END;
  }
  /// Signal method
  void signal() {
    pthread_cond_signal(&cond);
  }
};


/** Class to implement a barrier between the Threads
 *
 */
struct ThrBarrier {
  /// The POSIX condition variable
  ThrCond cond;
  /// the number of thread for the barrier
  volatile int n;
  /// the number of thread that currently wait for the barrier
  volatile int nbw;
public:
  /// Constructor
  ThrBarrier(int _n = 0): cond(),n(_n), nbw(0) {
  }
  /// Destructor
  virtual ~ThrBarrier() {
  }
  /// Set the number of Thread to wait.
  virtual void setNb(int _n) {
    n = _n;
    nbw=0;
    cond.reset();
  }
  /// Method called by the different thread to wait, the last one will broadcast to everyone.
  virtual void Wait() {
    cond.lock();
    nbw++;
    if (n <= nbw) {
      cond.bcast();
      cond.unlock();
    } else {
      cond.wait();
      cond.unlock();
    }
  }
};
 /** @}
  */

/** Class that represents a thread
  * An object of this class is associated to each thread that is enroled in a bobpp solver.
  * The main method executed by the thread is the loop method.
  * One can give a new algorithm to be executed to the thread executing the run methods.
  */
struct Thread {
  /// Id of the thread
  pthread_t id;
  /// The condition of the thread (use to wait work)
  ThrCond cond;
  /// pointer on a Algorithm
  base_Algo *a;
  /// the barrier to synchronize the thread after an execution on an algo.
  ThrBarrier *bar;
  /// the attribut to modify the stack size
  pthread_attr_t attr;
  /// enumeration for the state of a thread
  enum StateThr {
    ThWait, /*!< the thread wait for works */
    ThHasWork, /*!< the thread has work but have not begin to process it. */
    ThWork, /*!< the thread is working */
    ThStop/*!< the thread is stopped */
  };
  /// the State of a thread
  StateThr state;
public:
  /// Constructor
  Thread();
  /// Destructor
  virtual ~Thread() { }
  /// Thread initialization method
  virtual void init();
  /// The main loop
  virtual void loop();
  /// Method to run an algorithm, the algorithm must be ready
  void run(base_Algo *al, ThrBarrier *thb) {
    cond.lock();
    DBGAFF_ENV("Thread::run()","Give a new Algo to run\n");
    if (state != ThWait) {
      cond.unlock();
    }
    a = al;
    bar = thb;
    state = ThHasWork;
    DBGAFF_ENV("Thread::run()","Wake up the thread to run the given algorithm\n");
    cond.bcast();
    cond.unlock();
  }
  /// Method to stop the thread
  void stop() {
    cond.lock();
    DBGAFF_ENV("Thread::stop()","Stop the thread\n");
    state = ThStop;
    cond.bcast();
    cond.unlock();
  }
  /// Methods to get the pthread_id of a the thread.
  pthread_t thid() {
    return id;
  }
};

/** Class to have the Bob identifier of a thread...
 */
class ThrId: public EnvId {
  strbuff<10> num;
public:
  /// Constructor
  ThrId(): EnvId(), num() {}
  /// Destructor
  virtual ~ThrId() { }
  /// the id() method
  virtual long id();
  /// the id() method
  virtual long nb();
};

/** The threaded Environment static class
 * The threaded environment need a static class.
 * This class is used to store the threaded specific parameters (i.e. --thr). 
 * It creates the POSIX threads and stored each of the associated Thread objects
 * thr run method allows to give a ready bobpp algorithm to be executed by one thread.
 */
class ThrEnvProg {
protected:
  /// Array of threads
  static Thread *tt;
  /// Number of threads
  static int nbth;
  /// Do we need a comm thread ?
  static int comm_th;
  /// Number of priority queues
  static int nbpq;
  /// Number of priority queues
  static int cpuset_sz;
  /// Boolean to know if the instance is duplicated in each thread/algo or not.
  static bool multi_inst;
public:
  /// Constructor
  ThrEnvProg() { }
  /// the init method of the environment
  static void init(int _cth=0) {
    std::cout << "Init du ThrEnvProg\n";
    core::opt().add(std::string("--thr"), Property("-n", "Number of thread used", sysconf(_SC_NPROCESSORS_ONLN), &nbth));
    core::opt().add(std::string("--thr"), Property("-d", "Number of priority queues", 1, &nbpq));
    core::opt().add(std::string("--thr"), Property("-t", "used one instance per algorithm", &multi_inst));
#ifdef BOBPP_HAVE_PTHREAD_SETAFFINITY_NP
    core::opt().add(std::string("--thr"), Property("-c", "cpuset size (0=compute)",0, &cpuset_sz));
#endif
    core::opt().add(std::string("--thr"), Property("-s", "the Stack size used by each thread", (int)(PTHREAD_STACK_MIN*10)));
    comm_th = _cth;
  }
  /// The start method of the environment, the real threads are created
  static void start() {
    core::log().setEId(new ThrId());
    //std::cout << "In start:" << nbth << "\n";
    if (nbth == 0) {
      std::cerr << "Number of thread 0\n";
      exit(1);
    }
    tt = new Thread[nbth];
    for (int i = 0; i < nbth; i++)
      tt[i].init();
  }
  /// The stop method of the environment, the real threads are stopped
  static void end() {
    for (int i = 0 ; i < nbth; i++) {
      tt[i].stop();
      pthread_join(tt[i].id, NULL);
    }
    delete[] tt;
  }
  /// Search the rank of a thread.
  static int rank() {
    int i;
    if (tt == 0) return 0;
    for (i = 0; i < n_algo_thread(); i++) {
      if (pthread_equal(tt[i].thid(), pthread_self())) {
        return i;
      }
    }
    return 0;
  }
  /// get the value of the option multi_inst.
  static bool instance_multi() {
    return multi_inst;
  }
  /// get the default stack size
  static size_t stack_size() {
    return (size_t)core::opt().NVal("--thr", "-s");
  }
  /// Method that returns the number of priority queues
  static int npq() {
    return nbpq;
  }
  /// Method that returns the size of a cpuset
  static int cpuset_size() {
    return (cpuset_sz==0 ? nbth/nbpq : cpuset_sz);
  }
  /** Method that returns the total number of threads
    * @return the total number of running threads : search threads+ communication thread
    */
  static int n_thread() {
    return nbth+1;
  }

  /** Method that returns the number of search threads
    * @return the number of thread that execute the search algorithm
    */
  static int n_algo_thread() {
    return nbth;
  }
  /** Method to give an Algo to a thread
   * @param i the indice of the thread
   * @param a an algo to compute.
   * @param thb the barrier to synchronize all the thread at the end of the execution of the algorithm.
   */
  static void run(int i, base_Algo *a, ThrBarrier *thb) {
    tt[i].run(a, thb);
  }
};

  /** 
  * @defgroup ThrEnvProgDSGRP The threaded Bobpp data structures
  * @brief this module stores the Goal et GPQ and the Link class modified to be used in a threaded environment.
  * 
  * Each of the Bobpp data structures used in a threaded environment must be accessed using mutual exclusion routines.
  * Then each of the object must stores a ThrMutex object in order to prevent concurrent accesses.
  * @{
  */


/** The Goal data structure for the Threaded environment
 * Mainly this class is exactly the same as the sequential one, except that each method use the 
 * internal ThrMutex to avoid inconsistency.
 */
template<class Trait>
class ThrGoal: public Trait::Goal {
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
  /// the mutex used to insure access to the real data in mutual exclusion mode.
  ThrMutex mut;
public:
  /** Constructor
    * @param b a boolean, if true the stat of the goal are logged.
    */
  ThrGoal(bool b) : TheGoal(b),mut() {
     //printf("Alloc THR Goal\n");
  }
  /** Constructor
    * @param id the algorithm identifier
    * @param b a boolean, if true the stat of the goal are logged.
    */
  ThrGoal(const Id & id,bool b) : TheGoal(id,b),mut() {
     //printf("Alloc THR Goal id b\n");
  }
  /** Constructor
    * @param tg the source goal
    */
  ThrGoal(ThrGoal &tg): Trait::Goal(tg),mut() {
  }
  /** Destructor
    */
  virtual ~ThrGoal() {
  }
  /** Intialize the Goal.
    * @param i Instance used to initialize the Goal
    */
  virtual void Init(TheInstance *i) {
    //printf("Init THR Goal id b\n");
    mut.lock();
    TheGoal::Init(i);
    mut.unlock();
  }
  /** Method to test if the node is suitable to be explore later.
    * @param n the node to test
    */
  virtual bool is4Search(TheNode *n) {
    bool b;
    mut.lock();
    b = TheGoal::is4Search(n);
    mut.unlock();
    return b;
  }
  /** Method called when a node is a solution
    * @param n the solution node
    * @param u flag to display the solution if the solution is updated.
    */
  virtual bool update(TheNode *n, int u=0) {
    bool b;
    mut.lock();
    b = TheGoal::update(n,u);
    mut.unlock();
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool current_merge(const TheGoal *g) {
    bool b;

    mut.lock();
    b = TheGoal::current_merge(g);
    mut.unlock();
    return b;
  }
  /** The merging method is used to update the goal with another one.
    * this method is not really used in this environmment.
    * @param g TheGoal used to updated the Goal.
    */
  virtual bool final_merge(const TheGoal *g) {
    bool b;
    mut.lock();
    b = TheGoal::final_merge(g);
    mut.unlock();
    return b;
  }
  /** Method to display the Goal.
    * @param os the std::ostream used to display.
    */
  virtual std::ostream &Prt(std::ostream &os) const {
    TheGoal::Prt(os);
    mut.display(os);
    return os;
  };

};

/** class that represents a Link in threaded environment
  * A link is the class to be able to communicate data. 
  * At this time links are used for the Simulation space algorithm, to allow the nodes to
  * communicate data.
  * The data must be encapsulate in Message object. The Message type must be 
  * defined in the Link template parameter.
  * This Threaded implementation used a ThrMutex in order to avoid inconsistency.
  * In this implementation, a Message is tagged.
  */
template<class Link>
class ThrLink : public Link {
protected:
  /// the mutex used to insure access to the real data in mutual exclusion mode.
  ThrMutex mut;
 public:
   /// Type of the message sent using the link.
   typedef typename Link::TheMess Mess;
   /// Type of tag of the message sent using the link.
   typedef typename Mess::TheTag Tag;
   /// Type of data of the message sent using the link.
   typedef typename Mess::TheData Data;
   /** Constructor
     */
   ThrLink() : Link(),mut() {}
   /** Copy Constructor
     * @param _l the link to copy
     */
   ThrLink(ThrLink<Link> &_l) : Link(_l),mut() {}
   /** Destructor
     */
   virtual ~ThrLink() {}
   /** send a message in the link
     * @param m the message to push
     */
   virtual void push_message(const Mess &m) { 
     mut.lock();
     Link::push_message(m);
     mut.unlock();
   }
   /** get a message from the link
     * @return the message.
     */
   virtual Mess pop_message() { 
     mut.lock();
     Mess m=Link::pop_message();
     mut.unlock();
     return m;
   }
   /** get a Data from the link
     * @return the Data associated with the message.
     */
   virtual Data pop_data() { 
     mut.lock();
     Data d= Link::pop_data();
     mut.unlock();
     return d;
   }
   /** test if the link has a tagged message
     * @param t the required tag 
     */
   virtual bool has_message(Tag t) {
     mut.lock();
     bool r=Link::has_message(t);
     mut.unlock();
     return r;
   }
   /** test if the link is empty or not
     */
   virtual bool empty() { 
     mut.lock();
     bool r=Link::empty(); 
     mut.unlock();
     return r;
   }
   /** dump the content of a link
     */
   virtual void dump() {
     mut.lock();
     Link::dump(); 
     mut.unlock();
   }

};

/** Class to represent the priority queue use by several threads
 *
 * This class uses the Bob::ThrMutex for the access to the
 * priority queue that stores the pending nodes.
 *
 * This class is reponsable of the terminaison of the algorithm
 * if the priority queue is empty. A thread must have to wait, 
 * until another thread inserts a new node.
 * The terminaison arrives if all threads are waiting.
 */
template<class Node, class PriComp,class Goal,class TheSPQ>
class ThrPQ : public PQInterface<Node, PriComp,Goal> {
  ThrCond cond; // the condition variable
  int Wth;             // number of waiting threads
  int Need_NodeforLB;   // flag to manage the load balancing when the ThrPQ is use in distributed environment
  std::vector<TheSPQ *>pq;         // the local priority queue.
  std::vector<ThrMutex> mut; // the associated mutex.
public:

  /// Constructor
  ThrPQ(const Id &id,bool l) : PQInterface<Node, PriComp,Goal>(),cond(),Wth(0),Need_NodeforLB(0),
             pq(ThrEnvProg::npq()), mut(ThrEnvProg::npq()) {
    for (int i=0;i<ThrEnvProg::npq(); i++ ) {
      pq[i]=0;
    }
    //pq[0] = pqp;
    //pq[0]->Init(NId(0,id),true);
    for (int i=0;i<ThrEnvProg::npq(); i++ ) {
      pq[i]=new TheSPQ(NId(i,id),true);
    }
    //nb_node();
  }
  /// Destructor
  virtual ~ThrPQ() {
    for (int i=0;i<ThrEnvProg::npq(); i++ ) {
      if( pq[i]!=0 ) { 
        delete pq[i];
      }
    }
  }
  ///Reset method
  virtual void Reset() {
    for (int i=0;i<ThrEnvProg::npq(); i++ ) {
       pq[i]->Reset();
    }
  }
  /// Remove a reference on the Priority Queue
  virtual void remRef() {
    cond.lock();
    PQInterface<Node, PriComp,Goal>::remRef();
    cond.unlock();
  }
  /// Add a reference on the Priority Queue
  virtual void addRef() {
    cond.lock();
    PQInterface<Node, PriComp,Goal>::addRef();
    cond.unlock();
  }
  /// get the index of the priority queue associated with the thread
  int getiPQ(int d=0) { 
     if ( ThrEnvProg::npq()==1 && d!=0 ) return -1; 
     if ( ThrEnvProg::npq()==2 && d==1 ) return -1; 
     int ip = ((ThrEnvProg::rank()*ThrEnvProg::npq()/ThrEnvProg::n_algo_thread())+d+ThrEnvProg::npq())%ThrEnvProg::npq();
     //std::cout << "r:"<<ThrEnvProg::rank()<<" ip:"<<ip<<" npq:"<<ThrEnvProg::npq()<<"\n";
     return ip;
  }
  /// Insertion
  virtual void Ins(Node *n) {
    int ip=getiPQ();
    mut[ip].lock();
    pq[ip]->Ins(n);
    mut[ip].unlock();
    cond.lock();
    if (Wth > 0) {
      cond.bcast();
    } 
    cond.unlock();
  }
  /// Delete greater operation.
  virtual int DelG(Goal &g) {
    int nb=0;
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      mut[i].lock();
      nb += pq[i]->DelG(g);
      mut[i].unlock();
    }
    return nb;
  }
  /** Method that tries to delete a node from one of the internal priority queues.
   * @return the deleted node
   */
  virtual Node *SearchDel() {
    Node *n;
    //int r = ThrEnvProg::rank();
    int ip=getiPQ();
    mut[ip].lock();
    n = pq[ip]->Del();
    if ( n!=0 ) {
      mut[ip].unlock();
      return n;
    }
    //std::cout <<"t:"<<r<<" nope for 0:"<<ip<<std::endl;
    mut[ip].unlock();
    ip=getiPQ(-1);
    if ( ip!=-1 ) {
      mut[ip].lock();
      n = pq[ip]->DelLB();
      if ( n!=0 ) {
        mut[ip].unlock();
        return n;
      }
      mut[ip].unlock();
    }
    ip=getiPQ(1);
    if ( ip!=-1 ) {
      mut[ip].lock();
      n = pq[ip]->Del();
      if ( n!=0 ) {
        mut[ip].unlock();
        return n;
      }
      //std::cout <<"t:"<<r<<" nope for +1:"<<ip<<std::endl;
      mut[ip].unlock();
    }
    return 0;
  }
  /** Delete the best node for load balancing
    * To modify, since this method actually deletes only nodes from 2 or 3 internal priority queue.
    */
  virtual Node *DelLB() {
    Node *n;
    //int r = ThrEnvProg::rank();
    int ip=getiPQ();
    mut[ip].lock();
    n = pq[ip]->DelLB();
    if ( n!=0 ) {
      mut[ip].unlock();
      return n;
    }
    //std::cout <<"t:"<<r<<" nope for 0:"<<ip<<std::endl;
    mut[ip].unlock();
    ip=getiPQ(-1);
    if ( ip!=-1 ) {
      mut[ip].lock();
      n = pq[ip]->DelLB();
      if ( n!=0 ) {
        mut[ip].unlock();
        return n;
      }
      mut[ip].unlock();
    }
    ip=getiPQ(1);
    if ( ip!=-1 ) {
      mut[ip].lock();
      n = pq[ip]->DelLB();
      if ( n!=0 ) {
        mut[ip].unlock();
        return n;
      }
      //std::cout <<"t:"<<r<<" nope for +1:"<<ip<<std::endl;
      mut[ip].unlock();
    }
    return 0;
  }

  /// Delete the best node
  virtual Node *Del() {
    Node *n;
    while ((n = SearchDel()) == 0) {
      cond.lock();
      Wth++;
      if (Wth >= PQInterface<Node, PriComp,Goal>::ref) {
        Wth = 100000;
        cond.bcast();
        cond.unlock();
        //printf(" All Threads are waiting \n");
        return 0;
      }
      if (Wth == PQInterface<Node, PriComp,Goal>::ref-1) {
        //printf("************* We are waiting the last thread! %d:%d/%d\n",ThrEnvProg::rank(),Wth,PQInterface<Node, PriComp,Goal>::ref);
      }
      //std::cout << " Wait :"<<Wth<<" "<<ThrEnvProg::rank()<<std::endl;
      cond.wait();
      if (Wth >= PQInterface<Node, PriComp,Goal>::ref) {
         cond.unlock();
         return 0;
      }
      Wth--;
      cond.unlock();
    }
    return n;
  }
  /** Method that returns the number of waiting threads
    */
  virtual int waiting_threads() {
     cond.lock();
     int w = Wth;
     cond.unlock();
     return w;
  }
  /** Method that returns the number of waiting threads
    */
  virtual bool are_all_wait() {
    return waiting_threads()== PQInterface<Node, PriComp,Goal>::ref-1;
  }
  /** wake up all the waiting threads to end the search
    */
  virtual bool wake_up_for_end() {
    cond.lock();
    if ( Wth==PQInterface<Node, PriComp,Goal>::ref-1 ) {
       Wth = 100000;
       cond.bcast();
       cond.unlock();
       return true;
    }
    cond.unlock();
    return false;
  }
  /** Function for a non-computing thread waiting for the first insertion
    */
  virtual void wait_for_start() {
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      mut[i].lock();
      if (pq[i]->getStat()->get_counter('i').get() != 0) {
		  mut[i].unlock();
		  return;
	  }
      mut[i].unlock();
    }
    cond.lock();
    cond.wait();
    cond.unlock();
  }
  virtual long nb_node() {
    long nbnd=0;
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      mut[i].lock();
      nbnd +=pq[i]->nb_node();
      mut[i].unlock();
    }
    return nbnd;
  }
  /** method to test if node are required by other threads
    */
  virtual bool need_node() {  
    int ip=getiPQ();
    return (waiting_threads()!=0 && pq[ip]->need_node()) || Need_NodeforLB; 
  }
  /** Ask node for Load Balencing
    */
  virtual void set_need_node4LB(int v) { Need_NodeforLB = v; }
  /// Prints the statistics and contents
  virtual ostream &Prt(ostream &os = std::cout) const {
    pq[0]->getStat()->display_title(os);
    pq[0]->getStat()->display_label(os);
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      mut[i].display(os);pq[i]->getStat()->display_data(os);
    }
    cond.display(os);
    return os;
  }
  /// Prints the statistics and contents
  virtual ostream &display_data(ostream &os = std::cout) const {
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      pq[i]->getStat()->display_data(os);
    }
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
       mut[i].display(os);
    }
    cond.display(os);
    return os;
  }
  /** display the statistics of the priority Queue
    */
  virtual void display_stat(std::ostream &os = std::cout) {
    pq[0]->getStat()->display_title(os);
    pq[0]->getStat()->display_label(os);
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      mut[i].display(os);pq[i]->getStat()->display_data(os);
    }
    cond.display(os);
    //return os;
  }

  /** dump the stat (used in the collect)
    */
  virtual void stat_dump(strbuff<> &s) {
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      pq[i]->stat_dump(s);
    }

  }

  virtual void log_header() {
    for (int i=0;i<ThrEnvProg::npq();i++ ) {
      pq[i]->log_header();
    }
  }
};
 /** @}
  */


/** The Threaded Wrapper for Algorithms.
 */
template<class Trait,class ThePriLB,template <class Node,class PriComp,class PriLB, class Goal> class SPQ>
class ThrAlgoEnvProg : public AlgoEnvProg<Trait> {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::PriComp ThePriComp;
  //typedef PQSkew<TheNode, ThePriComp, TheGoal> ThePQ;
  //typedef PQHn<TheNode, ThePriComp, TheGoal> ThePQ;
  typedef SPQ<TheNode, ThePriComp, ThePriLB, TheGoal> ThePQ;
  typedef ThrPQ<TheNode, ThePriComp, TheGoal, ThePQ> TheThrPQ;
  typedef ThrGoal<Trait> TheThrGoal;
  typedef AlgoEnvProg<Trait> aep;

private:
  ThrBarrier thb;
  TheInstance **thinst;
  TheStat **st;
  TheStat *cumul_st;
  PQInterface<TheNode,ThePriComp,TheGoal> *thpq;
  bool local_alloc_pq;
  bool local_alloc_goal;
protected:
  /// the array of algorithm objects
  TheAlgo **pal;
  /// the multi-threaded goal
  TheThrGoal *thrgoal;
  /** Initialize the instance.
    */
  virtual void init_instance(TheInstance *inst) =0;
  /** Method to Allocate all the object used by the environment.
    * Many of them are given by the Trait Class.
    */
  virtual void Alloc(TheInstance *inst, PQInterface<TheNode,ThePriComp,TheGoal> *_thpq=0, TheThrGoal *_thrgoal=0) {
    
    local_alloc_pq = _thpq==0;
    if ( _thpq==0 )  {
      thpq = new ThrPQ<TheNode, ThePriComp,TheGoal,ThePQ>(*(inst->id()),true);
      //std::cout << "Allocate priority queue !\n";
    } else 
      thpq = _thpq;
    BOBASSERT(thpq != 0, Exception, "Threaded Env", "ThrPq allocation failed");
    local_alloc_goal = _thrgoal==0;
    if ( _thrgoal==0 ) 
      thrgoal = new ThrGoal<Trait>(*(inst->id()),true);
    else 
      thrgoal = _thrgoal;
    BOBASSERT(thrgoal != 0, Exception, "Threaded Env", "ThrGoal allocation failed");
    thrgoal->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
    st = new TheStat *[ThrEnvProg::n_algo_thread()];
    BOBASSERT(st != 0, Exception, "Threaded Env", "Tab Stat allocation failed");
    pal = new TheAlgo *[ThrEnvProg::n_algo_thread()];
    BOBASSERT(pal != 0, Exception, "Threaded Env", "Tab Algo allocation failed");
    if (ThrEnvProg::instance_multi()) {
      thinst = new TheInstance *[ThrEnvProg::n_algo_thread()];
      BOBASSERT(theinst != 0, Exception, "Threaded Env", "Tab Instance allocation failed");
    }  else thinst = 0;
    cumul_st = new TheStat(*(inst->id()));
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      if (ThrEnvProg::instance_multi()) {
        thinst[i] = new TheInstance(*inst);
        BOBASSERT(thinst[i] != 0, Exception, "Threaded Env", "Instance allocation failed");
        inst = thinst[i];
      }
      st[i] = new TheStat(*(inst->id()));
      BOBASSERT(st[i] != 0, Exception, "Threaded Env", "Stat allocation failed");
      pal[i] = new TheAlgo(inst, thpq, thrgoal, st[i]);
      BOBASSERT(pal[i] != 0, Exception, "Threaded Env", "Algo allocation failed");
    }
    core::log().start_header();
    st[0]->log_header();
    thpq->log_header();
    thrgoal->log_header();
    core::log().end_header();

  }
  /** Method to display the Statistics and the Goal
    */
  virtual void display_stat() {
    if ((AlgoEnvProg<Trait>::isdisp()&2) == 2) {
      cumul_st->reset();
      pal[0]->getStat()->display_title(std::cout);
      pal[0]->getStat()->display_label(std::cout);
      //cumul_st->merge(*(pal[0]->getStat()));
      for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
        pal[i]->getStat()->display_data(std::cout);
        cumul_st->merge(*(pal[i]->getStat()));
      }
      cumul_st->display_data(std::cout);
      thpq->Prt(std::cout);
      thrgoal->display_stat(std::cout);
    }
  }
  /** Method to display the Statistics and the Goal
    */
  virtual void display_goal() {
    if ((AlgoEnvProg<Trait>::isdisp()&1) == 1) {
      thrgoal->Prt(std::cout);
    }
  }
  /** Stop the associated algorithms and save the state of the search
   */
  virtual void stop() {
    AlgoEnvProg<Trait>::state_stop();
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      pal[i]->toStop();
    }
  }

  /** Stop the associated algorithms and save the state of the search
   */
  virtual void stop_and_save() {
    AlgoEnvProg<Trait>::state_stop_and_save();
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      pal[i]->toStop();
    }
  }

  /** Method to Free all the object allocated in the Alloc Method.
    */
  virtual void Free() {
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      delete pal[i];
      if (ThrEnvProg::instance_multi()) {
        delete thinst[i];
      }
    }
    if (ThrEnvProg::instance_multi()) {
      delete[] thinst;
    }
    delete[] pal;
    if ( local_alloc_pq ) delete thpq;
  }

  /** Method to initialize the search from a file.
    * @param inst the instance
    * @param _thpq the priority queue where restoring the nodes.
    * @param _thrgoal the goal where restoring the solutions or related Goal's datas.
    */
  virtual bool init_from_file(TheInstance *inst,PQInterface<TheNode,ThePriComp,TheGoal> *_thpq=0, TheThrGoal *_thrgoal=0) {
#ifdef BOBPP_ENABLE_FT
    if ( core::opt().BVal("--","-ft_rs") && AlgoEnvProg<Trait>::Level()==0 ) {
      std::cout << "ThrEnvProg : Restoring Data\n"; 
      DeSerialFile fti(core::opt().SVal("--","-ft_file"));
      if ( !fti.is_open() ) {
         std::cout << "EnvProg : File "<<core::opt().SVal("--","-ft_file")<<" not found (exit)\n"; 
         exit(1);
      }

      inst->UnPack(fti);
      Alloc(inst,_thpq,_thrgoal);
      thrgoal->UnPack(fti);
      thpq->UnPack(fti);
      cumul_st->UnPack(fti);
      fti.close();
      thrgoal->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
      //al = new TheAlgo(inst, pq, tg, st);
      std::cout << "ThrEnvProg : Data are restored\n"; 
      return true;
    }
#endif
    return false;
  }
  /** Method to save the state of te search on a file.
    * @param inst the instance
    */
  virtual bool save_on_file(TheInstance *inst) {
#ifdef BOBPP_ENABLE_FT
    if (pal[0]->isEtStop() && AlgoEnvProg<Trait>::state_is_save()==0  && AlgoEnvProg<Trait>::Level()==0 ) {
      std::cout << "ThrEnvProg : algo is stopped, save data\n"; 
      SerialFile fto(core::opt().SVal("--","-ft_file"));
      inst->Pack(fto);
      thrgoal->Pack(fto);
      thpq->Pack(fto);
      cumul_st->Pack(fto);
      fto.close();
      std::cout << "EnvProg : Data are saved\n"; 
      return true;
    }
#endif
    return false;
  }
  /** init is the method that initializes the search for the two interfaces.
   * @param nd the node to explore from.
   * @param inst the instance associated with the search.
   * @param is_master a boolean to indicate if the process is a main process or not. This boolean is used when the threaded environment is 
   *        a part of the distributed one i.e. the mpx environment.
    * @param _thpq the optional priority queue 
    * @param _thrgoal the optional goal 
   */
  virtual void init(TheNode *nd, TheInstance *inst,bool is_master=true,PQInterface<TheNode,ThePriComp,TheGoal> *_thpq=0, TheThrGoal *_thrgoal=0) {
    if ( is_master ) {
      if ( !init_from_file(inst) ) {
        init_instance(inst);
        Alloc(inst,_thpq,_thrgoal);
        thrgoal->Init(inst);
        thrgoal->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
        if (nd == 0) inst->InitAlgo(pal[0]);
        else pal[0]->GPQIns(nd);
      }
    } else {
      init_instance(inst);
      Alloc(inst,_thpq,_thrgoal);
      thrgoal->Init(inst);
      thrgoal->set_verbosity(BaseAlgoEnvProg::isdisp_goal());
    }
  }
  /** Method that run the algorithms for each of the slave threads.
    */
  virtual void run(){
    thb.setNb(ThrEnvProg::n_thread());
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      ThrEnvProg::run(i, pal[i], &thb);
    }
  }
  /** Method that run the master thread.
    */
  virtual void run_master() {
    thb.Wait();
  }
  /** Master thread will wait the Algorithm barrier.
    */
  virtual void wait_master() {
    thb.Wait();
  }
  /** end() method that display the statistics if needed and save the state of the search 
    * on file.
    * @param inst a pointer to the instance to save.
    */
  virtual void end(TheInstance *inst) {
    aep::display();
    save_on_file(inst);
  }
  /** protected method THat is the implementation of the 2 functors
    * @param nd the root node of the search
    * @param inst the instance.
    */
  virtual void doit(TheNode *nd,TheInstance *inst) {
    init(nd,inst,true);
    AlgoEnvProg<Trait>::state_running();
    run();

    DBGAFF_ENV("ThrAlgoEnvProg::doit","master will stop\n");
    run_master();
    DBGAFF_ENV("ThrAlgoEnvProg::doit","master has finished\n");
    end(inst);
    Free();
  }
  /** Method to pack the statistics
    * @param ser the Serialize object.
    */
  virtual void Pack_Stat(Serialize &ser) {
      int nbt=ThrEnvProg::n_algo_thread();
      ser.Pack(&nbt,1);
      for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
          st[i]->Pack(ser);
      }
  }
public:
  /** the constructor
    * @param _v a flag to indicate if the algorithm will dispaly the statistics and/or the solution
    * @param _l the level where this Environment will be used.
    */
  ThrAlgoEnvProg(int _v=BaseAlgoEnvProg::disp_default(),int _l=0) : AlgoEnvProg<Trait>(_v,_l),thb() {
  }
  /** the destructor
    */
  virtual ~ThrAlgoEnvProg() {
    for (int i = 0 ; i < ThrEnvProg::n_algo_thread() ; i++) {
      delete st[i];
    }
    if ( local_alloc_goal ) delete thrgoal;
    delete[] st;
    delete cumul_st;
  }
  /** get the goal after a resolution
   * @return the goal with the solution.
   */
  TheGoal *goal() {
    return thrgoal;
  }
  /** get the stat after a resolution
   * @return the stat.
   */
  TheStat *stat() {
    return st[0];
  }
  /** get the array of thread stats
    * @return the array of stat
  */
  TheStat **stat_array() {
    return st;
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

  /** dump the all the stat.
    */
  virtual void dump() {
     strbuff<> s;
     s << core::Time()<<" { ";
     core::opt().dump(s);
     s << " } ";
     cumul_st->dump(s);  
     thpq->stat_dump(s);
     thrgoal->stat_dump(s);
     s << " t:1," << core::dTime();
     core::docollect(s);
  }

};


/** The Threaded Algorithms Wrapper for Branch and Bound.
 */
template<class Trait>
class ThrBBAlgoEnvProg : public ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB> {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename Trait::Instance TheInstance;
  /// the constructor
  ThrBBAlgoEnvProg() : ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~ThrBBAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/** The Threaded Algorithms Wrapper for simple Divide and Conquer.
 */
template<class Trait>
class ThrSDCAlgoEnvProg : public ThrAlgoEnvProg<Trait,BreathPri<typename Trait::Node>,PQLB > {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename Trait::Instance TheInstance;
  /// the constructor
  ThrSDCAlgoEnvProg() : ThrAlgoEnvProg<Trait,BreathPri<typename Trait::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~ThrSDCAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/** The Threaded Algorithms Wrapper for Costed Divide and Conquer.
 */
template<class Trait>
class ThrCDCAlgoEnvProg : public ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB > {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename Trait::Instance TheInstance;
  /// the constructor
  ThrCDCAlgoEnvProg() : ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~ThrCDCAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }

};

/** The Threaded Algorithms Wrapper for SimSp Algorithms
 */
template<class Trait>
class ThrSimSpAlgoEnvProg : public ThrAlgoEnvProg<Trait,HighPri<typename Trait::Node>,PQSP >, public SimSpEnvProg<Trait>  {
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Goal TheGoal;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::Link TheLink;
  typedef typename Trait::PriComp ThePriComp;
  typedef typename Trait::Key TheKey;

public:
  /// the constructor
  ThrSimSpAlgoEnvProg() : ThrAlgoEnvProg<Trait,HighPri<typename Trait::Node>,PQSP >(),SimSpEnvProg<Trait>()  {
  }
  /// Destructor
  virtual ~ThrSimSpAlgoEnvProg() {}
protected:
  /** Get a new link according to the environment
    */
  virtual TheLink *new_env_link(const TheKey &Key) { 
      return new ThrLink<TheLink>();
  }
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(this); }


};

/** The Threaded Algorithms Wrapper for Variable Neihgborhood search.
 */
template<class Trait>
class ThrVNSAlgoEnvProg : public ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB> {
public:
  /// The type of the instance given by the Trait class (template).
  typedef typename Trait::Instance TheInstance;
  /// the constructor
  ThrVNSAlgoEnvProg() : ThrAlgoEnvProg<Trait,BestEPri<typename Trait::Node>,PQLB >() {
  }
  /// Destructor
  virtual ~ThrVNSAlgoEnvProg() {}
protected:
  /** Initialize the instance.
    * @param inst the instance to initialize
    */
  virtual void init_instance(TheInstance *inst) {inst->Init(); }
};

/**
 * @}
 */

} // End of the namespace
#endif

