/*
 * Bob++/include/BobStat.h
 *
 *  These classes are used to store de statistics of the algorithms
 */
#ifndef BOBSTAT
#define BOBSTAT

namespace Bob {

/** @defgroup STATGRP The Bob++ statistics classes
 *  @ingroup CoreGRP
 *
 *  This module is a part of the core of the Bob++ library
 *
 *  This module proposes a set of classes to store the statistics
 *  about the different activities.
 *
 *  We have choosen to store time statistics (@ref stat_timer)
 *  We have choosen to store counter (@ref stat_counter)
 *  The class stat could store several stat_timer, stat_counter
 *  and also pointers on other stat objects.
 *
 *  A stat object is associate with each bob++ components.
 *  For example, a BBAlgo object use a stat object which is a BBStat.
 *  A BBStat object inherits indirectly from the stat object. In
 *  the constructor, some  stat_counter or stat_timer objects are added using
 *  respectively add_counter() or add_timer() stat method.
 *  @{
 */

/// Buffer used to store the complete Id of a stat.
typedef strbuff<30> name_st;
/// Buffer used to display all the labels of the stats on te header log.
typedef strbuff<> buff_st;

/** stat_base is a class to identify counter or time stat statistic 
  */
class stat_base:public CId  {
protected:
  /// long name of the stat
  name_st nst;
  /// 0 not logged, 1 logged and global, 2 logged and local (default)
  int logged;
public:
  /** Constructor
    * @param l a int giving the logging type
    */
  stat_base(int l=2) : CId('?'),nst("def"),logged(l) {}
  /** Constructor
    * @param sb the stat_base to copy from
    */
  stat_base(const stat_base &sb) : CId(sb), nst(sb.nst),logged(sb.logged) {}
  /** Constructor
    * @param s the char identifying the stat.
    * @param id the parent identifier.
    * @param l a int giving the logging type
    */
  stat_base(char s,const Id &id, int l=2): CId(s,id),nst("def"),logged(l) {
  }
  /** Constructor
    * @param s the char identifying the stat.
    * @param id the parent identifier.
    * @param _nst the long name of the stat.
    * @param l a int giving the logging type
    */
  stat_base(char s, const Id &id, const char *_nst,int l=2): CId(s,id),nst(_nst),logged(l) {
  }
  /// Destructor
  virtual ~stat_base() { }
  /** set method
    * @param c the char identifying the stat.
    * @param i the parent identifier
    * @param _nst the long name of the stat.
    * @param l a int giving the logging type
    */
  virtual void set(char c,const Id &i,const char *_nst,int l=2) {
    CId::set(c,i);
    nst.copy(_nst);
    logged = l;
  }
  /** Set the parent identifier
    * @param i the parent identifier
    * @param l a int giving the logging type
    */
  virtual void set(const Id &i,int l=2) {
    CId::set(i);
    logged=l;
  }
  /** Construct the log line header of the stat.
    * Put the id, the name, and the type of the stat on a buffer
    * @param os the buffer
    * @param type the type (counter or timer) of the stat.
    */
  virtual void log_header(buff_st &os,const char *type) const {
    if ( !is_logged() ) return;
    if ( is_local() ) 
      os << "l "<<str() << ":"<<type<<":" << nst << "\n";
    else 
      os << "g "<<str() << ":"<<type<<":" << nst << "\n";
  }
  /** dump the stat
    */
  void dump(strbuff<> &s) const {
     s << ' '<<get()<<':';
  }
  /** returns if the statistic is local
    */
  virtual bool is_local() const { return logged==2; }
  /** returns if the statistic is logged
    */
  virtual bool is_logged() const { return logged>0; }
  /** Pack method to Serialize the stat_base
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("Stat_base::Pack", "Packing base stat");
    CId::Pack(bs);
    nst.Pack(bs);
    bs.Pack(&logged,1);
  }
  /** Unpack method to deserialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("Stat_base::UnPack", "Unpacking base stat");
    CId::UnPack(bs);
    nst.UnPack(bs);
    bs.UnPack(&logged,1);
  }

};

/**
 * The stat_timer class stores the time execution of a function
 * or something. It stores an identifier the methods start() and end() may
 * be used to indicate the beginning and the end of the component we want
 * to profile.
 * These methods use the core::dtime() method to get the time.
 *
 */
class stat_timer : public stat_base {
protected:
  /// The number of runs
  unsigned long long nbr;
  /// Start time
  double st;
  /// current time
  double ct;
  /// delta time
  double dt;
  /// cumulating timers
  double cumt;
public:
  /// Constructor
  stat_timer(int l=2): stat_base(l), nbr(0L), st(0), ct(0), dt(0), cumt(0) {
    DBGAFF_MEM("stat_timer::stat_timer", "Constructor()");
  }
  /// Constructor
  stat_timer(const stat_timer &stt): stat_base(stt), nbr(stt.nbr), st(stt.st), ct(stt.ct), dt(stt.dt), cumt(stt.cumt) {
    DBGAFF_MEM("stat_timer::stat_timer", "Const stat_timer");
  }
  /// Constructor
  stat_timer(char s,const Id &id,int l=2): stat_base(s,id,l), nbr(0L), st(0), ct(0), dt(0), cumt(0) {
    DBGAFF_MEM("stat_timer::stat_timer", "Constructor(char *)");
  }
  /// Constructor
  stat_timer(char s,const Id &id, const char *_nst,int l=2): stat_base(s,id,_nst,l), nbr(0), st(0), ct(0), dt(0), cumt(0) {
    DBGAFF_MEM("stat_timer::stat_timer", "Constructor(char,char *)");
  }
  /// Destructor
  virtual ~stat_timer() {
    DBGAFF_MEM("stat_timer::~stat_timer", "Destructor");
  }
  /// Intialize the Statistics
  void init(char _n, const Id &s, int _log=2) {
    set(_n, s,"",_log);
    nbr = 0L;
    st = ct = dt = cumt = 0.0;
  }
  /// Intialize the Statistics
  void init(char _n, const Id &s, const char *_nst,int _log=2) {
    set(_n, s,_nst,_log);
    nbr = 0;
    st = ct = dt = cumt = 0.0;
  }
  /** reset the statistics
    */
  void reset() { 
    nbr = 0;
    st = ct = dt = cumt = 0.0;
  }
  /// Set the start time
  virtual void start();
  /// Set the end time
  virtual void end();
  /** Set the start time for a computation for one node
    * @param id the identifier of the related node
    */
  virtual void start(node_id id);
  /** Set the end time for a computation for one node
    * @param id  the identifier of the related node
    */
  virtual void end(node_id id);
  /** dump the stat
    * @param s the strbuff to fill
    */
  void dump(strbuff<> &s) const {
     stat_base::dump(s);
     s << nbr<<","<<cumt;
  }

