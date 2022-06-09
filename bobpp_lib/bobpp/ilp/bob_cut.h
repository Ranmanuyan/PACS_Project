
/*
 * bob_cut.h
 *
 * This source is a part of the Bob++ LP based Branch and Bound Algo level.
 */

#include <vector>

#ifndef BOBLP_W_CUT
#define BOBLP_W_CUT

namespace  Bob {


/** @defgroup LPBB_GENCUT Generic Cutting classes
 *  @ingroup LPBBAlgoGRP
 * @{
 * This file stores all the classes use to be able to add cuts to the
 * linear program in order to accelerate a Mixed integer program resolution.
 * This code use the Glock cut functions.
 * The Linear program resolution is based on the Glop library.
 * THis part of the library is uses three groups of classes 
 *    - the CC (CutControl) classes that controls the cuts procedure (CutControl, CC_RootOnlyIteFixed CC_All).
 *    - the CutGenerator classes that call the Glop cuts generation function.
 *    - the CutGenerator classes that call other CutGenerator classes to be
 *      able to add different types of cuts with different Cutcontrol.
 *
 */

/** @defgroup LBBB_GENCUTCONTROL Cut Control classes
 * @{
 */

/** Abstract structure that controls the execution of a cut genrator
 */
struct CutControl {
  /** Destructor
    */
  virtual ~CutControl() {}
  /** start method return true if the Cut generator could be executed.
    * @param lp the Linear programm
    * @param b_val the value of the best known solution
    * @param level the level of the node
    * @return true if the Cut generator could be executed, false otherwise.
    */
  virtual bool start(Bob::LP *lp, double b_val, int level) = 0;
  /** end method return true if the Cut generator loop must finish.
    * @param ite the number of iterations of Cut generator loop
    * @param lp the Linear programm
    * @param oobj_val the value of the linear program before the generation of the cuts
    * @param nb_cut number of cuts added
    * @return true if the Cut generator loop must end.
    */
  virtual bool end(int ite, Bob::LP *lp, double oobj_val, int nb_cut) = 0;
};

/** Cut control structure only on the Root and for a maximal number of iteration
 */
struct CC_RootOnly_IteFixed {
  /// Number of iterations
  int max_ite;

  /** Constructor
    */
  CC_RootOnly_IteFixed(const char *s) {
    max_ite = Bob::core::opt().NVal(s, "-emi");
  }
  /** Destructor
    */
  virtual ~CC_RootOnly_IteFixed() {
  }
  /** start method return true if the level is less than 1.
    * @param lp the Linear programm
    * @param b_val the value of the best known solution
    * @param level the level of the node
    * @return true the level is less than 1
    */
  virtual bool start(Bob::LP *lp, double b_val, int level) {
    return level < 1;
  }
  /** end method return true if the Cut generator loop must finish.
    * @param ite the number of iterations of Cut generator loop
    * @param lp the Linear programm
    * @param oobj_val the value of the linear program before the generation of the cuts
    * @param nb_cut number of cuts added
    * @return true if the Cut generator loop must end.
    */
  virtual bool end(int ite, Bob::LP *lp, double oobj_val, int nb_cut) {
    return nb_cut == 0 || (lp->get_obj_val() - oobj_val) < ACCURACY || ite >= max_ite;
  }
  /** static method to define option on the command line
   * @param optn the name of the group option.
   */
  static void opt(const char *optn) {
    core::opt().add(std::string(optn), Property("-emi", "the maximal number of iterations to stop the cut generation", 10));
  }

};

/** Structure that stores all the informations that control
 * the cut generations procedure execution.
 */
struct CC_All {
  /// max level to start the cut generation
  int max_level;
  /// maximal number of fractional variables to start
  int max_frac_var;
  /// Gap (value of the bound according to the best known solution)
  double gap_start;
  /// Gap (value of the new bound according to the value of the bound before adding cuts)
  double gap_end;
  /// Maximal number of cuts added
  int max_nb_cuts;
  /// Maximal number of iterations
  int max_this_cut;

