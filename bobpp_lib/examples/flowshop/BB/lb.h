/**
 *   \file  lb.h
 *   \brief  This file group the classes/functions use to compute the Permutation 
 *   Flow shop Lower Bound
 *
 *  \author  Bertrand Le Cun (blec), Bertrand.Lecun@prism.uvsq.fr
 *
 *  \internal
 *    Created:  12.12.2007
 *   Revision:  $Id: doxygen.templates.example,v 1.4 2007/08/02 14:35:24 mehner Exp $
 *   Compiler:  gcc/g++
 *    Company:  PRiSM Laboratory University of Versailles-Saint-Quentin
 *  Copyright:  Copyright (c) 2007, Bertrand Le Cun
 *
 *  This source code is released for free distribution under the terms of the
 *  GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */


#include"pfs.h"

/** Think about  : the OneMachine and TwoMachine objects should be stored on the Instance.
 * then the function LB1 and LB2 take the pfi the per and the Scheduled
 * and compute the bound for this node.
 * At this time the construction of these two objects is made at each evaluation... 
 */

/**
 *  \class Jd 
 *  \brief class Job description that stores a value t and an identifier id.
 */
struct Jd {
   /// the value
   int t;
   /// the identifier
   int id;
   /* Pack method
    */
   void Pack(Bob::Serialize &bs) const {
         Bob::Pack(bs,&id);
         Bob::Pack(bs,&t);
   } 
   /* UnPack method
    */
   void UnPack(Bob::DeSerialize &ds) {
         Bob::UnPack(ds,&id);
         Bob::UnPack(ds,&t);
   } 
};

inline std::ostream &operator<<(std::ostream &os,const Jd &j) {
  os <<"("<<j.t<<","<<j.id<<")";
  return os;
}

/**
 *  \brief compare two Jd objects (according to the t value)
 *  \param  j1 the fist Jd object
 *  \param  j2 the second Jd object
 *  \return a boolean 
 */
bool Jd_cmpC(const Jd &j1,const Jd &j2) {
   return j1.t<j2.t;
}

/**
 *  \brief compare two Jd objects (according to the t value)
 *  \param  j1 the fist Jd object
 *  \param  j2 the second Jd object
 *  \return a boolean 
 */
bool Jd_cmpD(const Jd &j1,const Jd &j2) {
   return j1.t>j2.t;
}


/** type Jsd is an array of Jd
 */
typedef Bob::pvector<Jd> Jsd;

/** type Jssd is an array of Jsd
 * then Jssd is a two dimensional array of Jd
 */
typedef Bob::pvector< Jsd > Jssd;

/** 
 * \brief struct J_Order store a job permutation and the job lag associated to a 
 * couple of machine.
 */
struct J_Order {
   Jsd lag;
   Bob::Permutation pm;

   J_Order() : lag(),pm() {}
   J_Order(int n) : lag(n),pm(n) {}
   void Pack(Bob::Serialize &bs) const {
         Bob::Pack(bs,&lag);
         Bob::Pack(bs,&pm);
   } 
   void UnPack(Bob::DeSerialize &ds) {
         Bob::UnPack(ds,&lag);
         Bob::UnPack(ds,&pm);
   } 

};

inline std::ostream &operator<<(std::ostream &os,const J_Order &jo) {
  jo.pm.Prt(os);
  return os;
}


typedef Bob::pvector< Bob::pvector< J_Order > > l_Date;

void DispJdTab(Jsd &v,int nb) {
      for (int j=0;j<nb;j++ ) { 
            std::cout<< "("<<v[j].id<<","<<v[j].t<<")";
      }
      std::cout<<std::endl;
}

void ConsRelease(Jssd &r,const PFSInstance *pfi) {

   for (int i=0;i<pfi->nbm;i++ ) {
      r[i].resize(pfi->nbj);
      for (int j=0;j<pfi->nbj;j++ ) { r[i][j].t=0;r[i][j].id=j; }
   }
   for (int i=1;i<pfi->nbm;i++ ) {
      for (int j=0;j<pfi->nbj;j++ ) { 
         r[i][j].t=r[i-1][j].t+pfi->d[j][i-1];
      }
      //std::sort(r[i].begin(),r[i].end(),Jd_cmpC);
   }
}