  /** merge method use to collect the statistics from another stat_timer.
    * @param st the stat_timer to merge
    */
  void merge(const stat_timer &st) {
    nbr += st.nbr;
    cumt += st.cumt;
  }
  /// gets the cumulating time
  double getc() const {
    return cumt;
  }
  /// gets the number of utilization of the stat_timer
  double getn() const {
    return nbr;
  }
  /** Pack method to Serialize the stat_timer
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("Stat_timer::Pack", "Packing time stat");
    stat_base::Pack(bs);
    bs.Pack(&nbr, 1);
    bs.Pack(&st, 1);
    bs.Pack(&ct, 1);
    bs.Pack(&dt, 1);
    bs.Pack(&cumt, 1);
  }
  /** Unpack method to deserialize the stat_timer
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("Stat_timer::UnPack", "Unpacking time stat");
    stat_base::UnPack(bs);
    bs.UnPack(&nbr, 1);
    bs.UnPack(&st, 1);
    bs.UnPack(&ct, 1);
    bs.UnPack(&dt, 1);
    bs.UnPack(&cumt, 1);
  }
  /** Construct the log line header of the stat_timer.
    * Put the id, the name, and the type of the stat on a buffer
    * @param os the buffer
    */
  virtual void log_header(buff_st &os) const {
    stat_base::log_header(os,"TIME");
  }

