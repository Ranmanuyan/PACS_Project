/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    AOC Team of PRiSM laboratory
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
 *  File   : queen.cpp
 *  Author : B. Le Cun.
 *  Date   : 10-2006.
 *  Comment: Source file for N-Queen
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

/** \mainpage N-Queen/Bob++
    @author Bertrand Le cun 
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the classical N-Queen problem
using the divide and conquer technique

*/

/// Forward decalration of the info for the N-Queen problem
class QNProblem;
/// Forward decalration of the info for the N-Queen problem
class QNInstance;
/// Forward decalration of a Node for the N-Queen problem
class QNNode;
/// Forward decalration of a the GenChild for the N-Queen problem
class QNGenChild;

/// the Trait for the N-Queen problem
class QNTrait {
  public:    
    typedef QNNode Node;
    typedef QNInstance Instance ;
    typedef QNGenChild GenChild;
    typedef Bob::DepthPri<QNNode> PriComp;
#ifdef COUNT
    typedef Bob::SDCGoalCount<QNTrait> Goal;
#else
    typedef Bob::SDCGoalOne<QNTrait> Goal;
#endif
    typedef Bob::SDCAlgo<QNTrait> Algo; 
    typedef Bob::SDCStat Stat;
};

/// the QN Node that inherits from Simple Divide and Conquer Node, Bob::SDCNode.
class QNNode : public Bob::SDCNode {
  protected:
  /// Where is the queen on the line i.
  vector<int> pos;
  /// chessboard to verify the incompatibility.
  vector<int> c;
  public :
  /// Constructor
  QNNode() : Bob::SDCNode(), pos(), c() {}
  /// Constructor
  QNNode(int size) : Bob::SDCNode(),pos(size),c(size*size) {
     for (int i=0; i< Size();i++) { pos[i] = -1; }
     for (int i=0; i< Size()*Size();i++) { c[i] = 0; }
  }
  /// Constructor
  QNNode(QNNode &qn) : Bob::SDCNode(qn),pos(qn.pos), c(qn.c) { }
  /// Constructor
  QNNode(const QNNode &qn) : Bob::SDCNode(qn),pos(qn.pos), c(qn.c) { }
  /// Constructor
  QNNode(QNNode *qn) : Bob::SDCNode(qn),pos(qn->pos), c(qn->c) { }
  /// Copy method
  virtual void copy(QNNode *qn) {
     Bob::SDCNode::copy(qn);
     //pos.resize(Size());
     //for (int i=0; i< Size();i++) { pos[i] = qn->pos[i]; }
     //c.resize(Size()*Size());
     //for (int i=0; i< Size()*Size();i++) { c[i] = qn->c[i]; }
     pos=qn->pos; 
     c = qn->c;
  }
  /// returns if the queen i has a position or not.
  bool busy(int i) { return pos[i]!=-1; }
  /// returns the position of the queen of the line i
  int &Pos(int i) { return pos[i]; }
  //int Dist() const { return Bob::base_Node::Dist(); }
  /// returns the number of queens that cover the position (i,j)
  int &Echi(int i,int j) { return c[i*Size()+j]; }
  /// returns the number of queens that cover the position (i,j)
  int Echi(int i,int j) const { return c[i*Size()+j]; }
  /// Szie of the chessboard
  int Size() const { return pos.size(); }
  /// Display a QN Node
  virtual ostream &Prt(ostream &os) const {
     os << "Dist:"<<dist()<<" Size:"<<Size()<<" Sol:"<<isSol()<<"\n";
     os << "Pos ";
     for (int i=0; i< Size();i++) { os << pos[i] <<","; }
     if ( isPb() ) { 
        os<<"\n C :\n";
        for (int i=0; i< Size();i++) {
          for (int j=0;j<Size();j++) { 
              os <<Echi(i,j)<<",";
          }
          os<<"\n";
        }
     } else {
        os<<"\n";
        for (int i=0; i< Size();i++) {
          for (int j=0;j<Size();j++) { 
              os <<(pos[i]==j ? "R " : ". ");
          }
          os<<"\n";
        }
     }
     return os;
  }
  /// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("QNNode::Pack()", "Called");
        int s,d;
        Bob::SDCNode::Pack(bs);
        s=Size();
        bs.Pack(&s,1);
        for (int i=0; i< Size();i++) { d=pos[i];bs.Pack(&d,1); }
        for (int i=0; i< Size()*Size();i++) { d=c[i];bs.Pack(&d,1); }
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("QNNode::UnPack()", "Called");
        int s,d;
        Bob::SDCNode::UnPack(bs);
        bs.UnPack(&s,1);
        pos.resize(s);
        c.resize(s*s);
        for (int i=0; i< s;i++) { bs.UnPack(&d,1);pos[i]=d; }
        for (int i=0; i< s*s;i++) { bs.UnPack(&d,1);c[i]=d; }
  }

};

