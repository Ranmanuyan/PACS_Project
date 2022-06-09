/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    CaRO Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                           Authors:  B. Le Cun 
 *                    (C) 2011 All Rights Reserved
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
 * the CaRO Team), nor the Authors make any representations about the 
 * suitability of this software for any purpose.  This software is 
 * provided ``as is'' without express or implied warranty.
 *
 */

/* 
 *  File   : trap.cpp
 *  Author : B. Le Cun.
 *  Date   : 11-2011.
 *  Comment: Source file for the trap problem
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

#define max(a,b) ((a)<(b)?(b):(a))
#define min(a,b) ((a)>(b)?(b):(a))

/** \mainpage Trap/Bob++
    @author Bertrand Le cun 
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the trap problem
using the divide and conquer technique

The trap problem consist in minimizing the number of traps
to put on a garden (represented by a grid) to be sure to cover
all mole area. 
In this version of the problem a mole area is 3 cells of the grid
(3 horizontal or vertical).

The trap problem is the dual of the mole problem.

For a 5x5 grid garden, an optimal solution is
\verbatim
- - T - - 
- - T - - 
T T - T T 
- - T - - 
- - T - - 
\endverbatim

Another one has 3 symetrical equivalent solutions
\verbatim
B - - B - 
- - B - - 
- B - - B 
B - - B - 
- - B - - 
\endverbatim

This implementation uses the simple Costed Divide and Conquer method.
*/


const int grid_size=5;

/// Forward decalration of the info for the mole problem
class TrapProblem;
/// Forward decalration of the info for the mole problem
class TrapInstance;
/// Forward decalration of a Node for the mole problem
class TrapNode;
/// Forward decalration of a the GenChild for the mole problem
class TrapGenChild;

/// the Trait for the mole problem
class TrapTrait {
  public:    
    typedef TrapNode Node;
    typedef TrapInstance Instance ;
    typedef TrapGenChild GenChild;
    typedef Bob::DepthEPri<TrapNode> PriComp;
#ifdef COUNT
    typedef Bob::CDCGoalCount<TrapTrait> Goal;
#else
    typedef Bob::CDCGoalBest<TrapTrait> Goal;
#endif
    typedef Bob::CDCAlgo<TrapTrait> Algo; 
    typedef Bob::CDCStat Stat;
};

/// the Trap Node that inherits from Costed Divide and Conquer Node, Bob::CDCIntMinNode.
class TrapNode : public Bob::CDCIntMinNode {
  protected:
  
  int sx;
  int sy;
  /// Where is the mole on the line i.
  Bob::pvector<Bob::pvector<int> >g;
  int ti,tj;
  void inc(int n=1) { 
    tj+=n; 
    if ( tj>=sy ) { tj=0; ti++; } 
    if ( ti>=sx ) setSol();
  }
  public :
  /// Constructor
  TrapNode() : Bob::CDCIntMinNode(), sx(0),sy(0), g(),ti(0),tj(0) {}
  /// Constructor
  TrapNode(int _sx,int _sy) : Bob::CDCIntMinNode(0),sx(_sx),sy(_sy),g(sx,Bob::pvector<int>(sy)),ti(0),tj(0) {
    for (int i=0; i<sx ;i++) { 
      for (int j=0; j<sy ;j++) { 
        g[i][j] = 0; 
      }
    }
  }
  /// Constructor
  TrapNode(TrapNode &mn) : Bob::CDCIntMinNode(mn), sx(mn.sx), sy(mn.sy),g(mn.g),ti(mn.ti),tj(mn.tj) { }
  /// Constructor
  TrapNode(const TrapNode &mn) : Bob::CDCIntMinNode(mn),sx(mn.sx), sy(mn.sy),g(mn.g),ti(mn.ti),tj(mn.tj) { }
  /// Constructor
  TrapNode(TrapNode *mn) : Bob::CDCIntMinNode(mn),sx(mn->sx), sy(mn->sy), g(mn->g),ti(mn->ti),tj(mn->tj) { }
  ///
  bool putTrap(bool put) { 
    dist()++;
    if ( put ) {
      g[ti][tj]=2;
      setCost(getCost()+1);
    }
    inc(1);
    return check_sol();
  }
  int check_sol_H() {
    for (int i=0; i< sx;i++) {
      int nbc=0;
      for (int j=0;j<sy;j++) { 
        if ( g[i][j]==2 ) nbc=0;
        if ( g[i][j]==0 ) nbc++;
        //std::cout << nbc <<" ";
        if (nbc==3 ) {
          if ( (j<tj&&i==ti) || i<ti ) return 0;
          return 1;
        }
      }
      //std::cout << std::endl;
    }
    return 2;
  }
  int check_sol_V() {
    for (int j=0;j<sy;j++) { 
      int nbc=0;
      for (int i=0; i< sx;i++) {
        if ( g[i][j]==2 ) nbc=0;
        if ( g[i][j]==0 ) nbc++;
        //std::cout << nbc <<" ";
        if (nbc==3 ) {
          if ( (j<tj&&i==ti) || i<ti ) return 0;
          return 1;
        }
      }
      //std::cout << std::endl;
    }
    return 2;
  }
  bool check_sol() {
    int ch=check_sol_H();
    //std::cout << std::endl;
    int cv=check_sol_V();
    //std::cout << "H:"<<ch<<" V:"<<cv<<std::endl;
    if ( ch==0 || cv==0 ) return false;
    if ( ch==2 && cv==2 ) setSol();
    return true;
  }
  char getchar(int i,int j) const {
    if ( g[i][j]==2 ) return 'B';
    //if ( g[i][j]==1 ) return '-';
    return '-';
  }
  /// Display a Trap Node
  virtual ostream &Prt(ostream &os) const {
     os << "Dist:"<<dist()<<" Cost:"<<getCost()<<" Sol:"<<isSol()<<" ("<<ti<<","<<tj<<")\n";
     for (int i=0; i< sx;i++) {
       for (int j=0;j<sy;j++) { 
         os << getchar(i,j)<<" ";
       }
       os<<"\n";
     }
     return os;
  }
  /// Pack method to serialize the Node
  virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("TrapNode::Pack()", "Called");
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("TrapNode::UnPack()", "Called");
  }

};