  /** Display the stat_timer in an ostream
    * @param o the ouput
    * @param s the stat_timer to display
    */
  friend std::ostream& operator<< (std::ostream& o, const stat_timer& s);
};


/**
 * The stat_counter class that stores a counter
 */
class stat_counter : public stat_base {
protected:
  /// the counter
  unsigned long long nb;
  /// the maximum
  unsigned long long max;
  /// the minimum
  unsigned long long  min;
public:
  /// Constructor
  stat_counter(int l=2) : stat_base(l), nb(0L), max(0L), min(0L) {}
  /// Constructor
  stat_counter(const stat_counter &sc) : stat_base(sc), nb(sc.nb), max(sc.max), min(sc.min)  {}
  /// Destructor
  virtual ~stat_counter() {}
  /** reset the statistics
    */
  void reset() { 
    nb = max = min = 0;
  }
  /** Initialize the stat_counter
    * @param _n a char * to identify the counter
    * @param i the parent identifier
    * @param _l boolean tells if the stats is logged or not
    */
  void init(char _n, const Id &i,int _l=2) {
    set(_n, i,"",_l);
    nb = max = min = 0;
  }
  /** Initialize the stat_counter
    * @param _n a char  to identify the counter
    * @param i the parent identifier
    * @param _nst the label of the Statistic.
    * @param _l boolean tells if the stats is logged or not
    */
  void init(char _n, const Id &i, const char *_nst, int _l= 2) {
    set(_n, i,_nst,_l);
    nb = max = min = 0;
  }
  /** add a value to the counter
    * @param v the value to add
    */
  void add(int v) {
    nb += v;
    update_log();
  }
  /** add a value to the counter
    * @param ni the node identifier
    * @param v the value to add
    */
  void add(node_id ni,int v) {
    nb += v;
    update_log(ni);
  }
  /** add a value to the counter
    * @param ni a const char * to be logged with the stat
    * @param v the value to add
    */
  void add(const char *ni,int v) {
    nb += v;
    update_log(ni);
  }

  /** ++ operator
    */
  void operator++(int n) {
    nb++;
    update_log();
  }
  /** -- operator
    */
  void operator--(int n) {
    nb--;
    update_log();
  }
  /** += operator
    * @param n the value to add
    */
  void operator+=(int n) {
    nb += n;
    update_log();
  }
  /** += operator
    * @param n the value to remove
    */
  void operator-=(int n) {
    nb -= n;
    update_log();
  }
  /** Method to update internal value
    */
  void update_mix();
  /** Method to update internal value and log the stat
    */
  void update_log();
  /** Method to update internal value and log the stat with a char * as node info
    */
  void update_log(const char *ni);
  /** Method to update internal value and log the stat with a node identifier
    * @param ni the node identifier
    */
  void update_log(node_id ni);
  /** get the value
    */
  unsigned long long  get() const {
    return nb;
  }
  /** get the maximal of the value
    */
  unsigned long long get_max()const  {
    return max;
  }
  /** dump the stat
    * @param s the strbuff to fill
    */
  void dump(strbuff<> &s) const {
     stat_base::dump(s);
     s << nb;
  }
  /** merge method use to collect the statistics from another stat.
    * @param sc the stat_counter to collect
    */
  void merge(const stat_counter &sc) {
    nb += sc.nb;
    update_mix();
  }
  /** Pack method to Serialize the stat_counter
    * @param bs the Serialize object
    */
  virtual void Pack(Serialize &bs) const {
    DBGAFF_PCK("stat_counter::Pack", "Packing stat counter");
    bs.Pack(&nb, 1);
    bs.Pack(&max, 1);
    bs.Pack(&min, 1);
  }
  /** Unpack method to deserialize the stat_counter
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("stat_counter::UnPack", "Unpacking stat counter");
    bs.UnPack(&nb, 1);
    bs.UnPack(&max, 1);
    bs.UnPack(&min, 1);
  }

  /** Construct the log line header of the stat_counter.
    * Put the id, the name, and the type of the stat on a buffer
    * @param os the buffer
    */
  virtual void log_header(buff_st &os) const {
    stat_base::log_header(os,"COUNT");
  }

