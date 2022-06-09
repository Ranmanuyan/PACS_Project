
#include<bobpp/thr/thr.h>


namespace  Bob {


Thread *ThrEnvProg::tt = 0;
int ThrEnvProg::comm_th = 0;
int ThrEnvProg::nbth = 0;
int ThrEnvProg::nbpq = 1;
int ThrEnvProg::cpuset_sz = 0;
bool ThrEnvProg::multi_inst = false;

void *GoThread(void *a) {
  Thread *t = (Thread *)a;

#if 0
#ifdef BOBPP_HAVE_PTHREAD_SETAFFINITY_NP
  static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
  int cs = ThrEnvProg::cpuset_size();
  if ( cs !=-1 ) {
   cpu_set_t cpuset;
   int r = ThrEnvProg::rank();
   int npq = ThrEnvProg::npq();
   int ipq = (r*npq)/ThrEnvProg::n_algo_thread();
   int cpu;
   pthread_mutex_lock(&mut);
   std::cout<<"r="<<r<<" ipq="<<ipq<<" cs="<<cs;

   CPU_ZERO(&cpuset);
   for (int j = 0; j < cs; j++) {
     cpu = j*npq+ipq;
     CPU_SET(cpu, &cpuset);
     std::cout << " " << cpu;
   }
   std::cout << std::endl;
   pthread_mutex_unlock(&mut);
   int s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
   if (s != 0) {
      std::cerr << "cpuset failed for thread :"<<r<<":"<<pthread_self()<<std::endl;
   }
  }
#endif
#endif

  t->loop();
  return 0;
}

Thread::Thread() : cond(),a(0), bar(0), state(ThWait) {
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, ThrEnvProg::stack_size());
}

void Thread::init() {
  pthread_create(&id, &attr, GoThread, (void *)this);
}

void Thread::loop() {
  DBGAFF_ENV("Thread::loop","Begins");
  cond.lock();
  if (state == ThWait)  {
    DBGAFF_ENV("Thread::loop","Thread is waiting");
    cond.wait();
    DBGAFF_ENV("Thread::loop","Wake up after first wait");
  } 
  while (state != ThStop) {
    if (state == ThHasWork) {
      DBGAFF_ENV("Thread::loop","Has Work");
      BOBASSERT(a != 0, StateException, "Thread::loop()", "Thread get work but pointer is null !");
      state = ThWork;
      cond.unlock();
      DBGAFF_ENV("Thread::loop","Execute Algo");
      (*a)();
      DBGAFF_ENV("Thread::loop","Algo Finish");
      cond.lock();
      if (state == ThWork) {
        a = 0;
        state = ThWait;
      }
      cond.unlock();
      bar->Wait();
      DBGAFF_ENV("Thread::loop","Thread is after Algo barrier");

    }
    cond.lock();
    if (state == ThStop) {
      DBGAFF_ENV("Thread::loop","Thread is in Stopping");
      break;
    }
    if (state == ThHasWork) {
      DBGAFF_ENV("Thread::loop","Thread has received work");
      continue;
    }
    state = ThWait;
    DBGAFF_ENV("Thread::loop","Thread is waiting");
    cond.wait();
    DBGAFF_ENV("Thread::loop","Wake up after wait");
  }
  cond.unlock();
  DBGAFF_ENV("Thread::loop","Ends");
}

long ThrId::id() {
  return (long)ThrEnvProg::rank();
}
long ThrId::nb() {
  return (long)ThrEnvProg::n_algo_thread();
}


};
