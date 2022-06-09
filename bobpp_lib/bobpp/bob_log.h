/*
 * Bob++/bob_log.h
 *
 *  These classes are used for the logging information of the bob library
 */
#ifndef BOBLOG_ABSTRACT
#define BOBLOG_ABSTRACT

#include <sstream>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <iostream>

//#include <thr/thr.h>



namespace Bob {

/** @defgroup LOGGRP The Bob++ Logging classes
 *  @ingroup IOGRP
 *  @{
 *  When an application is running or after its execution, it is
 *  very usefull to be able to get information about the behaviour 
 *  of the application.
 *  For example, to compare and analyze two Branch and Bound, knowing
 *  the execution time, the number of pruned nodes, the times spent in 
 *  evaluation, must be stored during their execution.

 *  It has been decided that each process creates one log file.
 *  Then, the information stored in the file could be produced by
 *  only one process, with only one thread but also from several threads...
 *  We would that log file stores a maximum of information
 *  Mainly, these informations are generated using the statistics objects
 *  (see @ref STATGRP).
 *
 *  The different information to log are :
 *   - the activities of threads/processors to solve an instance
 *     of a problem using an algorithm. The data to store are:
 *       - the beginning and the end time of of an algorithm
 *       - the number of execution of the genchild::operator()
 *       - the statistics associated with the algorithm
 *       - the number of access on the local or shared data structures
 *   - the number of operation on one shared data structure
 *
 *
 *   To identify the different components, we associated an identifier to
 *   each of them.
 *   we dissociate two of them :
 *    - the envid the identifier associated with a thread or a process
 *    (according to the environment). An envid is hierarchical, meaning that,
 *    for example, it is composed by a process id and a thread id or is equal
 *    to only one process id.
 *    - a id an object identifier, an id represents :
 *          - an instance :  to identify the resolution of an instance of a
 *             problem.
 *          - an algorithm : associated with an instance on different envid
 *               (EnvId:InstanceId:'B'|'S'|'C')
 *          - a genchild : associated with an algorithm
 *               (EnvId:InstanceId:AlgoId:"GC")
 *          - a data structure : associated with an envid. Data structure
 *            is the more complicated object to identify, because it could be
 *            associated with a thread to collect the activity of the thread
 *            on the data structure or with a process.
 *            The data structure is then shared beween the threads and then
 *            the statistics represent the sum of the activity of the
 *            threads that are associated with the process.
 *
 *
 */

/** type use by the log to indentify a node
  */
typedef unsigned long node_id;


class EnvId;
class SeqId;
class stat;

/** The Log_base class
 * This class is a virtual version and do nothing.
 */
class Log_base {
public:
  /// constructor
  Log_base();
  /// destructor
  virtual ~Log_base();
  /** Display a strbuff in an critical section.
    * @param lm the strbuff to display.
    */
  virtual void out(strbuff<> const &lm) {}
  /** virtual method to display a strbuff to a real media with a time at the beginning of the line.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out(strbuff<> const &lm) {}
  /** virtual method to display a strbuff to a real media.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out_wot(strbuff<> const &lm) {}
  /** Display log line for a counter
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, node_id  c, const char *ni) {}
  /** Display log line for a stat_timer
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, char w, node_id c, double t,const char *ni) {}
  /** Display log line for a counter without node info
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    */
  virtual void out(const char *what, node_id  c) {}
  /** Display log line for a stat_timer without node info
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    */
  virtual void out(const char *what, char w, node_id  c, double t) {}
  /** Display log line for a counter with a node identifier
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, node_id  c, node_id ni) {}
  /** Display log line for a stat_timer with node identifier
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, char w, node_id  c, double t,node_id ni) {}

  /** start the log header.
    */
  virtual void start_header() { }
  /** end the log header.
    */
  virtual void end_header() { }
  /** log the header of the stat
    * @param st the stat to log.
    */
  virtual void log_header(const strbuff<> &st) {}
  /** Set the log object
    * @param _ei the environment Id of the log
    */
  void setEId(EnvId *_ei);
protected:
/// The EnvId environment identifier
  EnvId *ei;

  
};

/** The Log_real class
 * This class is a real version and the only method to reimplement is the 
 * out(strbuff<> const &s).
 */
class Log_real : public Log_base {
public:
  /// constructor
  Log_real():Log_base() { 
     pthread_mutex_init(&mut, 0);
  }
  /// destructor
  virtual ~Log_real() {
    pthread_mutex_destroy(&mut);
  }
  /** Lock the log for a thread
    */
  void lock() {
    pthread_mutex_lock(&mut);
  }
  /** Unlock the log for a thread
    */
  void unlock() {
    pthread_mutex_unlock(&mut);
  }
  /** Display a strbuff in an critical section.
    * @param lm the strbuff to display.
    */
  virtual void out(strbuff<> const &lm); 
  /** virtual method to display a strbuff to a real media with a time at the beginning of the line
    * @param lm the strbuff to display.
    */
  virtual void nolock_out(strbuff<> const &lm)=0;
  /** virtual method to display a strbuff to a real media.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out_wot(strbuff<> const &lm)=0;
  /** Display log line for a counter with node info as char *
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, node_id  c, const char *ni);
  /** Display log line for a stat_timer with node info as char *
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, char w, node_id  c, double t,const char *ni);
  /** Display log line for a counter without node info
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    */
  virtual void out(const char *what, node_id  c);
  /** Display log line for a stat_timer without node info
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    */
  virtual void out(const char *what, char w, node_id  c, double t);
  /** Display log line for a counter with a node identifier
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, node_id  c, node_id ni);
  /** Display log line for a stat_timer with node identifier
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a node_id  that stores cumulative timer
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, char w, node_id  c, double t,node_id ni);
  /** start the log header.
    */
  virtual void start_header(); 
  /** end the log header.
    */
  virtual void end_header();
  /** log the header of the stat
    * @param st the stat to log.
    */
  virtual void log_header(const strbuff<> &st);
protected:
  /// the mutex
  pthread_mutex_t mut;
};

/** The Log_file class
 * This class write the information on a file
 */
class Log_file : public Log_real {
public:
  /// constructor
  Log_file(std::string file): Log_real(), os(file.c_str(), std::ios::trunc) { }
  /// destructor
  virtual ~Log_file() {
    os.close();
  }
  /** Display a strbuff this a time at the beginning the line.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out(strbuff<> const &lm);
  /** virtual method to display a strbuff to a real media.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out_wot(strbuff<> const &lm);

protected:
  /// the ostream to save all logging information on a file
  std::ofstream os;
};

#ifndef WIN32
/** The Log_net class
 * This class send the log to a network host
 */
class Log_net : public Log_real {
protected:
  /// Output socket id
  int os;
public:
  /// constructor
  Log_net(std::string host, int port);
  /// destructor
  virtual ~Log_net();
  /** Display a strbuff
    * @param lm the strbuff to display.
    */
  virtual void nolock_out(strbuff<> const &lm);
  /** virtual method to display a strbuff to a real media.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out_wot(strbuff<> const &lm);
};
#endif


/** The Log class
 * This class is an interface for the real Log object that could be
 * a simple Log_base object (it does nothing) or a Log_file.
 */
class Log : public Log_base {
public:
  /// Constructor
  Log();
  /// Destructor
  virtual ~Log();
  /** Set the log object
    * @param lb the real log used by the wrapper
    */
  void setLog(Log_base *lb);
  /** Set the log object
    * @param _ei the environment Id of the log
    */
  void setEId(EnvId *_ei) { l->setEId(_ei); }
  /** Display a strbuff in an critical section.
    * @param lm the strbuff to display.
    */
  virtual void out(strbuff<> const &lm) {l->out(lm); }
  /** virtual method to display a strbuff to a real media.
    * @param lm the strbuff to display.
    */
  virtual void nolock_out(strbuff<> const &lm) {l->nolock_out(lm); }
  /** Display log line for a counter
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, node_id  c, const char *ni);
  /** Display log line for a stat_timer
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a double that stores cumulative timer
    * @param ni a char * that stores eventually the node information
    */
  virtual void out(const char *what, char w, node_id  c, double t,const char *ni);
  /** Display log line for a counter without node info
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    */
  virtual void out(const char *what, node_id  c);
  /** Display log line for a stat_timer without node info
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a double that stores cumulative timer
    */
  virtual void out(const char *what, char w, node_id  c, double t);
  /** Display log line for a counter with a node identifier
    * @param what a char * that stores the statistic identifier
    * @param c a node_id  that stores the counter
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, node_id  c, node_id ni); 
  /** Display log line for a stat_timer with node identifier
    * @param what a char * that stores the statistic identifier
    * @param w a char to indicate the command 's' or 'e'
    * @param c a node_id  that stores the counter
    * @param t a double that stores cumulative timer
    * @param ni a node_id that stores the node identifier
    */
  virtual void out(const char *what, char w, node_id  c, double t,node_id ni);
  /** start the log header.
    */
  virtual void start_header() { l->start_header(); }
  /** end the log header.
    */
  virtual void end_header() { l->end_header(); }
  /** log the header of the stat
    * @param st the stat to log.
    */
  virtual void log_header(const strbuff<> &st) { l->log_header(st); }
protected:
  /// The real log class
  Log_base *l;
};

/**
 * @}
 */

};
#endif
