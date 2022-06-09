/*
 * =====================================================================================
 *
 *       Filename:  node.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  18.09.2008 14:25:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */

#include "vrp.hpp"
#define MAXPATH 100

int Path2Col(const Path &pa,int *ind,double *val,int max) {
   int tab[MAXPATH];
   int nb=0;
   memset(tab,0,sizeof(int)*MAXPATH);
   for (unsigned int i=0;i<pa.size();i++) {
      if ( pa.get(i)>0 && pa.get(i)<max ) {
         tab[pa.get(i)-1]++;
      }
   }
   for (unsigned int i=0; i<MAXPATH; i++ ) {
      if ( tab[i]>0 ) {
         ind[nb]=i;
         val[nb]=(double) tab[i];
         nb++;
      }
   }
   return nb;
}

VrpNode::VrpNode(const TheInstance *inst) :Bob::BBLPNode<VrpTrait>(inst), FCVRP_Graph(inst->getgraph()),paths() { 
             for (int i=1;i<size()-1 ; i++ )  {
                  paths.push_back(Path(0.0,0,i,size()-1));
             }

      }


void VrpNode::eval(const TheInstance *inst, TheStat *st, double max) {
        double rd[lp->get_num_rows()];
        //double cd[lp->get_num_cols()];
        int ind[MAXPATH];
        bool fin;
        double val[MAXPATH];
        int ck=GSP_IV;
        std::vector<Path> ps;
        lp->set_obj_lower_limit(max);
        lp->solve(st, dist(), max);
        std::cout << " -------- Eval : "<<lp->get_obj_val()<<"\n";

        setEval(lp->get_obj_val());
        if ( dist()!=0 ) { 
           std::cout << "C'est la fin  not the root=================================\n";
           lp->do_test_integer();
           if (lp->isInteger()) { std::cout <<" C'est une solution\n";setSol(); }
           return;
        }
        set(inst->getgraph());
        do { 
            lp->get_row_dual(rd,0,lp->get_num_rows()-1);
            std::cout << " Row dual:";
            for (int i=0; i< lp->get_num_rows();i++) {
               std::cout << "("<<i<<","<<rd[i]<<")";
            }
            std::cout << "\n";
            compute_sp((*this),rd,ps);
            fin=true;
            for (unsigned int i=0;i<ps.size();i++ ) {
                  int nbv=Path2Col(ps[i],ind,val,inst->getgraph()->size()-1);
                  paths.push_back(ps[i]);
                  lp->add_col(nbv,ind,val,(double)(ps[i].get_cost()),0.0,1.0,0);
                  lp->set_col_kind(&ck,lp->get_num_cols()-1,lp->get_num_cols()-1);
                  std::cout << "Insertion d'une colonne\n";
                  ps[i].Prt(std::cout);
                  fin=false;
            }
            ps.clear();
            lp->solve(st, dist(), max);
            std::cout << " -------- Eval : "<<lp->get_obj_val()<<"\n";
            lp->write_lpt( "vrpp1.lp");
        } while ( !fin ) ;
        std::cout << "C'est la fin ===================================================\n";
        setEval(lp->get_obj_val());
        this->Prt(std::cout);
        lp->do_test_integer();
        if (lp->isInteger()) { std::cout <<" C'est une solution\n";setSol(); }
      }

