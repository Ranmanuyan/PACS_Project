/*
 * Bob++ MIP solver
 */

#include <bobpp/bobpp>
#ifdef Threaded
#  include <bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif
#include <string>
#include <vector>
#include <cmath>
#include <glop/glop.h>

#include "config.h"
#ifdef HAVE_LIBGLOCK
#  include "glop/glock.h"
#endif

#define ACCURACY 0.000001

class MipProblem;
class MipInstance;
class MipGenChild;
class MipNode;
class MipStat;


struct _gup_serialize {
  Bob::Serialize *s;
  _gup_serialize(Bob::Serialize *_s): s(_s) {}
  ~_gup_serialize() {}
};

struct _gup_deserialize {
  Bob::DeSerialize *d;
  _gup_deserialize(Bob::DeSerialize *_d): d(_d) {}
  ~_gup_deserialize() {}
};

int gup_pack_char(gup_serialize * s, const char *p, int count) {
  s->s->Pack(p, count);
  return count;
}

int gup_pack_int(gup_serialize * s, const int *p, int count) {
  s->s->Pack(p, count);
  return count;
}

int gup_pack_double(gup_serialize * s, const double *p, int count) {
  s->s->Pack(p, count);
  return count;
}

int gup_unpack_char(gup_deserialize * s, char *p, int count) {
  s->d->UnPack(p, count);
  return count;
}

int gup_unpack_int(gup_deserialize * s, int *p, int count) {
  s->d->UnPack(p, count);
  return count;
}

int gup_unpack_double(gup_deserialize * s, double *p, int count) {
  s->d->UnPack(p, count);
  return count;
}

class MipTrait {
public:
  typedef MipNode Node;
  typedef MipInstance Instance;
  typedef MipGenChild GenChild;
  typedef Bob::BBAlgo<MipTrait> Algo;
  typedef Bob::BBGoalBest<MipTrait> Goal;
  typedef Bob::BestThenDepthPri<MipNode> PriComp;
  //typedef Bob::BestEPri<MipNode> PriComp;
  //typedef Bob::DepthPri<MipNode> PriComp;
  typedef MipStat Stat;
};


/**
 * The Statistics for MIP algorithm
 */
class MipStat : public Bob::BBStat {
public:
  /// Constructor
  MipStat(const Id &s) :
      Bob::BBStat(s) {
#ifdef HAVE_LIBGLOCK
    add_timer('c', "Cuts generation", true);
#endif
    add_timer('r', "Root LP resolution", true);
  }
  /// Destructor
  virtual ~MipStat() {}
  /// Start the cuts generation time_stat
#ifdef HAVE_LIBGLOCK
  void cuts_start() {
    get_timer('c').start();
  }
  /// Stop the cuts generation time_stat
  void cuts_end() {
    get_timer('c').end();
  }
#endif
  /// Start the root LP resolution time_stat
  void lp_start() {
    get_timer('r').start();
  }
  /// Stop the root LP resolution time_stat
  void lp_end() {
    get_timer('r').end();
  }
};

/**
 * Base class for implementation of branching rules
 */
class Branching {
protected:
  // MipNode
  MipNode & node;
  // variable number for next branch
  int bestvar;
  // value of best variable
  double bestval;
  static int type;
  static long int thres;

public:
  Branching(MipNode & _node) : node(_node), bestvar(-1), bestval(0.) {}
  Branching(const Branching & b, MipNode & _node) :
      node(_node), bestvar(b.bestvar), bestval(b.bestval) {}
  virtual ~Branching() {}

  int best() {
    return bestvar;
  }
  double bestValue() {
    return bestval;
  }

  virtual int branch() = 0;
  virtual void Pack(Bob::Serialize &bs) {
    bs.Pack(&bestvar, 1);
    bs.Pack(&bestval, 1);
  }

  virtual void UnPack(Bob::DeSerialize &bs)  {
    bs.UnPack(&bestvar, 1);
    bs.UnPack(&bestval, 1);
  }

  static void init() {
    std::string rule = Bob::core::opt().SVal("--mip", "-b");
    type = -1;
    if (!rule.compare("mf"))
      type = 1;
    else if (!rule.compare("sb"))
      type = 2;
    else if (!rule.compare("eb"))
      type = 3;
    else if (!rule.compare("sbmf"))
      type = 4;
    else if (!rule.compare("ff"))
      type = 5;
    else if (!rule.compare("lf"))
      type = 6;
    else if (!rule.compare("lef"))
      type = 7;
    else {
      std::cout << "Invalid branching rule: " << rule << endl;
      exit(1);
    }
    thres = Bob::core::opt().NVal("--mip", "-s");
  }
  static Branching * gen(MipNode & _node);
  static Branching * gen(const Branching * b, MipNode & _node);
};

