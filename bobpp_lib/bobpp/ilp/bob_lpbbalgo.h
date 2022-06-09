
/*
 * bob_bx.h
 *
 * This source include the data type for the Branch and X Algo level.
 */

#ifndef BOBBBLP_ABSTRACT
#define BOBBBLP_ABSTRACT

#include <limits>
#include <string>
#include <vector>
#include <cmath>
#include <glop/ppglop.h>
#include <bobpp/bobpp.h>

#include "config.h"

// header files that stores the different LPs classes
#include <bobpp/ilp/bob_lp.h>

// header file that stores the different cuts generators
#include<bobpp/ilp/bob_cut.h>

// header file that stores the differents branching strategies.
#include<bobpp/ilp/bob_branch.h>

namespace  Bob {

/** @defgroup LPBBAlgoGRP The LP Based Branch and X module.
  *  @brief This module describe all classes to implement an LP based
  *  Branch and cut and price algorithm .
  *  @ingroup BBAlgoGRP
  *  @{

This module includes all the classes to facilitate the development of an 
LP based branch and X. 
As all the others Bob++ algorithms, the Stat class, Node class, Instance class, and 
Genchild class are redefined. These classes are called BBLPStat, BBLPNode, 
BBLPInstance and BBLPGenchild and are based on the Branch and Bound classes.

In order to implement a LP Based branch and Bound, the classes use an object that 
stores the linear program. This module proposes several LP classes.
 - LP simple lp which stores the linear program.
 - LPvarcut<class Trait> which stores the linear program, but in addition, during the resolution of the
   problem, some cuts or variables could be dynamicaly generated. The way to choose the different 
   cuts generation algorithms and controls algorithms are explained in the \ref LPBB_GENCUT pages.

This Bob++ module also proposes different branching strategies that are 
 - Most Fractional branching (MostFracBranching<class Trait>)
 - Strong Branching (StrongBranching<class Trait>), 
 - Entropy branching (EntropyBranching<class Trait>),
 - Strong then Most Fractional Branching (StrongThenMostFracBranching<class Trait>)
This different Branching stategies are detailed in the \ref LPBB_Branching pages.

Other features have to be implemented, for example different heuristics to generate a solution.

*/

/**
 * The Statistics for the BBLP algorithm
 */
class BBLPStat : public LPStat {
public:
  /// Constructor
  BBLPStat(const Id &s) : LPStat(s) {
    add_timer('c', "Cuts generation");
    add_timer('v', "Vars generation");
    add_counter('u', "number of cuts");
    add_counter('a', "number of variables");
  }
  /// Destructor
  virtual ~BBLPStat() {}
  /// Start the cut generation time_stat
  void cut_start() {
    get_timer('c').start();
  }
  /// Stop the cut generation time_stat
  void cut_end() {
    get_timer('c').end();
  }
  /// Start the var generation time_stat
  void var_start() {
    get_timer('v').start();
  }
  /// Stop the var generation time_stat
  void var_end() {
    get_timer('v').end();
  }

};

/** The Node of the BBLP
 * This class that represent a node of the LP Based Branch and Bound.
 * It stores
 *    - the Linear program (lp attibut).
 *    - the Branching Information class to konw how to branch on the node
 */
template<class Trait>
class BBLPNode : public BBDoubleMinNode {
  typedef typename Trait::LP TheLP;
  typedef typename Trait::Node TheNode;
  typedef typename Trait::BranchingInfo TheBranchingInfo;
  typedef typename Trait::Stat TheStat;
  typedef typename Trait::Instance TheInstance;
protected:
  /// the Linear program
  TheLP *lp;
  /// the Branchinf information
  TheBranchingInfo bi;
 

public :
  /** Constructor
   */
  BBLPNode() : BBDoubleMinNode(std::numeric_limits<double>::max()), lp(0), bi() {}
  /** Constructor
   * @param n the node to copy from
   */
  BBLPNode(const BBLPNode *n) : BBDoubleMinNode(*n), lp(new TheLP(*(n->lp))), bi(n->bi) {
    lp->add_ref();
  }
  /** Constructor
   * @param n the node to copy from
   */
  BBLPNode(const BBLPNode &n) : BBDoubleMinNode(n), lp(new TheLP(*(n.lp))), bi(n.bi) {
    lp->add_ref();
  }
  /** Constructor
   * @param inst the instance used to build the node. 
   */
  BBLPNode(const TheInstance *inst) : BBDoubleMinNode(), lp(0), bi() {
    lp = new TheLP(*(inst->lp));
    BOBASSERT(lp!=0,NullPointerException,"BBLPNode","Init form instance : the new lp is null");
    lp->add_ref();
  }
  /** Destructor
   */
  virtual ~BBLPNode() {
    if (lp != 0) {
      lp->rem_ref();
      if (lp->is_null_ref()) {
        delete lp;
        lp = 0;
      }
    }
  }
  /** method to copy a node in another one
   * @param ln the source node.
   */
  virtual void copy(const BBLPNode<Trait> &ln) {
      std::cout << "Copy the node\n";
      lp = ln.lp;
      lp->add_ref();
      bi = ln.bi;
  }
  /** get the associated LP
    * @return the internal Linera program
    */
  TheLP *Lp() { return lp; }
  /** return the Branching Information of the node
   * @return the branching information
   */
  TheBranchingInfo &getBranchingInfo() {
    return bi;
  }
  /** Test if the node i.e. the associated Linear program is feasible or not.
   */
  bool isFeasible() {
    return lp->isFeasible();
  }
  /** Performs a simple evaluation
   * @param inst the Instance
   * @param st the statistics
   * @param max the value of the incubent.
   */
  void one_eval(const TheInstance *inst, TheStat *st, double max) {
    lp->set_obj_upper_limit(max);
    lp->one_solve(st, dist(), max);
    setEval(lp->get_obj_val());
    if ( isFeasible() ) {
      lp->do_test_integer();
      if (lp->isInteger()) setSol();
    }
  }
  /** Performs a complete evaluation with cuts and variables generation
   * @param inst the Instance
   * @param st the statistics
   * @param max the value of the incubent.
   */
  virtual void eval(const TheInstance *inst, TheStat *st, double max) {
    lp->set_obj_upper_limit(max);
    lp->solve(st, dist(), max);
    setEval(lp->get_obj_val());
    if ( isFeasible() ) {
      lp->do_test_integer();
      if (lp->isInteger()) setSol();
    }
  }
  /** Compute the Branching strategy according the chossen one.
   * @param inst the Instance
   * @param st the statistics
   * @param b_val the value of the incubent.
   */
  virtual void comp_branch(const TheInstance *inst, TheStat *st, double b_val) {
    inst->br->branch(lp, &bi, *st, dist(), b_val);
  }
  /** Do the ith branches 
   * @param i the branch to compute.
   * @param st the statistics
   */
  virtual void do_branch(int i, TheStat *st) {
    bi.do_branch(i, (TheNode *)this);
  }
  /** Pack the node
   *  @param bs the Bob::Serialize object
   */
  virtual void Pack(Bob::Serialize &bs) const {
  int haslp=1;
    //std::cout<< "--Deb pack Node "<< getEval()<<":"<<dist()<<"\n";
    BBDoubleMinNode::Pack(bs);
    if ( lp ) {
      //std::cout <<"  with lp\n";
      bs.Pack(&haslp,1);
      lp->Pack(bs);
    } else {
      //std::cout <<"  without lp\n";
      haslp=0;bs.Pack(&haslp,1);
    }
    bi.Pack(bs);
    //std::cout<< "--Fin pack Node \n";
  }
  /** Method to unpack the node
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    int haslp;
    //std::cout<< "--Deb Unpack Node \n";
    BBDoubleMinNode::UnPack(bs);
    bs.UnPack(&haslp,1);
    if ( haslp==1 ) {
      lp = new TheLP();
      //std::cout <<"  with lp\n";
      BOBASSERT(lp!=0,NullPointerException,"BBLPNode","UnPack : lp is null");
      lp->UnPack(bs);
      lp->add_ref();
    } else {
      //std::cout <<"  without lp\n";
      lp=0;
    }
    bi.UnPack(bs);
    //std::cout<< "--Fin Unpack Node "<< getEval()<<":"<<dist()<<"\n";
  }

};

/** The instance of the BBLP module
 * Mainly the instance stores the LP of the root nodes, and the object 
 * representing the branching strategy.
 */
template<class Trait>
class BBLPInstance : public BBInstance<Trait> {
  typedef typename Trait::Instance TheInstance;
public:
  /// type of the LP
  typedef typename Trait::LP TheLP;
  /// type of the Node
  typedef typename Trait::Node TheNode;
  /// type of the Branching strategy
  typedef typename Trait::Branching TheBranching;
  /// the file name of the lp.
  std::string fname;
  /// the LP
  TheLP *lp;
  /// the Branching strategy 
  TheBranching *br;
  /// the root node
  TheNode *root;
  /// the initial solution.
  TheNode *isol;

