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
 *  File   : queen.c
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

/// the QN Node that inherits from Simple Divide and Conque Node BobSDCNode.
class QNNode : public Bob::SDCNode {
  protected:
  /// Where is the queen on the line i.
  vector<int> pos;
  /// chessboard to verify the incompatibility.
  vector<int> c;
  public :
  /// count the number of free position by column:w
  vector<int> fc;
  /// count the number of free position by column:w
  vector<int> fl;
  /// Constructor
  QNNode() : Bob::SDCNode(){}
  /// Constructor
  QNNode(int size) : Bob::SDCNode(),pos(size),c(size*size),fc(size),fl(size) {
     for (int i=0; i< Size();i++) { pos[i] = -1; }
     for (int i=0; i< Size()*Size();i++) { c[i] = 0; }
     for (int i=0; i< Size();i++) { fl[i] = size; fc[i]=size;}
  }
  /// Constructor
  QNNode(QNNode &qn) : Bob::SDCNode(qn),pos(qn.pos), c(qn.c),fc(qn.fc),fl(qn.fl)  { }
  /// Constructor
  QNNode(QNNode *qn) : Bob::SDCNode(qn),pos(qn->pos), c(qn->c),fc(qn->fc),fl(qn->fl)  { }
  /// Destructor
  virtual ~QNNode() { }
  /// returns if the queen i has a position or not.
  bool busy(int i) { return pos[i]!=-1; }
  /// returns the position of the queen of the line i
  int &Pos(int i) { return pos[i]; }
  //int dist() const { return Bob::base_Node::dist(); }
  /// returns the number of queens that cover the position (i,j)
  int &Echi(int i,int j) { return c[i*Size()+j]; }
  /// returns the number of queens that cover the position (i,j)
  bool Forb(int i,int j) {  
      if ( Echi(i,j)==0) { fl[i]--; fc[j]--; }
      Echi(i,j)++; 
      if ( fl[i]<=0 || fc[j]<=0 ) 
               return false;
      return true;
  }
  /// returns the number of queens that cover the position (i,j)
  int Echi(int i,int j) const { return c[i*Size()+j]; }
  /// Szie of the chessboard
  int Size() const { return pos.size(); }
  /// Display a QN Node
  virtual ostream &Prt(ostream &os) const {
     os << "dist:"<<dist()<<" Size:"<<Size()<<" Sol:"<<isSol()<<"\n";
     os << "Pos ";
     for (int i=0; i< Size();i++) { os << pos[i] <<","; }
     if ( isPb() ) { 
        os<<"\n C :\n   ";
        for (int i=0; i< Size();i++) {
             os<<fc[i]<<",";
        }
        os<<"\n";
        for (int i=0; i< Size();i++) {
          os <<fl[i]<<":";
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
};

/// the QN GenChild that inherits from the basic GenChild intanciates with the QNTrait class.
class QNGenChild : public Bob::SDCGenChild<QNTrait>  {
   public:
     /// Constructor
     QNGenChild(const QNTrait::Instance *_inst, QNTrait::Algo *_al) : 
                 Bob::SDCGenChild<QNTrait>(_inst,_al) {}

     /// The function class
     virtual bool operator()(QNNode *n) {
        bool fl=false;
        int curr,j;
#ifdef NEXTL
        curr=n->dist();
#else 
        curr=0;
        for (j=0; j<n->Size();j++ ) {
            if ( n->Pos(j)==-1 ) {
               if ( n->Pos(curr)!=-1 ) curr=j;
               else if ( n->fl[curr]>n->fl[j] ) curr=j;
            }
        }
#endif
        std::cout <<"The Curr "<< curr << ":"<<n->dist()<<"-------------\n";
        //n->Prt(std::cout);
        for (j=0; j<n->Size();j++ ) {
           if ( n->Echi(curr,j)==0 ) {
              QNNode *t;
              t = new QNNode(n);
              t->dist() = n->dist()+1;
              if ( !Place(t,curr,j,n)) { delete t;continue; }
              if ( t->dist()==t->Size()) t->setSol();
              t->Prt(std::cout);
              fl+=algo->Search(t);
           }
        }
        return fl;
              
     }
     /// accessory method
     bool Place(QNNode *n, int i,int j,QNNode *p) {
        int l;
        n->Pos(i)=j;
        n->fl[i]=n->Size()*3;
        n->fc[j]=n->Size()*3;
        std::cout << "Place "<<i<<" en "<<j<<"\n";
        for (l=0;l<n->Size();l++ ) { 
           if ( !n->Forb(l,j) || !n->Forb(i,l) ) return false;
           if ( l+(i-j)>=0 && l+(i-j)<n->Size() ) {
               if ( !n->Forb(l+(i-j),l) ) return false;
           }
           if ( (i+j)-l>=0 && (i+j)-l<n->Size()) {
               if ( !n->Forb(i+j-l,l) ) return false;
           }
        }
        if ( p->dist()==0 ) {
           p->Forb(i,j);
           p->Forb(i,n->Size()-j-1);
           p->Forb(j,i);
           p->Forb(j,n->Size()-i-1);
           p->Forb(n->Size()-i-1,j);
           p->Forb(n->Size()-i-1,n->Size()-j-1);
           p->Forb(n->Size()-j-1,n->Size()-i-1);
           p->Forb(n->Size()-j-1,i);
        }
        return true;
     }

};

class QNProblem : public Bob::Problem<QNTrait> {
        public:
                QNProblem() : Bob::Problem<QNTrait>() {}
                virtual ~QNProblem() {}
};


/// the BobInfoAlgo class for the QN.
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
   ///Intialization of the algorithm
   void InitAlgo(QNTrait::Algo *a) const {
       QNTrait::Node *n = new QNTrait::Node(Size);

       n->Prt(std::cout);
       a->GPQIns(n);
   }
};

int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--qn"),Bob::Property("-s","size of the N-Queen problem",8));
#ifdef Threaded
        Bob::ThrSDCAlgoEnvProg<QNTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#else 
        Bob::SeqSDCAlgoEnvProg<QNTrait> env;
        Bob::core::Config(n,v);
#endif
        QNProblem mp();
        QNInstance *Instance=new QNInstance(Bob::core::opt().NVal("--qn","-s"));

        // Solve the instance i.e. all the tree
        std::cout<<"-------- Start to solve the entire tree"<<std::endl;
        env(Instance);
#ifdef Threaded
        Bob::ThrEnvProg::end();
#endif
        Bob::core::End();
        delete Instance;

}

