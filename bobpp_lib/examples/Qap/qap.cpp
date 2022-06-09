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
 *  File   : qap.h
 *  Author : T. Mautor & B. Le Cun.
 *  Date   : Mar 30 1999.
 *  Date   : May 18 2007 modification for the New Bob++ version.
 *  Comment: header file QAP Application.
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


#define NMAX 64
#define PROF 2

/** \mainpage QAP/Bob++
    @author Bertrand Le cun Thierry Mautor
    @version 1.0

This code illustrate how to write an application on top of
Bob++. The problem solved is the Quadratic Assignment Problem (QAP).
A Branch and bound is used to exactly solve some quite large instances.

So for now the method is quite old, new work has been done for this problem.

*/

class QAPInstance;
class QAPNode;
class QAPGenChild;
class QAPStat;

class QAPTrait {
  public:    
    typedef QAPNode Node;
    typedef QAPInstance Instance;
    typedef QAPGenChild GenChild;
    typedef Bob::BBAlgo<QAPTrait> Algo;
    typedef Bob::BBGoalBest<QAPTrait> Goal;
    typedef Bob::DepthEPri<QAPNode> PriComp;
    typedef QAPStat Stat;
};

//int Work;

/// the QAP Node
class QAPNode : public Bob::BBIntMinNode {
protected:

public:
  int   Size;
  int   lvl;
  int   tfa[NMAX];
  int   tpe[NMAX];
  int   colsep;
  int   resym;
  int   forb[NMAX];
  int   acl[NMAX][NMAX];
  int   bcl[NMAX][NMAX];
    /// Contructor
    QAPNode() : Bob::BBIntMinNode(0) { Size=0;lvl=0;}
    /// Contructor
    QAPNode(int s) : Bob::BBIntMinNode(0) { 
        Size = s;
        colsep = 0;
        resym = 0;
    }
    /// Contructor
    QAPNode(int s,int cost) : Bob::BBIntMinNode(cost) { 
        Size = s;
        colsep = 0;
        resym = 0;
        setSol();
    }
    QAPNode(const QAPNode &p) : Bob::BBIntMinNode(p){
    int i;
        Size = p.Size;
        lvl = p.lvl;
        dist() = lvl;
        if ( p.isPb() ) { 
           colsep = p.colsep;
           resym = p.resym;
           memcpy(tfa,p.tfa,sizeof(int)*Size);
           memcpy(tpe,p.tpe,sizeof(int)*Size);
           memcpy(forb,p.forb,sizeof(int)*Size);
           for (i=0;i<Size;i++) {
             memcpy(acl[i],p.acl[i],sizeof(int)*(Size-1));
             memcpy(bcl[i],p.bcl[i],sizeof(int)*(Size-1));
           }
        } else {
           setSol();
           memcpy(tfa,p.tfa,sizeof(int)*Size);
           memcpy(tpe,p.tpe,sizeof(int)*Size);
        }

    }
    /// Contructor
    QAPNode(QAPNode *p) : Bob::BBIntMinNode(p) { 
    int i;
        Size = p->GetSize();
        lvl = p->lvl;
        dist() = lvl;
        if ( p->isPb() ) { 
           colsep = p->colsep;
           resym = p->resym;
           memcpy(tfa,p->tfa,sizeof(int)*Size);
           memcpy(tpe,p->tpe,sizeof(int)*Size);
           memcpy(forb,p->forb,sizeof(int)*Size);
           for (i=0;i<Size;i++) {
             memcpy(acl[i],p->acl[i],sizeof(int)*(Size-1));
             memcpy(bcl[i],p->bcl[i],sizeof(int)*(Size-1));
           }
        } else {
           setSol();
           memcpy(tfa,p->tfa,sizeof(int)*Size);
           memcpy(tpe,p->tpe,sizeof(int)*Size);
        }
    }
    /// Destructor
    virtual ~QAPNode() {
      //std::cout <<"***************** Strange delete a recursive node "<<dist()<<":"<<getSol()<<" **************\n";
      //std::cout<<"Destroy "<<((void *)this)<<std::endl; 
    }
    virtual void copy(const QAPNode &p) {
    int i; 
        Bob::BBIntMinNode::copy(p);
        Size = p.Size;
        if ( p.isPb() ) { 
           lvl = p.lvl;
	   dist() = lvl;
           colsep = p.colsep;
           resym = p.resym;
           memcpy(tfa,p.tfa,sizeof(int)*Size);
           memcpy(tpe,p.tpe,sizeof(int)*Size);
           memcpy(forb,p.forb,sizeof(int)*Size);
           for (i=0;i<Size;i++) {
             memcpy(acl[i],p.acl[i],sizeof(int)*(Size-1));
             memcpy(bcl[i],p.bcl[i],sizeof(int)*(Size-1));
           }
        } else {
           setSol();
           memcpy(tfa,p.tfa,sizeof(int)*Size);
           memcpy(tpe,p.tpe,sizeof(int)*Size);
        }

    }
    /** Fill the strbuff with node information
      * @param st the strbuff to fill.
      */
    virtual void log_space(Bob::strbuff<> &st) {
      Bob::BBIntMinNode::log_space(st);
      if ( lvl==0 ) { st<<",\"root\""; }
      else {
        st << ",\"" << tfa[lvl-1]<<"->"<<tpe[lvl-1]<<"\"";
      }
    }