class MipNode : public Bob::BBDoubleMinNode {
  bool feasible;
  bool serialized;
  // the bound arrays for serialized Nodes
  int lub_size;
  double * lb;
  double * ub;
  // the modified LP, including cuts.
  GSP * lp;
  // the status of the variables (for warm start resolution of non-root nodes)
  int column_status_size;
  int row_status_size;
  int * column_status;
  int * row_status;
  // branching strategy
  Branching * branch;

public:
  /// Constructor for empty Node with infinite evaluation
  MipNode() : Bob::BBDoubleMinNode(), feasible(false), serialized(false),
      lub_size(0), lb(0), ub(0), lp(0), column_status_size(0),
      row_status_size(0), column_status(0), row_status(0),
      branch(Branching::gen(*this)) {
    DBGAFF_USER("MipNode::MipNode()", "Called");
    setEval(GSP_INF);
  }
  /// Constructor for initial Node
  MipNode(GSP * _lp, int depth) : Bob::BBDoubleMinNode(), feasible(false),
      serialized(false), lub_size(0), lb(0), ub(0), lp(_lp),
      column_status_size(0), row_status_size(0), column_status(0),
      row_status(0), branch(Branching::gen(*this)) {
    DBGAFF_USER("MipNode::MipNode(GSP *, int)",
                "Called");
    dist() = depth + 1;
  }
  /// Child Node constructor
  MipNode(MipNode & n, bool copy_lp) : Bob::BBDoubleMinNode(n), feasible(false),
      serialized(false), lub_size(0), lb(0), ub(0), lp(0),
      column_status_size(0), row_status_size(0), column_status(0),
      row_status(0), branch(0) {
    DBGAFF_USER("MipNode::MipNode(MipNode &, bool)", "Called");
    if (copy_lp) {
      lp = gsp_copy_prob(n.lp);
      gsp_set_verbosity(lp, 0);
      gsp_set_presolve(lp, 0);
      branch = Branching::gen(n.branch, *this);
    } else {
      lp = n.lp;
      n.lp = 0;
      branch = Branching::gen(n.branch, *this);
    }
    dist() = n.dist() + 1;
  }
  /// Copy constructor
  MipNode(const MipNode & n) : Bob::BBDoubleMinNode(n), feasible(false),
      serialized(true), lub_size(n.lub_size), lb(0), ub(0), lp(0),
      column_status_size(n.column_status_size),
      row_status_size(n.row_status_size), column_status(0), row_status(0),
      branch(Branching::gen(*this)) {
    //std::cout << " Copy from Const MipNode\n";
    DBGAFF_USER("MipNode(const MipNode &)", "Called");
    dist() = n.dist();
    if (n.lp) {
      column_status_size = gsp_get_num_cols(n.lp);
      row_status_size = gsp_get_num_rows(n.lp);
      column_status = new int[column_status_size];
      row_status = new int[row_status_size];
      gsp_get_col_stat(n.lp, column_status, 0, column_status_size - 1);
      gsp_get_row_stat(n.lp, row_status, 0, row_status_size - 1);
    } else {
      column_status = new int[column_status_size];
      row_status = new int[row_status_size];
      lb = new double[lub_size];
      ub = new double[lub_size];
      memcpy(column_status, n.column_status, sizeof(int)*column_status_size);
      memcpy(row_status, n.row_status, sizeof(int)*row_status_size);
      memcpy(lb, n.lb, sizeof(double)*lub_size);
      memcpy(ub, n.ub, sizeof(double)*lub_size);
    }
  }
  virtual ~MipNode() {
    DBGAFF_USER("MipNode::~MipNode", "Called");
    if (lp)
      gsp_delete_prob(lp);
    if (lb) {
      delete [] lb;
      delete [] ub;
    }
    if (column_status)
      delete [] column_status;
    if (row_status)
      delete [] row_status;
    if (branch)
      delete branch;
  }
  void eval(MipTrait::Algo *algo) {
    MipStat *stat = algo->getStat();
    DBGAFF_USER("MipNode::eval()", "Called");
    double e = algo->getGoal()->getBest();
    gsp_set_obj_upper_limit(lp, e);
    if (dist() % 2 == 1) glock_impliedBounds(lp, 0);
  
    stat->lp_start();
    if (gsp_solve(lp) == 0 && gsp_get_status(lp) == GSP_LP_OPT) {

      if (dist() % 15 == 0 && dist() > 0) {
        //Mixed Integer Rounding Cuts
        int cpt = 0;
        int nb_tot = 0;
        int nb_cuts;
        double obj, obj2 = gsp_get_obj_val(lp);
        double gap;
        do {
          obj = obj2;
          gap = 0;
          nb_cuts = glock_mir(lp, 0 , 1);
          if (nb_cuts > 0) {
            nb_tot += nb_cuts;
            gsp_solve(lp);
            obj2 = gsp_get_obj_val(lp);
            gap = 100 * (obj2 - obj) / obj;
          }
          cpt++;

        } while (gap > 0.001 && cpt < 5);
      }
      feasible = true;
      val = gsp_get_obj_val(lp);
      setEval(val);

      if (e == 0 || val <= e) {
        if (branch->branch() == -1) {
          std::cout << "Solution found: " << getEval() << std::endl;

          setSol();
        }
      }
    }
    stat->lp_end();
  }

  void addCut(int var, double value, bool direction_is_low) {
    DBGAFF_USER("MipNode::addCut()", "Called");
    double bd;
#if 0
    double lb, ub;
    gsp_get_lb(lp, &lb, var, var);
    gsp_get_ub(lp, &ub, var, var);
    cout << "dist=" << dist() << " adding cut: " << gsp_get_col_name(lp, var)
    << " (" << var << ", lb=" << lb << ", ub=" << ub << ") "
    << (direction_is_low ? "> " : "< ") << (direction_is_low ? ceil(bd) : floor(bd))
    << endl;
#endif
    if (direction_is_low) {
      bd = ceil(value);
      gsp_set_col_bounds(lp, &bd, NULL, var, var);
    } else {
      bd = floor(value);
      gsp_set_col_bounds(lp, NULL, &bd, var, var);
    }
  }

  void recoverFromInstance(GSP * _lp) {
    DBGAFF_USER("MipNode::recoverFromInstance()", "Called");
    int i, j, ncols;
    lp = gsp_copy_prob(_lp);
    gsp_set_verbosity(lp, 1);
    gsp_set_presolve(lp, 1);

    ncols = gsp_get_num_cols(lp);
    gsp_set_col_stat(lp, column_status, 0, ncols - 1);
    gsp_set_row_stat(lp, row_status, 0, gsp_get_num_rows(lp) - 1);
    gsp_warmup_basis(lp);

    int col_kind[ncols];
    gsp_get_col_kind(lp, col_kind, 0, ncols - 1);
    j = 0;
    for (i = 0; i < ncols; i++) {
      if (col_kind[i] == GSP_IV) {
        gsp_set_col_bounds(lp, &lb[j], &ub[j], i, i);
        j++;
      }
    }
    serialized = false;
  }

  bool isFeasible() {
    return feasible;
  }

  bool isSerialized() {
    return serialized;
  }

  int bestVar() {
    return branch->best();
  }

  double bestVal() {
    return branch->bestValue();
  }

  GSP * getLp() {
    return lp;
  }

