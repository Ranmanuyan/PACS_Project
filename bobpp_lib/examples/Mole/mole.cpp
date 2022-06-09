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
 *  File   : mole.cpp
 *  Author : B. Le Cun.
 *  Date   : 11-2011.
 *  Comment: Source file for the mole problem

This code illustrate how to write an application on top of
Bob++. The problem solved is the mole problem
using the divide and conquer technique

The mole problem consist in maximizing the number of mole are
on a garden (represented by a grid) in order that 2 mole area are disjoint. 
In this version of the problem a mole area is 3 cells of the grid
(3 horizontal or vertical). And the garden is 5x5 grid

The mole problem is the dual of the trap problem.

For a 5x5 grid garden, an optimal solution is
\verbatim
H - - V V 
H - - - - 
V V   - - 
- - H - - 
- - H - - 
\endverbatim
Another exists but is the symetricaly equivalent.
\verbatim
V V H - - 
- - H - - 
- -   V V 
H - - - - 
H - - - - 
\endverbatim

This implementation uses the simple Costed Divide and Conquer method.

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

/** \mainpage Mole/Bob++
    @author Bertrand Le cun 
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the mole problem
using the divide and conquer technique

*/

const int grid_size=5;

/// Forward decalration of the info for the mole problem
class MolProblem;
/// Forward decalration of the info for the mole problem
class MolInstance;
/// Forward decalration of a Node for the mole problem
class MolNode;
/// Forward decalration of a the GenChild for the mole problem
class MolGenChild;

/// the Trait for the mole problem
class MolTrait {
  public:    
    typedef MolNode Node;
    typedef MolInstance Instance ;
    typedef MolGenChild GenChild;
    typedef Bob::DepthPri<MolNode> PriComp;
#ifdef COUNT
    typedef Bob::CDCGoalCount<MolTrait> Goal;
#else
    typedef Bob::CDCGoalBest<MolTrait> Goal;
#endif
    typedef Bob::CDCAlgo<MolTrait> Algo; 
    typedef Bob::CDCStat Stat;
};

/// the Mol Node that inherits from Simple Divide and Conquer Node, Bob::SDCNode.
class MolNode : public Bob::CDCIntMaxNode {
  protected:
  
  int sx;
  int sy;
  /// Where is the mole on the line i.
  Bob::pvector<Bob::pvector<int> >g;
  int ni,nj;
  void inc(int n=1) { 
    nj+=n; 
    if ( nj>=sy ) { nj=0; ni++; } 
    if ( ni>=sx ) setSol();
  }
  public :
  /// Constructor
  MolNode() : Bob::CDCIntMaxNode(), sx(0),sy(0), g(),ni(0),nj(0) {}
  /// Constructor
  MolNode(int _sx,int _sy) : Bob::CDCIntMaxNode(0),sx(_sx),sy(_sy),g(sx,Bob::pvector<int>(sy)),ni(0),nj(0) {
    for (int i=0; i<sx ;i++) { 
      for (int j=0; j<sy ;j++) { 
        g[i][j] = 0; 
      }
    }
  }
  /// Constructor
  MolNode(MolNode &mn) : Bob::CDCIntMaxNode(mn), sx(mn.sx), sy(mn.sy),g(mn.g),ni(mn.ni),nj(mn.nj) { }
  /// Constructor
  MolNode(const MolNode &mn) : Bob::CDCIntMaxNode(mn),sx(mn.sx), sy(mn.sy),g(mn.g),ni(mn.ni),nj(mn.nj) { }
  /// Constructor
  MolNode(MolNode *mn) : Bob::CDCIntMaxNode(mn),sx(mn->sx), sy(mn->sy), g(mn->g),ni(mn->ni),nj(mn->nj) { }
  /// mole i has a position or not.
  void Next(int dir) { 
    dist()++;
    switch(dir) {
      case 'H' : 
        g[ni][nj]=2; g[ni][nj+1]=1;g[ni][nj+2]=1;inc(3);break;
      case 'V' : 
        g[ni][nj]=4; g[ni+1][nj]=1;g[ni+2][nj]=1;inc(1);break;
      case 'T' : 
        inc(1);
        return;
      default : return ;
    }
    setCost(getCost()+1);
  }
  bool is_ok_H() {
    if ( (sy-nj)<3 ) { /*std::cout <<"H pas place\n";*/ return false; }
    if ( g[ni][nj]!=0 || g[ni][nj+1]!=0 || g[ni][nj+2]!=0 ) { /*std::cout<<"H y en a qui gene\n";*/ return false;}
    return true;
  }
  bool is_ok_V() {
    if ( (sx-ni)<3 ) { /*std::cout <<"V pas de place\n";*/ return false; }
    if( g[ni][nj]!=0 || g[ni+1][nj]!=0 || g[ni+2][nj]!=0 ) { /*std::cout<<"V y en a qui gene\n";*/ return false;}
    return true;
  }
  char getchar(int i,int j) const {
    if ( g[i][j]==2 ) return 'H';
    if ( g[i][j]==4 ) return 'V';
    if ( g[i][j]==1 ) return '-';
    return ' ';
  }
  /// Display a Mol Node
  virtual ostream &Prt(ostream &os) const {
     os << "Dist:"<<dist()<<" Cost:"<<getCost()<<" Sol:"<<isSol()<<" ("<<ni<<","<<nj<<")\n";
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
        DBGAFF_USER("MolNode::Pack()", "Called");
  }