    /// Dispay a vector
    void AffVect(ostream &os, const char *ti, const int *t,int size) const {
        int i;
        if ( ti!=0 ) os << ti << ": ";                       
        for (i = 0; i < size; i++)                                 
              os << t[i] << " ";                       
        os << "\n";                       
    }
    /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs)  const {
       DBGAFF_ENV("QAPNode::Pack","Pack a node");

	    Bob::BBIntMinNode::Pack(bs);
	    bs.Pack(&Size,1);
		 bs.Pack(tfa,Size);
		 bs.Pack(tpe,Size);
	    //if ( isPb() ) {
		    bs.Pack(&lvl,1);
		    bs.Pack(&colsep,1);
		    bs.Pack(&resym,1);
		    bs.Pack(forb,Size);
	       for (int i=0;i<Size;i++) {
                bs.Pack(acl[i],Size);
                bs.Pack(bcl[i],Size);
          }
	    //}
       DBGAFF_ENV("QAPNode::Pack","------ Finish");
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("QAPNode::UnPack","UnPack a node");
	    Bob::BBIntMinNode::UnPack(bs);
	    bs.UnPack(&Size,1);
		 bs.UnPack(tfa,Size);
		 bs.UnPack(tpe,Size);
	    //if ( isPb() ) {
		    bs.UnPack(&lvl,1);
		    bs.UnPack(&colsep,1);
		    bs.UnPack(&resym,1);
		    bs.UnPack(forb,Size);
	       for (int i=0;i<Size;i++) {
               bs.UnPack(acl[i],Size);
               bs.UnPack(bcl[i],Size);
          }
	    //} 
       DBGAFF_ENV("QAPNode::UnPack","------ Finish");
    }

    /// Display a QAP Node
    virtual ostream &Prt(ostream &os) const {
        int i;
        if ( isPb() ) {
           os << "------------------------------------\n";
           os<< "  BobQAP:" << val << " Size:"<<Size<<" Lvl:"<<lvl<<"\n";
           os<< "  Colsep:" << colsep << " sym :"<<resym<<" \n";
           AffVect(os,"tfa",tfa,Size);
           AffVect(os,"tpe",tpe,Size);
           os << "acl :";                       
           for (i = 0; i < Size-1-lvl; i++)                                 
              AffVect(os,0,acl[i],Size-1-lvl);
           os << "bcl :";                       
           for (i = 0; i < Size-1-lvl; i++)                                 
              AffVect(os,0,bcl[i],Size-1-lvl);
        } else {
           int T[NMAX];
           for (i = 0; i < Size; i++) T[tfa[i]] = tpe[i];
           os<< "  BobQAP:" << val << "\n";
           AffVect(os,"Affect",T,Size);
           AffVect(os,"tfa",tfa,Size);
           AffVect(os,"tpe",tpe,Size);
        }
        return os;
    }

    /// return the size of the problem
    int GetSize() { return Size; }
    /// initialization of the node as the root nodes
    void InitAsRoot(const QAPInstance *al);
    ///
    void msort(const QAPInstance *da);
    ///
    int tstsym(int n,const QAPInstance *d);
    ///
    void conscl(const QAPInstance *da,int e, int f, QAPNode *pere, int c, int n);
    ///
    void consacl(const QAPInstance *da,int e,int f,int g,QAPNode *pere,int c,int n);
    ///
    int cousol(const QAPInstance *da);
    /// resolution where the subproblem size is 3
    void resol3(const QAPTrait::Instance *);
    /// function for the evaluation.
    void evalu(const QAPInstance *da,int locbks);
    /// linear affectation methods.
    int lsapr(int n, int *c, int *ys, int *yt,int *spalte);
    /// linear affectation methods.
    int lsapr2(int n, int *c, int *p);
};

class QAPStat : public Bob::BBStat {
   public:
      QAPStat(const Id &s) : BBStat(s) { 
      }
      virtual ~QAPStat() { }
};

class QAPGenChild : public Bob::BBGenChild<QAPTrait>  {
        public:
          QAPGenChild(const QAPInstance *_inst, QAPTrait::Algo *_sch) : 
                 Bob::BBGenChild<QAPTrait>(_inst,_sch) {}
          virtual ~QAPGenChild() {}
          virtual bool operator()(QAPNode *n);
};


/// the BobInfoAlgo class for the QAP.
class QAPInstance : public Bob::BBInstance<QAPTrait> {
public:
/// The file name of the problem instance
std::string file;
  /// the 2 matrix (flow and distance).
int a[NMAX][NMAX], b[NMAX][NMAX];
/// the Size of the problem.
int Size;
/// the inital Upper Bound
int IUpBd;
/// Handle symetry or not (see Thierry Mautor works).
int Sym;