  virtual ostream &Prt(ostream &os = std::cout) const {
    if (lp) {
      os << "LP Objective value: " << gsp_get_obj_val(lp) << std::endl;
      os << "nrows = " << gsp_get_num_rows(lp) << std::endl;
      os << "ncols = " << gsp_get_num_cols(lp) << std::endl;
    }
    os << "Evaluation = " << getEval() << std::endl;
    os << "dist = " << dist() << std::endl;
    os << "row_status_size = " << row_status_size << endl;
    os << "column_status_size = " << column_status_size << endl;
#if 0
    os << "row_status = [";
    for (i = 0;i < row_status_size;i++)
      os << " " << row_status[i];
    os << " ]" << endl;
    os << "column_status = [";
    for (i = 0;i < column_status_size;i++)
      os << " " << column_status[i];
    os << " ]" << endl;
    for (i = 0;i < nc;i++)
      cuts[i].Prt(os) << endl;
#endif
    return os;
  }

/// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
    DBGAFF_USER("MipNode::Pack()", "Called");
    int i;
    Bob::BBDoubleMinNode::Pack(bs);
    if (lp) {
      int ncols = gsp_get_num_cols(lp);
      int nrows = gsp_get_num_rows(lp);
      int col_st[ncols];
      int row_st[nrows];
      int nint = gsp_get_num_int(lp);
      double lb[nint];
      double ub[nint];
      int j;
      bs.Pack(&ncols, 1);
      bs.Pack(&nrows, 1);
      gsp_get_col_stat(lp, col_st, 0, ncols - 1);
      gsp_get_row_stat(lp, row_st, 0, nrows - 1);
      bs.Pack(col_st, ncols);
      bs.Pack(row_st, nrows);
      gsp_get_col_kind(lp, col_st, 0, ncols - 1);
      bs.Pack(&nint, 1);
      j = 0;
      for (i = 0;i < ncols;i++) {
        if (col_st[i] == GSP_IV) {
          gsp_get_lb(lp, &lb[j], i, i);
          gsp_get_ub(lp, &ub[j], i, i);
          j++;
        }
      }
      bs.Pack(lb, nint);
      bs.Pack(ub, nint);
    } else {
      bs.Pack(&column_status_size, 1);
      bs.Pack(&row_status_size, 1);
      bs.Pack(column_status, column_status_size);
      bs.Pack(row_status, row_status_size);
      bs.Pack(&lub_size, 1);
      bs.Pack(lb, lub_size);
      bs.Pack(ub, lub_size);
    }
    branch->Pack(bs);
  }

/// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_USER("MipNode::UnPack()", "Called");
    Bob::BBDoubleMinNode::UnPack(bs);
    bs.UnPack(&column_status_size, 1);
    bs.UnPack(&row_status_size, 1);
    if (column_status_size) {
      column_status = new int[column_status_size];
      row_status = new int[row_status_size];
      bs.UnPack(column_status, column_status_size);
      bs.UnPack(row_status, row_status_size);
    }
    bs.UnPack(&lub_size, 1);
    if (lub_size) {
      lb = new double[lub_size];
      ub = new double[lub_size];
      bs.UnPack(lb, lub_size);
      bs.UnPack(ub, lub_size);
    }
    serialized = true;
    branch->UnPack(bs);
  }
};


#if 0
static int compare_double(const void *a_, const void *b_) {
  const double *a = (const double *)a_;
  const double *b = (const double *)b_;
  if (*a > *b)
    return -1;
  return (*a < *b);
}
#endif


/**
 * First Fractional variable branching rule
 */
class FirstFracBranching : public Branching {
public:
  FirstFracBranching(MipNode & _node): Branching(_node)
      /*, bestfrc(0), ratio(1)*/ {}
  FirstFracBranching(const FirstFracBranching & b, MipNode & _node):
      Branching(b, _node)/*, bestfrc(b.bestfrc), ratio(b.ratio)*/ {}

  virtual int branch() {
    double frc, val;
    GSP *lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int kind[ncols];
    int j;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    bestvar = -1;
    bestval = 0.0;
    for (j = 0; j < ncols; j++) {
      if (kind[j] != GSP_IV)
        continue;
      gsp_get_col_prim(lp, &val, j, j);
      frc = fabs(val - nearbyint(val));
      if (frc >= ACCURACY) {
        bestvar = j;
        bestval = val;
        break;
      }
    }
    return bestvar;
  }
  virtual void Pack(Bob::Serialize &bs) {
    Branching::Pack(bs);
  }
  virtual void UnPack(Bob::DeSerialize &bs) {
    Branching::UnPack(bs);
  }
};


/**
 * Last Fractional variable branching rule
 */
class LastFracBranching : public Branching {
public:
  //double bestfrc, ratio;
  LastFracBranching(MipNode & _node): Branching(_node)
      /*, bestfrc(0), ratio(1)*/ {}
  LastFracBranching(const LastFracBranching & b, MipNode & _node):
      Branching(b, _node)/*, bestfrc(b.bestfrc), ratio(b.ratio)*/ {}

  virtual int branch() {
    double frc, valu;
    GSP *lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int kind[ncols];
    int j;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    bestvar = -1;
    bestval = 0.0;
    if (gsp_get_obj_upper_limit(lp) <= GSP_INF) {
      for (j = ncols - 1; j > 0; j--) {
        if (kind[j] != GSP_IV)
          continue;
        gsp_get_col_prim(lp, &valu, j, j);
        frc = fabs(valu - nearbyint(valu));
        if (frc >= ACCURACY) {
          bestvar = j;
          bestval = valu;
          break;
        }
      }
    }
    return bestvar;
  }
  virtual void Pack(Bob::Serialize &bs) {
    Branching::Pack(bs);
  }
  virtual void UnPack(Bob::DeSerialize &bs) {
    Branching::UnPack(bs);
  }
};

/**
 * Most Fractional variable branching rule
 */
class MostFracBranching : public Branching {
public:
  //double bestfrc, ratio;
  MostFracBranching(MipNode & _node): Branching(_node)
      /*, bestfrc(0), ratio(1)*/ {}
  MostFracBranching(const MostFracBranching & b, MipNode & _node):
      Branching(b, _node)/*, bestfrc(b.bestfrc), ratio(b.ratio)*/ {}

