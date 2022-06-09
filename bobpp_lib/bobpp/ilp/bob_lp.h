/*
 * bob_lp.h
 *
 * This source include the data type for the Branch and Bound Algo level.
 * This the unit of task scheduled by the Kernel class.
 */

#include <vector>


#ifndef BOBLP_ABSTRACT
#define BOBLP_ABSTRACT

/** @defgroup LP_IO Structures and functions used to Pack and Unpack LP data.
 * @ingroup LP_Class
 * Structures to pack and to unpack the data using the glop interface
 * @{
 */
/** Gup Serialize 
 */
struct _gup_serialize {
  /// pointer on the bobpp serialization object.
  Bob::Serialize *s;
  /** Constructor
   */
  _gup_serialize(Bob::Serialize *_s): s(_s) {}
  /** Destructor
   */
  ~_gup_serialize() {}
};

/** Gup deSerialize 
 */
struct _gup_deserialize {
  /// pointer on the bobpp deserialization object.
  Bob::DeSerialize *d;
  /** Constructor
   */
  _gup_deserialize(Bob::DeSerialize *_d): d(_d) {}
  /** Destructor
   */
  ~_gup_deserialize() {}
};

/** pack an array of char */
int gup_pack_char(gup_serialize * s, const char *p, int count);

/** pack an array of int */
int gup_pack_int(gup_serialize * s, const int *p, int count);

/** pack an array of double */
int gup_pack_double(gup_serialize * s, const double *p, int count);

/** unpack an array of char */
int gup_unpack_char(gup_deserialize * s, char *p, int count);

/** unpack an array of int */
int gup_unpack_int(gup_deserialize * s, int *p, int count);

/** unpack an array of double */
int gup_unpack_double(gup_deserialize * s, double *p, int count);

/**
 * @}
 */


namespace  Bob {

/** @defgroup LP_Class LP class
 * @ingroup LPBBAlgoGRP
 * @{
 */


/** Precision Macro
 * \todo put this accuracy constant in the Bob::core
 */
#define ACCURACY  0.000001

/**
 * The Statistics for LP statistics
 */
class LPStat : public BBStat {
public:
  /// Constructor
  LPStat(const Id &s) : BBStat(s) {
    add_timer('r', "Resolution");
  }
  /// Destructor
  virtual ~LPStat() {}
  /// Start LP resolution time_stat
  void lp_start() {
    get_timer('r').start();
  }
  /// Stop LP resolution time_stat
  void lp_end() {
    get_timer('r').end();
  }
};



/** Class used to represent an LP.
 * Mainly this class is used to store the initial linear program, and is used
 * also to store an LP that could be copy or modify.
 * This class is a derived class of the pb class includes in the glop library 
 * (https://software.prism.uvsq.fr/glop/).
 */
class LP : public ppglop::pb {
  bool serialized;
  bool integer;
protected:
  /// Boolean to know if the integer test has been done.
  bool integer_done;
public:
  //static int nbLP;
  /** Constructor
   */
  LP() : ppglop::pb("noname"), serialized(false), integer(false) , integer_done(false) {
    DBGAFF_USER("LP::LP()", "Called");
    //nbLP++;
    //std::cout <<" MEMORY LP Created"<<nbLP<<"\n";
  }
  /** Constructor
   */
  LP(const LP & n) : ppglop::pb(n), serialized(n.isSerialized()),
      integer(n.isInteger()) , integer_done(n.integer_done) {
    DBGAFF_USER("LP::LP(const LP &n)", "Called");
    //nbLP++;
    //std::cout <<" MEMORY LP Created & Copy"<<nbLP<<"\n";
  }
  /** Destructor
   */
  virtual ~LP();
  /** Initialiaze an LP reading a file .mps or .lp
   */
  void read(const char *fname);
  /** Normalize the LP
   */
  void Normalize();
  /** Solve the Linear program
   */
  template<class TheStat>
  int one_solve(TheStat *stat, int dist, double bs) {
    int ret;
    DBGAFF_USER("LP::onesolve()", "Called");
    set_obj_upper_limit(bs);
    set_verbosity(0);
    stat->lp_start();
    ret = ppglop::pb::solve();
    integer_done = false;
    stat->lp_end();
    return ret;
  }

