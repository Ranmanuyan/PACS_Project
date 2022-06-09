
#include "vrp.hpp"
#include <stack>

#define ISNEG(a) (a<-1e-8)

struct label {
   int vi;
   long ld;
   double cost;
   int tm;
   label *prec;
   label() : vi(-1), ld(-1),cost(0.0), tm(-1),prec(0) {}
   label(int i,label* p,long l) : vi(i), ld(l),cost(0.0),tm(-1),prec(p) {}
   label(const label &l) : vi(l.vi),ld(l.ld),cost(l.cost),tm(l.tm),prec(l.prec) {}
   void Prt(std::ostream &os) {
      os << "Lab:"<<vi<<" c:"<<cost<<" l:"<<ld<<" p:"<<(prec==0?-1:prec->vi)<<"\n";
   }
};

void compute_sp(const FCVRP_Graph &cvp, double *rd, std::vector<Path> &pa) {
   std::vector< std::vector<std::vector<label *> > > st(cvp.get_gi().get_load()+1,
                std::vector<std::vector<label *> >(cvp.size(),std::vector<label*>(2,(label *)0)));
   label *blab[cvp.size()];
   for (int l=0;l<=cvp.get_gi().get_load();l++) {
      st[l][0][0] = new label(0,0,0);
      st[l][0][1] = new label(0,0,0);
   }
   for (int l=1;l<=cvp.get_gi().get_load();l++) {
      for (int i=1;i<cvp.size();i++) {
         long ldi = cvp.get(i).get_load();
         if ( l-ldi>=0 ) {
            //std::cout <<" For :"<<i<<" Load demand:"<<ldi<<"\n";
            label *lab;
            int prec0,prec1;
            for (int j=0;j<cvp.size();j++) {
               blab[j]=0;
            }
            if ( st[l-1][i][0]!=0 && st[l-1][i][0]->prec!=0 ) { 
               blab[st[l-1][i][0]->prec->vi]=st[l-1][i][0]; 
            }
            if ( st[l-1][i][1]!=0 && st[l-1][i][1]->prec!=0 ) { 
               blab[st[l-1][i][1]->prec->vi]=st[l-1][i][1]; 
            }
            for (int j=0;j<cvp.size()-1;j++) {
               if ( i!=j && st[l-ldi][j][0]!=0 ) {
                  int bp=0;
                  if ( st[l-ldi][j][0]->prec!=0 && st[l-ldi][j][0]->prec->vi==i ) {bp=1;}
                  lab = new label(i,st[l-ldi][j][bp],st[l-ldi][j][bp]->ld+ldi);
                  lab->cost=st[l-ldi][j][bp]->cost+cvp.get(j,i).get()-(i!=(cvp.size()-1)?rd[i-1]:0);
                  if ( blab[j]==0 || blab[j]->cost>lab->cost ) {
                     //std::cout <<"      on:"<<j<<" New lab:";lab->Prt(std::cout);
                     blab[j] = lab;
                  } 
               }
            }
            prec0=0;
            for (int j=0;j<cvp.size(); j ++ ) {
               if ( blab[prec0]==0 || (blab[j]!=0 && blab[j]->cost<blab[prec0]->cost) ) prec0=j;
            }
            st[l][i][0] = blab[prec0];
            if ( blab[prec0]!=0 ) {
               //std::cout << "on ("<<l<<","<<i<<",0) Best ";st[l][i][0]->Prt(std::cout);
               prec1=0;
               for (int j=0;j<cvp.size(); j ++ ) {
                  if ( j!=prec0 && (blab[prec1]==0 || (blab[j]!=0 && blab[j]->cost<blab[prec1]->cost))) 
                      prec1=j;
               }
               st[l][i][1] = blab[prec1];
               if ( blab[prec1]!=0 ) {
                  //std::cout << "on ("<<l<<","<<i<<",1) Best ";st[l][i][1]->Prt(std::cout);
               }
            }
         }
          
      }
   }
   label *oldlab=0;
   label *lab;
   for (int l=1;l<=cvp.get_gi().get_load();l++) {
      lab = st[l][cvp.size()-1][0];
      if ( lab!=0 &&  ISNEG(lab->cost) && ( oldlab==0 || lab->cost<oldlab->cost ) ) {
         oldlab=lab;
      }
   }
   if (oldlab==0 ) return ;
   lab=oldlab;
   std::stack<int> vi;
   long dist=0;
   int i = lab->vi;
   vi.push(i);
   label *lc = lab->prec;
   while (lc) {
     //std::cout <<"C("<<i<<","<<lc->vi<<")="<<cvp.get(lc->vi,i).get()<<" ct:"<<dist<<"\n";
     dist+= cvp.get(lc->vi,i).get();
     i = lc->vi;
     vi.push(i);
     lc = lc->prec;
   }
   Path p;
   p.set_cost(dist);
   while ( !vi.empty() ) {
            p.add(vi.top());
            vi.pop();
   }
   pa.push_back(p);
   //std::cout << "redCost :"<<lab->cost<<"\n";
   //p.Prt(std::cout);
   oldlab=lab;

}