  virtual int branch() {
    double frc, val;
    GSP *lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int kind[ncols];
    int j, nf;
    int fracv[ncols];
    double bestfrc;
    //double bestfrc2, bestvar2;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    bestvar = -1;
    bestfrc = 0.0;
    //bestvar2 = -1;
    //bestfrc2 = 0.0;
    bestval = 0.0;
    //bestfrc2 = 0.0;
    nf = 0;
    for (j = 0; j < ncols; j++) {
      if (kind[j] != GSP_IV)
        continue;
      gsp_get_col_prim(lp, &val, j, j);
      frc = fabs(val - nearbyint(val));
      if (frc >= ACCURACY) {
        fracv[nf++] = j;
        if (frc > bestfrc) {
          bestfrc = frc;
          bestvar = j;
          bestval = val;
        }/* else if (frc > bestfrc2) {
          bestfrc2 = frc;
          bestvar2 = j;
        }*/
      }
    }
#if 0
    //qsort(localscore,nf,sizeof(double),compare_double);
    ratio = 1.0;
    //cout << "dist=" << node.dist() << "\tnf=" << nf;
    if (nf > 1) {
      ratio = bestfrc / bestfrc2;
      /*      cout << "\tsc[0]=" << localscore[0] << "\tsc[1]=" << localscore[1]
                << "\tsc[" << nf-1 << "]=" << localscore[nf-1]
                << "\tratio=" << ratio;
      */
    }
    //cout << endl;
#endif
    return bestvar;
  }
  virtual void Pack(Bob::Serialize &bs) {
    Branching::Pack(bs);
  }
  virtual void UnPack(Bob::DeSerialize &bs) {
    Branching::UnPack(bs);
  }
};

/**
 * Less Fractional variable branching rule
 */
class LessFracBranching : public Branching {
public:
  //double bestfrc, ratio;
  LessFracBranching(MipNode & _node): Branching(_node)
      /*, bestfrc(0), ratio(1)*/ {}
  LessFracBranching(const LessFracBranching & b, MipNode & _node):
      Branching(b, _node)/*, bestfrc(b.bestfrc), ratio(b.ratio)*/ {}

  virtual int branch() {
    double frc, val;
    GSP *lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int kind[ncols];
    int j, nf;
    int fracv[ncols];
    double bestfrc;
    //double bestfrc2, bestvar2;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    bestvar = -1;
    bestfrc = 0.5;
    //bestvar2 = -1;
    //bestfrc2 = 0.0;
    bestval = 0.0;
    //bestfrc2 = 0.0;
    nf = 0;
    for (j = 0; j < ncols; j++) {
      if (kind[j] != GSP_IV)
        continue;
      gsp_get_col_prim(lp, &val, j, j);
      frc = fabs(val - nearbyint(val));
      if (frc >= ACCURACY) {
        fracv[nf++] = j;
        if (frc < bestfrc) {
          bestfrc = frc;
          bestvar = j;
          bestval = val;
        }/* else if (frc > bestfrc2) {
          bestfrc2 = frc;
          bestvar2 = j;
        }*/
      }
    }
#if 0
    //qsort(localscore,nf,sizeof(double),compare_double);
    ratio = 1.0;
    //cout << "dist=" << node.dist() << "\tnf=" << nf;
    if (nf > 1) {
      ratio = bestfrc / bestfrc2;
      /*      cout << "\tsc[0]=" << localscore[0] << "\tsc[1]=" << localscore[1]
                << "\tsc[" << nf-1 << "]=" << localscore[nf-1]
                << "\tratio=" << ratio;
      */
    }
    //cout << endl;
#endif
    return bestvar;
  }
  virtual void Pack(Bob::Serialize &bs) {
    Branching::Pack(bs);
  }
  virtual void UnPack(Bob::DeSerialize &bs) {
    Branching::UnPack(bs);
  }
};

/**
 * Generic Strong Branching (score functions must be implemented)
 */
class GenericStrongBranching : public Branching {
  int n_iter, n_lp;
protected:
  //double avgscore, std_dev, ratio;
public:
  GenericStrongBranching(MipNode & _node):
      Branching::Branching(_node), n_iter(0), n_lp(0) {}

  GenericStrongBranching(const GenericStrongBranching &b, MipNode &_node):
      Branching::Branching(b, _node), n_iter(b.n_iter), n_lp(b.n_lp) {}

  virtual int branch() {
    double frc, bd, lb, ub;
    int ret, status;
    double curscore, bestscore = -1e+300;
    GSP * lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int kind[ncols];
    double val[ncols];
    double z[2];
    int fracv[ncols];
    int i, j, k, nf, itlim;
    int iter, iter2;
    //double varscore, d;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    bestvar = -1;
    bestval = 0.0;
    gsp_get_col_prim(lp, val, 0, ncols - 1);
    iter2 = gsp_get_iteration_count(lp);
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
        gsp_get_lb(lp, &lb, j, j);
        gsp_get_ub(lp, &ub, j, j);
        bd = floor(val[j]);
        for (k = 0; k < 2; k++) {
          // k = 0 : Set upper bound to floor value of frac variable
          // k = 1 : Set lower bound to ceil value of frac variable
          gsp_set_col_bounds(lp,
                             &((k == 0) ? lb : bd), &((k == 0) ? bd : ub), j, j);
          itlim = n_lp ? (n_iter * 2 / n_lp) : 0;
          gsp_set_iteration_limit(lp, itlim);
          ret = gsp_solve(lp);
          iter = gsp_get_iteration_count(lp);
          n_iter += iter - iter2;
          //cout << "iteration limit=" << itlim << "\tn=" << iter - iter2 << endl;
          n_lp ++;
          iter2 = iter;
          status = gsp_get_status(lp);
          if (ret == GSP_E_OBJUL || (ret == 0 && status == GSP_LP_NOFEAS)) {
            bestvar = j;
            bestval = val[j];
            gsp_set_col_bounds(lp, &lb, &ub, j, j);
            goto endofsearch;
          } else if (ret != 0 && ret != GSP_E_ITLIM) {
            cout << "branch:gsp_solve: " << gup_const_string(ret) << endl;
            abort();
          }
          bd += 1.0;
          z[k] = eval();
        }
        gsp_set_col_bounds(lp, &lb, &ub, j, j);
        curscore = score(frc, z);
        //localscore[i] = curscore;
        if (curscore > bestscore) {
          bestscore = curscore;
          bestvar = j;
          bestval = val[j];
        }
      }
    }
