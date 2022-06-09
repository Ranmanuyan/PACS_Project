/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    Opale Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                     Authors:  B. Le Cun F. Galea
 *                    (C) 2007 All Rights Reserved
 *
 *                              NOTICE
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * Neither the institutions (Versailles University, PRiSM Laboratory, 
 * the Opale Team), nor the Authors make any representations about the 
 * suitability of this software for any purpose.  This software is 
 * provided ``as is'' without express or implied warranty.
 *
 */

/* 
 *  File   : flowshop.cpp
 *  Author : B. Le Cun based on source of the ETSI Plugtest.
 *  Date   : Nov 11 2007.
 *  Comment: Source file to solve the permutation flowshop using the Bob++ library
 */


#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>

#include <bobpp/bobpp>
#ifdef Threaded
#  include <bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif



/** \mainpage Permutation-Flowshop/Bob++
    @author Bertrand Le cun
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the permutation flowshop. 
A Branch and bound is used to exactly solve some quite large instances.

*/


class PFSInstance;
class PFSNode;
class PFSGenChild;
class PFSStat;

class PFSTrait {
  public:    
    typedef PFSNode Node;
    typedef PFSInstance Instance;
    typedef PFSGenChild GenChild;
    typedef Bob::CDCAlgo<PFSTrait> Algo;
    typedef Bob::CDCGoalBest<PFSTrait> Goal;
    typedef Bob::DepthPri<PFSNode> PriComp;
    typedef Bob::CDCStat Stat;
};

/** Class used to compute the cost of a partial solution.
 */
class MachineDate : public Bob::pvector<int> {
public:
      MachineDate() : Bob::pvector<int>() {} 
      MachineDate(int n) : Bob::pvector<int>(n,0) { } 
      MachineDate(const MachineDate &md) : Bob::pvector<int>(md) {} 
      int get(int i) const { return (*this)[i]; }
      void put(int i,int v) { (*this)[i]=v; }
      int getCost() const { return get(size()-1); }
      void Compute(const PFSInstance &pfi, int i);

};


/** the Node of the search
 */
class PFSNode : public Bob::CDCIntMinNode {
protected:
int nbj;
int nbm;
Bob::Permutation per;
MachineDate md;
public:
    /// Contructor
    PFSNode() : Bob::CDCIntMinNode(),nbj(0),nbm(0) { }
    /// Contructor
    PFSNode(int _nbj,int _nbm) : Bob::CDCIntMinNode(0),nbj(_nbj),nbm(_nbm),per(_nbj),md(_nbm) { 
    }
    /// Contructor
    PFSNode(int _nbj,int _nbm,int cost) : Bob::CDCIntMinNode(cost),nbj(_nbj),nbm(_nbm),per(_nbj),md(_nbm) { 
    }
    PFSNode(const PFSNode &p) : Bob::CDCIntMinNode(p),nbj(p.nbj),nbm(p.nbm),per(p.per),md(p.md) {
    }
    /// Contructor
    PFSNode(PFSNode *p) : Bob::CDCIntMinNode(*p),nbj(p->nbj),nbm(p->nbm),per(p->per),md(p->md) { 
    }
    /// Contructor
    PFSNode(const PFSInstance *pi);
    /// Destructor
    virtual ~PFSNode() {
    }
    /// Copy method
    virtual void copy(PFSNode *n) {
      Bob::CDCIntMinNode::copy(n);
      nbj = n->nbj;
      nbm = n->nbm;
      per.copy(n->per);
      md = n->md;
    }
    /// Fixe the job i
    void Fixe(int i,const PFSInstance &pfi);
    /// get the permutation
    Bob::Permutation &perm() { return per; }
    /// get the MachineDate 
    MachineDate &macdate() { return md; }
    /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs)  const {
       DBGAFF_ENV("PFSNode::Pack","Pack a node");
	    Bob::CDCIntMinNode::Pack(bs);
       bs.Pack(&nbj,1);
       bs.Pack(&nbm,1);
       per.Pack(bs);
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("PFSNode::UnPack","UnPack a node");
	    Bob::CDCIntMinNode::UnPack(bs);
       bs.UnPack(&nbj,1);
       bs.UnPack(&nbm,1);
       per.UnPack(bs);
       for (int i=0;i<nbm;i++) bs.UnPack(&md[i],1);
    }

    /// Display a PFS Node
    virtual ostream &Prt(ostream &os) const {
        if ( isPb() ) {
           os << "Cost :"<<getCost()<<"\n";
           per.Prt(os);
        } else {
           os << "Solution Cost :"<<getCost()<<"\n";
        }
        return os;
    }
};

/** the genchild class for the permutation flowshop
 */
class PFSGenChild : public Bob::CDCGenChild<PFSTrait>  {
        public:
          PFSGenChild(const PFSInstance *_inst, PFSTrait::Algo *_sch) : 
                 Bob::CDCGenChild<PFSTrait>(_inst,_sch) {}
          virtual ~PFSGenChild() {}
          virtual bool operator()(PFSNode *n);
};



/// The Instance class for the PFS.
class PFSInstance : public Bob::CDCInstance<PFSTrait> {
public:
/// The file name of the problem instance
std::string file;
int nbj,nbm;
unsigned long** d;
unsigned long *jc;
//std::vector< std::vector < unsigned long > > m_ja;
//std::vector< std::vector < unsigned long > > m_jd;

   /// Constructor
   PFSInstance() : Bob::CDCInstance<PFSTrait>("pfs"),file("") {
   }
   /// Constructor
   PFSInstance(std::string _file) : Bob::CDCInstance<PFSTrait>("pfs"),file(_file){
   }
   /// Destructor
   virtual ~PFSInstance() {}
   /// Initialization of the instance
   void Init() { read(); }
   /// the method that initialize and insert the root nodes.
   void InitAlgo(PFSTrait::Algo *al) const {
       PFSNode *qn;
       qn = new PFSNode(nbj,nbm);
       //al->log_space(qn);
       al->GPQIns(qn);
   }
   /// the method that initialize and insert the initial upper bound.
   PFSNode *getSol() {
       PFSNode *sol = new PFSNode(this);
       return sol;
   }
   /// reads the data from files.
   void read();
   unsigned long get(int i,int j) const { return d[j][i]; }
   unsigned long getjc(int i) const { return jc[i]; }
   /// Pack method to serialize the BobNode
   virtual void Pack(Bob::Serialize &bs) const  {
       DBGAFF_ENV("PFSInstance::Pack","Pack an instance");
	    Bob::CDCInstance<PFSTrait>::Pack(bs);
       DBGAFF_ENV("PFSInstance::Pack","------- Finish");
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("PFSInstance::UnPack","UnPack an instance");
	    Bob::CDCInstance<PFSTrait>::UnPack(bs);
       DBGAFF_ENV("PFSInstance::UnPack","------- Finish");
    }
};