void ConsDelivery(Jssd &q,const PFSInstance *pfi) {

   for (int i=0;i<pfi->nbm;i++ ) {
      q[i].resize(pfi->nbj);
      for (int j=0;j<pfi->nbj;j++ ) { q[i][j].t=0;q[i][j].id=j; }
   }
   for (int i=pfi->nbm-2;i>-1;i-- ) {
      for (int j=0;j<pfi->nbj;j++ ) { 
          q[i][j].t=q[i+1][j].t+pfi->d[j][i+1];
      }
      //std::sort(q[i].begin(),q[i].end(),Jd_cmpC);
   }
}

/*int getMin(Jsd &r) {
   int id=0;
   for (int i=0;i<r.size();i++) {
        if ( r[i].t<r[id].t ) id=i;
   }
   return id;
}*/

int getMinFree(Jsd &r,const Bob::Permutation &per) {
   for (unsigned int i=0;i<r.size();i++) {
        if ( per.isfree(r[i].id) ) return i;
   }
   return -1;
}

int getMinFreeNS(Jsd &r,const Bob::Permutation &per) {
   int min=-1;
   for (unsigned int i=0;i<r.size();i++) {
        if ( per.isfree(r[i].id) && (min==-1 || r[min].t>r[i].t )) { min=i; }
   }
   return min;
}

int getValMinFreeNS(Jsd &r,const Bob::Permutation &per) {
   int min=getMinFreeNS(r,per);
   if ( min==-1 ) { return 0; }
   return r[min].t;
}

int getpk(int m,const PFSInstance *pfi,const Bob::Permutation &per) {
   int pm=0;
   for (int j=0;j<pfi->nbj;j++ ) { 
      if ( per.isfree(j) ) {
         pm+=pfi->d[j][m];
      }
   }
   return pm;
}

/**
 *  \class One Machine is a simple lower bound for the one machine problem.
 *  \brief
 *   
 *  \par 
 */