#if 0
    qsort(localscore, nf, sizeof(double), compare_double);
    ratio = nf ? (localscore[0] / localscore[nf-1]) : 1.0;
    /*
        cout << "dist=" << node.dist() << "\tnf=" << nf <<"\tsc[0]="
            << localscore[0] << "\tsc[" << nf-1 << "]=" << localscore[nf-1]
            << "\tratio=" << ratio << endl;
    */
    avgscore = 0.0;
    for (i = 0; i < nf; i++) {
      avgscore += localscore[i];
    }
    avgscore /= nf;
    varscore = 0.0;
    for (i = 0; i < nf; i++) {
      d = localscore[i] - avgscore;
      varscore += d * d;
    }
    varscore /= nf;
    std_dev = sqrt(varscore);
#endif
endofsearch:
    gsp_set_iteration_limit(lp, 0);
    return bestvar;
  }
  /// evaluation of subproblem in the sense of the score function
  virtual double eval() = 0;
  /// score function, using evaluations of the two subproblems
  virtual double score(double frc, double z[2]) = 0;

  virtual void Pack(Bob::Serialize &bs) {
    Branching::Pack(bs);
    bs.Pack(&n_iter, 1);
    bs.Pack(&n_lp, 1);
  }
  virtual void UnPack(Bob::DeSerialize &bs) {
    Branching::UnPack(bs);
    bs.UnPack(&n_iter, 1);
    bs.UnPack(&n_lp, 1);
  }
};

/**
 * Standard Strong Branching
 */
class StrongBranching : public GenericStrongBranching {
public:
  StrongBranching(MipNode & _node): GenericStrongBranching(_node) {}
  StrongBranching(const StrongBranching & b, MipNode & _node):
      GenericStrongBranching(b, _node) {}

  double eval() {
    return gsp_get_obj_val(node.getLp());
  }
  double score(double frc, double z[2]) {
    double zmin = z[0];
    double zmax = z[1];
    if (zmin > zmax) {
      zmin = zmax;
      zmax = z[0];
    }
    //return zmin + 10 * zmax;
    //return 10 * zmin + zmax;
    //return zmax - zmin;
    //return zmax - 2 * zmin;
    //return zmax - 10 * zmin;
    //return 5 * zmax - zmin;
    return 5 * zmax + zmin;
    //return (1-frc) * z[0] + frc * z[1];
  }
};

/**
 * Entropy Branching
 */
class EntropyBranching : public GenericStrongBranching {
public:
  EntropyBranching(MipNode & _node): GenericStrongBranching(_node) {}
  EntropyBranching(const EntropyBranching & b, MipNode & _node):
      GenericStrongBranching(b, _node) {}

  double eval() {
    double entropy = 0.0;
    GSP *lp = node.getLp();
    int ncols = gsp_get_num_cols(lp);
    int i;
    int kind[ncols];
    double val[ncols];
    double frc;

    gsp_get_col_kind(lp, kind, 0, ncols - 1);
    gsp_get_col_prim(lp, val, 0, ncols - 1);
    for (i = 0; i < ncols; i++) {
      if (kind[i] == GSP_IV) {
        frc = val[i] - floor(val[i]);
        entropy += 0.5 - fabs(0.5 - frc);
      }
    }

    return entropy;
  }
  double score(double frc, double z[2]) {
    return -((1 - frc) * z[0] + frc * z[1]);
  }
};

class StrongThenMostFracBranching : public StrongBranching {
  MostFracBranching *mf;
public:
  StrongThenMostFracBranching(MipNode & _node): StrongBranching(_node),
      mf(new MostFracBranching(_node)) {}
  StrongThenMostFracBranching(const StrongThenMostFracBranching & b,
                              MipNode & _node): StrongBranching(b, _node),
      mf(new MostFracBranching(_node)) {}
  ~StrongThenMostFracBranching() {
    delete mf;
  }

#if 1
  virtual int branch() {
    int dist = StrongBranching::node.dist();
    //if (dist == 0 || ratio > 1.01) {
    if (dist < thres) {
      /*cout << "dist=" << StrongBranching::node.dist()
          << " avgscore=" <<  avgscore
          << " std_dev=" << std_dev << " ratio=" << ratio << endl;*/
      return StrongBranching::branch();
    } else {
      bestvar = mf->branch();
      bestval = mf->bestValue();
      return bestvar;
    }
  }
#else
  virtual int branch() {
    bestvar = mf->branch();
    if (mf->bestfrc >= 0.1) {
      bestval = mf->bestValue();
      return bestvar;
    } else
      return StrongBranching::branch();
  }
#endif
};

class StrongThenLastFracBranching : public StrongBranching {
  LastFracBranching *mf;
public:
  StrongThenLastFracBranching(MipNode & _node): StrongBranching(_node),
      mf(new LastFracBranching(_node)) {}
  StrongThenLastFracBranching(const StrongThenLastFracBranching & b,
                              MipNode & _node): StrongBranching(b, _node),
      mf(new LastFracBranching(_node)) {}
  ~StrongThenLastFracBranching() {
    delete mf;
  }

#if 1
  virtual int branch() {
    int dist = StrongBranching::node.dist();
    //if (dist == 0 || ratio > 1.01) {
    if (dist < thres) {
      /*cout << "dist=" << StrongBranching::node.dist()
          << " avgscore=" <<  avgscore
          << " std_dev=" << std_dev << " ratio=" << ratio << endl;*/
      return StrongBranching::branch();
    } else {
      bestvar = mf->branch();
      bestval = mf->bestValue();
      return bestvar;
    }
  }
#else
  virtual int branch() {
    bestvar = mf->branch();
    if (mf->bestfrc >= 0.1) {
      bestval = mf->bestValue();
      return bestvar;
    } else
      return StrongBranching::branch();
  }
#endif
};