/// the Trap GenChild that inherits from the basic GenChild intanciates with the TrapTrait class.
class TrapGenChild : public Bob::CDCGenChild<TrapTrait>  {
   public:
     /// Constructor
     TrapGenChild(const TrapTrait::Instance *_inst, TrapTrait::Algo *_al) : 
                 Bob::CDCGenChild<TrapTrait>(_inst,_al) {}

     /// The function class
     virtual bool operator()(TrapNode *n) {
        bool fl=false;
        //bool sol=false;
        //std::cout <<" ------ Gen child ------\n";
        //n->Prt(std::cout);
        TrapNode *t = new TrapNode(n);
        if ( t->putTrap(true) ) {
          if ( t->isSol() && t->getCost()==7) {
            std::cout <<std::endl<<"Put 1 ins"<<std::endl;
            t->Prt(std::cout);
          }
          fl = algo->Search(t);
        } else {
          //std::cout <<std::endl<<"del"<<std::endl;
          //t->Prt(std::cout);
          delete t;
        }
        t = new TrapNode(n);
        if ( t->putTrap(false)) {
          if ( t->isSol()  && t->getCost()==7 ) {
            std::cout <<std::endl<<"Put 0 ins"<<std::endl;
            t->Prt(std::cout);
          }
          fl = algo->Search(t) || fl;
        } else {
          //std::cout <<std::endl<<"del"<<std::endl;
          //t->Prt(std::cout);
          delete t;
        }
        //std::cout <<" ------------\n";
        return fl; 
     }
};

class TrapProblem : public Bob::Problem<TrapTrait> {
        public:
                TrapProblem() : Bob::Problem<TrapTrait>() {}
                virtual ~TrapProblem() {}
};


/// the Instance class for the Trap.
class TrapInstance : public Bob::SDCInstance<TrapTrait> {
public:
/// the Size of the grid.
int sx;
int sy;

int cover_type;

  /// Constructor
  TrapInstance() : Bob::SDCInstance<TrapTrait>("ml"),sx(),sy(),cover_type(0) {
  }
  /// Constructor
  TrapInstance(int _s) : Bob::SDCInstance<TrapTrait>("ml"),sx(_s),sy(_s),cover_type(0) {
  }
  /// Initialization of the instance
  void Init() {}
  ///Intialization of the Algorithm
  void InitAlgo(TrapTrait::Algo *a) const {
    TrapTrait::Node *n = new TrapTrait::Node(sx,sy);

    n->Prt(std::cout);
    a->GPQIns(n);
  }
  /// Get the initial best known solution
  TrapNode *getSol() { 
   //std::cout<<"Instance::getSol \n"; 
    TrapNode *mn = new TrapNode(sx,sy); 
    mn->setCost(20);
    return mn;
  }

   /// Pack method to serialize the Instance
   virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("KSInstance::Pack()", "Called");
        Bob::SDCInstance<TrapTrait>::Pack(bs);
   }
   /// Unpack method to deserialize the Instance
   virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("KSInstance::UnPack()", "Called");
        Bob::SDCInstance<TrapTrait>::UnPack(bs);
   }

};

int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--ml"),Bob::Property("-s","size of the grid problem",grid_size));
#ifdef Threaded
        Bob::ThrSDCAlgoEnvProg<TrapTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#elif defined(Atha)
        Bob::AthaSDCAlgoEnvProg<TrapTrait> env;
        Bob::AthaEnvProg::init(n, v);
        Bob::core::Config(n, v);
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::init(n, v);
        Bob::mpx::SDCAlgoEnvProg<TrapTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
        Bob::core::Config(n, v);
        Bob::mpx::EnvProg::start();
#else
        Bob::SeqSDCAlgoEnvProg<TrapTrait> env;
        Bob::core::Config(n,v);
#endif
        TrapProblem mp();
        TrapInstance *Instance=new TrapInstance(Bob::core::opt().NVal("--ml","-s"));

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