  /** Constructor
    */
  CC_All(const char *s) {
    max_level = Bob::core::opt().NVal(s, "-sml");
    max_frac_var = Bob::core::opt().NVal(s, "-smfv");
    gap_start = Bob::core::opt().DVal(s, "-sg");
    gap_end   = Bob::core::opt().DVal(s, "-eg");
    max_nb_cuts = Bob::core::opt().NVal(s, "-emc");
    max_this_cut = Bob::core::opt().NVal(s, "-emi");
  }
  /** Destructor
    */
  virtual ~CC_All() {
  }
  /** start method return true if the level is less than 1.
    * @param lp the Linear programm
    * @param b_val the value of the best known solution
    * @param level the level of the node
    * @return true the level is less than 1
    */
  virtual bool start(Bob::LP *lp, double b_val, int level) {
    return ((b_val -lp->get_obj_val()) / b_val) < gap_start ||
           level < max_level || lp->get_num_bin_frac() < max_frac_var;
  }
  /** end method return true if the Cut generator loop must finish.
    * @param ite the number of iterations of Cut generator loop
    * @param lp the Linear programm
    * @param oobj_val the value of the linear program before the generation of the cuts
    * @param nb_cut number of cuts added
    * @return true if the Cut generator loop must end.
    */
  virtual bool end(int ite, Bob::LP *lp, double oobj_val, int nb_cut) {
    return nb_cut == 0 || ((lp->get_obj_val() - oobj_val) / lp->get_obj_val()) < gap_end ||
           nb_cut > max_nb_cuts || ite > max_this_cut;
  }
  /** 
   * static method to define the option of the cut control generation strategy
   * @param optn the name of the group option.
   */
  static void opt(const char *optn) {

    core::opt().add(std::string(optn), Property("-sml", "the maximal level to start the cut generation", 1));
    core::opt().add(std::string(optn), Property("-smfv", "the maximal number of fractional variables to start the cut generation", 100));
    core::opt().add(std::string(optn), Property("-sg", "the maximal gap to start the cut generation", 0.10));
    core::opt().add(std::string(optn), Property("-eg", "the maximal gap to stop the cut generation", 0.05));
    core::opt().add(std::string(optn), Property("-emc", "the maximal number of cuts generated to stop the cut generation", 20));
    core::opt().add(std::string(optn), Property("-emi", "the maximal number of iterations to stop the cut generation", 10));
  }
};

/**
 * @}
 */
/** @defgroup CUTGEN Cut Generator classes
 * @{
 */


/** the size of a cut generator name
 */
#define MAX_CUT_NAME 20
/** The Cut Generator class
  * \brief the abstract class that adds one type of cuts
  */
template<class Trait, class CutControl>
class CutGenerator {
  CutControl ci;
  char cut_name[MAX_CUT_NAME];
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  CutGenerator() : ci() {
    cut_name[0] = 0;
  };
  /** Constructor
    * @param opt the name of the cut generator
    */
  CutGenerator(const char *opt) : ci(opt) {
    strcpy(cut_name, opt);
  };
  /** Constructor
    */
  virtual ~CutGenerator() { }
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) = 0;
  /** Method that performs the loop that calls the add_cut method
    */
  virtual int do_loop(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    if (ci.start(lp, b_val, dist)) {
      double oobj_val = lp->get_obj_val();
      int ite = 0;
      int nbcut = 0, nbci;
      do {
        oobj_val = lp->get_obj_val();
        nbci = add_cut(lp, st, verbose, dist, b_val);
        if (nbci == 0) {
          break;
        }
        nbcut += nbci;
        lp->one_solve(st, dist, b_val);
        ite++;
        printf("%15s %20.3f %7.2f %7d %9d %13.2f\n", cut_name, lp->get_obj_val(), (lp->get_obj_val() - oobj_val) / oobj_val, nbcut, ite, 0.0);
      } while (!ci.end(ite, lp, oobj_val, nbcut)) ;
      return nbcut;
    }
    return 0;
  }
  /** The Static method to declare the Bobpp Options
   * @param cut_name the name of the cut.
    */
  static void opt(const char *cut_name) {
    CutControl::opt(cut_name);
  }
};

/** The Cut Generator class to generate nothing !
  * \brief A dummy class that generates no cuts.
  */
template<class Trait, class CutControl>
class NoCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  NoCutGenerator() : CutGenerator<Trait, CutControl>("--no") { };
  /** Destructor
     */
  virtual ~NoCutGenerator() { };

  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generation
    * \param verbose the verbosity of the cut generation.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    return 0;
  }
  /** opt() is a static methods used to add option on the application if needed
    */
  static void opt() {
  }
};

/** The Mixed Integer Rounding generation class
  * \brief class that generate the Mixed Integer Rounding cuts.
  */
template<class Trait, class CutControl>
class MixedIntegerRoundingCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  MixedIntegerRoundingCutGenerator() : CutGenerator<Trait, CutControl>("--cut_mir") { };
  /** Destructor
    */
  virtual ~MixedIntegerRoundingCutGenerator()  { };
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    *  see the verbose parameter of the associated Glop call.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_mir(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_mir");
  }
};

/** The Flow Cover generation class
  * \brief class that generate the Flow cover cuts.
  */
template<class Trait, class CutControl>
class FlowCoverCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  FlowCoverCutGenerator() : CutGenerator<Trait, CutControl>("--cut_flowcover") { };
  /** Destructor
    */
  virtual ~FlowCoverCutGenerator() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_flow_cover(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_flowcover");
  }
};

/** The Lift and Project generator class
  * \brief class that generate the Lift and Project cuts.
  */
template<class Trait, class CutControl>
class LiftandProjectCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  LiftandProjectCutGenerator() : CutGenerator<Trait, CutControl>("--cut_lap") { };
  /** Destructor
    */
  virtual ~LiftandProjectCutGenerator() { }
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_LandP(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_lap");
  }
};

/** The Gomory Fractional generator class
  * \brief class that generate the Gomory Fractional.
  */