  /** Constructor
   */
  BBLPInstance() : BBInstance<Trait>("BBLP"), fname(""), lp(0), br(new TheBranching()),isol(0) {
    DBGAFF_USER("BBLPInstance::BBLPInstance()", "Called");
    BOBASSERT(br!=0,NullPointerException,"BBLPInstance","BBLPInstance : br is null");
  }
  /** Constructor
   * @param _fname the file name that stores the instance.
   */
  BBLPInstance(const char * _fname) : BBInstance<Trait>("BBLP"), fname(_fname),
      lp(0) , br(new TheBranching()),isol(0) {
    DBGAFF_USER("BBLPInstance::BBLPInstance()", "Called");
    BOBASSERT(br!=0,NullPointerException,"BBLPInstance","BBLPInstance : br is null");
  }

  /** Constructor
   * @param mii the Instance to copy.
   */
  BBLPInstance(const BBLPInstance &mii) : Bob::BBInstance<Trait>(mii), fname(mii.fname),
      lp(mii.lp), br(new TheBranching()),isol(mii.isol) {
    lp->add_ref();
    BOBASSERT(br!=0,NullPointerException,"BBLPInstance","BBLPInstance : br is null");
  }
  /** Constructor
   * @param mii the Instance to copy.
   */
  BBLPInstance(BBLPInstance &mii) : Bob::BBInstance<Trait>(mii), fname(mii.fname),
      lp(mii.lp) , br(new TheBranching()),isol(mii.isol) {
    lp->add_ref();
    BOBASSERT(br!=0,NullPointerException,"BBLPInstance","BBLPInstance : br is null");
  }
  /** Destructor
   */
  virtual ~BBLPInstance() {
    if (lp != 0) {
      lp->rem_ref();
      if (lp->is_null_ref()) {
        delete lp;
        lp = 0;
      }
    }
    if (br != 0) {
      delete br;
    }
  }
  /** the copy method
   */
  virtual void copy(const BBLPInstance &li) {
      std::cout << "Copy the instance\n";
      fname = li.fname;
      lp = li.lp;
      lp->add_ref();
  }

