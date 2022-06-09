
#include<bobpp/thr/thr.h>
#include<cstdlib>
#include<libspe2.h>

#include "spe_defs.hpp"

/*! This is the Bob++ parallel environment for the 
 *  Cell Broadband Engine architecture
 */

/** Handle for embedded SPU program */
extern spe_program_handle_t bobpp_spu_handle;

/** Convert a PPE pointer (32 bit or 64 bit) to 64 bit effective address */
static inline uint64_t ppe_ea(void *ptr) {
  return (uint64_t)((unsigned long)ptr);
}

/** Convert a 64 bit effective address to PPE pointer (32 bit or 64 bit) */
static inline void *ppe_ptr(uint64_t ea) {
  return (void *)ea;
}

namespace Bob {

class base_Callback {
  static pthread_once_t cb_key_once;
  static pthread_key_t cb_key;
  static void cb_key_alloc() {
    pthread_key_create(&cb_key, NULL);
  }
public:
  base_Callback() {
    pthread_once(&cb_key_once, cb_key_alloc);
    pthread_setspecific(cb_key, this);
  }
};

template <class Trait>
class PPECallback : base_Callback {
  typedef typename Trait::Algo TheAlgo;
  typedef typename Trait::Node TheNode;
protected:
  TheAlgo *a;

private:
  /** Base PPE callback function */
  static int CB(void *ls_base, unsigned int data_ptr) {
    unsigned int params_offset = *((unsigned int*)(((char*)ls_base)+data_ptr));
    cb_id fid = (cb_id)(params_offset >> 24);
    cbdata *dat = (cbdata*)(((char*)ls_base)+(params_offset&0xffffff));

    PPECallback<Trait> *cb = (PPECallback<Trait>*)pthread_getspecific(cb_key);

    //std::cout << "PPECallback::CB() fid="<<fid <<" dat="<<(void*)dat<< std::endl;
/*    std::cout << "PPECallback::CB() fid="<<fid <<" offset="<<std::hex<<params_offset<< std::endl;
    std::cout << "PPECallback::CB() ls_base="<<ls_base<< std::endl;
    std::cout << "PPECallback::CB() dat="<<dat<< std::endl;
    std::cout << "PPECallback::CB() dat->l[0]="<<std::hex<<dat->l[0]<< std::endl;
*/
    if (fid&0x80)
      return cb->AppCB(fid, dat);

    switch(fid) {
      case CB_NEWNODE:
        dat->l[0] = cb->cb_newNode();
        break;
      case CB_GPQGET:
        cb->cb_gpqGet(dat->l[0], dat->i[2]);
        break;
      case CB_SEARCH:
        cb->cb_search(dat->l[0]);
        break;
/*      case CB_SETEVAL:
        cb->cb_seteval(dat);
        break;
      case CB_GETEVAL:
        cb->cb_geteval(dat);
        break;
*/
      default:
        std::cerr<<"unknown PPE callback id: "<<fid<<std::endl;
        return 1;
    }
    return 0;
  }

  /* Callback methods */
  static uint64_t cb_newNode() {
    TheNode *n = new TheNode();
    return ppe_ea((void*)n);
  }

  void cb_gpqGet(uint64_t &ea, uint32_t &dist) {
    //std::cout << "PPECallback::cb_gpqGet() "<<(void*)a<< std::endl;
    TheNode *n = a->GPQGet();
    //std::cout << "PPECallback::cb_gpqGet()1 "<<(void*)n<< std::endl;
    ea = ppe_ea(n);
    //std::cout << "PPECallback::cb_gpqGet()2 "<<std::hex<<ea<< std::endl;
    dist = (n?n->dist():0);
    //std::cout << "PPECallback::cb_gpqGet()3 "<<dist<< std::endl;
  }

  void cb_search(uint64_t ea) {
    a->Search((TheNode*)ea);
  }

  /*void cb_seteval(cbdata *dat) {
  }
  void cb_geteval(cbdata *dat) {
  }*/

protected:
  /** User defined PPE callback function */
  virtual int AppCB(int id, cbdata *dat) = 0;

public:
  PPECallback(TheAlgo *_a) : base_Callback(), a(_a) {
    spe_callback_handler_register((void*)CB, 0x10, SPE_CALLBACK_NEW);
  }
  virtual ~PPECallback() {}
};

template <class Trait>
struct SPEThread : public Thread {
  typedef typename Trait::Callback TheCallback;
  typedef typename Trait::Algo TheAlgo;

public:
  /// Constructor
  SPEThread() : Thread() {
    std::cout<<"SPEThread::SPEThread()"<<std::endl;
  };

