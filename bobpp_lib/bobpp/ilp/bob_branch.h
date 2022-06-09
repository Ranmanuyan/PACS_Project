
/*
 * bob_branch.h
 *
 * Branching for the B&X
 *
 */

#ifndef BOBBXBR_ABSTRACT
#define BOBBXBR_ABSTRACT

namespace  Bob {

/** @defgroup LPBB_Branching Branching classes
 * @ingroup LPBBAlgoGRP
 * @{
 * This file stores all the classes used to perform the branching strategy
 * envolved in a a Mixed integer program resolution.
 * Two groups of classes are defined.
 *    - the classes that stores the information to branch called BranchingInfo (@ref LPBB_Branching_Info)
 *    - the classes that compute the information stored by the first (@ref LPBB_Branching_Strategy).
 *
 * At this time, the BranchingInfo classes proposes, are the Generic_BranchingInfo and 
 * the DichoBranchingInfo (v_i<=d or v_i>=d+1)
 * The branching strategy is to fixe i and d, the library proposes at this time
 * Most fractional, Strong branching, Entropy Branching strategies.
 */

/** @defgroup LPBB_Branching_Info Branching Information classes
 * @{
 */

/** Generic Branching Info
 *  This class is an abstract class that stores the information
 *  used to perform the branching procedure.
 */
template<class Trait>
class Generic_BranchingInfo {
  typedef typename Trait::LP TheLP;
  typedef typename Trait::Node TheNode;

public:
  /// Constructor
  Generic_BranchingInfo() {}
  /// Destructor
  virtual ~Generic_BranchingInfo() {}
  /** method that performs the ith branch on a LP.
    * @param i the index of the branch
    * @param n the linear program to modify
    */
  virtual void do_branch(int i, TheNode *n) = 0;
  /** Method that returns the number of children
    */
  virtual int nbChildren() = 0;
  /** Method to pack the information
    * @param bs the Bob::Serialize object
    */
  virtual void Pack(Bob::Serialize &bs) const = 0;
  /** Method to unpack the information
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs) = 0;
};

/** Dichotomic Branching Info
 *  This class is a concrete class that stores the information
 *  used to perform a Dichotomic branching.
 *  The Branching strategy creates 2 new nodes according to the index of the variable i
 *  and the value v.  On the first node, the uuper bound of the variable i is v, on
 *  the second the lower bound of the variable i is v.
 */
template<class Trait>
class DichoBranchingInfo : public Generic_BranchingInfo<Trait> {
  int var;
  double val;
public :
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the LP.
  typedef typename Trait::Node TheNode;