  /// Initialize the Instance
  virtual void Init() {
    lp = new TheLP();
    lp->add_ref();
    BOBASSERT(lp!=0,NullPointerException,"BBLPInstance","Init : lp is null");
    if (strcmp(fname.c_str(), "")) {
      if (lp->read_lpt(fname.c_str()) != 0) {
        std::cerr << "BBLPInstance::Init():  Filename " << fname << " not found, exit !";
        exit(1);
      }
      lp->set_class(GSP_LP);
      std::cout <<"--------------- Dans le Init BBLPInstance ---------------------\n";
    }
  }
  /** Initialize the algorithm
   * @param a the algo
   */
  virtual void InitAlgo(typename Trait::Algo *a) const {
    std::cout <<"--------------- Dans le InitAlgo BBLPInstance ---------------------\n";
    TheNode *nd = new TheNode((const TheInstance *)this);
    BOBASSERT(nd!=0,NullPointerException,"BBLPInstance","InitAlgo : the new node is null");
    nd->eval((TheInstance *)this, a->getStat(), a->getGoal()->getBest());
    //a->log_space(nd);
    a->GPQIns(nd);
  }
  /** Get the initial best known solution
   * @return the initial solution.
   */
  virtual TheNode *getSol() {
    if ( isol!=0 )  return isol;
    return new TheNode();
  }
  /** Pack method to serialize the Instance
    * @param bs the Bob::Serialize object
    */