class OneMachine {
   public:
   Jssd r,d;
   Bob::pvector<int> pk;
   int nbj;
   OneMachine() : r(),d(),pk(),nbj(0) { }
   OneMachine(const PFSInstance *pfi) : r(pfi->nbm),d(pfi->nbm),pk(pfi->nbm) {
      ConsRelease(r,pfi);
      ConsDelivery(d,pfi);
      for (int i=0;i<pfi->nbm;i++ ) {
         pk[i]=0;
      }
      for (int i=0;i<pfi->nbm;i++ ) {
         for (int j=0;j<pfi->nbj;j++ ) { 
            pk[i]+=pfi->d[j][i];
         }
      }
   }
   int doJacksonMi(int m, const PFSInstance *pfi, const Bob::Permutation &per, const Scheduled &md) {
      int t=max(r[m][getMinFreeNS(r[m],per)].t,md.getr(m));
      Bob::pvector<int> sched(pfi->nbj);
      int nbf=per.nbFree();
      int i;
      int mint;
      int maxt=0;
      Bob::Permutation jp(per);
      Jsd q(d[m]);
      
      std::sort(q.begin(),q.end(),Jd_cmpC);

      //std::cout << "Do Jackson for :"<<m<< " nbf:"<<nbf<<" t:"<<t<<std::endl;
      for (i=0;i<pfi->nbj;i++) {
         if ( per.isfree(i) ) { sched[i]=pfi->d[i][m]; std::cout << "sch["<<i<<"]="<<sched[i]<<"\n";}
      }
      //std::cout<<"r:";DispJdTab(r[m],pfi->nbj);
      //std::cout<<"q:";DispJdTab(q,pfi->nbj);
      while (nbf) {
         
         for ( mint=-1,i=pfi->nbj-1;i>=0;i-- ) {
            if ( sched[q[i].id] && t>=r[m][q[i].id].t )  { break; }
            if ( sched[q[i].id] && (mint ==-1 || r[m][q[i].id].t<r[m][q[mint].id].t ))  { mint=i; }
         }
         if ( i==-1 ) { std::cerr << "Strange\n"; 
            if ( mint==-1 ) { std::cerr << "very Strange\n";exit(1); }
            i = mint;
            t = r[m][q[mint].id].t;
         }
         jp.fixeR(q[i].id);
         t += pfi->d[q[i].id][m];
         if ( (t+q[i].t)>maxt ) { maxt=t+q[i].t;}
         sched[q[i].id]=0;
         //std::cout << "Ajout de "<<q[i].id<<" p:"<<pfi->d[q[i].id][m]<<" t:"<<t<<std::endl;
         nbf--;
      }
      t+= q[0].t;
      //std::cout<<"Simple Jackson for "<<m<<"="<<t<<" et "<<maxt<<std::endl;
      return t;
   }
   int doJackson(const PFSInstance *pfi, const Bob::Permutation &per, const Scheduled &md) {
      //std::cout <<"------- Debut Jackson One Machine evaluation---------\n";
      for (int i=0;i<pfi->nbm;i++) {
         doJacksonMi(i,pfi,per,md);
      }
      //std::cout <<"------- Fin Jackson One Machine evaluation---------\n";
      return 0;
      
   }
   int LB1(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md) {
      int v,vr,vd,mid,mir,p,maxk=0;
      AFFMESS(std::cout <<"------- Debut LB1 One Machine evaluation---------\n");
      AFFMESS(per.Prt(std::cout));
      AFFMESS(md.Prt(std::cout));
      for (int i=0;i<pfi->nbm;i++ ) {
           mir=getMinFreeNS(r[i],per);
           vr = max(r[i][mir].t,md.getr(i));
           mid=getMinFreeNS(d[i],per);
           vd = max(d[i][mid].t,md.getd(i));
           p = getpk(i,pfi,per);
           AFFMESS(std::cout<<i<<" r:"<<vr<<" p:"<<p<<" d:"<<vd<<std::endl);
           v=vr+p+vd;
           if ( v>maxk ) maxk=v;
           //std::cout << "Eval 1 for "<<i<<":"<<v<<std::endl;
      }
      AFFMESS(std::cout << " Max Eval 1: "<<maxk<<"\n");
      AFFMESS(std::cout <<"------- Fin LB1 One Machine evaluation---------\n");
      return maxk; 
   }

   void Prt(std::ostream &os) {
      os<<" release :"<<std::endl;
      for (unsigned int i=0;i<r.size();i++ ) {
         for (int j=0;j<nbj;j++ ) { 
            os << "("<<r[i][j].id<<","<<r[i][j].t<<")";
         }
         os<<std::endl;
      }
      os<<" delivery :"<<std::endl;
      for (unsigned int i=1;i<d.size();i++ ) {
         for (int j=0;j<nbj;j++ ) { 
            os << "("<<d[i][j].id<<","<<d[i][j].t<<")";
         }
         os<<std::endl;
      }
      os<< "Sum \n";
      for (unsigned int i=1;i<d.size();i++ ) {
            os << pk[i]<<std::endl;
      }
   }
   void Pack(Bob::Serialize &bs) const {
         Bob::Pack(bs,&r);
         Bob::Pack(bs,&d);
         Bob::Pack(bs,&pk);
         Bob::Pack(bs,&nbj);
   } 
   void UnPack(Bob::DeSerialize &ds) {
         Bob::UnPack(ds,&r);
         Bob::UnPack(ds,&d);
         Bob::UnPack(ds,&pk);
         Bob::UnPack(ds,&nbj);
   } 

};

class TwoMachine {
   public:
   /// Release Date r[machine][Jobs] : Jd
   Jssd r;
   /// Delevery time d[machine][Jobs] : Jd
   Jssd d;
   /// Lags between two machines
   l_Date la;
   /// Number of jobs
   int nbj;
   /// Constructor
   TwoMachine(): r(),d(),la(),nbj(0) {}
};


class TwoMachineLB5 : public TwoMachine {
   public:
   /// Constructor
   TwoMachineLB5(): TwoMachine() {}
   
