/* 
 *  File   : flowshop.cpp
 *  Author : Lei Liu @polimi
 *  Date   : 10 Apr, 2021
 *  Comment: 
 */

#pragma once
#include <bobpp/bobpp>

#define Threaded

#ifdef Threaded
#  include <bobpp/thr/thr.h>
#endif

#include <string>
#include <vector>
#include <Eigen/unsupported/Eigen/MatrixFunctions>
#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;


#ifndef PFSHEADER
#define PFSHEADER
class PFSInstance;
class PFSNode;
class PFSGenChild;
class PFSStat;



class PFSTrait {
  public:    
    typedef PFSNode Node;
    typedef PFSInstance Instance;
    typedef PFSGenChild GenChild;
    typedef Bob::BBAlgo<PFSTrait> Algo;
    typedef Bob::BBGoalBest<PFSTrait> Goal;
    typedef Bob::DepthEPri<PFSNode> PriComp;   // depth first 
    typedef Bob::BBStat Stat;
};


#define AFFMESS(m)


// low bound class
class OneMachine;





/** Class used to compute the cost of a full schedule
 */
class Scheduled {
Bob::pvector<int> r;
Bob::pvector<int> d;
public:
      Scheduled() : r(),d() {} 
      Scheduled(int n) : r(n,0),d(n,0) { } 
      Scheduled(const Scheduled &md) : r(md.r),d(md.d) {} 
      virtual ~Scheduled() {} 
      virtual void copy(const Scheduled &md){}
      
      double getCost(const PFSInstance &pfi, const Bob::Permutation &per, double x1LB);
      // get the state numbers for blocks
      static std::vector<std::vector<int> > getstateNum(int n);
      void structureCon(std::vector<int> PS, const PFSInstance *pfi, MatrixXd &Qstar);
      double calObjDiscrete(std::vector<int> PS, const PFSInstance *pfi, const Bob::Permutation &per, double x1LB, double x2UB);
      static double bisection(MatrixXd Qstar, VectorXd a, double VaRalpha, double x1, double x2);
      static double boost_Bisect(MatrixXd Qstar,  VectorXd a,double x1lb,  double x2ub, double VaRalpha);
      static double boost_Bracket(MatrixXd Qstar,  VectorXd a,double x1lb,  double x2ub, double VaRalpha);
      static double false_pos(MatrixXd Qstar, double VaRalpha, VectorXd a,double x1, double x2);
      static double Illinois(MatrixXd Qstar,  double VaRalpha, VectorXd a,double x1,  double x2);
      static double cdfCal(MatrixXd Qstar, VectorXd a, double x1);
      static double cdfCal2( MatrixXd A,  VectorXd v, double x1);
      void balance_matrix(Eigen::MatrixXd &A, Eigen::MatrixXd &Aprime, Eigen::MatrixXd &D);
      void calAlpha(MatrixXd A, std::vector<int> mList, std::vector<double> thetaList, int &sNum, int &numM);
      void sANDm(int &sNum, int &numM, MatrixXd A);
      double CAM_algo(MatrixXd &A, VectorXd &v, double t);
      double cdfCal3( MatrixXd A,  VectorXd v, double x1);
      

      /// Pack method to serialize the BobNode
      virtual void Pack(Bob::Serialize &bs)  const {
         DBGAFF_ENV("Scheduled::Pack","Pack a Scheduled");
      }
      /// Unpack method to deserialize the BobNode
      virtual void UnPack(Bob::DeSerialize &bs)  {
         DBGAFF_ENV("Scheduled::UnPack","UnPack a Scheduled");
      }
};



/*
* the Node of the search
*/
class PFSNode : public Bob::BBDoubleMinNode {
protected:
int nbj;
int nbm;
Bob::Permutation per;
Scheduled sc;

public:
    double x1LB;  // parent node VaR value as the x1 for bisection of child node
    double x2UB;  // current best VaR value as the x2 for bisection of child node
    /// Contructor
    PFSNode() : Bob::BBDoubleMinNode(),nbj(0),nbm(0),per(),sc() { }
    /// Contructor
    PFSNode(int _nbj,int _nbm) : Bob::BBDoubleMinNode(0.0),nbj(_nbj),nbm(_nbm),per(_nbj),sc(_nbm) { 
    }
    /// Contructor
    PFSNode(int _nbj,int _nbm,double cost) : Bob::BBDoubleMinNode(cost),nbj(_nbj),nbm(_nbm),per(_nbj),sc(_nbm) { 
    }
    PFSNode(const PFSNode &p) : Bob::BBDoubleMinNode(p),nbj(p.nbj),nbm(p.nbm),per(p.per),sc(p.sc) {
    }
    /// Contructor
    PFSNode(PFSNode *p) : Bob::BBDoubleMinNode(*p),nbj(p->nbj),nbm(p->nbm),per(p->per),sc(p->sc) { 
    }
    /// Contructor
    PFSNode(const PFSInstance *pi,const Bob::Permutation &per);
    /// Fixe the job i on the next free location
    void FixeR(int i,const PFSInstance &pfi);
    /// Fixe the job i on the last free location
    void FixeD(int i,const PFSInstance &pfi);