  /// Constructor
  DichoBranchingInfo(): var(-1), val(0.0) { }
  /** Constructor
   * @param _v the index of the variable to branch
   * @param _d the value of the variable.
   */
  DichoBranchingInfo(int _v, double _d): var(_v), val(_d) { }
  /** Destructor
    */
  virtual ~DichoBranchingInfo() {}
  /** returns the index of the variable to branch
    * @return the index of the variable on the LP
    */
  int get_var() {
    return var;
  }
  /** Sets the index of the variable to branch
    * @param _v the index of the variable on the LP
    */
  void set_var(int _v) {
    var = _v;
  }
  /** returns the value of the variable to branch
    * @return the value of the bound of the variable on the LP
    */
  double get_val() {
    return val;
  }
  /** Sets the value of variable to branch
    * @param _d the value of the bound of the variable on the LP
    */
  void set_val(double _d) {
    val = _d;
  }
  /** method that performs the ith branch on a LP.
    * @param i the index of the branch
    * @param n the linear program to modify
    */
  virtual void do_branch(int i, TheNode *n) {
    TheLP *lp = n->Lp();
    DBGAFF_USER("MipNode::addCut()", "Called");
    double bd;
    if (i == 0) {
      bd = ceil(val);
      lp->set_col_bounds(&bd, NULL, var, var);
      //cout<<"do_branch: "<<lp->get_col_name(var)<<" > "<<bd<<endl;
    } else {
      bd = floor(val);
      lp->set_col_bounds(NULL, &bd, var, var);
      //cout<<"do_branch: "<<lp->get_col_name(var)<<" < "<<bd<<endl;
    }
  }
  /** Here this method returns 2.
    * @return the number of children.
    */
  virtual int nbChildren() {
    return 2;
  }
  /** Method to pack the information
    * @param bs the Bob::Serialize object
    */
  virtual void Pack(Bob::Serialize &bs) const {
    bs.Pack(&var, 1);
    bs.Pack(&val, 1);
  }
  /** Method to unpack the information
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    bs.UnPack(&var, 1);
    bs.UnPack(&val, 1);
  }
};

/**
 * @}
 */

/** @defgroup LPBB_Branching_Strategy Branching Strategy classes
 * @{
 * This group proposes different Dichotomic Branching Strategies.
 */


/**
 * Abstract Base class for implementation of branching rules
 * this class proposes the branch method that admit a LP to initialize the
 * BranchingInfo object.
 */
template<class Trait>
class GenericBranching {
  char name[10];
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the statistics.
  typedef typename Trait::Stat TheStat;
  /** Constructor
    * @param _n the name of the branching strategy
    */
  GenericBranching(const char *_n = 0) {
    if (_n != 0)
      strcpy(name, _n);
    else
      strcpy(name, "noname");
  }
  /** Destructor
    */
  virtual ~GenericBranching() {}
  /** Test to compare the strategy name with another
    * @param _n the name of the branching strategy
    * @return true if the names are the same.
    */
  virtual bool is_same(const char *_n) {
    return strcmp(_n, name) == 0;
  }
  /** Fixe the value in the Branching Information object using the LP
   * @param lp the Linear Program
   * @param bi the Branching Information
   * @param st the statistics object
   * @param dist the level of the node.
   * @param b_val value of the incubent.
   *
   * @return
   */
  virtual int branch(TheLP *lp, TheBranchingInfo *bi, TheStat &st, int dist, double b_val) const = 0;
  /** Method to pack the information
    * @param bs the Bob::Serialize object
    */
  virtual void Pack(Bob::Serialize &bs) const {
    int s = strlen(name);
    Bob::Pack(bs, &s, 1);
    Bob::Pack(bs, name, s);
  }
  /** Method to unpack the information
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    int s;
    Bob::UnPack(bs, &s);
    Bob::UnPack(bs, name, s);
  }
};



/**
 * Most Fractional variable branching rule.
 * The goal is to branch on the variable which is the closest to 0.5
 */
template<class Trait>
class MostFracBranching : public GenericBranching<Trait> {
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;

  /** Constructor
   */
  MostFracBranching() : GenericBranching<Trait>("mf") {  }
  /** Destructor
    */
  virtual ~MostFracBranching() {}
  /** Fixe the value in the Branching Information object with the LP
   * @param lp the Linear Program
   * @param bi the Branching Information
   * @param st the statistics object
   * @param dist the level of the node.
   * @param b_val the value of the incubent.
   *
   * @return 1 if the branching is ok
   */