   /// Compute the johnson order
   void doJohnson(const PFSInstance *pfi, int nbj) {
      std::vector<Jd> tr(nbj),td(nbj);
      int ri,di,jt;

      for (int k=0;k<pfi->nbm-1;k++ ) {
         for (int li=k+1;li<pfi->nbm;li++ ) {
            for (int j=0;j<nbj;j++ ) { 
                tr[j]=la[k][li].lag[j];
                tr[j].t+=pfi->d[j][k];
                td[j]=la[k][li].lag[j];
                td[j].t+=pfi->d[j][li];
            }
            std::sort(tr.begin(),tr.end(),Jd_cmpC);
            std::sort(td.begin(),td.end(),Jd_cmpC);
            for (ri=di=0; ri<nbj || di<nbj ;) {
               if ( ri<nbj ) {
                  jt= tr[ri].id; 
                  //std::cout <<"D On r("<<ri<<")="<<jt<<std::endl;
                  while ( ri<nbj && !la[k][li].pm.isfree(jt) ) 
                        { ri++; if ( ri<nbj) jt= tr[ri].id; }
                  //std::cout <<"F on r("<<ri<<")="<<jt<<std::endl;
               }
               if ( di<nbj ) {
                  jt= td[di].id; 
                  //std::cout <<"D On d("<<di<<")="<<jt<<std::endl;
                  while ( di<nbj && !la[k][li].pm.isfree(jt) ) 
                        { di++;if ( di<nbj) jt= td[di].id; }
                  //std::cout <<"F on d("<<di<<")="<<jt<<std::endl;
               }
               if ( ri>=nbj && di>=nbj ) break;
               if ( di>=nbj || tr[ri].t<=td[di].t ) {
                  //std::cout <<"Add r("<<ri<<")="<<tr[ri].id<<std::endl;
                  la[k][li].pm.fixeR(tr[ri].id);
                  ri++;
                  continue;
               }
               if ( ri>=nbj || tr[ri].t>=td[di].t ) {
                  //std::cout <<"Add d("<<di<<")="<<td[di].id<<std::endl;
                  la[k][li].pm.fixeD(td[di].id);
                  di++;
               }

               /*jt= tr[ri].id; 
               while ( ri<nbj && !la[k][li].pm.isfree(jt) ) { ri++;jt= tr[ri].id; }
               jt= td[di].id; 
               while ( di<nbj && !la[k][li].pm.isfree(jt) ) { di++;jt= td[di].id; }
               if ( ri>=nbj && di>=nbj ) break;
               if ( di>=nbj || tr[ri].t<=td[di].t ) {
                  la[k][li].pm.fixeR(tr[ri].id);
                  continue;
               }
               if ( ri>=nbj || tr[ri].t>=td[di].t ) {
                  la[k][li].pm.fixeD(td[di].id);
               }*/
            }
            std::cout<<"La permut ("<<k<<","<<li<<")=";la[k][li].pm.Prt(std::cout);
         }
      }
   }
   /** build internal data with the PFSInstance object
     */
   void Init(const PFSInstance *pfi){
      r.resize(pfi->nbm);d.resize(pfi->nbm);la.resize(pfi->nbm);nbj=pfi->nbj; 
      if ( nbj==0 ) {
         std::cout <<"--------------------ERGH ---------------------------\n";
         exit(1);
      }
      ConsRelease(r,pfi);
      ConsDelivery(d,pfi);

      for (int i=0;i<pfi->nbm;i++ ) {
         la[i].resize(pfi->nbm);
         for(int k=0;k<pfi->nbm;k++) { 
            la[i][k].lag.resize(nbj);
            la[i][k].pm.init(nbj);
            for (int j=0;j<nbj;j++ ) { la[i][k].lag[j].t=0;la[i][k].lag[j].id=j; }
         }
      }
      for (int k=0;k<pfi->nbm-1;k++ ) {
         for (int li=k+1;li<pfi->nbm;li++ ) {
            for (int j=0;j<pfi->nbj;j++ ) { 
               if ( li>k+1 ) la[k][li].lag[j].t=la[k][li-1].lag[j].t+pfi->d[j][li-1];
               la[k][li].lag[j].id=j;
            }
         }
      }
      doJohnson(pfi,nbj);
      /*for (int i=0;i<pfi->nbm;i++ ) {
         std::sort(r[i].begin(),r[i].end(),Jd_cmpC);
         std::sort(d[i].begin(),d[i].end(),Jd_cmpC);
      }*/
      //doJackson(pfi,nbj);
      for (int i=0;i<pfi->nbm;i++ ) {
         std::cout << "r["<<i<<"]=";
         DispJdTab(r[i],nbj);
         std::cout << "d["<<i<<"]=";
         DispJdTab(d[i],nbj);
      }
   }
   int solve(int k,int l,const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md) {
      Scheduled tm(2);
      Bob::Permutation tp(per);
      Bob::Permutation *pkl=&(la[k][l].pm);
      tm.putr(0,max(md.getr(k),getValMinFreeNS(r[k],per)));
      tm.putr(1,max(md.getr(l),getValMinFreeNS(r[l],per)));
      //std::cout<<"Initialy : on 0:"<<tm.getr(0)<<" on 1:"<<tm.getr(1)<<std::endl;
      for (int i=0;i<pfi->nbj;i++ ) {
         int j=pkl->geti(i);
         if ( tp.isfree(j) ) {
            tp.fixeR(j);
            tm.putr(0,tm.getr(0)+pfi->d[j][k]);
            tm.putr(1,max(tm.getr(1),(tm.getr(0)+la[k][l].lag[j].t))+pfi->d[j][l]);
            //std::cout<<"Add :"<<j<<" on 0:"<<tm.getr(0)<<" on 1:"<<tm.getr(1)<<std::endl;
         }
      }
      tm.putr(0,tm.getr(0)+max(md.getd(k),getValMinFreeNS(d[k],per)));
      tm.putr(1,tm.getr(1)+max(md.getd(l),getValMinFreeNS(d[l],per)));
      AFFMESS(std::cout<<"Finally: on 0:"<<tm.getr(0)<<" on 1:"<<tm.getr(1)<<std::endl);
      return max(tm.getr(0),tm.getr(1));
   }
   int LB5(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md) {
      int ckl=0;
      int maxk=0;
      AFFMESS(std::cout <<"------- Debut Two Machine evaluation---------\n");
      for (int k=0;k<pfi->nbm-1;k++ ) {
         for (int l=k+1;l<pfi->nbm;l++ ) {
            ckl=solve(k,l,pfi,per,md);
            AFFMESS(std::cout << "Solve on "<<k<<","<<l<<":"<<ckl<<std::endl);
            if ( ckl>maxk ) maxk=ckl;
         }
      }
      AFFMESS(std::cout << " Eval 5: "<<maxk<<"\n");
      AFFMESS(std::cout <<"------- Fin Two Machine evaluation---------\n");
      return maxk;
   }

   void Prt(std::ostream &os) {
      os<<" release :"<<std::endl;
      for (unsigned int i=0;i<r.size();i++ ) {
         for (int j=0;j<nbj;j++ ) { 
            os << "("<<r[i][j].id<<","<<r[i][j].t<<")";
         }
         os<<std::endl;
      }
      os<<" delivery :"<<std::endl;
      for (unsigned int i=1;i<d.size();i++ ) {
         for (int j=0;j<nbj;j++ ) { 
            os << "("<<d[i][j].id<<","<<d[i][j].t<<")";
         }
         os<<std::endl;
      }
   }
   void Pack(Bob::Serialize &bs) const {
         Bob::Pack(bs,&r);
         Bob::Pack(bs,&d);
         Bob::Pack(bs,&la);
         Bob::Pack(bs,&nbj);
   } 
   void UnPack(Bob::DeSerialize &ds) {
         Bob::UnPack(ds,&r);
         Bob::UnPack(ds,&d);
         Bob::UnPack(ds,&la);
         Bob::UnPack(ds,&nbj);
   } 

};