    // double getCost(const PFSInstance &pfi, const Bob::Permutation &per);




    /// Destructor
    virtual ~PFSNode() {
    }
    virtual void copy(const PFSNode &pn) {
      Bob::BBDoubleMinNode::copy(pn);
      nbj= pn.nbj;
      nbm= pn.nbm;
      x1LB = pn.x1LB;
      x2UB = pn.x2UB;
      per.copy(pn.per);
      sc.copy(pn.sc);
    }
    /// get the permutation
    Bob::Permutation &perm() { return per; }
    /// get the Scheduled 
    Scheduled &sched() { return sc; }



    /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs)  const {
       DBGAFF_ENV("PFSNode::Pack","Pack a node");
      Bob::BBDoubleMinNode::Pack(bs);
       Bob::Pack(bs,&nbj,1);
       Bob::Pack(bs,&nbm,1);
       Bob::Pack(bs,&x1LB,1);
       Bob::Pack(bs,&x2UB,1);
       Bob::Pack(bs,&per);
       Bob::Pack(bs,&sc);
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("PFSNode::UnPack","UnPack a node");
      Bob::BBDoubleMinNode::UnPack(bs);
       Bob::UnPack(bs,&nbj,1);
       Bob::UnPack(bs,&nbm,1);
       Bob::UnPack(bs,&x1LB,1);
       Bob::UnPack(bs,&x2UB,1);
       Bob::UnPack(bs,&per);
       Bob::UnPack(bs,&sc);
    }

    /// evaluate the node
    void eval(const PFSInstance *pfi,double minCost, double x1LB, double x2UB);
    

    /// Display a PFS Node
    virtual ostream &Prt(ostream &os) const {
        if ( isPb() ) {
           os << "["<<getEval()<<"]\n ";
           per.Prt(os);
           // sc.Prt(os);
        } else {
           os << "Solution Cost :"<<getEval()<<"\n";
           per.Prt(os);
        }
        return os;
    }
};

/** the genchild class for the permutation flowshop */
// Branch scheme
class PFSGenChild : public Bob::BBGenChild<PFSTrait>  {
    public:
      PFSGenChild(const PFSInstance *_inst, PFSTrait::Algo *_sch) : 
             Bob::BBGenChild<PFSTrait>(_inst,_sch) {}
      virtual ~PFSGenChild() {}
      virtual bool operator()(PFSNode *n);
};

/*
SchedulingInstance is a virtual calss that inherited from BBInstance
This class can be inherited by any kind of scheduling problems, i.e. flow shop scheduling in this work, single machine scheduling, job shop scheduling

*/

// A new class for scheduling problem
class SchedulingInstance : public Bob::BBInstance<PFSTrait>{
public:
    SchedulingInstance(const char *_n) : Bob::BBInstance<PFSTrait> (_n){}

    virtual ~SchedulingInstance() {}
};



/// The Instance class of our flow shop  problem
class PFSInstance : public virtual SchedulingInstance{
public:
/// The file name of the problem instannce
std::string file;
int lobd;
int nbj,nbm;
double ** d_b;
OneMachine *lb1m;  ////  instance Lower bound
int line;
double biLB;
double biUB;
   /// default Constructor without parameters
   // PFSInstance();
   /// Constructor with parameters
   // PFSInstance(std::string _file,int _lobd, int _line);
   PFSInstance() : SchedulingInstance("pfs"),file(""),lobd(0),nbj(0),nbm(0),d_b(0),lb1m(0), line(0){lobd=0;}
   PFSInstance(std::string _file,int _lobd, int _line) : SchedulingInstance("pfs"),file(_file),lobd(_lobd),line(_line),nbj(0),nbm(0),d_b(0){}

   
   /// Destructor
   virtual ~PFSInstance() {}
   virtual void copy(const PFSInstance &pfi);
   
   /// Initialization of the instance
   void Init();
   /// the method that initialize and insert the root nodes.
   /// the method that initialize and insert the initial upper bound.
   PFSNode *getSol();
   void InitAlgo(PFSTrait::Algo *al) const {
       PFSNode *qn;
       qn = new PFSNode(nbj,nbm);
       qn->eval(this,0,biLB , biUB); //this is the LB limit, give a small number
       // std::cout << "Root Evaluation :"<<qn->getEval()<<std::endl;
       al->GPQIns(qn);
   }
   /// reads the data from files.
   void read();
   /// Pack method to serialize the BobNode
   virtual void Pack(Bob::Serialize &bs) const;
   /// Unpack method to deserialize the BobNode
   virtual void UnPack(Bob::DeSerialize &bs);
};


#endif
