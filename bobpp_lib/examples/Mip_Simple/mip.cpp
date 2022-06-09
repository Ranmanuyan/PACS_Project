/*
 * Sample Branch & Bound program : MIP solver
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

#define ACCURACY 0.000001

class MipProblem;
class MipInstance;
class MipGenChild;
class MipNode;

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
    typedef Bob::BestEPri<MipNode> PriComp;
    //typedef Bob::DepthPri<MipNode> PriComp;
    typedef Bob::BBStat Stat;
};

class intvar {
public:
    int num;
    std::string name;
    void Pack(Bob::Serialize &bs) const {
        bs.Pack(&num, 1);
        bs.Pack(&name, 1);
    }
    void UnPack(Bob::DeSerialize &bs)  {
        bs.UnPack(&num, 1);
        bs.UnPack(&name, 1);
    }
    std::ostream &Prt(std::ostream &os = std::cout) const {
        os << "/" << name << "/ =" << num << std::endl;
        return os;
    }
};


struct mip_cut {
    int var;
    double bound;
    bool direction_is_low;
    void Pack(Bob::Serialize &bs) const {
        bs.Pack(&var, 1);
        bs.Pack(&bound, 1);
        bs.Pack(&direction_is_low, 1);
    }
    void UnPack(Bob::DeSerialize &bs) {
        bs.UnPack(&var, 1);
        bs.UnPack(&bound, 1);
        bs.UnPack(&direction_is_low, 1);
    }
    std::ostream & Prt(const vector<intvar> & it, ostream &os = std::cout) const {
        int i = 0;
        while (it[i].num != var) i++;
        os << it[i].name;
        if (direction_is_low)
            os << " >= " << ceil(bound);
        else
            os << " <= " << floor(bound);
        return os;
    }
    std::ostream & Prt(ostream &os = std::cout) const {
        if (direction_is_low)
            os << "var[" << var << "] >= " << ceil(bound);
        else
            os << "var[" << var << "] <= " << floor(bound);
        return os;
    }
};

class MipNode : public Bob::BBDoubleMinNode {
    bool feasible;
    bool serialized;
    // the cuts that have already been performed.
    vector<mip_cut> cuts;
    // the modified LP, including cuts.
    GSP * lp;
    // the status of the variables (for warm start resolution of non-root nodes)
    int column_status_size;
    int row_status_size;
    int * column_status;
    int * row_status;
    // variable number for next branch
    int bestvar;
    double bestval;

    void addCutLp(mip_cut &c) {
        if (c.var < 0)
            abort();
        //std::cout << "adding cut: x[" << c.var << "] "
        //	<< (c.direction_is_low?"> ":"< ") << c.bound << std::endl;
        static double one = 1.0;
        int dir = c.direction_is_low ? GSP_LO : GSP_UP;
        gsp_add_row(lp, 1, &c.var, &one, dir, c.bound, NULL);
    }

public:
    MipNode() : Bob::BBDoubleMinNode(), feasible(false), serialized(false),
            cuts(0), lp(0), column_status_size(0), row_status_size(0),
            column_status(0), row_status(0), bestvar(-1), bestval(0.) {
        DBGAFF_USER("MipNode::MipNode()", "Called");
        setEval(GSP_INF);
    }
    MipNode(GSP * _lp, int depth) : Bob::BBDoubleMinNode(), feasible(false),
            serialized(false), cuts(0), lp(_lp),  column_status_size(0),
            row_status_size(0), column_status(0), row_status(0), bestvar(-1),
            bestval(0.) {
        DBGAFF_USER("MipNode::MipNode(GSP *, const vector<intvar> *, int)",
                    "Called");
        dist() = depth + 1;
    }
    MipNode(MipNode & n, bool copy_lp) : Bob::BBDoubleMinNode(n), feasible(false),
            serialized(false), cuts(n.cuts), lp(0), column_status_size(0),
            row_status_size(0), column_status(0), row_status(0), bestvar(n.bestvar),
            bestval(n.bestval) {
        DBGAFF_USER("MipNode::MipNode(MipNode &, bool)", "Called");
        if (copy_lp) {
            lp = gsp_copy_prob(n.lp);
            gsp_set_verbosity(lp, 1);
            gsp_set_presolve(lp, 0);
        } else {
            lp = n.lp;
            n.lp = 0;
        }
        dist() = n.dist() + 1;
    }
    MipNode(const MipNode & n) :
            Bob::BBDoubleMinNode(n), feasible(false), serialized(true), cuts(n.cuts),
            lp(0), column_status_size(n.column_status_size),
            row_status_size(n.row_status_size),
            column_status(0), row_status(0),  bestvar(n.bestvar), bestval(n.bestval) {
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
            memcpy(column_status, n.column_status, sizeof(int)*column_status_size);
            memcpy(row_status, n.row_status, sizeof(int)*row_status_size);
        }
    }
    virtual ~MipNode() {
        DBGAFF_USER("MipNode::~MipNode", "Called");
        if (lp)
            gsp_delete_prob(lp);
        if (column_status)
            delete [] column_status;
        if (row_status)
            delete [] row_status;
    }
    virtual void copy(const MipNode &n) {
         Bob::BBDoubleMinNode::copy(n);
         feasible = n.feasible;
         serialized= n.serialized;
         cuts= n.cuts;
         column_status_size = n.column_status_size;
         row_status_size = n.row_status_size;
         column_status = new int[column_status_size];
         row_status = new int[row_status_size];
         memcpy(column_status, n.column_status, sizeof(int)*column_status_size);
         memcpy(row_status, n.row_status, sizeof(int)*row_status_size);
         bestvar = n.bestvar;
         bestval = n.bestval;
    }
    void eval(const vector<intvar> & it) {
        DBGAFF_USER("MipNode::eval()", "Called");
        gsp_solve(lp);
        if (gsp_get_status(lp) != GSP_LP_NOFEAS) {
            unsigned int i, num;
            double val, frc, bestfrc;
            const std::string *name;
            feasible = true;
            val = gsp_get_obj_val(lp);
            setEval(val);
            //std::cout << "Eval "<<val<<std::endl;
            i = 0;

            bestvar = -1;
            bestfrc = 0.0;
            bestval = 0.0;
            for (i = 0; i < it.size(); i++) {
                num = it[i].num;
                gsp_get_col_prim(lp, &val, num, num);
                frc = fabs(val - nearbyint(val));
                if (frc >= ACCURACY && frc > bestfrc) {
                    bestfrc = frc;
                    bestvar = num;
                    bestval = val;
                    name = &it[i].name;
                }
            }

            if (bestvar == -1) {
                std::cout << "Solution found: " << getEval() << std::endl;
                setSol();
            }
            //else
            //  std::cout << "Branching on " << *name << "=" << bestval << std::endl;
        }
        /*    else
            {
              unsigned int i;
              std::cout<<"Infeasible node :"<<std::endl;
              for(i=0;i<cuts.size();i++)
                cuts[i].Prt(it);
            }
        */
    }

    void addCut(int var, double value, bool direction_is_low) {
        DBGAFF_USER("MipNode::addCut()", "Called");
        mip_cut c;
        c.var = var;
        c.bound = direction_is_low ? ceil(value) : floor(value);
        c.direction_is_low = direction_is_low;
        addCutLp(c);
        cuts.push_back(c);
    }

    void recoverFromInstance(GSP * lp) {
        DBGAFF_USER("MipNode::recoverFromInstance()", "Called");
        int i, n_cuts;
        this->lp = gsp_copy_prob(lp);
        gsp_set_verbosity(this->lp, 1);
        gsp_set_presolve(this->lp, 0);
        n_cuts = cuts.size();
        for (i = 0; i < n_cuts; i++)
            addCutLp(cuts[i]);
        gsp_set_col_stat(this->lp, column_status, 0, gsp_get_num_cols(this->lp) - 1);
        gsp_set_row_stat(this->lp, row_status, 0, gsp_get_num_rows(this->lp) - 1);
        gsp_warmup_basis(this->lp);
        serialized = false;
    }

    bool isFeasible() {
        return feasible;
    }

    bool isSerialized() {
        return serialized;
    }

    int bestVar() {
        return bestvar;
    }

    double bestVal() {
        return bestval;
    }

    GSP * getLp() {
        return lp;
    }

    virtual ostream &Prt(ostream &os = std::cout) const {
        int /*i,*/ nc = cuts.size();
        if (lp) {
            os << "LP Objective value: " << gsp_get_obj_val(lp) << std::endl;
            os << "nrows = " << gsp_get_num_rows(lp) << std::endl;
            os << "ncols = " << gsp_get_num_cols(lp) << std::endl;
        }
        os << "Evaluation = " << getEval() << std::endl;
        os << "dist = " << dist() << std::endl;
        os << "bestval = " << bestval << endl;
        os << "bestvar = " << bestvar << endl;
        os << "row_status_size = " << row_status_size << endl;
        os << "column_status_size = " << column_status_size << endl;
        os << "n_cuts = " << nc << endl;
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
        int n_cuts = cuts.size();
        Bob::BBDoubleMinNode::Pack(bs);
        bs.Pack(&n_cuts, 1);
        for (i = 0; i < n_cuts; i++)
            cuts[i].Pack(bs);
        bs.Pack(&bestvar, 1);
        bs.Pack(&bestval, 1);
        if (lp) {
            int ncols = gsp_get_num_cols(lp);
            int nrows = gsp_get_num_rows(lp);
            int column_status[ncols];
            int row_status[nrows];
            bs.Pack(&ncols, 1);
            bs.Pack(&nrows, 1);
            gsp_get_col_stat(lp, column_status, 0, ncols - 1);
            gsp_get_row_stat(lp, row_status, 0, nrows - 1);
            bs.Pack(column_status, ncols);
            bs.Pack(row_status, nrows);
        } else {
            bs.Pack(&column_status_size, 1);
            bs.Pack(&row_status_size, 1);
            bs.Pack(column_status, column_status_size);
            bs.Pack(row_status, row_status_size);
        }
    }