   /// Constructor
   QAPInstance() : Bob::BBInstance<QAPTrait>("qap"),file(""),Size(0),IUpBd(0), Sym(0) {
   }
   /// Constructor
   QAPInstance(std::string _file) : Bob::BBInstance<QAPTrait>("qap"),file(_file),Size(0),IUpBd(0), Sym(0) {
       lectu(file);
   }
   /// Destructor
   virtual ~QAPInstance() {}
   /**
   *  \brief method to copy an instance
   *  \param  i the source instance
   */
   virtual void copy(const QAPInstance &ai) {
      int i;
      Bob::BBInstance<QAPTrait>::copy(ai);
      file=ai.file;
      Size=ai.Size;
      IUpBd=ai.IUpBd;
      Sym=ai.Sym;
      for (i=0;i<Size;i++) {
          memcpy(a[i],ai.a[i],sizeof(int)*(Size));
          memcpy(b[i],ai.b[i],sizeof(int)*(Size));
      }
   }
   /// Initialization of the instance
   void Init() {}
   /// the method that initialize and insert the root nodes.
   void InitAlgo(QAPTrait::Algo *al) const {
       QAPNode *qn;
       qn = new QAPNode();
       qn->InitAsRoot(this);
       //al->log_space(qn);
       al->GPQIns(qn);
   }
   /// the method that initialize and insert the initial upper bound.
   QAPNode *getSol() {
       QAPNode *sol = new QAPNode(0,getIUPBD());
       //std::cout << "Demande de la solution initial :"<<getIUPBD()<<"\n"; 
       //std::cout << *sol<<"\n";
       return sol;
   }
   /// reads the data from files.
   void lectu(const string &v);
   /// returns the size of te problem
   int GetSize() const { return Size; }
   /// returns a particular value of the flow matrix
   int GetA(int i,int j) const { return a[i][j]; }
   /// returns a particular value of the distance matrix
   int GetB(int i,int j) const { return b[i][j]; }
   /// returns the initial upper bound
   int getIUPBD() const { return IUpBd; }
   /// returns if the symetry must be handle or not.
   int GetSym() const { return Sym; }
   /// Pack method to serialize the BobNode
    virtual void Pack(Bob::Serialize &bs) const  {
       DBGAFF_ENV("QAPInstance::Pack","Pack an instance");
	    Bob::BBInstance<QAPTrait>::Pack(bs);
	    bs.Pack(&Size,1);
	    bs.Pack(&IUpBd,1);
	    bs.Pack(&Sym,1);
	    for (int i=0;i<Size;i++) {
                bs.Pack(a[i],Size);
                bs.Pack(b[i],Size);
            }
       DBGAFF_ENV("QAPInstance::Pack","------- Finish");
    }
    /// Unpack method to deserialize the BobNode
    virtual void UnPack(Bob::DeSerialize &bs)  {
       DBGAFF_ENV("QAPInstance::UnPack","UnPack an instance");
	    Bob::BBInstance<QAPTrait>::UnPack(bs);
	    bs.UnPack(&Size,1);
	    bs.UnPack(&IUpBd,1);
	    bs.UnPack(&Sym,1);
	    for (int i=0;i<Size;i++) {
                bs.UnPack(a[i],Size);
                bs.UnPack(b[i],Size);
            }
       DBGAFF_ENV("QAPInstance::UnPack","------- Finish");
    }
};



void QAPInstance::lectu(const string &strin)
{
   FILE *f;
   int  i, j;
   int cost;
   int test_return;

   /* lecture des matrices, de la taille et de bks */
   if ((f = fopen(strin.c_str(), "r")) == NULL) {
       fprintf(stderr,"Erreur ouverture de fichier %s\n",strin.c_str());
       exit(1);
   }
   test_return = fscanf(f, "%u\n", &Size);
   if (test_return != 1) {
       fprintf(stderr,"Erreur de lecture dans le fichier %s\n",strin.c_str());
       exit(1);
   }
   cout << "Taille du probleme "<< Size <<"\n";
   for (i = 0; i < Size ; i++) {
      for (j = 0; j < Size ; j++) {
         test_return = fscanf(f, "%d", &cost);
		   if (test_return != 1) {
				fprintf(stderr,"Erreur de lecture dans le fichier %s\n",strin.c_str());
				exit(1);
		   }
         a[i][j] = cost;
         printf("%2d ",a[i][j]);
      }
      test_return = fscanf(f, "\n");
      printf("\n");
   }
   printf("-----------\n");
   for (i = 0; i < Size; i++) {
      for (j = 0; j < Size; j++) {
         test_return = fscanf(f, "%d", &cost);
		   if (test_return != 1) {
				fprintf(stderr,"Erreur de lecture dans le fichier %s\n",strin.c_str());
				exit(1);
		   }
         b[i][j] = cost;
         printf("%2d ",b[i][j]);
      }
      test_return = fscanf(f, "\n");
      printf("\n");
   }
   test_return = fscanf(f, "%d\n", &IUpBd);
   if (test_return != 1) {
       fprintf(stderr,"Erreur de lecture dans le fichier %s\n",strin.c_str());
       exit(1);
   }
   test_return = fscanf(f, "%d\n", &i);
   if (test_return != 1) {
       fprintf(stderr,"Erreur de lecture dans le fichier %s\n",strin.c_str());
       exit(1);
   }
   printf(" Sym :%d UB:%d\n",i,IUpBd);
   Sym = i;
   fclose(f);
}

