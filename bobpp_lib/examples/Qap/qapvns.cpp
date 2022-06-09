
#include<limits>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <bitset>
#include <bobpp/bobpp>

#ifdef Threaded
#  include <bobpp/thr/thr.h>
#elif defined(Atha)
#  include <bobpp/atha/atha.h>
#elif defined(MPxMPI)
#  include <bobpp/mpx/mpx.hpp>
#endif

#define NMAX 30

class APVNSInstance;
class APVNSNode;
class APVNSGenChild;
//class APVNSStat;

class APVNSTrait {
public:
  typedef APVNSNode Node;
  typedef APVNSInstance Instance;
  typedef Bob::VNSAlgo<APVNSTrait> Algo;
  typedef APVNSGenChild GenChild;
  //typedef Bob::BBGoalBest<APVNSTrait> Goal;
  typedef Bob::VNSGoalBest<APVNSTrait> Goal;
  //typedef Bob::BestDPri<APVNSNode> PriComp;
  typedef Bob::BreathPri<APVNSNode> PriComp;
  typedef Bob::VNSStat Stat;
};

struct SumCout {
   int nb;int cst;
   SumCout() : nb(0),cst(0) { }
   void add(int c) { nb++; cst+=c;}
   int val() { return (nb==0 ? 0 : cst/nb);}
   virtual ostream &Prt(std::ostream& os=std::cout) const {
      os << "("<<nb<<":"<<cst<<")";
      return os;
   }
   virtual void Pack(Bob::Serialize &bs)  const {Bob::Pack(bs,&nb,1);Bob::Pack(bs,&cst,1);}
   virtual void UnPack(Bob::DeSerialize &bs)  {Bob::UnPack(bs,&nb,1);Bob::UnPack(bs,&cst,1);}

};
std::ostream &operator<<(std::ostream &os,const SumCout &sc) {
   return sc.Prt(os);
}

//Bob::pvector<Bob::pvector<SumCout> > statval;

/** The Option structure
  */
struct APVNSOption {
  static std::string filename;
  static int max_step;
  static int nbwalk;
  static void option();
};

class APVNSNode : public Bob::VNSIntMinNode {
  public:
    Bob::Permutation p;
    Bob::pvector<int> ci;
    Bob::pvector<int> cj;
    APVNSNode() : Bob::VNSIntMinNode(),p(),ci(),cj() { }
    APVNSNode(const APVNSInstance &hi,int i=0);
    APVNSNode(const APVNSInstance &inst,Bob::Permutation &_p);
    virtual ostream &Prt(ostream &os=std::cout) const {
      Bob::VNSIntMinNode::Prt(os);
      p.Prt(os);
      return os;
    }

};

class APVNSInstance : public Bob::VNSInstance<APVNSTrait> {
  public:
  /// the 2 matrix (flow and distance).
  int a[NMAX][NMAX], b[NMAX][NMAX];
  /// the Size of the problem.
  int Size;

   /// Constructor
   APVNSInstance(int nbw=1) : Bob::VNSInstance<APVNSTrait>("qap",nbw),Size(0) {
       lectu();
   }
   virtual int size() const { return Size; }
   virtual void InitAlgo(APVNSTrait::Algo *al) const;
   virtual void Init();
   virtual APVNSNode *getSol();
   virtual void lectu();
};

class APVNSGenChild : public Bob::VNSGenChild<APVNSTrait> {
  public:
    APVNSGenChild(const APVNSInstance *_hi, Bob::VNSAlgo<APVNSTrait> *al);
    //virtual bool operator()(APVNSNode *g);
};

std::string APVNSOption::filename;
int APVNSOption::max_step;
int APVNSOption::nbwalk;


void APVNSOption::option() {
  std::string grp = "--qap";
  Bob::core::opt().add(grp, Bob::Property("-f", "file name of the instance", "Text/n08.dat", &APVNSOption::filename));
  Bob::core::opt().add(grp, Bob::Property("-s", "the maximum steps of the algorithm", 10, &APVNSOption::max_step));
  Bob::core::opt().add(grp, Bob::Property("-w", "the number of walks", 1, &APVNSOption::nbwalk));
};