  /// operator to display the statistics
  friend std::ostream& operator<< (std::ostream& o, const stat_counter& s);
};


class stat;

/** MapCount is the type which associate a stat_counter with a
  * char key in a std::map.
  */
typedef std::map<char, stat_counter> MapCount;
/** MapTime is the type which associate a stat_timer with a
  * char key in a std::map.
  */
typedef std::map<char, stat_timer> MapTime;

/** The stat class
 *
 * The stat class It could store several stat_timer, stat_counter.
 * each of them are referenced by a char.
 * A counter could be added with the add_counter method
 * A timer could be added with the add_timer method
 * get_timer and get_counter methods are used to get a counter 
 * or a timer according to the char passed by parameter.
 */
class stat : public stat_base {
protected:
/// the map that stores all the counters
  MapCount mc;
/// the map that stores all the timers
  MapTime mt;
    
public:
  /// Constructor
  stat(): stat_base(), mc(), mt() {
    DBGAFF_MEM("stat::stat()", "Constructor");
  }
  /// Constructor
  stat(const stat &st): stat_base(st), mc(st.mc), mt(st.mt) {
    DBGAFF_MEM("stat::stat(cons stat)", "Constructor");
  }
  /** Constructor
    * @param _n the identifier for this statitic
    * @param f the father identifier
    * @param l bool if true the stat is logged
    */
  stat(char _n, const Id &f,int l=2): stat_base(_n, f,l), mc(), mt() {
    DBGAFF_MEM("stat::stat(char *,Id &)", "Constructor");
  }
  /** Constructor
    * @param _n the identifier for this statitic
    * @param f the father identifier
    * @param _nst the long name of the stat.
    * @param l bool if true the stat is logged
    */
  stat(char _n, const Id &f, const char *_nst,int l=2): stat_base(_n, f,_nst,l), mc(), mt() {
    DBGAFF_MEM("stat::stat(char *,Id &)", "Constructor");
  }
  /// Destructor
  virtual ~stat() {
    mc.clear();
    mt.clear();
    DBGAFF_MEM("stat::~stat()", "Destructor");
  }
  /** Initialize the father Id
    * @param i the father identifier
    * @param b a bool, if true the stat will be logged
    */
  void init(const Id &i,int b=2) {
    set(i);
    update(i);
  }
  /** update the internal name of the Id of all stat
   */
  void update(const Id &i,int disp=2);
  /// reset all the statistics
  void reset(); 
  /** merge method use to collect the statistics from another stat.
    * @param s the stat_counter to collect
    */
  void merge(const stat &s) ;
  /** @name Counter
    *  these methods are for counters.
    */
  //@{
  /** This method adds a counter
    * @param n the name of the counter
    * @param nst the label of the counter.
    * @param l boolean the counter is logged or not.
    */
  void add_counter(char n, const char *nst, int l=2) {
    mc[n].init(n, *this, nst, l);
  }
  /** add a value to the counter identified by n
    * @param n the identifier of the counter to modify
    * @param i the value to add to the counter
    */
  void add(char n, int i) {
    mc[n].add(i);
  }
  /** obtain the counter identifed by the char n
    * @param n the identifier
    */
  stat_counter &get_counter(char n) {
    return mc[n];
  }
  //@}
  /** @name Timer
    *  these methods are for timers.
    */
  //@{
  /** This method adds a timer
    * @param n the name of the timer
    * @param nst the label of the counter.
    * @param l true if the timer is logged
    */
  void add_timer(char n, const char *nst,int l=2) {
    mt[n].init(n, *this, nst,l);
  }
  /** obtain the timer identified by the char n
    * @param n the identifier
    */
  stat_timer &get_timer(char n) {
    return mt[n];
  }
  //@}
  /** Pack method to Serialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void Pack(Serialize &bs) const ;
  /** Unpack method to deserialize the SchedNode
    * @param bs the DeSerialize object
    */
  virtual void UnPack(DeSerialize &bs);

  /** @name the Display methods
    */
  //@{
  /** Construct the log header for the stat.
    * write on the log the log header of each stat_timer ou each stat_counter 
    * stored on the stat object
    */
  void log_header()const ;
  /** dump the stat
    * @param s the strbuff to fill
    */
  void dump(strbuff<> &s) const ;