  virtual int branch(TheLP *lp, TheBranchingInfo *bi, TheStat &st, int dist, double b_val) const {
    double frc, val;
    int ncols = lp->get_num_cols();
    int kind[ncols];
    int j, nf;
    int fracv[ncols];
    int bestvar;
    double bestval;
    double bestfrc;
    //double bestfrc2, bestvar2;

    lp->get_col_kind(kind, 0, ncols - 1);
    bestvar = -1;
    bestval = 0.0;
    bestfrc = 0.0;
    nf = 0;
    for (j = 0; j < ncols; j++) {
      if (kind[j] != GSP_IV)
        continue;
      lp->get_col_prim(&val, j, j);
      frc = fabs(val - nearbyint(val));
      if (frc >= ACCURACY) {
        fracv[nf++] = j;
        if (frc > bestfrc) {
          bestfrc = frc;
          bestvar = j;
          bestval = val;
        }
      }
    }
    bi->set_var(bestvar);
    bi->set_val(bestval);
    return 1;
  }
  /** Static method opt() to eventualy declare the options of the 
   * branching strategy for the application
   */
  static void opt() {
  }
};


/**
 * Generic Strong Branching (score functions must be implemented)
 * the goal of the strong braning is to perform limited resolution 
 * of LP, to test the best variable to branch.
 */
template<class Trait>
class GenericStrongBranching : public GenericBranching<Trait> {
  mutable int n_iter, n_lp;
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;
  /** Constructor
   */
  GenericStrongBranching(const char *_n = 0) : GenericBranching<Trait>(_n), n_iter(0), n_lp(0) {}
  /** Destructor
    */
  virtual ~GenericStrongBranching() {}
  /** Fixe the value in the Branching Information object with the LP
   * @param lp the Linear Program
   * @param bi the Branching Information
   * @param st the statistics object
   * @param dist the level of the node.
   * @param b_val the value of the incubent.
   *
   * @return 1 if the branching is ok
   */
  virtual int branch(TheLP *lp, TheBranchingInfo *bi, TheStat &st, int dist, double b_val) const {
    double frc, bd, lb, ub;
    int ret, status;
    double curscore, bestscore = -1e+300;
    int ncols = lp->get_num_cols();
    int kind[ncols];
    double val[ncols];
    double z[2];
    int bestvar;
    double bestval;
    int fracv[ncols];
    int i, j, k, nf, itlim;
    int iter, iter2;
    //double varscore, d;

    lp->get_col_kind(kind, 0, ncols - 1);
    bestvar = -1;
    bestval = 0.0;
    lp->get_col_prim(val, 0, ncols - 1);
    iter2 = lp->get_iteration_count();
    nf = 0;
    for (j = 0; j < ncols; j++) {
      if (kind[j] != GSP_IV)
        continue;
      frc = val[j] - floor(val[j]);
      if ((0.5 - fabs(0.5 - frc)) >= ACCURACY)
        fracv[nf++] = j;
    }
    //double localscore[nf];
    for (i = 0; i < nf; i++) {
      j = fracv[i];
      frc = val[j] - floor(val[j]);
      if ((0.5 - fabs(0.5 - frc)) >= ACCURACY) {
        lp->get_lb(&lb, j, j);
        lp->get_ub(&ub, j, j);
        bd = floor(val[j]);
        for (k = 0; k < 2; k++) {
          // k = 0 : Set upper bound to floor value of frac variable
          // k = 1 : Set lower bound to ceil value of frac variable
          lp->set_col_bounds(&((k == 0) ? lb : bd), &((k == 0) ? bd : ub), j, j);
          itlim = n_lp ? (n_iter * 2 / n_lp) : 0;
          lp->set_iteration_limit(itlim);
          ret = lp->one_solve((LPStat *) & st, dist, b_val);
          iter = lp->get_iteration_count();
          n_iter += iter - iter2;
          //cout << "iteration limit=" << itlim << "\tn=" << iter - iter2 << endl;
          n_lp ++;
          iter2 = iter;
          status = lp->get_status();
          if (ret == GSP_E_OBJUL || (ret == 0 && status == GSP_LP_NOFEAS)) {
            bestvar = j;
            bestval = val[j];
            lp->set_col_bounds(&lb, &ub, j, j);
            goto endofsearch;
          } else if (ret != 0 && ret != GSP_E_ITLIM) {
            cout << "branch:gsp_solve: " << gup_const_string(ret) << endl;
            abort();
          }
          bd += 1.0;
          z[k] = eval(lp);
        }
        lp->set_col_bounds(&lb, &ub, j, j);
        curscore = score(frc, z);
        //localscore[i] = curscore;
        if (curscore > bestscore) {
          bestscore = curscore;
          bestvar = j;
          bestval = val[j];
        }
      }
    }
endofsearch:
    lp->set_iteration_limit(0);
    bi->set_var(bestvar);
    bi->set_val(bestval);
    return 1;
  }
  /** Evaluation of subproblem in the sense of the score function
   * @param lp TheLp used to evaluate the branch
   */
  virtual double eval(TheLP *lp) const = 0;
  /** The score function that computes the score for one variable according 
   * to the evaluations of the two subproblems obtained with branching on 
   * this variable.
   * @param frc rating of one branch against the other one (generally not used).
   * @param z array that stores the two evaluations of the two branches.
   * @return the score of the variable.
   */
  virtual double score(double frc, double z[2]) const = 0;

