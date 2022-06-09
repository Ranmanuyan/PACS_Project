/*
* @Author: Lei Liu
* @Date:   2021-05-27 15:57:26
* @Last Modified by:   Lei Liu
* @Last Modified time: 2022-06-09 17:03:21
*/

#pragma once
#include"lb.h"
#include<algorithm>
#include <math.h>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <Eigen/unsupported/Eigen/MatrixFunctions>
#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std; 

// the jobs that not assigned vector
std::vector<int> OneMachine::calNPS(const PFSInstance *pfi, const Bob::Permutation &per){
   std::vector<int>NPS;
   for(int i=0;i<pfi->nbj;i++) {
      if(per.isfree(i))
        NPS.push_back(i);
    }
    return NPS;
}

// CTMC for partial schedule
void OneMachine::structurePar(std::vector<int> PS, std::vector<int> NPS, const PFSInstance *pfi, MatrixXd &Q){
    int n= PS.size();
    int states = (pow(n,2)+3*n+2)/2 + NPS.size();

    MatrixXd Q1 = MatrixXd::Zero(states, states); 
    // CTMC row_num
    // get the states number for each block
    if(n>1){
      std::vector<std::vector<int> > stateNumVec = Scheduled::getstateNum(n);
      // get the matrix for each block
      for (int k = 0; k < n-1; ++k){
        int count = stateNumVec[k].size()-1;

        Q1(stateNumVec[k][0], stateNumVec[k][1]) = pfi->d_b[PS[k]][1];
        Q1(stateNumVec[k][0], stateNumVec[k][2]) = pfi->d_b[PS[k+1]][0];
        Q1(stateNumVec[k][1], stateNumVec[k+1][0]) = pfi->d_b[PS[k+1]][0];
        Q1(stateNumVec[k][2], stateNumVec[k+1][0]) = pfi->d_b[PS[k]][1];

        if(stateNumVec[k].size() > 3){
          for (int i = 2; i < stateNumVec[k].size()-1; ++i){
            Q1(stateNumVec[k][i], stateNumVec[k][i+1]) = pfi->d_b[PS[n-count+i]][0]; // need a enumerate of jobs until last one
            Q1(stateNumVec[k][i+1], stateNumVec[k+1][i]) = pfi->d_b[PS[k]][1];
          }
        }
      }
    }
    Q1(0, 1) = pfi->d_b[PS[0]][0];
    // last index for PS
    int PSlast = (pow(n,2)+3*n-2)/2;
    
    Q1(PSlast,PSlast+1) = pfi->d_b[PS[n-1]][1];

    int count =0;
    for (int i = PSlast+1; i < states-1; ++i){
      Q1(i,i+1) = pfi->d_b[NPS[count]][1];
      count++;
    }
    
    for (int i = 0; i < states; ++i){
        for (int j = i+1; j < states; ++j){
            Q1(i,i) += (-1 * Q1(i,j));
        }
    }
    Q = Q1.topLeftCorner(states-1,states-1);;
}


double OneMachine::calLBDis(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md, std::vector<int> PS, double x1LB, double x2UB){
  std::vector<int> NPS = calNPS(pfi, per);
  double alpha = 0.9;
  MatrixXd Q1;
  MatrixXd &Q = Q1;


  structurePar(PS, NPS, pfi, Q);
  
  VectorXd a(Q.rows());
  for (int i = 1; i < Q.rows(); ++i){
        a[i]=0;
    }
  a[0] = 1.0;
  
  double VaR_value = Scheduled::bisection(Q,  a,  alpha, x1LB, x2UB);
  NPS.resize(0);

  return VaR_value;
}


double OneMachine::LowerBound(const PFSInstance *pfi,const Bob::Permutation &per,Scheduled &md,double x1LB, double x2UB) {
  std::vector<int> j2i =  per.get_j2i();
  std::vector<int> PS;
  for(int i=0; i<j2i.size(); i++){
    if(j2i[i] != -1){
      PS.push_back(j2i[i]);
    }
  }
  double lowerBound;

  if(PS.empty()){
    lowerBound = 0;
  }else{
    lowerBound = calLBDis(pfi, per, md, PS, x1LB, x2UB);
  }

  std::vector <int>().swap(PS);   // clear the vevctor

  return lowerBound;
}

void OneMachine::Pack(Bob::Serialize &bs) const {
     Bob::Pack(bs,&nbj);
} 
void OneMachine::UnPack(Bob::DeSerialize &ds) {
     Bob::UnPack(ds,&nbj);
}