template<class Trait, class CutControl>
class GomoryFractionalCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  GomoryFractionalCutGenerator() : CutGenerator<Trait, CutControl>("--cut_gomfr") { };
  /** Destructor
    */
  virtual ~GomoryFractionalCutGenerator()  { };
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_gomory_fractional(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_gomfr");
  }
};

/** The Gomory Mixed Integer generator class
  * \brief class that generate the Gomory Mixed Integer.
  */
template<class Trait, class CutControl>
class GomoryMixedIntegerCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  GomoryMixedIntegerCutGenerator() : CutGenerator<Trait, CutControl>("--cut_gomi") { };
  /** Destructor
    */
  virtual ~GomoryMixedIntegerCutGenerator() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_gomory_mixed_integer(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_gomi");
  }
};


/** The Knapsack cover generator class
  * \brief class that generate the Knapsack cover.
  */
template<class Trait, class CutControl>
class KnapsackCoverCutGenerator : public CutGenerator<Trait, CutControl> {
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  KnapsackCoverCutGenerator() : CutGenerator<Trait, CutControl>("--cut_knapsac") { };
  /** Destructor
    */
  virtual ~KnapsackCoverCutGenerator() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generator
    * \param verbose the verbosity of the cut generator.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int ret;
    st->cut_start();
    ret = lp->cg_knapsack_cover(verbose);
    st->cut_end();
    return ret;
  }
  /** static method to define option on the command line
   */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cut_knapsac");
  }
};

/** The Cut Generator class to compose the generation of two types of cuts !
  * \brief Cuts Genrator for 2 types of cuts.
  */
template<class Trait, class cg1, class cg2, class CutControl>
class CutGenerator2 : public CutGenerator<Trait, CutControl> {
  cg1 c1;
  cg2 c2;
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  CutGenerator2() : CutGenerator<Trait, CutControl>("--cg2"), c1(), c2() { };
  /** Destructor
    */
  virtual ~CutGenerator2() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generation
    * \param verbose the verbosity of the cut generation.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int nbcut = 0;
    nbcut += c1.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c2.do_loop(lp, st, verbose, dist, b_val);
    st.addcut(nbcut);
    return nbcut;
  }
  /** opt() is a static methods used to add option on the application if needed
    */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cg2");
    cg1::opt();
    cg2::opt();
  }
};

/** The Cut Generator class to compose the generation of three types of cuts !
  * \brief Cuts Genrator for 3 types of cuts.
  */
template<class Trait, class cg1, class cg2, class cg3, class CutControl>
class CutGenerator3 : public CutGenerator<Trait, CutControl> {
  cg1 c1;
  cg2 c2;
  cg3 c3;
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  CutGenerator3() : CutGenerator<Trait, CutControl>("--cg3"), c1(), c2(), c3() { };
  /** Destructor
    */
  virtual ~CutGenerator3() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generation
    * \param verbose the verbosity of the cut generation.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int nbcut = 0;
    nbcut += c1.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c2.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c3.do_loop(lp, st, verbose, dist, b_val);
    return nbcut;
  }
  /** opt() is a static methods used to add option on the application if needed
    */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cg3");
    cg1::opt();
    cg2::opt();
    cg3::opt();
  }
};

/** The Cut Generator class to compose the generation of three types of cuts !
  * \brief Cuts Genrator for 4 types of cuts.
  */
template<class Trait, class cg1, class cg2, class cg3, class cg4, class CutControl>
class CutGenerator4 : public CutGenerator<Trait, CutControl> {
  cg1 c1;
  cg2 c2;
  cg3 c3;
  cg4 c4;
public:
  /// type definition of the LP
  typedef typename Trait::LP TheLP;
  /// type definition of the statistics
  typedef typename Trait::Stat TheStat;
  /** Constructor
    */
  CutGenerator4() : CutGenerator<Trait, CutControl>("--cg4"), c1(), c2(), c3(), c4() { };
  /** Destructor
    */
  virtual ~CutGenerator4() {}
  /** The virtual method that adds cut to the LP
    * \param lp the Linear program
    * \param st the stat that stores the activity of the cut generation
    * \param verbose the verbosity of the cut generation.
    * \param dist the depth of the node in the search tree
    * \param b_val the value of the incubent
    *
    * \return true if the method has inserted a cut otherwise false.
    */
  virtual int add_cut(TheLP *lp, TheStat *st, int verbose, int dist, double b_val) {
    int nbcut = 0;
    nbcut += c1.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c2.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c3.do_loop(lp, st, verbose, dist, b_val);
    nbcut += c4.do_loop(lp, st, verbose, dist, b_val);
    return nbcut;
  }
  /** opt() is a static methods used to add option on the application if needed
    */
  static void opt() {
    CutGenerator<Trait, CutControl>::opt("--cg4");
    cg1::opt();
    cg2::opt();
    cg3::opt();
    cg4::opt();
  }
};
/**
 * @}
 */


/** @}
 */

};
#endif

