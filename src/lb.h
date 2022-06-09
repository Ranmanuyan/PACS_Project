/*
* @Author: Lei Liu
* @Date:   2021-05-27 15:57:26
* @Last Modified by:   Lei Liu
* @Last Modified time: 2022-06-09 17:18:05
*/


#pragma once
#include <bobpp/bobpp>
#include "pfs.h"


class OneMachine;

// low bound class
class OneMachine{
public:
   int nbj;
   // OneMachine();
   double LowerBound(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md,double x1LB, double x2UB);
   // the jobs that not assigned vector
   std::vector<int> calNPS(const PFSInstance *pfi, const Bob::Permutation &per);
   // CTMC for partial schedule
   void structurePar(std::vector<int> PS, std::vector<int> NPS, const PFSInstance *pfi, MatrixXd &Q);
   double calLBDis(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md, std::vector<int> PS,double x1LB, double x2UB);

   /// Pack method to serialize the BobNode
   virtual void Pack(Bob::Serialize &bs) const;
   /// Unpack method to deserialize the BobNode
   virtual void UnPack(Bob::DeSerialize &bs);

   virtual ~OneMachine(){};
};