/// Unpack method to deserialize the Node
    virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("MipNode::UnPack()", "Called");
        int i;
        int n_cuts;
        mip_cut c;
        Bob::BBDoubleMinNode::UnPack(bs);
        bs.UnPack(&n_cuts, 1);
        for (i = 0; i < n_cuts; i++) {
            c.UnPack(bs);
            cuts.push_back(c);
        }
        bs.UnPack(&bestvar, 1);
        bs.UnPack(&bestval, 1);
        bs.UnPack(&column_status_size, 1);
        bs.UnPack(&row_status_size, 1);
        if (column_status_size) {
            column_status = new int[column_status_size];
            row_status = new int[row_status_size];
            bs.UnPack(column_status, column_status_size);
            bs.UnPack(row_status, row_status_size);
        }
        serialized = true;
    }
};

class MipProblem : public Bob::Problem<MipTrait> {
public:
    MipProblem() : Bob::Problem<MipTrait>() {}
    virtual ~MipProblem() {}
};

class MipInstance : public Bob::BBInstance<MipTrait> {
    const char * fname;
public:
    vector<intvar> integers;
    GSP * lp;

    /// Constructor
    MipInstance() : Bob::BBInstance<MipTrait>("MIP"), fname(0), integers(), lp(0) {
        DBGAFF_USER("MipInstance::MipInstance()", "Called");
    }
    /// Constructor
    MipInstance(const char * _fname) : Bob::BBInstance<MipTrait>("MIP"),
            fname(_fname), lp(0) {
        intvar iv;
        int i, ncols, k;
        DBGAFF_USER("MipInstance::MipInstance()", "Called");
        lp = gup_read_lpt((char *)fname);
        gsp_set_verbosity(lp, 1);
        gsp_set_presolve(lp, 0);
        ncols = gsp_get_num_cols(lp);
        for (i = 0; i < ncols; i++) {
            gsp_get_col_kind(lp, &k, i, i);
            if (k == GSP_IV) {
                k = GSP_CV;
                gsp_set_col_kind(lp, &k, i, i);
                iv.num = i;
                iv.name = gsp_get_col_name(lp, i);
                integers.push_back(iv);
            }
        }
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
        gsp_solve(lp);
        if (gsp_get_status(lp) == GSP_LP_NOFEAS) {
            std::cerr << " Non feasible Problem\n";
            exit(1);
        }

    }