  /** Solve the Linear program
   */
  template<class TheStat>
  int solve(TheStat *stat, int dist, double bs) {
    int ret;
    DBGAFF_USER("LP::solve()", "Called");
    set_obj_upper_limit(bs);
    set_verbosity(0);
    stat->lp_start();
    ret = ppglop::pb::solve();
    integer_done = false;
    stat->lp_end();
    return ret;
  }

  /** Test if the LP is feasible or not.
   */
  bool isFeasible() {
    return get_status() == GSP_LP_FEAS || get_status() == GSP_LP_OPT;
  }
  /** return true if all the variables are integer
   */
  bool isInteger() const {
    return integer;
  }
  /** Test if variables are integer or not.
   */
  bool do_test_integer();
  /** return true if the LP has been serialized
   */
  bool isSerialized() const {
    return serialized;
  }
  /** the Prt method
   */
  virtual ostream &Prt(ostream &os = std::cout) const {
    os << "LP Objective value: " << get_obj_val() << std::endl;
    os << "nrows = " << get_num_rows() << std::endl;
    os << "ncols = " << get_num_cols() << std::endl;
    os << "Evaluation = " << get_obj_val() << std::endl;
    //os << "n_cuts = " << nc << endl;
    return os;
  }

  /// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
    DBGAFF_USER("LP::Pack()", "Called");
    bs.Pack(&serialized, 1);
    bs.Pack(&integer, 1);
    bs.Pack(&integer_done, 1);
    gup_serialize s(&bs);
    this->pack(&s);
    //this->print();
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs) {
    DBGAFF_USER("LP::UnPack()", "Called");
    bs.UnPack(&serialized, 1);
    bs.UnPack(&integer, 1);
    bs.UnPack(&integer_done, 1);
    gup_deserialize s(&bs);
    this->unpack(&s);
    //this->print();
  }

};




/** The Linear solver with the Cut and variables generation
  * \brief LP with cut/var generation
  *
  * The goal of this class is to store the linear program.
  * But when the solve method is invoked, the cut and the var generators algorithms
  * are also invoked.
  *
  * This class use a CutGenerator object. The class CutGenerator
  * is defined in the Trait class.
  * The solve method uses the CutGenerator::do_loop method in order to add
  * the cuts.
  * This class use a VarGenerator object. The class VarGenerator
  * is defined in the Trait class.
  * The solve method uses the VarGenerator::do_loop method in order to add
  * the cuts.
  */
template<class Trait>
class LPvarcut : public LP {
  typedef typename Trait::CutGenerator TheCutGenerator;
  typedef typename Trait::Instance TheInstance;
  typedef typename Trait::Stat TheStat;
  TheCutGenerator cg;

public:
  /** Constructor
    */
  LPvarcut() : LP(), cg() {}
  /** Constructor
    */
  LPvarcut(const LPvarcut &n) : LP(n), cg() {}
  /**Destructor
    */
  virtual ~LPvarcut() {}
  /** Solve method
    * @param stat the statistics
    * @param dist the level of the associated node
    * @param b_val the value of best known solution.
    * @return
    */
  virtual int solve(TheStat *stat, int dist, double b_val) {
    DBGAFF_USER("LPvarcut::solve()", "Called");
    cg_impliedBounds(0);
    cg_reduceCoef(0);
    set_obj_upper_limit(b_val);
    LP::one_solve(stat, dist, b_val);
    if (get_status() != GSP_LP_OPT && get_status() != GSP_LP_FEAS) {
      return get_status();
    }
    cg.do_loop(this, stat, 0, dist, b_val);
    return get_status();
  }
};


/** @}
 */

};

#endif