  /// Destructor
  virtual ~SPEThread() { }
  /// The main loop
  virtual void loop() {
    std::cout<<"SPEThread::loop() "<< pthread_self() <<": Starting in Loop\n";

    unsigned int entry_point;
    spe_context_ptr_t my_context;
    int retval;
    //std::cout<< pthread_self() <<": Starting in Loop\n";
    pthread_mutex_lock(&mut);
    if (state == ThWait)  {
      //std::cout << pthread_self() <<": Wait\n";
      pthread_cond_wait(&cond, &mut);
    }
    pthread_mutex_unlock(&mut);
    // Setup PPE callback functions
    TheCallback cb((TheAlgo*)a);
    while (state != ThStop) {
      if (state == ThHasWork) {
        /* Create the SPE Context */
        my_context = spe_context_create(SPE_EVENTS_ENABLE | SPE_MAP_PS, NULL);
        /* Load the embedded code into this context */
        spe_program_load(my_context, &bobpp_spu_handle);
        entry_point = SPE_DEFAULT_ENTRY;

        BOBASSERT(a != 0, StateException, "Thread::loop()", "Thread get work but pointer is null !");
        std::cout << pthread_self() << "r:" << ThrEnvProg::rank() << ": Run\n";
        state = ThWork;
        //(*a)();
        /* Run the SPE program until completion */
        retval = spe_context_run(my_context, &entry_point, 0, /*pdata*/0, 0, 0);
        if (retval)
          perror("An error occurred running the SPE program");

        pthread_mutex_lock(&mut);
        if (state == ThWork) {
          a = 0;
          state = ThWait;
        }
        pthread_mutex_unlock(&mut);
        std::cout << pthread_self() << "r:" << ThrEnvProg::rank() << ": Algo Finish \n";
        bar->Wait();
        spe_context_destroy(my_context);
      }
      std::cout << pthread_self() << "r:" << ThrEnvProg::rank() << ": Re Wait\n";
      pthread_mutex_lock(&mut);
      if (state == ThStop) {
        pthread_mutex_unlock(&mut);
        break;
      }
      if (state == ThHasWork) {
        pthread_mutex_unlock(&mut);
        continue;
      }
      state = ThWait;
      pthread_cond_wait(&cond, &mut);
      pthread_mutex_unlock(&mut);
    }
    std::cout << pthread_self() << "r:" << ThrEnvProg::rank() << ": Exit of Loop\n";
  }
};


template <class Trait>
class CellEnvProg : public ThrEnvProg {
public:
  static void init() {
    ThrEnvProg::init();
  }
  static void start() {
    std::cout << "In CellEnvProg::start:" << nbth << "\n";
    if (nbth == 0) {
      std::cerr << "Number of thread 0\n";
      exit(1);
    }
    tt = new SPEThread<Trait>[nbth];
    for (int i=0; i<nbth; i++)
      tt[i].init();
  }
  static void stop() {
    ThrEnvProg::stop();
  }
};


/** The Cell Wrapper for Algorithms.
 */
template<class Trait>
class CellAlgoEnvProg : public ThrAlgoEnvProg<Trait> {
  typedef typename Trait::Node TheNode;
  typedef typename Trait::Instance TheInstance;
  typedef ThrAlgoEnvProg<Trait> taep;
protected:
  /** doit is the method that do the job for the two interface.
   * @param nd the node to explore from.
   * @param inst the instance associated with the search.
   */
  virtual void doit(TheNode *nd, TheInstance *inst) {
    std::cout<<"CellAlgoEnvProg::doit()"<<std::endl;
    ThrBarrier thb(ThrEnvProg::nthread()+1);
    inst->Init();
    Alloc(inst);
    taep::thrgoal->Init(inst);
    if (nd==0 ) inst->InitAlgo(taep::pal[0]);
    else taep::pal[0]->GPQIns(nd);
    for (int i = 0 ; i < ThrEnvProg::nthread() ; i++) {
      CellEnvProg<Trait>::run(i, taep::pal[i], &thb);
    }
    thb.Wait();
    taep::display();
    ThrAlgoEnvProg<Trait>::Free();
  }
};

/** The Cell Algorithms Wrapper for Branch and Bound.
 */
template<class Trait>
class CellBBAlgoEnvProg : public CellAlgoEnvProg<Trait> {
public:
  /// the constructor
  CellBBAlgoEnvProg() : CellAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~CellBBAlgoEnvProg() {}
};

/** The Cell Algorithms Wrapper for simple Divide and Conquer.
 */
template<class Trait>
class CellSDCAlgoEnvProg : public CellAlgoEnvProg<Trait> {
public:
  /// the constructor
  CellSDCAlgoEnvProg() : CellAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~CellSDCAlgoEnvProg() {}
};

/** The Cell Algorithms Wrapper for Costed Divide and Conquer.
 */
template<class Trait>
class CellCDCAlgoEnvProg : public CellAlgoEnvProg<Trait> {
public:
  /// the constructor
  CellCDCAlgoEnvProg() : CellAlgoEnvProg<Trait>() {
  }
  /// Destructor
  virtual ~CellCDCAlgoEnvProg() {}
};



} /* namespace Bob */