    /// Constructor
    MipInstance(const MipInstance &mii) : Bob::BBInstance<MipTrait>(mii),
            fname(mii.fname), integers(mii.integers), lp(mii.lp) {}
    /// Constructor
    MipInstance(MipInstance &mii) : Bob::BBInstance<MipTrait>(mii),
            fname(mii.fname), integers(mii.integers), lp(mii.lp) {}
    virtual ~MipInstance() {
        //gsp_delete_prob(lp);
    }
    /// The init method
    void Init() {}
    /// The init Algo method
    void InitAlgo(MipTrait::Algo *a) const {
        std::cout << "MipInstance::Init(Algo) Begins" << std::endl;
        GSP * nlp;
        nlp = gsp_copy_prob(lp);
        gsp_set_verbosity(nlp, 1);
        gsp_set_presolve(nlp, 0);
        MipNode *nd = new MipNode(nlp, -1);
        nd->eval(integers);
        //a->log_space(nd);
        a->GPQIns(nd);
        std::cout << "MipInstance::Init(Algo) ends" << std::endl;
    }
    /// Get the initial best known solution
    MipNode *getSol() {
        std::cout << "Instance::GetSol\n";
        return new MipNode();
    }
    /// Pack method to serialize the Instance
    virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("MipInstance::Pack()", "Called");
        Bob::BBInstance<MipTrait>::Pack(bs);
        int n_ints = integers.size();
        int i;
        bs.Pack(&n_ints, 1);
        for (i = 0; i < n_ints; i++)
            integers[i].Pack(bs);
        gup_serialize s(&bs);
        gup_pack(lp, &s);
    }
    /// Unpack method to deserialize the Instance
    virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("MipInstance::UnPack()", "Called");
        Bob::BBInstance<MipTrait>::UnPack(bs);
        int n_ints;
        int i;
        intvar iv;
        if (lp)
            gsp_delete_prob(lp);
        bs.UnPack(&n_ints, 1);
        for (i = 0; i < n_ints; i++) {
            iv.UnPack(bs);
            integers.push_back(iv);
        }
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
        const vector<intvar> & it = inst->integers;
        if (n->isSerialized()) {
            //std::cout<<"recoverFromInstance "<<n->getEval()<<" "<<n->bestVar()<<std::endl;
            n->recoverFromInstance(inst->lp);
        }
        bestvar = n->bestVar();
        bestval = n->bestVal();
        if (bestvar == -1) {
            std::cout << "Sortie avec bestVar -1 :" << n->getEval() << " " << it.size() << "\n";
        }
        for (i = 0; i < 2; i++) {
            child = new MipNode(*n, i == 0);
            child->addCut(bestvar, bestval, i == 1);
            algo->start_eval(child,n);
            child->eval(it);
            algo->end_eval(child);
            if (child->isFeasible()) {
                fl +=algo->Search(child);
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
    Bob::core::opt().add(std::string("--mip"),
                         Bob::Property("-f", "lp file name", "roy.lp"));
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
    Bob::mpx::BBAlgoEnvProg<MipTrait> env;
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