  /** The friend operator<< to display a stat
    */
  friend std::ostream& operator<< (std::ostream& o, const stat& s);
  /** old method to display a stat
    */
  void display(ostream &os) {
    display_title(os);
    display_label(os);
    display_data(os);
  }
  /** method to display the title of the stat
    */
  void display_title(ostream &os);

  /** method to display the labels of the stat
    */
  void display_label(ostream &os,const char *lib="",int size=0);
  /** method to display the data of the stat.
    */
  void display_data(ostream &os,const char *lib="",int size=0);
  //@}
};

/** inline method to display a timer on a std::ostream
 */
inline std::ostream& operator<< (std::ostream& o, const stat_timer& s) {
  o << s.str() << "-t:" << s.cumt << "(" << s.nbr << "," << (s.cumt / s.nbr) << ")";
  return o;
}

/** inline method to display a counter on a std::ostream
 */
inline std::ostream& operator<< (std::ostream& o, const stat_counter & s) {
  o << s.str() << "-c:" << s.nb << "(" << s.max << "," << s.min << ")";
  return o;
}

/** inline method to display a stat on a std::ostream
 */
inline std::ostream& operator<< (std::ostream& o, const stat& s) {
  o << s.str() << "-s {\n";
  for (MapCount::const_iterator i = s.mc.begin(); i != s.mc.end() ; i++) {
    o << i->second << "\n";
  }
  for (MapTime::const_iterator i = s.mt.begin(); i != s.mt.end() ; i++) {
    o << i->second << "\n";
  }
  o << "}\n";
  return o;
}

/** MapStat is the type which associate a stat * with a
  * int key in a std::map.
  */
typedef std::vector<stat> VecStat;


/** class to store different stat classes.
  * this class is usefull for example to store the stats 
  * for one algorithm or for several algorithms.
  */
class stat_set {
   /// Algo stat
   VecStat as;
   /// priority queue stat
   VecStat pqs;
   /// Goal stat
   VecStat gs;
public:
  /** Constructor
    * the default size of the three map is 1.
    */
  stat_set(int n=1) : as(n),pqs(n),gs(n) {}
  /** Destructor
    */
  virtual ~stat_set() {
  }
  /** resize the map associated with algorithm
    * @param n the number of algorithm stat
    */
  void resize_algo_stat(int n) { as.resize(n); }
  /** resize the map associated with algorithm
    * @param n the number of priority queue stat
    */
  void resize_pq_stat(int n) { pqs.resize(n); }
  /** resize the map associated with algorithm
    * @param n the number of goal stat
    */
  void resize_goal_stat(int n) { gs.resize(n); }
  /** get the map associated with algorithm
    * @returns the algorithm's stat map
    */
  VecStat &get_algo_stats() { return as; }
  /** get the map associated with priority queue
    * @returns the priority queue's stat map
    */
  VecStat &get_pq_stats() { return pqs; }
  /** get the map associated with goal
    * @returns the goal stat map
    */
  VecStat &get_goal_stats() { return gs; }
  /** merge an algorithm stat
    * @param st the stat to merge
    * @param i the index of the merged stat
    */
  void merge_algo_stat(const stat &st, int i=0) {
    as[i].merge(st);
  }
  /** merge a priority queue stat
    * @param st the stat to merge
    * @param i the index of the merged stat
    */
  void merge_pq_stat(const stat &st, int i=0) {
    pqs[i].merge(st);
  }
  /** merge a goal stat
    * @param st the stat to merge
    * @param i the index of the merged stat
    */
  void merge_goal_stat(const stat &st, int i=0) {
    gs[i].merge(st);
  }
  /** get an algorithm stat
    * @param n the index of the algorithm stat
    */
  stat &get_algo_stat(int n=1) { return as[n]; }
  /** get a pq stat
    * @param n the index of the pq stat
    */
  stat &get_pq_stat(int n=1) { return pqs[n]; }
  /** get a goal stat
    * @param n the index of the goal stat
    */
  stat &get_goal_stat(int n=1) { return gs[n]; }

};

/**
 * @}
 */

};
#endif