static int cmpint(const void *l1, const void *l2)
{
   return *((int *)l1) - *((int *)l2);
}

void QAPNode::InitAsRoot(const QAPInstance *dai) {
   int i;
       setPb();
       Size = dai->GetSize();
       for (i = 0; i < Size; i++) {
          tfa[i] = tpe[i] = i; 
	       forb[i]=0;
       }
       colsep=lvl = 0;
       resym = dai->GetSym(); 
       msort(dai);
       std::cout << "Evalu Root :"<<resym<<std::endl;
       evalu(dai,dai->getIUPBD());
       std::cout << "Fin Evalu Root :"<<resym<<std::endl;
}

void QAPNode::msort(const QAPInstance *da)
{
   int             i, j, k;

   for (i = 0; i < Size; i++) {
      for (k = j = 0; j < Size; j++)
         if (i != j) {
            acl[i][k] = da->GetA(i,j);
            k++;
         }
      if ( k!=(Size-1) ) { std::cerr <<"ERROR\n"; exit(2); }
      qsort((char *) (acl[i]), Size - 1, sizeof(int), cmpint);

      for (k = j = 0; j < Size; j++)
         if (i != j) {
            bcl[i][k] = da->GetB(i,j);
            k++;
         }
      qsort((char *) (bcl[i]), Size - 1, sizeof(int), cmpint);
      if ( k!=(Size-1) ) { std::cerr <<"ERROR\n"; exit(2); }
   }
}


void QAPNode::conscl(const QAPInstance *da,int e, int f, QAPNode *pere, int c, int n)
{
   int             i, j;
   i = 0;
   //std::cout <<"Not copy "<<c<<" "<<pere->colsep<<" : "<<da->GetB(c,pere->colsep)<<"\n";

   while (pere->bcl[e][i] != da->GetB(c,pere->colsep) ) {
      bcl[f][i] = pere->bcl[e][i];
      i++;
   }
   i++;
   for (j = i; j < n ; j++)
      bcl[f][j - 1] = pere->bcl[e][j];
}

void QAPNode::consacl(const QAPInstance *da,int e, int f, int g, QAPNode *pere, int c, int n)
{
   int             i, j;

   i = 0;
   while (pere->acl[e][i] != da->GetA(c,g) ) {
      acl[f][i] = pere->acl[e][i];
      i++;
   }
   i++;
   for (j = i; j < n ; j++)
      acl[f][j - 1] = pere->acl[e][j];
}

int QAPNode::cousol(const QAPInstance *da)
{
   int             i, j;
   int       cout;

   for (cout = i = 0; i < Size; i++)
       for (j = 0; j < Size; j++)
           cout += da->GetA(tfa[i],tfa[j]) * da->GetB(tpe[i],tpe[j]);
   return (cout);
}

void QAPNode::resol3(const QAPTrait::Instance *inst)
{
   int             i, pel[4];
   int             Mp[6][4];
   int besti,bestc;

   Mp[0][1] = Mp[1][1] = Mp[2][2] = Mp[3][3] = Mp[4][2] = Mp[5][3] = 1;
   Mp[0][2] = Mp[1][3] = Mp[2][1] = Mp[3][1] = Mp[4][3] = Mp[5][2] = 2;
   Mp[0][3] = Mp[1][2] = Mp[2][3] = Mp[3][2] = Mp[4][1] = Mp[5][1] = 3;
   pel[1] = tpe[Size - 1];
   pel[2] = tpe[Size - 2];
   pel[3] = tpe[Size - 3];
   besti=0;
   bestc=32000000;
   for (i = 0; i < 6; i++) {
      tpe[Size - 1] = pel[Mp[i][1]];
      tpe[Size - 2] = pel[Mp[i][2]];
      tpe[Size - 3] = pel[Mp[i][3]];
      setEval(cousol(inst));
      //if ( besti==0 ) bestc=getEval();
      if (bestc > getEval()) {
         besti=i;
         bestc=getEval();
      }
   }
   tpe[Size - 1] = pel[Mp[besti][1]];
   tpe[Size - 2] = pel[Mp[besti][2]];
   tpe[Size - 3] = pel[Mp[besti][3]];
   setEval(bestc);
   //std::cout <<" ----------------- Solution Trouvée:"<<bestc<<"\n";
   setSol();
}