/*--------------------- Method of the node ---------------------*/
APVNSNode::APVNSNode(const APVNSInstance &inst,int i) : Bob::VNSIntMinNode(),p(inst.size()),ci(inst.size()),cj(inst.size()) {
  srand(i);
  std::vector<bool> f(inst.size(),false);
  int c=0;
  for (int i=0;i<inst.size();i++ ) {
    int j = rand()%(inst.size()-i);
    int k;
    std::cout<<"j="<<j<<std::endl;
    for ( k=0;k<inst.size() && j>0;k++ ) if ( !f[k] ) j--;
    if ( f[k] ) {
      for (; k<inst.size() && f[k] ;k++);
      if ( f[k] ) {
        std::cerr << "Gloups Error:"<<k<<":"<<j<<"\n";
        exit(1);
      }
    }
    f[k]=true;
    std::cout << "Fixe "<<i<<" sur "<<k<<std::endl;
    p.fixe(i,k);
  }
  for (int i=0;i<inst.size();i++ ) {
    ci[i]=0;
    for (int j=0;j<inst.size();j++ ) {
      ci[i]+=inst.a[i][j]*inst.b[p.getj(i)][p.getj(j)];
    }
    c+=ci[i];
  }
  setCost(c);
  set_ls(1);
}
APVNSNode::APVNSNode(const APVNSInstance &inst,Bob::Permutation &_p) : Bob::VNSIntMinNode(),p(_p),ci(inst.size()),cj(inst.size()) {
  int c=0;
  for (int i=0;i<inst.size();i++ ) {
     ci[i]=0;
     for (int j=0;j<inst.size();j++ ) {
         int k = p.getj(i);
         int l = p.getj(j);
         ci[i]+=inst.a[i][j]*inst.b[k][l];
         cj[k]+=inst.a[i][j]*inst.b[k][l];
     }
     c+=ci[i];
  }
  setCost(c);
}


/*--------------------- Method of the instance ---------------------*/

void APVNSInstance::InitAlgo(APVNSTrait::Algo *al) const {
  for (int i=0;i<this->get_nbwalk();i++ ) {
    APVNSNode *n=new APVNSNode(*this,i);
    std::cout <<"Insertion solution racine walk:"<<i<<"\n";
    n->set_walk(i);
    al->GPQIns(n);
  }
}

void APVNSInstance::Init() {
}


APVNSNode *APVNSInstance::getSol() {
  std::cout <<"oAppel instance::getSol\n";
  return 0;
}

void APVNSInstance::lectu()
{
   FILE *f;
   int  i, j;
   int cost;
   int test_return;

   /* lecture des matrices, de la taille et de bks */
   if ((f = fopen(APVNSOption::filename.c_str(), "r")) == NULL) {
       fprintf(stderr,"Erreur ouverture de fichier %s\n",APVNSOption::filename.c_str());
       exit(1);
   }
   test_return = fscanf(f, "%u\n", &Size);
   if (test_return != 1) {
       fprintf(stderr,"Erreur de lecture dans le fichier %s\n",APVNSOption::filename.c_str());
       exit(1);
   }
   cout << "Taille du probleme "<< Size <<"\n";
   for (i = 0; i < Size ; i++) {
      for (j = 0; j < Size ; j++) {
         test_return = fscanf(f, "%d", &cost);
		   if (test_return != 1) {
				fprintf(stderr,"Erreur de lecture dans le fichier %s\n",APVNSOption::filename.c_str());
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
				fprintf(stderr,"Erreur de lecture dans le fichier %s\n",APVNSOption::filename.c_str());
				exit(1);
		   }
         b[i][j] = cost;
         printf("%2d ",b[i][j]);
      }
      test_return = fscanf(f, "\n");
      printf("\n");
   }
   fclose(f);
}

