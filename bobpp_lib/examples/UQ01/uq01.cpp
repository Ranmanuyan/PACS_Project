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
 *  File   : q01.h
 *  Author : B. Le Cun based on a tentative of F. Roupin
 *  Date   : April the 19th 2011.
 */


#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <signal.h>
#include <fstream>

#include <bobpp/bobpp>
#ifdef Threaded
#  include <bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif

#include <bobpp/util/bob_util.hpp>

#define NMAX 128 

/** \mainpage UQ01/Bob++
    @author Bertrand Le cun and Frederic Roupin
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the Unconstrained Quadratic 0/1 problem with a very simple lower bound.
*/

class Q01Instance;
class Q01Node;
class Q01GenChild;
class Q01Stat;

typedef int eval_t;

class Q01Trait {
  public:    
    typedef Q01Node Node;
    typedef Q01Instance Instance;
    typedef Q01GenChild GenChild;
    typedef Bob::BBAlgo<Q01Trait> Algo;
    typedef Bob::BBGoalBest<Q01Trait> Goal;
    typedef Bob::DepthEPri<Q01Node> PriComp;
    typedef Q01Stat Stat;
};


/// the Q01 Node
class Q01Node : public Bob::BBIntMinNode {
protected:
    Bob::pvector<int> x;
public:
    /// Contructor
    Q01Node() : Bob::BBIntMinNode(std::numeric_limits<eval_t>::max()) , x() { }
    /// Contructor
    Q01Node(int s) : Bob::BBIntMinNode(std::numeric_limits<eval_t>::max()) , x(s,-1) { 
    }
    /// Contructor
    Q01Node(int s,eval_t cost) : Bob::BBIntMinNode(cost) { }
    /// Contructor
    Q01Node(const Q01Node &p) : Bob::BBIntMinNode(p) , x(p.x) { }
    /// Contructor
    Q01Node(Q01Node *p) : Bob::BBIntMinNode(p), x(p->x) { 
    }
    /// Destructor
    virtual ~Q01Node() { }
    /// Copy method
    virtual void copy(const Q01Node &p) {
        Bob::BBIntMinNode::copy(p);
        x.copy(p.x);
    }
    /// function for the evaluation.
    void evalu(const Q01Instance *da);

    /// Method to fix a variable to a value.
    void fixe(int wh,int v) {
        x[wh]=v; 
    }

    /** Fill the strbuff with node information
      * @param st the strbuff to fill.
      */
    virtual void log_space(Bob::strbuff<> &st) {
      Bob::BBIntMinNode::log_space(st);
      if ( dist()==0 ) { st<<",\"root\""; }
      else {
        st << ", \"x[" << dist()-1 <<"]<-"<<x[dist()-1]<<"\"";
      }
    }
    /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs)  const {
       DBGAFF_ENV("Q01Node::Pack","Pack a node");

	    Bob::BBIntMinNode::Pack(bs);
	    x.Pack(bs);
       DBGAFF_ENV("Q01Node::Pack","------ Finish");
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("Q01Node::UnPack","UnPack a node");
	    Bob::BBIntMinNode::UnPack(bs);
	    x.UnPack(bs);
       DBGAFF_ENV("Q01Node::UnPack","------ Finish");
    }

    /// Display a Q01 Node
    virtual ostream &Prt(ostream &os) const {
        Bob::BBIntMinNode::Prt(os);
        if ( isPb() ) {
           os << "Pb:"<<dist()<<"<"<<x.size()<<" Partial Sol :";
           for (int i=0;i<dist();i++ ) {
             os << ", x[" << i <<"]="<<x[i];
           }
           os << std::endl;
        } else {
           os << "Pb:"<<dist()<<"="<<x.size()<<" Sol :";
           for (unsigned int i=0;i<x.size();i++ ) {
             os << ", x[" << i <<"]="<<x[i];
           }
           os << std::endl;
        }
        return os;
    }
};

class Q01Stat : public Bob::BBStat {
   public:
      Q01Stat(const Id &s) : BBStat(s) { 
      }
      virtual ~Q01Stat() { }
};

class Q01GenChild : public Bob::BBGenChild<Q01Trait>  {
        public:
          Q01GenChild(const Q01Instance *_inst, Q01Trait::Algo *_sch) : 
                 Bob::BBGenChild<Q01Trait>(_inst,_sch) {}
          virtual ~Q01GenChild() {}
          virtual bool operator()(Q01Node *n);
};


/// the BobInfoAlgo class for the Q01.
class Q01Instance : public Bob::BBInstance<Q01Trait> {
public:
/// The file name of the problem instance
std::string file;
  /// the matrix 
int m[NMAX][NMAX];
/// the Size of the problem.
int Size;