bool QAPGenChild::operator()(QAPNode *p) {
   bool fl=false;
   int      i, j, k, son;
   const QAPInstance *da = inst;
   QAPNode  *fils,*f2;
   int bestSol = algo->getGoal()->getBest();
   //std::cout << "-------------------- PERE ----------"<<p->Size<<"--------------\n";
   //std::cout << "Branch on "<<p->colsep<< "Eval "<<p->getEval()<<std::endl;
   //p->Prt(std::cout);
   k=0;
   BOBASSERT(bestSol!=0,UserException,"QAPGenChild","The solution is null");
   BOBASSERT(p->Size<=inst->GetSize(),UserException,"QAPGenChild","Error problem with a Node");
   fils = get_node(p);
   if ( fils->rec() && fils->isDel() ) std::cout <<"Strange rec and del\n";
   fils->dist() = p->dist()+1;
   fils->lvl = p->lvl + 1;
   for (i = 0; i < p->Size; i++) {
      fils->tpe[i] = p->tpe[i];
      if (fils->tpe[i] == p->colsep) { 
         k = i;
      }
   }
   //Work++;
   //if ( (Work%1000)==0 ) { printf("Work %d\n",Work); }
   fils->tpe[k] = fils->tpe[p->lvl];
   fils->tpe[p->lvl] = p->colsep;
   if (fils->lvl != (p->Size - 3)) {
      if ( k>fils->lvl ) {
         for (j = fils->lvl; j < k; j++) {
            fils->conscl(da,j, j, p, fils->tpe[j], (p->Size - fils->lvl));
         }
      }
      if ( k>=fils->lvl ) 
           fils->conscl(da,p->lvl, k, p, fils->tpe[k], (p->Size - fils->lvl));
      for (j = k + 1; j < p->Size; j++)
         fils->conscl(da,j, j, p, fils->tpe[j], (p->Size - fils->lvl));
   }
   for (i = p->lvl; i < p->Size; i++) {
      if (p->forb[i] == 0) {
         algo->start_eval(fils,p);
         son = p->tfa[i];
         for (j = 0; j < p->Size; j++) {
            fils->tfa[j] = p->tfa[j];
            if (fils->tfa[j] == son)
               k = j;
         }
         fils->tfa[k] = fils->tfa[p->lvl];
         fils->tfa[p->lvl] = son;
         fils->resym = p->resym;
         if (fils->lvl != (p->Size - 3)) {
            if ( k>fils->lvl ) {
               for (j = fils->lvl; j < k; j++)
                  fils->consacl(da,j, j, son, p, fils->tfa[j], (p->Size - fils->lvl));
            }
            if ( k>=fils->lvl ) 
		         fils->consacl(da,p->lvl, k, son, p, fils->tfa[k], (p->Size - fils->lvl));
            for (j = k + 1; j < p->Size; j++)
               fils->consacl(da,j, j, son, p, fils->tfa[j], (p->Size - fils->lvl));
            fils->evalu(inst,bestSol);
            algo->end_eval(fils);
            if ( fils->getEval()<bestSol ) {
               if ( fils->rec() ) f2 = fils;
               else f2 = new QAPNode(fils);
               fl +=Search(f2);
            }
            else {
               algo->StPrun();
            }
         } else {
            fils->resol3(inst);
            algo->getStat()->end_eval(fils);
            f2 = new QAPNode(fils);
            std::cout << "Solution \n";
            fl +=Search(f2);
         }
      } else {
         //std::cout <<"Forb:"<<i<<std::endl;
         algo->StNoFea();
      }

   }
   if( fils->isDel() ) delete fils;
   return fl;
}