/*template<class Trait>
class Move {
 public:
  /// Type of a Node
  typedef typename Trait::Node TheNode;
  /// Type of the Info
  typedef typename Trait::Instance TheInstance;
  /// Type for the goal of the search
  typedef typename Trait::Goal TheGoal;
  /// Type of the Algo
  typedef typename Trait::Algo TheAlgo;

  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool one_rand=false)=0;
};*/

class Move2Opt : public Bob::Move<APVNSTrait> {
 public:
  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool one_rand=false) {
   bool fl=false;
   int imax=0;
   for (int i=0;i<inst->size();i++ ) {
       if ( g->ci[i]>g->ci[imax] ) imax = i;
   }
   std::cout <<"imax :"<<imax<<std::endl;
   int jx = g->p.getj(imax);
   for (int i=0;i<inst->size();i++ ) {
     if ( i!=imax) {
       Bob::Permutation np(g->p);
       int jt = np.getj(i);
       np.fixe(imax,jt);
       np.fixe(i,jx);
       APVNSNode *f=new APVNSNode(*inst,np);
       algo->getStat()->start_create(f,g);
       f->set(g);
       f->dist()= g->dist()+1;
       algo->getStat()->end_create(f);
       if ( algo->Search(f) ) {
          std::cout <<"Generate for i :"<<i<<" exchange :"<<jt<<","<<jx<<std::endl;
          f->Prt();
          fl=true;
       }
     }
   }
   int jmax=0;
   for (int i=0;i<inst->size();i++ ) {
       if ( g->cj[i]>g->cj[jmax] ) jmax = i;
   }
   std::cout <<"jmax :"<<jmax<<std::endl;
   int ix = g->p.geti(jmax);
   if (ix == imax ) return fl;
   for (int j=0;j<inst->size();j++ ) {
     if ( j!=jmax) {
       Bob::Permutation np(g->p);
       int it = np.geti(j);
       if ( it==imax ) continue;
       np.fixe(it,jmax);
       np.fixe(ix,j);
       APVNSNode *f=new APVNSNode(*inst,np);
       algo->getStat()->start_create(f,g);
       f->set(g);
       f->dist()= g->dist()+1;
       algo->getStat()->end_create(f);
       if ( algo->Search(f) ) {
          std::cout <<"Generate for j :"<<j<<" exchange :"<<it<<","<<ix<<std::endl;
          f->Prt();
          fl=true;
       }
     }
   }

   return fl;
  }
};

class MoveR2Opt : public Bob::Move<APVNSTrait> {
 public:
  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool one_rand=true) {
    int i1=rand()%inst->size();
    int j1 = g->p.getj(i1);
    int i2,j2;
    do {
      i2 = rand()%inst->size();
      j2 = g->p.getj(i2);
    } while ( i1==i2 );
    Bob::Permutation np(g->p);
    np.fixe(i1,j2);
    np.fixe(i2,j1);
    APVNSNode *f=new APVNSNode(*inst,np);
    algo->getStat()->start_create(f,g);
    f->dist()= g->dist()+1;
    f->set(g);
    algo->getStat()->end_create(f);
    if ( algo->Search(f) ) {
      f->set_ls(true);
      f->Prt();
      return true;
    }
    delete f;
    return false;
  }
};