/* Branching class members */
int Branching::type;
long int Branching::thres;

Branching * Branching::gen(MipNode & node) {
  switch (type) {
  case 1:
    return new MostFracBranching(node);
  case 2:
    return new StrongBranching(node);
  case 3:
    return new EntropyBranching(node);
  case 4:
    return new StrongThenLastFracBranching(node);
  case 5:
    return new FirstFracBranching(node);
  case 6:
    return new LastFracBranching(node);
  case 7:
    return new LessFracBranching(node);
  default:
    return 0;
  }
}

Branching * Branching::gen(const Branching * b, MipNode & node) {
  switch (type) {
  case 1:
    return new MostFracBranching(*(const MostFracBranching*)b, node);
  case 2:
    return new StrongBranching(*(const StrongBranching*)b, node);
  case 3:
    return new EntropyBranching(*(const EntropyBranching*)b, node);
  case 4:
    return new StrongThenLastFracBranching(
             *(const StrongThenLastFracBranching*)b, node);
  case 5:
    return new FirstFracBranching(*(const FirstFracBranching*)b, node);
  case 6:
    return new LastFracBranching(*(const LastFracBranching*)b, node);
  case 7:
    return new LessFracBranching(*(const LessFracBranching*)b, node);
  default :
    return 0;
  }
}

class MipProblem : public Bob::Problem<MipTrait> {
public:
  MipProblem() : Bob::Problem<MipTrait>() {}
  virtual ~MipProblem() {}
};

class MipInstance : public Bob::BBInstance<MipTrait> {
  const char * fname;
public:
  GSP * lp;

  /// Constructor
  MipInstance() : Bob::BBInstance<MipTrait>("MIP"), fname(0), lp(0) {
    DBGAFF_USER("MipInstance::MipInstance()", "Called");
  }
  /// Constructor
  MipInstance(const char * _fname) : Bob::BBInstance<MipTrait>("MIP"),
      fname(_fname), lp(0) {
    DBGAFF_USER("MipInstance::MipInstance()", "Called");
    Branching::init();
  }

  /// Constructor
  MipInstance(const MipInstance &mii) : Bob::BBInstance<MipTrait>(mii),
      fname(mii.fname), lp(mii.lp) {}
  /// Constructor
  MipInstance(MipInstance &mii) : Bob::BBInstance<MipTrait>(mii),
      fname(mii.fname), lp(mii.lp) {}
  virtual ~MipInstance() {
    //gsp_delete_prob(lp);
  }

  /// Initialize the Instance
  void Init() {
    const char *ext;
    ext = fname + strlen(fname);
    if (!strcmp(ext - 4, ".mps"))
      lp = gsp_read_mps(fname);
    else if (!strcmp(ext - 3, ".lp"))
      lp = gup_read_lpt((char*)fname);
  }