  /** Method to pack the information
    * @param bs the Bob::Serialize object
    */
  virtual void Pack(Bob::Serialize &bs) const {
    GenericBranching<Trait>::Pack(bs);
    bs.Pack(&n_iter, 1);
    bs.Pack(&n_lp, 1);
  }
  /** Method to unpack the information
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs) {
    GenericBranching<Trait>::UnPack(bs);
    bs.UnPack(&n_iter, 1);
    bs.UnPack(&n_lp, 1);
  }
};

/**
 * Standard Strong Branching
 */
template<class Trait>
class StrongBranching : public GenericStrongBranching<Trait> {
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;
  /** Constructor
   */
  StrongBranching() : GenericStrongBranching<Trait>("sb") {}
  /** Constructor
   * @param _n the name of the branching strategy
   */
  StrongBranching(const char *_n) : GenericStrongBranching<Trait>(_n) {}
  /** Destructor
   */
  virtual ~StrongBranching() {}

  /** Evaluation of subproblem in the sense of the score function
   * @param lp TheLp used to evaluate the branch
   */
  double eval(TheLP *lp) const {
    return lp->get_obj_val();
  }
  /** The score function that computes the score for one variable according 
   * to the evaluations of the two subproblems obtained with branching on 
   * this variable.
   * @param frc rating of one branch against the other one (generally not used).
   * @param z array that stores the two evaluations of the two branches.
   * @return the score of the variable.
   */
  double score(double frc, double z[2]) const {
    double zmin = z[0];
    double zmax = z[1];
    if (zmin > zmax) {
      zmin = zmax;
      zmax = z[0];
    }
    return zmin + 10 * zmax;
    //return 10 * zmin + zmax;
    //return zmax - zmin;
    //return zmax - 2 * zmin;
    //return zmax - 10 * zmin;
    //return 5 * zmax - zmin;
    //return (1-frc) * z[0] + frc * z[1]
  }
  /** Static method opt() to eventualy declare the options of the 
   * branching strategy for the application
   */
  static void opt() {
  }
};

/**
 * Entropy Branching
 */
template< class Trait>
class EntropyBranching : public GenericStrongBranching<Trait> {
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;
  /** Constructor
   */
  EntropyBranching() : GenericStrongBranching<Trait>("eb") {}
  /** Destructor
   */
  virtual ~EntropyBranching() {}
  /** Evaluation of subproblem in the sense of the score function
   * @param lp TheLp used to evaluate the branch
   */
  double eval(TheLP *lp) const {
    double entropy = 0.0;
    int ncols = lp->get_num_cols();
    int i;
    int kind[ncols];
    double val[ncols];
    double frc;

    lp->get_col_kind(kind, 0, ncols - 1);
    lp->get_col_prim(val, 0, ncols - 1);
    for (i = 0; i < ncols; i++) {
      if (kind[i] == GSP_IV) {
        frc = val[i] - floor(val[i]);
        entropy += 0.5 - fabs(0.5 - frc);
      }
    }

    return entropy;
  }
  /** The score function that computes the score for one variable according 
   * to the evaluations of the two subproblems obtained with branching on 
   * this variable.
   * @param frc rating of one branch against the other one.
   * @param z array that stores the two evaluations of the two branches.
   * @return the score of the variable.
   */
  double score(double frc, double z[2]) const {
    return -((1 - frc) * z[0] + frc * z[1]);
  }
  /** Static method opt() to eventualy declare the options of the 
   * branching strategy for the application
   */
  static void opt() {
  }
};

/**
 * Hybrid strategy.
 * The Strong brannching is really time consumming if it is 
 * used for the entire tree search. Then we propose a strategy where 
 * strong Branching strategy is used for the first level of the tree, 
 * Most fractional branching is used afterwards.
 */
template<class Trait>
class StrongThenMostFracBranching : public StrongBranching<Trait> {
  MostFracBranching<Trait> *mf;
  int thres;
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;
  /** Constructor
   */
  StrongThenMostFracBranching(): StrongBranching<Trait>("sbmf"),
      mf(new MostFracBranching<Trait>()), thres(0) {
    thres = Bob::core::opt().NVal("--lp", "-t");
  }
  /** Destructor
   */
  virtual ~StrongThenMostFracBranching() {
    delete mf;
  }
  /** Fixe the value in the Branching Information object with the LP
   * @param lp the Linear Program
   * @param bi the Branching Information
   * @param st the statistics object
   * @param dist the level of the node.
   * @param b_val the value of the incubent.
   *
   * @return 1 if the branching is ok
   */
  virtual int branch(TheLP * lp, TheBranchingInfo *bi, TheStat &st, int dist, double b_val) const {
    if (dist < thres) {
      return StrongBranching<Trait>::branch(lp, bi, st, dist, b_val);
    } else {
      return mf->branch(lp, bi, st, dist, b_val);
    }
  }
  /** Static method opt() to eventualy declare the options of the 
   * branching strategy for the application
   */
  static void opt() {
    core::opt().add(std::string("--lp"), Property("-t", "The depth threshold to switch from Strong branching to Most Fractional branching", 10));
  }

};

/** class that allows to choose the branching strategy at the command line.
 * The option  --lp -b allows to choose the wanted strategy.
 * executing the application with -help option will display all the 
 * possible options. for that application.
 */
template<class Trait>
class GenericBranchingSet : public GenericBranching<Trait> {
  std::vector<GenericBranching<Trait> *> gbs;
  int type;
  int thres;
public:
  /// Type definition of the Branching Info.
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  /// Type definition of the Branching Info.
  typedef typename Trait::Stat TheStat;
  /** Constructor
   */
  GenericBranchingSet()  {
    add_calc_branch(new MostFracBranching<Trait>());
    add_calc_branch(new StrongBranching<Trait>());
    add_calc_branch(new EntropyBranching<Trait>());
    add_calc_branch(new StrongThenMostFracBranching<Trait>());
    std::string rule = Bob::core::opt().SVal("--lp", "-b");
    type = -1;
    for (unsigned int i = 0;i < gbs.size();i++) {
      if (gbs[i] != 0 && gbs[i]->is_same(rule.c_str())) {
        type = i;
        return;
      }
    }
    std::cerr << "Error Branching strategy not found :" << rule << "\n";
    exit(1);
  }