class Move3Opt : public Bob::Move<APVNSTrait> {
 public:
  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool one_rand=true) {
    int i1=rand()%inst->size();
    int j1 = g->p.getj(i1);
    int i2,j2;
    int i3,j3;
    do {
      i2 = rand()%inst->size();
    } while ( i1==i2 );
    j2 = g->p.getj(i2);
    do {
      i3 = rand()%inst->size();
    } while ( i3==i1 || i3==i2 );
    j3 = g->p.getj(i3);

    Bob::Permutation np(g->p);
    np.fixe(i1,j2);
    np.fixe(i2,j3);
    np.fixe(i3,j1);
    APVNSNode *f=new APVNSNode(*inst,np);
    algo->getStat()->start_create(f,g);
    f->dist()= g->dist()+1;
    f->set(g);
    algo->getStat()->end_create(f);
    if ( algo->Search(f) ) {
      f->set_ls(true);
      f->Prt();
      return true;
    }
    delete f;
    return false;

  }
};
class Move4Opt : public Bob::Move<APVNSTrait> {
 public:
  virtual bool operator()(TheNode *g, const TheInstance *inst, TheAlgo *algo,bool one_rand=true) {
    int i1=rand()%inst->size();
    int j1 = g->p.getj(i1);
    int i2,j2;
    int i3,j3;
    int i4,j4;
    do {
      i2 = rand()%inst->size();
    } while ( i1==i2 );
    j2 = g->p.getj(i2);
    do {
      i3 = rand()%inst->size();
    } while ( i3==i1 || i3==i2 );
    j3 = g->p.getj(i3);
    do {
      i4 = rand()%inst->size();
    } while ( i4==i1 || i4==i2 || i4==i3);
    j4 = g->p.getj(i4);

    Bob::Permutation np(g->p);
    np.fixe(i1,j2);
    np.fixe(i2,j3);
    np.fixe(i3,j4);
    np.fixe(i4,j1);
    APVNSNode *f=new APVNSNode(*inst,np);
    algo->getStat()->start_create(f,g);
    f->dist()= g->dist()+1;
    f->set(g);
    algo->getStat()->end_create(f);
    if ( algo->Search(f) ) {
      f->set_ls(true);
      f->Prt();
      return true;
    }
    delete f;
    return false;

  }
};

APVNSGenChild::APVNSGenChild(const APVNSInstance *_hi, Bob::VNSAlgo<APVNSTrait> *al) : Bob::VNSGenChild<APVNSTrait>(_hi,al) { 
  set_ls(new Move2Opt());
  add_move(new MoveR2Opt());
  add_move(new Move3Opt());
  add_move(new Move4Opt());
}

/*--------------------- The Genchild operator ---------------------*/
/*bool APVNSGenChild::operator()(APVNSNode *g) {
   Move2Opt mo2o;
   MoveR2Opt mor2o;
   Move4Opt mo4o;

   std::cout << "--------------- Genchild ------------\n";
   std::cout << "Parent step:"<<g->getstep()<<" dist:"<<g->dist()<< " cost:"<<g->getCost()<<std::endl;
   if ( g->getstep()>APVNSOption::max_step ) return false;
   if ( mo2o(g,inst,algo) && g->dist()>=1 ) return true;
   g->dist()=0;
   g->setk(1);
   g->setstep(g->getstep()+1);
   std::cout << "--------------- Try 2 Neighbourhood Rand------------\n";
   if ( mor2o(g,inst,algo) && g->dist()>=1 ) return true;
   g->dist()=0;
   g->setk(2);
   std::cout << "--------------- Try 2 Neighbourhood------------\n";
   if ( mo4o(g,inst,algo) ) return true;
   std::cout << "--------------- ***** ------------\n";
   return false;
}*/

/*--------------------- The Main method ---------------------*/
int main(int n, char **v) {
  APVNSOption::option();
#ifdef Atha
  Bob::AthaBBAlgoEnvProg<APVNSTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrVNSAlgoEnvProg<APVNSTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::MPEnvProg::init(n, v);
  Bob::MPBBAlgoEnvProg<APVNSTrait> env;
  Bob::core::Config(n, v);
#else
  Bob::SeqVNSAlgoEnvProg<APVNSTrait> env;
  Bob::core::Config(n, v);
#endif

  //srand(getpid());
  APVNSInstance *Instance = new APVNSInstance(APVNSOption::nbwalk);

  //statval.resize(Instance->size(),Bob::pvector<SumCout>(Instance->size()));
  env(Instance);

#ifdef Atha
  Bob::AthaEnvProg::end();
#elif defined(Threaded)
  Bob::ThrEnvProg::end();
#elif defined(MPxMPI)
  Bob::MPEnvProg::end();
#endif
  delete Instance;
  Bob::core::End();
  return 0;
}