  /// Creates and inserts the root Node
  void InitAlgo(MipTrait::Algo *a) const {
    std::cout << "MipInstance::Init(Algo) Begins" << std::endl;
    GSP * nlp;
    int i, ncols;
    MipStat *stat = a->getStat();


    gsp_set_verbosity(lp, 0);
    gsp_set_presolve(lp, 1);
    ncols = gsp_get_num_cols(lp);
    if (gsp_get_obj_dir(lp) == GSP_MAX) {
      // Maximization problem, we need to negate all objective coefficients
      double obj_coef[ncols];
      gsp_get_obj_coef(lp, obj_coef, 0, ncols - 1);
      for (i = 0; i < ncols; i++)
        obj_coef[i] = -obj_coef[i];
      gsp_set_obj_coef(lp, obj_coef, 0, ncols - 1);
      gsp_set_const_term(lp, -gsp_get_const_term(lp));
      gsp_set_obj_dir(lp, GSP_MIN);
    }
    gsp_set_class(lp, GSP_LP);
    stat->lp_start();
    gsp_solve(lp);
    if (gsp_get_status(lp) == GSP_LP_NOFEAS) {
      std::cerr << " Non feasible Problem\n";
      exit(1);
    }
#ifdef HAVE_LIBGLOCK
    /*IMPLIED BOUNDS AND REDUCE COEF*/
    glock_impliedBounds(lp, 1);
    glock_reduceCoef(lp, 1);
    //int fix = gsp_fix_predictible(lp);
    //printf("Fix = %d\n", fix);
    stat->lp_start();
    gsp_solve(lp);
    printf("%f \n", gsp_get_obj_val(lp));
    if (Bob::core::opt().BVal("--mip", "-c")) {
      int nb_cuts;
      int nb_tot = 0;
      double obj, old_obj, obj2 = gsp_get_obj_val(lp);
      double gap;
      int cpt;
      clock_t start, end;
      double time;
      int nb_fixed;
      int num_bin = gsp_get_num_bin(lp);

      nb_fixed = gup_get_num_bin_frac(lp);
      printf("%d\n ", nb_fixed);
      printf("%s%34s %6s %8s %11s %11s\n", "Method", "New Obj", "Gap", "Add", "Rounds",  "Time");

      //Lift-and-Project cuts
      cpt = 0;
      old_obj = obj2;
      nb_tot = 0;
      if (nb_fixed > 39 || (nb_fixed > 5 && nb_fixed < 15)) {
        start = clock();
        do {
          obj = obj2;
          gap = 0;
          nb_cuts = glock_LandP(lp, 0);
          //stat.add_cuts(nb_cuts);
          if (nb_cuts > 0 && nb_cuts < 50) {
            nb_tot += nb_cuts;
            gsp_solve(lp);
            obj2 = gsp_get_obj_val(lp);
            gap = 100 * (obj2 - obj) / obj;
          }
          cpt++;
          nb_fixed = gup_get_num_bin_frac(lp);
          if (nb_fixed < ((num_bin / 20) < 20 ? num_bin / 20 : 20)) {
            end = clock();
            time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
            gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
            printf("glock_LandP%30.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
            goto skip;
          }
          //printf("new Obj %f\n", gsp_get_obj_val(lp));
        } while (gap > 3 && cpt < 2);
        end = clock();
        time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
        gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
        printf("glock_LandP%30.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
      }

      //Mixed Integer Gomory Cuts
      cpt = 0;
      start = clock();
      old_obj = gsp_get_obj_val(lp);
      nb_fixed = gup_get_num_bin_frac(lp);
      if (nb_fixed > 5 && nb_tot < 112) {
        nb_tot = 0;
        do {
          obj = obj2;
          gap = 0;
          nb_cuts = glock_gomory_mixed_integer(lp, 0 , 1);
          //stat.add_cuts(nb_cuts);
          if (nb_cuts > 0) {//&& nb_cuts<50) {
            nb_tot += nb_cuts;
            gsp_solve(lp);
            obj2 = gsp_get_obj_val(lp);
            gap = 100 * (obj2 - obj) / obj;
          }
          cpt++;
          nb_fixed = gup_get_num_bin_frac(lp);
          if (nb_fixed < 0 && nb_fixed < ((num_bin / 20) < 20 ? num_bin / 20 : 20)) {
            end = clock();
            time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
            gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
            printf("glock_gomory_mixed_integer%15.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
            goto skip;
          }
          //printf("new Obj %f\n", gsp_get_obj_val(lp));
          //if(gap<=0.05) {
          //	int nb_rows=gsp_get_num_rows(lp);
          //	gsp_del_rows(lp, nb_rows-nb_cuts, nb_rows-1);
          //}
        } while (gap > 0.05 && cpt < 8);
        end = clock();
        time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
        gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
        printf("glock_gomory_mixed_integer%15.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
      }
      //Fractional Gomory Cuts
      cpt = 0;
      start = clock();
      old_obj = gsp_get_obj_val(lp);
      nb_tot = 0;
      do {
        obj = obj2;
        gap = 0;
        nb_cuts = glock_gomory_fractional(lp, 0 , 1);
        //stat.add_cuts(nb_cuts);
        if (nb_cuts > 0) {
          nb_tot += nb_cuts;
          gsp_solve(lp);
          obj2 = gsp_get_obj_val(lp);
          gap = 100 * (obj2 - obj) / obj;
        }
        cpt++;
        nb_fixed = gup_get_num_bin_frac(lp);
        if (nb_fixed < 0 && nb_fixed < ((num_bin / 20) < 20 ? num_bin / 20 : 20)) {
          end = clock();
          time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
          gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
          printf("glock_gomory_fractional%18.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
          goto skip;
        }
        //printf("new Obj %f\n", gsp_get_obj_val(lp));
      } while (gap > 0.05 && cpt < 1);
      end = clock();
      time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
      gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
      printf("glock_gomory_fractional%18.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
      //}
      //Flow Cover Cuts
      cpt = 0;
      start = clock();
      old_obj = gsp_get_obj_val(lp);
      nb_fixed = gup_get_num_bin_frac(lp);
      nb_tot = 0;
      do {
        obj = obj2;
        gap = 0;
        nb_cuts = glock_flow_cover(lp, 0 , 1);
        //stat.add_cuts(nb_cuts);
        if (nb_cuts > 0 && nb_cuts < 50) {
          nb_tot += nb_cuts;
          gsp_solve(lp);
          obj2 = gsp_get_obj_val(lp);
          gap = 100 * (obj2 - obj) / obj;
        }
        cpt++;
        nb_fixed = gup_get_num_bin_frac(lp);
        if (nb_fixed < 0 && nb_fixed < ((num_bin / 20) < 20 ? num_bin / 20 : 20)) {
          end = clock();
          time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
          gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
          printf("glock_flow_cover%25.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
          goto skip;
        }
        //printf("new Obj %f\n", gsp_get_obj_val(lp));
      } while (gap > 0.005 && cpt < 15);
      end = clock();
      time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
      gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
      printf("glock_flow_cover%25.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);

      //Knapsack Cover Cuts
      cpt = 0;
      start = clock();
      old_obj = gsp_get_obj_val(lp);
      nb_fixed = gup_get_num_bin_frac(lp);
      nb_tot = 0;
      do {
        obj = obj2;
        gap = 0.0;
        nb_cuts = glock_knapsack_cover(lp, 0 , 1);
        cpt++;
        //stat.add_cuts(nb_cuts);
        if (nb_cuts > 0 && nb_cuts < 50) {
          nb_tot += nb_cuts;
          //nb_knap+=nb_cuts;
          gsp_solve(lp);
          obj2 = gsp_get_obj_val(lp);
          gap = 100 * (obj2 - obj) / obj;
        } else break;
        nb_fixed = gup_get_num_bin_frac(lp);
        if (nb_fixed < 0 && nb_fixed < ((num_bin / 20) < 20 ? num_bin / 20 : 20)) {
          end = clock();
          time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
          gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
          printf("glock_flow_cover%25.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
          //nb_cuts_added+=nb_knap;
          goto skip;
        }
        //printf("new Obj %f\n", gsp_get_obj_val(lp));
      } while (gap > 0.005 && cpt < 15);
      gsp_solve(lp);
      end = clock();
      time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
      gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
      printf("glock_knapsack_cover%21.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), fabs(gap), nb_tot, cpt, time);

      //Mixed Integer Rounding Cuts
      cpt = 0;
      start = clock();
      old_obj = gsp_get_obj_val(lp);
      nb_fixed = gup_get_num_bin_frac(lp);
      nb_tot = 0;
      do {
        obj = obj2;
        gap = 0;
        nb_cuts = glock_mir(lp, 0 , 1);
        //stat.add_cuts(nb_cuts);
        if (nb_cuts > 0) {
          nb_tot += nb_cuts;
          gsp_solve(lp);
          obj2 = gsp_get_obj_val(lp);
          gap = 100 * (obj2 - obj) / obj;
        }
        cpt++;
        /*if(gup_get_num_bin_frac(lp)<((gsp_get_num_bin(lp)/20)<20 ? gsp_get_num_bin(lp)/20 : 20)) {
        	end = clock();
        	time = ((double ) end - ( double ) start ) / ( double ) CLOCKS_PER_SEC;
          gap = 100*(gsp_get_obj_val(lp)-old_obj)/old_obj;
        	printf("glock_mir%28.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
        	goto skip;
        }*/
        //printf("new Obj %f\n", gsp_get_obj_val(lp));
      } while (gap > 0.001 && cpt < 20);
      end = clock();
      time = ((double) end - (double) start) / (double) CLOCKS_PER_SEC;
      gap = 100 * (gsp_get_obj_val(lp) - old_obj) / old_obj;
      printf("glock_mir%32.3f %7.2f %7d %9d %13.2f\n", gsp_get_obj_val(lp), gap, nb_tot, cpt, time);
      glock_impliedBounds(lp, 1);
      glock_reduceCoef(lp, 1);
      gsp_solve(lp);

skip:
      nb_fixed = gup_get_num_bin_frac(lp);
      /*
      int status = gsp_test_rounding(lp, &obj);

      if(status==GSP_INT_OPT)     		printf("Sol %d\n", obj);
      */
    }
    /*
    int fix = gsp_fix_predictible(lp);
    printf("Fix = %d\n", fix);
    gsp_solve(lp);*/
#endif
    int col_kind[ncols], nfrc;
    double val;
    gsp_get_col_kind(lp, col_kind, 0, ncols - 1);
    nfrc = 0;
    for (i = 0; i < ncols; i++) {
      if (col_kind[i] == GSP_IV) {
        gsp_get_col_prim(lp, &val, i, i);
        if (fabs(val - nearbyint(val)) > ACCURACY)
          nfrc++;
      }
    }
    std::cout << nfrc << " fractional variables at root" << std::endl;
    stat->lp_end();
    // Creation of root Node, and insertion in GPQ
    nlp = gsp_copy_prob(lp);
    gsp_set_verbosity(nlp, 0);
    gsp_set_presolve(nlp, 1);
    MipNode *nd = new MipNode(nlp, -1);
    nd->eval(a);
    //a->log_space(nd);
    a->GPQIns(nd);
    std::cout << "MipInstance::Init(Algo) ends" << std::endl;
  }

  /// Get the initial best known solution
  MipNode *getSol() {
    //std::cout << "Instance::GetSol\n";
    return new MipNode();
  }

  /// Pack method to serialize the Instance
  virtual void Pack(Bob::Serialize &bs) const {
    DBGAFF_USER("MipInstance::Pack()", "Called");
    Bob::BBInstance<MipTrait>::Pack(bs);
    gup_serialize s(&bs);
    gup_pack(lp, &s);
  }
  /// Unpack method to deserialize the Instance
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_USER("MipInstance::UnPack()", "Called");
    Bob::BBInstance<MipTrait>::UnPack(bs);
    if (lp)
      gsp_delete_prob(lp);
    gup_deserialize d(&bs);
    lp = gup_unpack(&d);
  }
};

class MipGenChild: public Bob::BBGenChild<MipTrait> {
public:
  /// Constructor of MipGenChild
  MipGenChild(const MipTrait::Instance *_inst, MipTrait::Algo *_al) :
      Bob::BBGenChild<MipTrait>(_inst, _al) {
    DBGAFF_USER("MipGenChild::MipGenChild()", "Called");
  }
  virtual ~MipGenChild() {
    DBGAFF_USER("MipGenChild::~MipGenChild()", "Called");
  }
  /// Operator called by the algo to Generate child
  bool operator()(MipNode *n) {
    bool fl = false;
    DBGAFF_USER("MipGenChild::operator()", "is beginning");
    MipNode *child;
    unsigned int i;
    int bestvar;
    double bestval;
    if (n->isSerialized()) {
      //std::cout<<"recoverFromInstance "<<n->getEval()<<" "<<n->bestVar()<<std::endl;
      n->recoverFromInstance(inst->lp);
    }
    bestvar = n->bestVar();
    bestval = n->bestVal();
    if (bestvar == -1) {
      std::cout << "Sortie avec bestVar -1 :" << n->getEval() << "\n";
    }
    for (i = 0; i < 2; i++) {
      child = new MipNode(*n, i == 0);
      algo->start_eval(child,n);
      child->addCut(bestvar, bestval, i == 1);
      child->eval(algo);
      algo->end_eval(child);
      if (child->isFeasible()) {
        fl+=algo->Search(child);
      } else {
        algo->StNoFea(child);
        delete child;
      }
    }
    DBGAFF_USER("MipGenChild::operator()", "is ending");
    return fl;
  }
};


int main(int n, char ** v) {
  Bob::core::opt().add("--mip", Bob::Property("-f", "lp file name", "roy.lp"));
  Bob::core::opt().add("--mip", Bob::Property("-b", "branching rule"
                       " (possible values = mf, sb, eb, sbmf, ff, lf, lef)", "mf"));
  Bob::core::opt().add("--mip", Bob::Property("-s", "sbmf threshold", 5));
#ifdef HAVE_LIBGLOCK
  Bob::core::opt().add("--mip", Bob::Property("-c", "generate cuts"));
#endif
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<MipTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrBBAlgoEnvProg<MipTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::BBAlgoEnvProg<MipTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<MipTrait> env;
  Bob::core::Config(n, v);
#endif
  MipProblem mp();

  MipInstance *Instance = new MipInstance(
    Bob::core::opt().SVal("--mip", "-f").c_str());
  // Initialize the instance.

  // Solve the instance i.e. all the tree
  std::cout << "-------- Start to solve the entire tree" << std::endl;
  env(Instance);
  /* Alloc one algo.
   * lance le Init de Instance
   * lance le () de l'algo
   */

  //MipNode *nd=new MipNode(Bob::core::opt().NVal("--bb","-lb"));
  // Initialize the node

  //std::cout<<"-------- Start to solve a subtree"<<std::endl;
  // solve only the subtree rooted by the node nd.
  //saenv(nd,Instance);
  /* Alloc one algo.
   * Recupere la meilleur solution connue
   * insere le nd
   * lance le () de l'algo
   */
#ifdef Atha
  Bob::AthaEnvProg::end();
#elif defined(Threaded)
  Bob::ThrEnvProg::end();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::end();
#endif
  delete Instance;
  Bob::core::End();
}
