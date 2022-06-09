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
 *  File   : pfs.cpp
 *  Author : B. Le Cun based on source of the ETSI Plugtest.
 *  Date   : Nov 11 2007.
 *  Comment: Source file to solve the permutation flowshop using the Bob++ library
 */

#include"pfs.h"



void PFSInstance::read() {
   std::ifstream fr(file.c_str());

   if ( !fr.is_open() ) {
      std::cerr<< "Could not open file "<<file.c_str()<<" : exit\n";
      exit(1);
   }

	try {
		fr >> nbj;
		fr >> nbm;
				
		if ( nbj != 0 ) {
			jc= new unsigned long[ nbj ];
			d = new unsigned long*[ nbj ];
		}
		
		unsigned long ji;
		for ( int i = 0; i< nbj ; i++ ) {
			fr >> ji;
			fr >> jc[ i];
			d[i]= new unsigned long[ nbm ];
			
			for ( int j= 0; j < nbm ; j++ )
				fr >> d[ i][ j];
		}
	}
	catch( ... ) {
		cout << "Error while loading data from stream..." << endl;
	}

   fr.close();

}

#define max(a,b) ((a)<(b)?(b):(a))

void MachineDate::Compute(const PFSInstance &pfi, int j) {
   put(0,get(0)+pfi.get(0,j));
   for (unsigned int i=1;i<size();i++ ) {
      put(i,max(get(i),get(i-1))+pfi.get(i,j));
   }
}
PFSNode::PFSNode(const PFSInstance *pi):Bob::CDCIntMinNode(0),nbj(pi->nbj),nbm(pi->nbm),per(nbj),md(nbm) { 
   for (int j=0;j<nbj;j++ ) {
      Fixe(j,*pi);
      dist()++;
   }
}

void PFSNode::Fixe(int j,const PFSInstance &pfi) {
   per.fixe(j,dist());
   md.Compute(pfi,j);
   setCost(md.getCost());
}

bool PFSGenChild::operator()(PFSNode *p) {
   bool fl = false;
   for (int j=0;j<inst->nbj;j++ ) {
      if ( p->perm().isfree(j) )  {
         PFSNode *nf; 
         int ir = p->dist()+1-Bob::core::get_depth_rec();
         if ( ir>=0 ) {
            //std::cout<<pthread_self()<<"   Rec node "<<p->dist()+1<<":"<<ir<<"\n";
            nf = get_rec_node(ir);
            if ( nf==0 ) {
               nf = new PFSNode(p); 
               declare_as_rec(nf,ir);
               //std::cout<<pthread_self()<<"  Declare node:"<<ir<<"\n";
            }  
            nf->copy(p);
            nf->addRef();
         } else {
            nf = new PFSNode(p); 
         }

         nf->Fixe(j,*inst);
         if ( nf->perm().isallfixed() ) {
            nf->setSol();
            //std::cout << "Solution :"<<nf->getCost()<<std::endl;
            //nf->Prt(std::cout);
         }
         nf->dist()++;
         fl+=Search(nf);
      } 
   }
   return fl;
}




int main(int n, char ** v) {
  Bob::core::opt().add(std::string("--pfs"), Bob::Property("-f", "Problem File", "../taillard/ta_20_5_01.txt"));
#ifdef Atha
  Bob::AthaCDCAlgoEnvProg<PFSTrait> env;
  Bob::AthaEnvProg::init(n, v);
  Bob::core::Config(n, v);
#elif defined(Threaded)
  Bob::ThrCDCAlgoEnvProg<PFSTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#elif defined(MPxMPI)
  Bob::mpx::EnvProg::init(n, v);
  Bob::mpx::CDCAlgoEnvProg<PFSTrait,Bob::mpx::lb_strat_2t,Bob::mpx::topo_a2a,long> env;
  Bob::core::Config(n, v);
  Bob::mpx::EnvProg::start();
#else
  Bob::SeqCDCAlgoEnvProg<PFSTrait> env;
  Bob::core::Config(n, v);
#endif

  PFSInstance *Instance = new PFSInstance(Bob::core::opt().SVal("--pfs", "-f"));
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
  exit(0);
}

