/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    Opale Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                           Authors:  B. Le Cun 
 *                    (C) 2006 All Rights Reserved
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
 *  File   : ks.c
 *  Author : B. Le Cun.
 *  Date   : 11-2006.
 *  Comment: Source file for a simple KnapSac
 */

#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <algorithm>
#include <bobpp/bobpp.h>
#ifdef Threaded
#include<bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif


/** \mainpage KS/Bob++
    @author Bertrand Le cun 
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the classical knapsac problem
using the divide and conquer technique 

*/

class KSInstance;
class KSNode;
class KSGenChild;

class KSTrait {
  public:    
    typedef KSNode Node;
    typedef KSInstance Instance;
    typedef KSGenChild GenChild;
    typedef Bob::DepthPri<KSNode> PriComp;
#ifdef COUNT
    typedef Bob::CDCGoalCount<KSTrait> Goal;
#else
    typedef Bob::CDCGoalBest<KSTrait> Goal;
#endif
    typedef Bob::CDCAlgo<KSTrait> Algo; 
    typedef Bob::CDCStat Stat;
};

/// the KS Node
class KSNode : public Bob::CDCIntMaxNode {
  protected:
  vector<bool> s;
  int v;
  public :
  /// Constrcutor
  KSNode() : Bob::CDCIntMaxNode(),s(),v(0){}
  /// Constrcutor
  KSNode(int size) : Bob::CDCIntMaxNode(0),s(size),v(0) {
     for (int i=0; i< Size();i++) { s[i] = false; }
  }
  /// Constrcutor
  KSNode(KSNode &qn) : Bob::CDCIntMaxNode(qn),s(qn.s),v(qn.v) { }
  /// Constrcutor
  KSNode(KSNode const &qn) : Bob::CDCIntMaxNode(qn),s(qn.s),v(qn.v) { }
  /// Constrcutor
  KSNode(KSNode *qn) : Bob::CDCIntMaxNode(qn),s(qn->s),v(qn->v) { }
  /// set the piece i
  void Piece(int i) { s[i]=true; }
  /// the number of pieces.
  int Size() const { return s.size(); }
  /// gives the volume occupied by the choosen piece.
  int Vol() const { return v; }
  /// gives the volume occupied by the choosen piece.
  int &Vol() { return v; }
  /// Display a QAP Node
  virtual std::ostream &Prt(std::ostream &os) const {
     os << "Vol:"<<Vol()<<" Cost:"<<getCost()<<" ";
     os << "s ";
     for (int i=0; i< Size();i++) { os << s[i] <<","; }
     os << "\n";
     return os;
  }
  /// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("KSNode::Pack()", "Called");
        int i,sz;
        bool b;
        Bob::CDCIntMaxNode::Pack(bs);
        sz= Size();
        bs.Pack(&sz,1);
        for (i = 0; i < Size(); i++) {
            b = s[i]; 
            bs.Pack(&b,1);
        }
        bs.Pack(&v, 1);
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("KSNode::UnPack()", "Called");
        int i,sz;
        bool b;
        Bob::CDCIntMaxNode::UnPack(bs);
        bs.UnPack(&sz,1);
        s.resize(sz);
        for (i = 0; i < sz; i++) {
            bs.UnPack(&b,1);
            s[i]=b; 
        }
        bs.UnPack(&v, 1);
  }
};

class KSGenChild : public Bob::CDCGenChild<KSTrait>  {
        public:
          /// Constructor 
          KSGenChild(const KSInstance *_inst, KSTrait::Algo *_sch) : 
                 Bob::CDCGenChild<KSTrait>(_inst,_sch) {}

          /// the operator() that generate the child nodes.
          virtual bool operator()(KSNode *n); 
          /// create a Child from a Node adding a piece in the bag.
          bool Child(KSNode *n, int p);
};

struct PieceCmp {
   bool operator()(const pair<int,int> &e2,const pair<int,int> &e1) { 
        return ((float)e2.second/(float)e2.first)> ((float)e1.second/(float)e1.first); }
};
PieceCmp pcmp;


/// the Bob::Instance class for the KS.
class KSInstance : public Bob::CDCInstance<KSTrait> {
public:
/// the Size of the problem.
int Sac;
vector<pair<int,int> > piece;