  virtual void Pack(Bob::Serialize &bs) const {
    DBGAFF_USER("BBLPInstance::Pack()", "Called");
    std::cout<< "--Deb pack instance\n";
    Bob::BBInstance<Trait>::Pack(bs);
    Bob::Pack(bs, &fname, 1);
    BOBASSERT(lp!=0,NullPointerException,"BBLPInstance","Pack : the lp is null");
    Bob::Pack(bs, lp);
    std::cout<< "--Fin pack instance\n";
  }
  /** Unpack method to deserialize the Instance
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_USER("BBLPInstance::UnPack()", "Called");
    std::cout<< "--- Deb Unpack instance\n";
    Bob::BBInstance<Trait>::UnPack(bs);
    Bob::UnPack(bs, &fname, 1);
    if (lp)
      delete lp;
    lp = new TheLP();
    BOBASSERT(lp!=0,NullPointerException,"BBLPInstance","UnPack : the new lp is null");
    Bob::UnPack(bs, lp);
    std::cout<< "--- Fin Unpack instance\n";

  }
};


/** The BBLPGenchild class the is classical Bobpp Genchild class the generates
 * child nodes using the branching strategy strores in the Instance.
 */
template<class Trait>
class BBLPGenChild: public BBGenChild<Trait> {
public:
  typedef typename Trait::Node TheNode;
  /** Constructor of BBLPGenChild
    * @param _inst the Instance of the problem to solve.
    * @param _al the associated algorithm.
    */
  BBLPGenChild(const typename Trait::Instance *_inst, typename Trait::Algo *_al) :
      BBGenChild<Trait>(_inst, _al) {
    DBGAFF_USER("BBLPGenChild::BBLPGenChild()", "Called");
  }
  /** Destructor of BBLPGenChild
    */
  virtual ~BBLPGenChild() {
    DBGAFF_USER("BBLPGenChild::~BBLPGenChild()", "Called");
  }
  /** Operator called by the algo to Generate child
    * @param n the node to explore.
    *
    * In this genchild, the Branching Information is use to know the number of
    * generated nodes. For each children node, the do_branch() is called,
    * then the eval() method.
    * The eval() method of the node also calls the Cut or Var Generator.
    *
    * If the node is feasible, the node is inserted in the priority queue 
    * otherwise it's deleted.
    */
  bool operator()(typename Trait::Node *n) {
    bool fl = false;
    DBGAFF_USER("BBLPGenChild::operator()", "is beginning");
    typename Trait::Node *child;
    typename Trait::Algo *al = BBGenChild<Trait>::algo;
    const typename Trait::Instance *instance = BBGenChild<Trait>::inst;
    int i;
    if ( n->isSol() ) {
        return al->Search(n);
    }
    n->comp_branch(instance, al->getStat(), al->getGoal()->getBest());
    for (i = 0; i < n->getBranchingInfo().nbChildren() ; i++) {
      child = new TheNode(*n);
      BOBASSERT(child!=0,NullPointerException,"BBLPGenChild","operator() : the new child node  is null");
      child->dist()++;
      al->start_eval(child,n);
      child->do_branch(i, al->getStat());
      child->eval(instance, al->getStat(), al->getGoal()->getBest());
      al->end_eval(child);
      if (child->isFeasible()) {
        fl +=al->Search(child);
      } else {
        al->StNoFea(child);
        delete child;
      }
    }
    DBGAFF_USER("BBLPGenChild::operator()", "is ending");
    return fl;
  }
};

/** @}
 */

}; // end of namespace
#endif