void QAPNode::evalu(const QAPInstance *dai,int locbks)
{
   int             nfree, i, j, ip, k, ind, jstar,nbsym;
   int             cost;
   int   ph1[NMAX], ph2[NMAX], perm[NMAX];
   int c[NMAX * NMAX];
#ifndef V2
  int wlig[NMAX], wcol[NMAX];
#endif
   int lbd;
   //int isup = 32000000;
   int iplus, xmax, xdum;

   nfree = Size - (lvl);
   for (i = 0; i < Size; i++) {
      ph1[i] = tfa[i];
      ph2[i] = tpe[i];
      forb[i] = 0;
   }
   nbsym = 0;
   if ( dai->GetSym()==0 && resym==0 ) {
      nbsym = tstsym(Size,dai);
      //std::cout << "nbSym " <<nbsym<<std::endl;
   }
   for (cost = i = 0; i < (lvl - 1); i++)
      for (j = i + 1; j < (lvl); j++) {
         cost += dai->GetA(ph1[i],ph1[j]) * dai->GetB(ph2[i],ph2[j]);
      }
   cost *=2;

   /* Construction of the C matrice */
   ip = lvl;
   for (i = ip; i < Size; i++)
      for (j = ip; j < Size; j++) {
         ind = j - ip + (nfree * (i - ip));
         for (c[ind] = k = 0; k < ip; k++) {
            c[ind] += 2 * dai->GetA(ph1[i],ph1[k]) * dai->GetB(ph2[j],ph2[k]);
	      }
         for (k = 0; k < (nfree - 1); k++)
            c[ind] += acl[i][k] * bcl[j][nfree - k - 2];
      }

#ifdef V2
   lbd = this->lsapr2(nfree, c, perm);
   cost += lbd;
   setEval(cost); 
   if ( cost <locbks) {
      iplus = locbks / Size;
      for (xmax = jstar = j = 0; j < nfree; j++) {
         for (xdum = i = 0; i < nfree; i++) {
            xdum += c[j + (nfree * i)];
            if (c[j + (nfree * i)] >= (locbks - cost))
               xdum += iplus;
         }
         if (xdum > xmax) {
            jstar = j;
            xmax = xdum;
         }
      }
      resym = nbsym==0 ? 1 : 0;
      colsep = ph2[jstar + lvl];
      xdum = locbks - cost ;
      for (i = 0; i < nfree; i++)
         if (c[jstar + (nfree * i)] > xdum) {
             forb[i+lvl] = 1;
         }
   } 

#else

   lbd = this->lsapr(nfree, c, wlig, wcol, perm);
   cost += lbd;
   setEval(cost); 
   //std::cout << getpid()<<" Eval Node :"<<cost<<std::endl;
   /* Compute the next column to branch */
   if ( cost <locbks) {
      iplus = locbks / Size;
      for (xmax = jstar = j = 0; j < nfree; j++) {
         for (xdum = i = 0; i < nfree; i++) {
            xdum += c[j + (nfree * i)] - wlig[i] - wcol[j];
            if (c[j + (nfree * i)] - wlig[i] - wcol[j] >= (locbks - cost))
               xdum += iplus;
         }
         if (xdum > xmax) {
            jstar = j;
            xmax = xdum;
         }
      }
      resym = nbsym==0 ? 1 : 0;
      colsep = ph2[jstar + lvl];
      xdum = locbks - cost + wcol[jstar];
      for (i = 0; i < nfree; i++)
         if (c[jstar + (nfree * i)] - wlig[i] > xdum) {
             forb[i+lvl] = 1;
         }
   } 
#endif
}

int QAPNode::lsapr(int n, int *c, int *ys, int *yt,int *spalte) 
{
int dplus[NMAX], dminus[NMAX];
int   zeile[NMAX], vor[NMAX], label[NMAX];
   int cost, ui, vj, d;
   int isup = 32000000;
   int             index, i, ik, j, j0, k, ind;

   /* Matrice reduction */
   for (i = 0; i < n; i++) {
      zeile[i] = spalte[i] = vor[i] = -1;
      ys[i] = yt[i] = 0;
   }
   for (ik = 1, i = 0; i < n; i++, ik++) {
      for (j = 1, ui = c[ik - 1], j0 = 0; j < n; j++, ik++)
         if (c[ik] < ui || (c[ik] == ui && zeile[j0] != -1)) {
            ui = c[ik];
            j0 = j;
         }
      ys[i] = ui;
      if (zeile[j0] == -1) {
         zeile[j0] = i;
         spalte[i] = j0;
      }
   }
   for (j = 0; j < n; j++)
      if (zeile[j] == -1)
         yt[j] = isup;
   for (ik = i = 0; i < n; i++)
      for (j = 0; j < n; j++, ik++)
         if (yt[j] != 0 && (c[ik] < yt[j] + ys[i])) {
            yt[j] = c[ik] - ys[i];
            vor[j] = i;
         }
   for (j = 0; j < n; j++)
      if (vor[j] != -1 && (spalte[vor[j]] == -1)) {
         zeile[j] = vor[j];
         spalte[vor[j]] = j;
      }
   for (i = 0; i < n; i++)
      if (spalte[i] == -1)
         for (j = 0, ik = n * i; j < n; j++, ik++)
            if (zeile[j] == -1 && (c[ik] == ys[i] + yt[j])) {
               spalte[i] = j;
               zeile[j] = i;
               j = n;
            }
   index=0;
   /* construction of the optimal assigment*/
   for (i = 0; i < n; i++)
      if (spalte[i] == -1) {
         for (j = 0, ik = n * i; j < n; j++, ik++) {
            vor[j] = i;
            label[j] = 1;
            dplus[j] = isup;
            dminus[j] = c[ik] - ys[i] - yt[j];
         }
         dplus[i] = 0;
         d = isup;
         while (d > 0) {
            for (j = 0; j < n; j++)
               if (label[j] == 1 && (dminus[j] < d)) {
                  d = dminus[j];
                  index = j;
               }
            if (zeile[index] < 0)
               break;
            label[index] = 0;
            k = zeile[index];
            dplus[k] = d;
            for (j = 0; j < n; j++) {
               vj = d + c[j + (k * n)] - ys[k] - yt[j];
               if (label[j] == 1 && dminus[j] > vj) {
                  dminus[j] = vj;
                  vor[j] = k;
               }
            }
            d = isup;
         }
         while (isup > 0) {
            k = vor[index];
            zeile[index] = k;
            ind = spalte[k];
            spalte[k] = index;
            if (k == i)
               break;
            index = ind;
         }
         for (j = 0; j < n; j++) {
            if (dplus[j] != isup)
               ys[j] += d - dplus[j];
            if (dminus[j] < d)
               yt[j] += dminus[j] - d;
         }
      }
   /* Compute the optimal cost*/
   for (cost = 0, i = 0; i < n; i++)
      cost += c[spalte[i] + n * i];
   for (i = 0; i < n; i++)
      for (j = 0, dplus[i] = isup; j < n; j++)
         if (j != spalte[i] && (c[j + (n * i)] < dplus[i] + ys[i] + yt[j]))
            dplus[i] = c[j + (n * i)] - ys[i] - yt[j];
   for (j = 0; j < n; j++) {
      i = zeile[j];
      if (dplus[i] != 0)
         for (k = 0; k < n; k++)
            if (k != i)
               c[j + (k * n)] += dplus[i];
   }

   return cost;
}