  /// Unpack method to deserialize the Node
  virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("MolNode::UnPack()", "Called");
  }

};

/// the Mol GenChild that inherits from the basic GenChild intanciates with the MolTrait class.
class MolGenChild : public Bob::CDCGenChild<MolTrait>  {
   public:
     /// Constructor
     MolGenChild(const MolTrait::Instance *_inst, MolTrait::Algo *_al) : 
                 Bob::CDCGenChild<MolTrait>(_inst,_al) {}

     /// The function class
     virtual bool operator()(MolNode *n) {
        bool fl=false;
        //bool sol=false;
        //std::cout <<" ------ Gen child ------\n";
        //n->Prt(std::cout);
        MolNode *mh = new MolNode(n);
        if ( mh->is_ok_H() ) {
          mh->Next('H');
          //std::cout << "Ok insert :";mh->Prt(std::cout);
          fl = algo->Search(mh);
        } 
        MolNode *mv = new MolNode(n);
        if ( mv->is_ok_V() ) {
          mv->Next('V');
          //std::cout << "Ok insert :";mv->Prt(std::cout);
          fl = algo->Search(mv) || fl ;
        } 
        MolNode *mt = new MolNode(n);
        mt->Next('T');
        //std::cout << "Ok insert :";mt->Prt(std::cout);
        fl = algo->Search(mt) || fl ;
        //std::cout <<" ------------\n";
        return fl; 
     }
};

class MolProblem : public Bob::Problem<MolTrait> {
        public:
                MolProblem() : Bob::Problem<MolTrait>() {}
                virtual ~MolProblem() {}
};


/// the Instance class for the Mol.
class MolInstance : public Bob::SDCInstance<MolTrait> {
public:
/// the Size of the grid.
int sx;
int sy;

int cover_type;

  /// Constructor
  MolInstance() : Bob::SDCInstance<MolTrait>("ml"),sx(),sy(),cover_type(0) {
  }
  /// Constructor
  MolInstance(int _s) : Bob::SDCInstance<MolTrait>("ml"),sx(_s),sy(_s),cover_type(0) {
  }
  /// Initialization of the instance
  void Init() {}
  ///Intialization of the Algorithm
  void InitAlgo(MolTrait::Algo *a) const {
    MolTrait::Node *n = new MolTrait::Node(sx,sy);

    n->Prt(std::cout);
    a->GPQIns(n);
  }
  /// Get the initial best known solution
  MolNode *getSol() { 
   std::cout<<"Instance::getSol \n"; 
    MolNode *mn = new MolNode(sx,sy); 
    mn->setCost(0);
    return mn;
  }

   /// Pack method to serialize the Instance
   virtual void Pack(Bob::Serialize &bs) const {
        DBGAFF_USER("KSInstance::Pack()", "Called");
        Bob::SDCInstance<MolTrait>::Pack(bs);
   }
   /// Unpack method to deserialize the Instance
   virtual void UnPack(Bob::DeSerialize &bs)  {
        DBGAFF_USER("KSInstance::UnPack()", "Called");
        Bob::SDCInstance<MolTrait>::UnPack(bs);
   }

};

int main(int n, char ** v) {

        Bob::core::opt().add(std::string("--ml"),Bob::Property("-s","size of the grid problem",grid_size));
#ifdef Threaded
        Bob::ThrSDCAlgoEnvProg<MolTrait> env;
        Bob::ThrEnvProg::init();
        Bob::core::Config(n,v);
        Bob::ThrEnvProg::start();
#elif defined(Atha)
        Bob::AthaSDCAlgoEnvProg<MolTrait> env;
        Bob::AthaEnvProg::init(n, v);
        Bob::core::Config(n, v);
#elif defined(MPxMPI)
        Bob::mpx::EnvProg::init(n, v);
        Bob::mpx::SDCAlgoEnvProg<MolTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
        Bob::core::Config(n, v);
        Bob::mpx::EnvProg::start();
#else
        Bob::SeqSDCAlgoEnvProg<MolTrait> env;
        Bob::core::Config(n,v);
#endif
        MolProblem mp();
        MolInstance *Instance=new MolInstance(Bob::core::opt().NVal("--ml","-s"));

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