   /// Constructor
   KSInstance() : Bob::CDCInstance<KSTrait>("KS") {
   }
   /// Constructor
   KSInstance(const KSInstance &ksi) : Bob::CDCInstance<KSTrait>("KS"),Sac(ksi.Sac),piece(ksi.piece) {
      std::cout << "Copy const KSInstance :"<<Sac<<"\n";
   }
   /// Reads the file information.
   void ReadFile(std::string s) {
      int nbp;
      std::ifstream fsc(s.c_str());
      fsc >> Sac;
      fsc >> nbp;
      piece.resize(nbp);
      for (int i=0 ; i<nbp ; i++ ) {
          fsc >> piece[i].first >> piece[i].second;
      }
      sort(piece.begin(),piece.end(),pcmp);
      std::cout<< "Taille de sac :"<< Sac<<"\n";
      std::cout<< "Nombre de piece :"<< piece.size()<<"\n";
      for (int i=0 ; i<nbp ; i++ ) {
          std::cout<<  "("<<piece[i].first<<","<<piece[i].second<<")\n";
      }
   }
   /// the number of piece
   int Size() const { return piece.size(); }
   /// Initialization of the Instance
   void Init() {
      ReadFile(Bob::core::opt().SVal("--ks","-f"));
   }
   ///Intialization of the Algorithm
   void InitAlgo(KSTrait::Algo *a) const {
       KSTrait::Node *n = new KSTrait::Node(Size());

       n->Prt(std::cout);
       a->GPQIns(n);
   }
   /// Get the initial best known solution
   KSNode *getSol() { 
       std::cout<<"Instance::getSol "<<0 <<"\n"; 
       return new KSNode(Size()); 
   }
    /// Pack method to serialize the Instance
    virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("KSInstance::Pack()", "Called");
        Bob::CDCInstance<KSTrait>::Pack(bs);
        bs.Pack(&Sac,1);
        int nbp = Size();
        bs.Pack(&nbp,1);
        for (int i=0 ; i<nbp ; i++ ) {
          bs.Pack(&(piece[i].first),1);
          bs.Pack(&(piece[i].second),1);
       }
    }
    /// Unpack method to deserialize the Instance
    virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("KSInstance::UnPack()", "Called");
        Bob::CDCInstance<KSTrait>::UnPack(bs);
        bs.UnPack(&Sac,1);
        int nbp;
        bs.UnPack(&nbp,1);
        for (int i=0 ; i<nbp ; i++ ) {
          bs.UnPack(&(piece[i].first),1);
          bs.UnPack(&(piece[i].second),1);
       }
    }



};

bool KSGenChild::operator()(KSNode *n) {
   bool fl=false;
   int curr=n->dist();
   KSNode *t,*ts,*t0;
   if ( curr==inst->Size() ) return false;
   //std::cout <<"----- in GenChild "<<curr<<" :";
   //n->Prt(std::cout);
   t = new KSNode(n);
   algo->StCost(t,n);
   if ( Child(t,curr)) { 
      t->dist()++;
      t->setSol();
      if (t->Vol()< inst->Sac ) {
         ts = new KSNode(t);
         ts->setPb();
         algo->StCost(ts,n);
         //ts->Prt(std::cout);
         fl +=algo->Search(ts);
      }
      fl +=algo->Search(t);
   }
   else {
      algo->StNoFea(t);
      delete t;
   }
   t0 = new KSNode(n);
   t0->dist()++;
   t0->setPb();
   algo->StCost(t0,n);
   //t0->Prt(std::cout);
   fl +=algo->Search(t0);
   return fl;
}  

bool KSGenChild::Child(KSNode *n, int p) {
   n->Vol()+= inst->piece[p].first;
   n->Piece(p);
   n->setCost(n->getCost()+inst->piece[p].second);
   return n->Vol()<= inst->Sac;
}


int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--ks"),Bob::Property("-f","the file problem","n1.txt"));
#ifdef Threaded
        Bob::ThrCDCAlgoEnvProg<KSTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#elif defined(Atha)
        Bob::AthaCDCAlgoEnvProg<KSTrait> env;
        Bob::AthaEnvProg::init(n, v);
        Bob::core::Config(n, v);
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::init(n, v);
        Bob::mpx::CDCAlgoEnvProg<KSTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
        Bob::core::Config(n, v);
        Bob::mpx::EnvProg::start();
#else
        Bob::SeqCDCAlgoEnvProg<KSTrait> env;
        Bob::core::Config(n,v);
#endif
        KSInstance *Instance=new KSInstance();

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
        delete Instance;
        Bob::core::End();
}

