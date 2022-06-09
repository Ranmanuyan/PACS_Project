/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    CARO Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                           Authors:  B. Le Cun 
 *                    (C) 2009 All Rights Reserved
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
 * the PNN Team), nor the Authors make any representations about the 
 * suitability of this software for any purpose.  This software is 
 * provided ``as is'' without express or implied warranty.
 *
 */

/* 
 *  File   : sat.c
 *  Author : B. Le Cun and S. Baarir.
 *  Date   : 12-2009.
 *  Comment: Source file for SAT
 */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <bobpp/bobpp>
#ifdef Threaded
#include<bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif

//#include<SolverTypes.h>


/** \mainpage Sat/Bob++
    @author Bertrand Le cun  Soheib Baarir
    @version 1.0

    This sat solver tries to solve a sat problem in cnf form.
     ( v_1 | v_2 ) & ( v_1 | !v_3 | !v_2 ) ...
*/

/// Forward decalration of the info for the SAT problem
class SATProblem;
/// Forward decalration of the info for the SAT problem
class SATInstance;
/// Forward decalration of a Node for the SAT problem
class SATNode;
/// Forward decalration of a the GenChild for the SAT problem
class SATGenChild;

/// the Trait for the SAT problem
class SATTrait {
  public:    
    typedef SATNode Node;
    typedef SATInstance Instance ;
    typedef SATGenChild GenChild;
    typedef Bob::DepthPri<SATNode> PriComp;
#ifdef COUNT
    typedef Bob::SDCGoalCount<SATTrait> Goal;
#else
    typedef Bob::SDCGoalOne<SATTrait> Goal;
#endif
    typedef Bob::SDCAlgo<SATTrait> Algo; 
    typedef Bob::SDCStat Stat;
};

typedef Bob::bit_vector TabLit;

struct Clause {
    Bob::pvector<int> li; /*!< index of literals */
    TabLit ls;            /*!< sens of literals */

    Clause():li(),ls() { }
    Clause(int nbv):li(nbv),ls(nbv) { }
    Clause(int nbv,int *l):li(nbv),ls(nbv) { 
      li.resize(nbv);ls.resize(nbv);
      for (int i=0;i<nbv;i++ ) { 
          li[i]=(l[i]<0?-l[i]:l[i]);
          ls.set(i,l[i]<0?0:1);
      }
    }
    Clause(const Clause &cl):li(cl.li),ls(cl.ls) { }
    int get_lit(int l) {
       for (unsigned int i=0;i<li.size();i++) {
          if ( l==li[i] ) return i;
       }
       return -1;
    }
    void init(int nbv,int *l) {
      li.resize(nbv);
      ls.resize(nbv);
      for (int i=0;i<nbv;i++ ) { 
          li[i]=(l[i]<0?-l[i]:l[i]);
          ls.set(i,l[i]<0?0:1);
      }
    }
    bool has_lit(int l) {
       return get_lit(l)!=-1;
    }
    int eval(const TabLit &is, const TabLit & vv) const {
       int v=0;
       for (unsigned int i = 0 ; i<li.size();i++ ) {
           if ( !is.test(li[i]) ) return -1;
           v |= (is.get(li[i]) & (vv.get(li[i]) ^ ls.get(i) )) ;
           //std::cout << li[i] <<std::endl;
           //std::cout << is.get(li[i]) <<std::endl;
           //std::cout << vv.get(li[i]) <<std::endl;
           //std::cout << ls.get(i) <<std::endl;
       }
       //std::cout << "V:"<<v<<std::endl;
       return v;
    }
};

typedef std::vector<Clause>  VecClause;


/// the SAT Node that inherits from Simple Divide and Conquer Node, Bob::SDCNode.
class SATNode : public Bob::SDCNode {
  protected:
    Bob::bit_vector vf,vv;

  public :
  /// Constructor
  SATNode() : Bob::SDCNode(),vf(),vv() {}
  /// Constructor
  SATNode(int nbv) : Bob::SDCNode(),vf(nbv),vv(nbv) {
  }
  /// Constructor
  SATNode(int nbv,const VecClause &cl) : Bob::SDCNode(),vf(nbv),vv(nbv) {
    //std::cout << vf<< std::endl;
    //std::cout << vv<< std::endl;
  }
  /// Constructor
  SATNode(SATNode &qn) : Bob::SDCNode(qn),vf(qn.vf),vv(qn.vv) { }
  /// Constructor
  SATNode(const SATNode &qn) : Bob::SDCNode(qn),vf(qn.vf),vv(qn.vv) { 
  }
  /// Constructor
  SATNode(SATNode *qn) : Bob::SDCNode(qn),vf(qn->vf),vv(qn->vv) { 
    //std::cout << vf<< std::endl;
    //std::cout << vv<< std::endl;
  }
  /// getMax
  int get_max_lit(const Bob::pvector<int> &nbcl) {
     int ind=-1;
     for (unsigned int i=1;i<nbcl.size();i++ ) {
         if ( !vf.test(i) && ( ind==-1 || nbcl[ind]<nbcl[i] )) ind=i;
     }
     return ind;
  }
  /// set variable i
  void set(int i,int v) {
     //std::cout << "Set "<<i<<":"<<v<<std::endl;
     vf.set(i);
     vv.set(i,v);
    //std::cout << vf<< std::endl;
    //std::cout << vv<< std::endl;
  }
  int eval(const VecClause &clauses) {
     int r,v=1;
     int rt[3]={0,0,0};
     //std::cout << "Eval :";
     for (unsigned int i=0;i<clauses.size();i++ ) {
         r = clauses[i].eval(vf,vv);
         rt[r+1]++;
         if ( r==0 ) return 0;
         if ( r==-1 || v==-1 ) v=-1;
     }
     if ( v==1 ) setSol();
     //std::cout << "N"<<rt[0]<<" F"<<rt[1]<<" V"<<rt[2]<<"Final:"<< v<<std::endl;
     return v;
  }
  /// Display a SAT Node
  virtual ostream &Prt(ostream &os) const {
     os << vv << std::endl;
     return os;
  }
  /// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("SATNode::Pack()", "Called");
        Bob::SDCNode::Pack(bs);
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("SATNode::UnPack()", "Called");
        Bob::SDCNode::UnPack(bs);
  }

};