int mint(int n,int *t)
{ int m=*t;
  int i;
  for(i=n;i--;) if(t[i]<m) m=t[i];
  return m;
}


int QAPNode::lsapr2(int n,int *t, int *p)
{ 
  int i,j,i2,q[NMAX],nzs=0,nzsa,c[NMAX],ic,vu[NMAX],lc[NMAX],cc[NMAX];
  int mt,m;
  m=0;
/*//#define piqj printf("p[%d]=%d q[%d]=%d\n",i,p[i],j,q[j]), p[i]=j, q[j]=i*/
#define piqj p[i]=j, q[j]=i
  for(i=n;i--;) p[i]=q[i]=-1;  /*// lignes et colonnes sans zéro*/
  for(i=n;i--;)              /*// Sur une ligne sans zéro marqué d'une étoile*/
  { m+=mt=mint(n,t+(i*n));     /*// on cherche le plus petit des éléments qu'on soustrait à chacun*/
    for(j=n;j--;) if(!(t[i*n+j]-=mt) && p[i]<0 && q[j]<0) piqj, ++nzs;/*// d'eux*/
  }
  /*//for(i=n;i--;) for(j=n;j--;) if(t[i][j]<0) printf("hongrois 1\n"),affm(n,t) ,exit(1);*/
  for(j=n;j--;) if(q[j]<0)   /*// Sur une colonne sans zéro marqué d'une étoile*/
  { mt=t[j];
    for(i=n;i--;) if(t[i*n+j]<mt) mt=t[i*n+j]; /*// on cherche le plus petit des éléments*/
    m+=mt; /*// qu'on soutrait à chacun d'eux*/
    for(i=n;i--;) if(!(t[i*n+j]-=mt) && p[i]<0 && q[j]<0) piqj, ++nzs;
  }
  //std::cout<< "Cost :"<<m<<std::endl;
  /*//for(i=n;i--;) for(j=n;j--;) if(t[i][j]<0) printf("hongrois 2\n"),affm(n,t) ,exit(1);*/
  for(;n!=nzs;)
  { if(1)
    do
    { nzsa=nzs;  /*// parcours en profondeur d'abord seul. c sert de pile*/
      for(i=n;i--;) vu[i]=-1;
      for(i=n;i--;) if(vu[i]<0 && p[i]<0) for(c[ic=0]=i;;)
      { if((j=++vu[i])==n) if(ic) i=c[--ic];
                           else   break;
        else
        if(t[i*n+j]) ; else
        if((i2=q[j])<0)
        { for(;ic>=0;ic--) i=c[ic], j=vu[i],/* printf("ic=%d : ",ic),*/ piqj;
          if(++nzs==n) return m; /*// Il y a n zéros marqués d'une étoile.*/
          if(1) i=0; break;
        } else
        if(vu[i2]<0) i=c[++ic]=i2;
      }
    }
    while(nzs>nzsa); /*// tant que le nombre de zéros marqués augmente*/
    else   /*// Ford-Fulkerson complet*/
    { int g[n][n+1], *gi, d[n], jc; /*// g[i]={2,4,n} si g[i][j]==0 <==> j in{2, 4}*/
      for(i=n;i--;*gi=n) for(gi=g[i],j=n;j--;) if(!t[i*n+j]) *gi++=j; /*// recherche des zéros*/
      for(;;)
      { /*// parcours en largeur d'abord. c sert de file d'attente*/
        for(ic=jc=0,i=n;i--;) vu[i]=-1, d[i]=p[i]<0?c[ic++]=i,0:n;  /*// distance à la source*/
        for(;jc<ic;)
         for(gi=g[i=c[jc++]];j=*gi++,j<n;)
          if(i2=q[j],i2<0) goto dfs; else
          if(d[i2]==n) c[ic++]=i2, d[i2]=1+d[i];
        break; /*// Le flot est maximal. Il faut créer de nouveaux 0 pour en marquer plus*/
        dfs: /*// parcours en profondeur d'abord*/
        for(i=n;i--;) if(d[i]) ; else for(c[ic=0]=i;;) /*// différents points de départ*/
         if((j=g[i][++vu[i]])==n) if(ic) i=c[--ic];    /*// On recule si on peut.*/
                                  else   break;        /*// autre point de départ*/
         else
         if((i2=q[j])<0)     /*// On a trouvé un chemin '*'*'...'*'*/
         { for(;ic>=0;ic--) i=c[ic], j=g[i][vu[i]],/*printf("ic=%d : ",ic),*/piqj;
           if(++nzs==n) return m; /*// Il y a une étoile de plus. C'est fini s'il y en a assez.*/
           break;            /*// autre point de départ (n.b. i=c[0])   */
         } else
         if(d[i2]>d[i] && vu[i2]<0) i=c[++ic]=i2; /*// On avance.*/
      }
      for(i=n;i--;) if(d[i]<n) vu[i]=0; /*// pour être compatible avec l'autre méthode*/
    }
    for(i=n;i--;) lc[i]=cc[i]=0;  /*// ni ligne ni colonne couverte*/
    for(i=n;i--;) if(j=p[i],j>=0) {if(vu[i]<0) lc[i]=1; else cc[j]=1;} /*// on couvre des lignes et des col.*/
    for(i=0;lc[i];) ++i; /*// la ligne i n'est pas couverte*/
    for(j=0;cc[j];) ++j; /*// la colonne j n'est pas couverte*/
    mt=t[i*n+j];          /*// initialisation pour le calcul du minimum*/
    for(i=n;i--;) if(!lc[i])
    for(j=n;j--;) if(!cc[j]) if(t[i*n+j]<mt) mt=t[i*n+j];  /*// minimum des nombres non couverts*/
    m+=(n-nzs)*mt;
    for(j=n;j--;) if( cc[j]) for(i=n;i--;) t[i*n+j]+=mt;
    for(i=n;i--;) if(!lc[i]) for(j=n;j--;) if(!(t[i*n+j]-=mt) && p[i]<0 && q[j]<0) piqj, ++nzs;
  }
  //std::cout<< "End Cost :"<<m<<std::endl;
  //std::cout<< "Mat :"<<t<<std::endl;
  return m;
}