/// the QN GenChild that inherits from the basic GenChild intanciates with the QNTrait class.
class QNGenChild : public Bob::SDCGenChild<QNTrait>  {
   public:
     /// Constructor
     QNGenChild(const QNTrait::Instance *_inst, QNTrait::Algo *_al) : 
                 Bob::SDCGenChild<QNTrait>(_inst,_al) {}

     /// The function class
     virtual bool operator()(QNNode *n) {
        bool fl =false;
        int curr;
        curr=n->dist();
        //std::cout <<"The Curr "<< curr << ":"<<n->dist()<<"-------------\n";
        //n->Prt(std::cout);
        for (int j=0; j<n->Size();j++ ) {
           if ( n->Echi(curr,j)==0 ) {
             QNNode *t;
             int ir = n->dist()+1-Bob::core::get_depth_rec();
             if ( ir>=0 ) {
                //std::cout<<pthread_self()<<"   Rec node "<<p->dist()+1<<":"<<ir<<"\n";
                t = get_rec_node(ir);
                if ( t==0 ) {
                  t = new QNNode(n); 
                  declare_as_rec(t,ir);
                  //std::cout<<pthread_self()<<"  Declare node:"<<ir<<"\n";
               }  
               t->copy(n);
               t->addRef();
             } else {
               t = new QNNode(n); 
             }
             t->dist() = n->dist()+1;
             Place(t,curr,j);
             if ( t->dist()==t->Size()) t->setSol();
             //t.Prt(BobCout);
             fl +=Search(t);
           }
        }
        return fl; 
     }
     /// accessory method
     void Place(QNNode *n, int i,int j) {
        int l;
        n->Pos(i)=j;
        //BobCout << "Place "<<i<<" en "<<j<<"\n";
        for (l=0;l<n->Size();l++ ) { 
           n->Echi(l,j)++; n->Echi(i,l)++;
           if ( l+(i-j)>=0 && l+(i-j)<n->Size() ) 
              n->Echi(l+(i-j),l)++; 
           if ( (i+j)-l>=0 && (i+j)-l<n->Size())
              n->Echi(i+j-l,l)++;
        }
     }

};

class QNProblem : public Bob::Problem<QNTrait> {
        public:
                QNProblem() : Bob::Problem<QNTrait>() {}
                virtual ~QNProblem() {}
};


/// the Instance class for the QN.
class QNInstance : public Bob::SDCInstance<QNTrait> {
public:
/// the Size of the problem.
int Size;

   /// Constructor
   QNInstance() : Bob::SDCInstance<QNTrait>("QU"),Size(0) {
   }
   /// Constructor
   QNInstance(int _s) : Bob::SDCInstance<QNTrait>("QU"),Size(_s) {
   }
   /// Initialization of the instance
   void Init() {}
   ///Intialization of the Algorithm
   void InitAlgo(QNTrait::Algo *a) const {
       QNTrait::Node *n = new QNTrait::Node(Size);

       n->Prt(std::cout);
       a->GPQIns(n);
   }
   /// Pack method to serialize the Instance
   virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("KSInstance::Pack()", "Called");
        Bob::SDCInstance<QNTrait>::Pack(bs);
        bs.Pack(&Size,1);
   }
   /// Unpack method to deserialize the Instance
   virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("KSInstance::UnPack()", "Called");
        Bob::SDCInstance<QNTrait>::UnPack(bs);
        bs.UnPack(&Size,1);
   }

};

int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--qn"),Bob::Property("-s","size of the N-Queen problem",30));
#ifdef Threaded
        Bob::ThrSDCAlgoEnvProg<QNTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#elif defined(Atha)
        Bob::AthaSDCAlgoEnvProg<QNTrait> env;
        Bob::AthaEnvProg::init(n, v);
        Bob::core::Config(n, v);
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::init(n, v);
        Bob::mpx::SDCAlgoEnvProg<QNTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
        Bob::core::Config(n, v);
        Bob::mpx::EnvProg::start();
#else
        Bob::SeqSDCAlgoEnvProg<QNTrait> env;
        Bob::core::Config(n,v);
#endif
        QNProblem mp();
        QNInstance *Instance=new QNInstance(Bob::core::opt().NVal("--qn","-s"));

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