   /// Constructor
   Q01Instance() : Bob::BBInstance<Q01Trait>("q01"),file(""),Size(0) {
   }
   /// Constructor
   Q01Instance(std::string _file) : Bob::BBInstance<Q01Trait>("q01"),file(_file),Size(0) {
       readfile(file);
   }
   /// Constructor
   Q01Instance(const Q01Instance &inst) : Bob::BBInstance<Q01Trait>(inst),file(inst.file),Size(inst.Size) {
      for (int i=0;i<Size;i++) {
          memcpy(m[i],inst.m[i],sizeof(int)*(Size));
      }
   }
   /// Destructor
   virtual ~Q01Instance() {}
   /**
   *  \brief method to copy an instance
   *  \param  i the source instance
   */
   virtual void copy(const Q01Instance &ai) {
      int i;
      Bob::BBInstance<Q01Trait>::copy(ai);
      file=ai.file;
      Size=ai.Size;
      for (i=0;i<Size;i++) {
          memcpy(m[i],ai.m[i],sizeof(int)*(Size));
      }
   }
   /// Initialization of the instance
   void Init() {}
   /// the method that initialize and insert the root nodes.
   void InitAlgo(Q01Trait::Algo *al) const {
       Q01Node *qn;
       qn = new Q01Node(Size);
       qn->evalu(this);
       //al->log_space(qn);
       al->GPQIns(qn);
   }
   /// the method that initialize and set the initial incubent.
   Q01Node *getSol() {
       Q01Node *sol = new Q01Node();
       sol->setSol();
       std::cout << *sol<<"\n";
       return sol;
   }
   /// reads the data from files.
   void readfile(const string &v);
   /// returns the size of te problem
   int getSize() const { return Size; }
   /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs) const  {
       DBGAFF_ENV("Q01Instance::Pack","Pack an instance");
	    Bob::BBInstance<Q01Trait>::Pack(bs);
	    bs.Pack(&Size,1);
	    for (int i=0;i<Size;i++) {
                bs.Pack(m[i],Size);
            }
       DBGAFF_ENV("Q01Instance::Pack","------- Finish");
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("Q01Instance::UnPack","UnPack an instance");
	    Bob::BBInstance<Q01Trait>::UnPack(bs);
	    bs.UnPack(&Size,1);
	    for (int i=0;i<Size;i++) {
                bs.UnPack(m[i],Size);
            }
       DBGAFF_ENV("Q01Instance::UnPack","------- Finish");
    }
};


class FileError {};

void Q01Instance::readfile(const string &strin)
{
   int  i, j,k,tmp,nbval;
   std::ifstream in(file.c_str());
   if (!in.is_open())
     throw FileError();
   in >> Size;
   cout << "Problem Size : "<< Size <<"\n";
   in >> nbval;

   for (i = 0; i < nbval ; i++) {
      for (j = 0; j < Size ; j++) {
          m[i][j]=0;
      }
   }
   for (k = 0; k < nbval ; k++) {
      in >> i >> j >> tmp;
      m[i-1][j-1] = tmp;
   }
   for (i = 0; i < Size ; i++) {
      for (j = 0; j < Size ; j++) {
          std::cout << m[i][j] <<" ";
      }
      std::cout << std::endl;
   }
}

bool Q01GenChild::operator()(Q01Node *p) {
    Q01Node *fils;
    int fl=0;
    
    for (int j = 0; j < 2; j++) {
         fils = new Q01Node(p);
         algo->start_eval(p,fils);
         fils->fixe(fils->dist(),j);
         fils->evalu(inst);
         algo->end_eval(fils);
         fils->dist()++;
         if ( fils->dist()==inst->getSize() ) fils->setSol();
         if ( algo->getGoal()->is4Search(fils) ) {
               fl +=algo->Search(fils);
         } else {
               algo->StPrun();
         }
         if( fils->isDel() ) delete fils;
   }
   return fl;
}

void Q01Node::evalu(const Q01Instance *dai) {
   double val=0.0;
   int i,j;
   int S = dai->getSize();
	
   //std::cout <<"Size of the problem :"<<S<<std::endl;
   for(i=0;i<S;i++){
      for(j=0;j<S;j++){
         if ( (x[i]!=-1) && (x[j]!=-1) ) val += (x[i])*(x[j])*dai->m[i][j];
	 else if ( dai->m[i][j]<0 ) val += dai->m[i][j];
      }
   }
   setEval((eval_t)val);
}


int main(int n, char ** v) {
  Bob::core::opt().add(std::string("--q01"), Bob::Property("-f", "Problem File", "pb/example.txt"));
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<Q01Trait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrBBAlgoEnvProg<Q01Trait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::BBAlgoEnvProg<Q01Trait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<Q01Trait> env;
  Bob::core::Config(n, v);
#endif

  Q01Instance *Instance = new Q01Instance(Bob::core::opt().SVal("--q01", "-f"));
  // Initialize the instance.

  // Solve the instance i.e. all the tree
  std::cout << "-------- Start to solve the entire tree" << std::endl;
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
  return 0 ;
}