/// the SAT GenChild that inherits from the basic GenChild intanciates with the SATTrait class.
class SATGenChild : public Bob::SDCGenChild<SATTrait>  {
   public:
     /// Constructor
     SATGenChild(const SATTrait::Instance *_inst, SATTrait::Algo *_al) : 
                 Bob::SDCGenChild<SATTrait>(_inst,_al) {}

     /// The function class
     virtual bool operator()(SATNode *n);
};


/// the Instance class for the SAT.
class SATInstance : public Bob::SDCInstance<SATTrait> {
public:
/// 
int nbv,nbc;
VecClause clauses;
Bob::pvector<int> nbcl;

   /// Constructor
   SATInstance() : Bob::SDCInstance<SATTrait>("SAT"),nbv(0),nbc(0),clauses(0),nbcl(0) {
   }
   /// Constructor
   SATInstance(const std::string &f) : Bob::SDCInstance<SATTrait>("SAT"),nbv(0),nbc(0),clauses(0),nbcl(0) {
    ifstream fs(f.c_str());
    int lit[100];
    std::string s;
    int v,j;
    if ( ! fs.is_open() ) {
      std::cerr << "Error file "<<f<<" not found !\n";
      exit(1);
    }
    fs >> s; fs >> s;
    fs >> nbv; fs >> nbc;
    nbv++;
    std::cout << "NbV:"<<nbv<<" nbc:"<<nbc<<std::endl;
    clauses.resize(nbc);
    nbcl.resize(nbv);
    for (int i=0;i<nbc;i++ ) {
      j=0;
      do {
        fs >> v;
        lit[j++]=v;
        if ( v<0 ) nbcl[-v]++; else nbcl[v]++;
      } while (v!=0);
      clauses[i].init(j-1,lit);
      //std::cout << "Clauses "<<i<<std::endl;
    }
   }
   /// Initialization of the instance
   void Init() {}
   ///Intialization of the Algorithm
   void InitAlgo(SATTrait::Algo *a) const {
       SATTrait::Node *n = new SATTrait::Node(nbv,clauses);

       n->Prt(std::cout);
       a->GPQIns(n);
   }
   /// Pack method to serialize the Instance
   virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("Instance::Pack()", "Called");
        Bob::SDCInstance<SATTrait>::Pack(bs);
        bs.Pack(&nbv,1);
        bs.Pack(&nbc,1);
   }
   /// Unpack method to deserialize the Instance
   virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("Instance::UnPack()", "Called");
        Bob::SDCInstance<SATTrait>::UnPack(bs);
        bs.UnPack(&nbv,1);
        bs.UnPack(&nbc,1);
   }

};

bool SATGenChild::operator()(SATNode *n) {
        bool fl = false;
        int curr,r;
        curr=n->dist()+1;
        //n->Prt(std::cout);
        curr = n->get_max_lit(inst->nbcl);
        //std::cout <<"The Curr "<< curr << ":"<<n->dist()<<","<<inst->nbcl[curr]<<"-------------\n";
        for (int j=0; j<2;j++ ) {
           SATNode *t;
           t = new SATNode(n);
           t->dist()++;
           t->set(curr,j);
           r = t->eval(inst->clauses);
           if ( r!=0 ) 
              fl +=algo->Search(t);
           else 
              delete t;
        }
        return fl;
}

std::string filename;

int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--sat"),Bob::Property("-f","file in dimacs format","Text/petit.txt",&filename));
#ifdef Threaded
        Bob::ThrSDCAlgoEnvProg<SATTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#elif defined(Atha)
        Bob::AthaSDCAlgoEnvProg<SATTrait> env;
        Bob::AthaEnvProg::init(n, v);
        Bob::core::Config(n, v);
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::init(n, v);
        Bob::mpx::SDCAlgoEnvProg<SATTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
        Bob::core::Config(n, v);
        Bob::mpx::EnvProg::start();
#else
        Bob::SeqSDCAlgoEnvProg<SATTrait> env;
        Bob::core::Config(n,v);
#endif
        SATInstance *Instance=new SATInstance(Bob::core::opt().SVal("--sat","-f"));

        // Solve the instance i.e. all the tree
        std::cout<<"-------- Start to solve the entire tree"<<std::endl;
        env(Instance);
#ifdef Atha
        Bob::AthaEnvProg::end();
#elif defined(Threaded)
        Bob::ThrEnvProg::end();
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::end();
#endif
        Bob::core::End();
        delete Instance;
}