  /** Destructor
   */
  virtual ~GenericBranchingSet()  {
     for ( typename std::vector<GenericBranching<Trait> *>::iterator it=gbs.begin() ; it!=gbs.end(); it++ ) {
       if ( (*it)!=0 ) { delete (*it); }
     }
  }
  /** Add a new branching strategy to the pool of strategies.
   */
  void add_calc_branch(GenericBranching<Trait> *gb) {
    gbs.push_back(gb);
  }

  /** Fixe the value in the Branching Information object with the LP
   * @param lp the Linear Program
   * @param i the Branching Information
   * @param st the statistics object
   * @param dist the level of the node.
   * @param b_val the value of the incubent.
   *
   * @return 1 if the branching is ok
   */
  virtual int branch(TheLP *lp, TheBranchingInfo *i, TheStat &st, int dist, double b_val) const {
    return gbs[type]->branch(lp, i, st, dist, b_val);
  }
  /** Static method opt() which declares the options --lp -b 
   * to be able to choose the branching strategy
   */
  static void opt() {
    MostFracBranching<Trait>::opt();
    StrongBranching<Trait>::opt();
    EntropyBranching<Trait>::opt();
    StrongThenMostFracBranching<Trait>::opt();
    core::opt().add(std::string("--lp"), Property("-b", "the name of the choosen branching strategies (mf,sb,eb,sbmf)", "mf"));
  }
};


/**
 * @}
 */

/**
 * @}
 */
};
#endif