int QAPNode::tstsym(int n,const QAPInstance *dai)
{
   int dismax, a1[NMAX][NMAX];
   int             i, j, k, l, x, nfree, nf, nbsym, ph1[NMAX];
   int             clas[NMAX], clas1[NMAX], dista[NMAX][NMAX], tbcla[2][NMAX];

   nf = lvl;
   nfree = n - nf;
   for (i = 0; i < n; i++) {
      ph1[i] = tfa[i];
      for (j = 0; j < n; j++)
         dista[i][j] = 0;
   }
   for (i = nf; i < n; i++)
      for (j = nf; j < n; j++)
         a1[i - nf][j - nf] = dai->GetA(ph1[i],ph1[j]);
   for (dismax = i = 0; i < nfree; i++)
      for (j = 0; j < nfree; j++)
         if (i != j) {
            if (a1[i][j] > dismax)
               dismax = a1[i][j];
            dista[i][a1[i][j]]++;
         }
   for (i = 0; i < nfree; i++) {
      clas[i] = clas1[i] = i;
   }

   for (nbsym = i = 0; i < nfree - 1; i++)
      if (clas[i] == i)
         for (j = i + 1; j < nfree; j++)
            if (clas[j] == j) {
               l = 0;
               for (k = 1; k <= dismax; k++)
                  if (dista[i][k] != dista[j][k])
                     l = 1;
               if (l == 0) {
                  clas1[j] = clas1[i];
                  for (k = 0; k < nf; k++)
                     if (dai->GetA(ph1[nf+i],ph1[k])!=dai->GetA(ph1[nf+j],ph1[k]))
                        l = 1;
                  if (l == 0) {
                     clas[j] = clas[i];
                     nbsym++;
                  }
               }
            }
   if (nbsym != 0) {
      for (l = i = 0; i < nfree - 1; i++)
         if (clas[i] == i)
            for (j = i + 1; j < nfree; j++)
               if (clas[i] == clas[j])
                  for (k = 1; k <= dismax; k++)
                     if (dista[i][k] != 0) {
                        for (x = 0; x < nfree; x++)
                           tbcla[0][x] = tbcla[1][x] = 0;
                        for (x = 0; x < nfree; x++) {
                           if (a1[i][x] == k)
                              tbcla[0][clas1[x]]++;
                           if (a1[j][x] == k)
                              tbcla[1][clas1[x]]++;
                        }
                        for (x = 0; x < nfree; x++)
                           if (tbcla[0][x] != tbcla[1][x])
                              l = 1;
                     }
      if (l == 1)
         nbsym = 0;
   }
   if (nbsym != 0)
      for (i = 0; i < nfree; i++)
         if (clas[i] < i)
            forb[nf + i] = 1;
   return nbsym;
}

int main(int n, char ** v) {
  Bob::core::opt().add(std::string("--qap"), Bob::Property("-f", "Problem File", "Text/n12.dat"));
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<QAPTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrBBAlgoEnvProg<QAPTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::BBAlgoEnvProg<QAPTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<QAPTrait> env;
  Bob::core::Config(n, v);
#endif
  //Work=0;
  QAPInstance *Instance = new QAPInstance(Bob::core::opt().SVal("--qap", "-f"));
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

