/*
* @Author: Lei Liu
* @Date:   2021-05-27 15:57:26
* @Last Modified by:   Lei Liu
* @Last Modified time: 2022-06-09 17:15:16
*/



#pragma once
#include <bobpp/bobpp>
#include "pfs.h"
#include "lb.h"
#include <time.h>
#include <stdlib.h>
#include <fstream> 
#include <iostream>
#include <string>
#include <boost/math/tools/tuple.hpp>
#include <boost/math/tools/roots.hpp>
using boost::math::policies::policy;
using boost::math::tools::newton_raphson_iterate;
using boost::math::tools::halley_iterate;
using boost::math::tools::eps_tolerance; // Binary functor for specified number of bits.
using boost::math::tools::bracket_and_solve_root;
using boost::math::tools::toms748_solve;
using namespace boost::math;

// Class PFSInstance

void PFSInstance::Init() {
   read();
}

PFSNode *PFSInstance::getSol() {
   PFSNode *sol=0;
   PFSNode *node;

   Bob::Permutation *per = new Bob::Permutation(); 
   per->init(nbj);
   
   for(int i=0; i<nbj; i++){
    per->fixe(i,i);
   }

   node = new PFSNode(this,*per);
   if ( sol==0 || sol->getEval()>node->getEval() ) {
    if ( sol!=0 ) delete sol; 
    sol=node;
    } else {
      delete node;
    }
   return sol;
}

void PFSInstance::read() {
  std::ifstream fr(file.c_str());
  if ( !fr.is_open() ) {
    std::cerr<< "Could not open file "<<file.c_str()<<" : exit\n";
    exit(1);
  }

  std::string feature;  
  int count =0;
  while(!fr.eof()){
    getline(fr, feature);
    if(count == line){
      stringstream stringin(feature); 
      try {
        stringin >> nbj;
        stringin >> nbm;
        stringin >> biLB;
        stringin >> biUB;
        if ( nbj != 0 ) {
          d_b = new double *[ nbj ];
        }
        
        for ( int i = 0; i< nbj ; i++ ) {
          d_b[i]= new double[ nbm ];
          for ( int j= 0; j < nbm ; j++ ){
             stringin >> d_b[i][j];
         }
        }          
       }
       catch( ... ) {
         cout << "Error while loading data from stream..." << endl;
       }
      break;
    }else{
      count++;
    }
  }
  fr.close();
}

void PFSInstance::copy(const PFSInstance &pfi) {
   Bob::BBInstance<PFSTrait>::copy(pfi);
   file = pfi.file;
   lobd = pfi.lobd;
   line = pfi.line;
   nbj=pfi.nbj;nbm=pfi.nbm;
   d_b = new double *[nbj];
   for ( int i = 0; i< nbj ; i++ ) {
     d_b[i]= new double[ nbm ];
     for ( int j= 0; j < nbm ; j++ ){
      d_b[i][j] = pfi.d_b[i][j];
     }  
   }
}

/// Pack method to serialize the BobNode
void PFSInstance::Pack(Bob::Serialize &bs) const  {
       // DBGAFF_ENV("PFSInstance::Pack","Pack an instance");
	    Bob::BBInstance<PFSTrait>::Pack(bs);
       Bob::Pack(bs,&file);
       Bob::Pack(bs,&line);
       Bob::Pack(bs,&lobd);
       Bob::Pack(bs,&nbj);
       Bob::Pack(bs,&nbm);
       for (int i=0;i<nbj;i++ ) {
            Bob::Pack(bs,d_b[i],nbm);
       }
       Bob::Pack(bs,lb1m);
       // DBGAFF_ENV("PFSInstance::Pack","------- Finish");
}
/// Unpack method to deserialize the BobNode
void PFSInstance::UnPack(Bob::DeSerialize &bs)  {
      // DBGAFF_ENV("PFSInstance::UnPack","UnPack an instance");
	    Bob::BBInstance<PFSTrait>::UnPack(bs);
       Bob::UnPack(bs,&file);
       Bob::UnPack(bs,&line);
       Bob::UnPack(bs,&lobd);
       Bob::UnPack(bs,&nbj);
       Bob::UnPack(bs,&nbm);
      d_b = new double *[ nbj ];
       for (int i=0;i<nbj;i++ ) {
            d_b[i]=new double[ nbm ];
            Bob::UnPack(bs,d_b[i],nbm);
       }
       lb1m=new  OneMachine();
       Bob::UnPack(bs,lb1m);
       // DBGAFF_ENV("PFSInstance::UnPack","------- Finish");
}






/*
*
* Class Scheduled
*
*/

// Full schedule objective function
double Scheduled::getCost(const PFSInstance &pfi, const Bob::Permutation &per, double x1LB){
  // the permutation list, if not assigned, use 0
  std::vector<int> j2i =  per.get_j2i();
  // PS is the vector only assigned jobs, here is the full schedule
  std::vector<int> PS = std::vector<int>(j2i.begin(), j2i.begin()+per.size()-per.nbFree());

  const PFSInstance *pi = &pfi;
  double objValue = calObjDiscrete(PS, pi, per, x1LB, pi->biUB);             // int totalTardiness = calTardiness(PS, pi);
  std::cout << "objValue: " << objValue <<std::endl;

  j2i.clear();
  std::vector <int>().swap(PS);   // clear the vevctor

  return objValue;
}


void Scheduled::structureCon(std::vector<int> PS, const PFSInstance *pfi, MatrixXd &Qstar){
  int n = PS.size();
  int states = (pow(n,2)+3*n+2)/2;
   MatrixXd Qstar1 = MatrixXd::Zero(states, states);
    // get the states number for each block
    std::vector<std::vector<int> > stateNumVec = getstateNum(n);
    
    // get the matrix for each block
    for (int k = 0; k < n-1; ++k){
      int count = stateNumVec[k].size()-1;

      Qstar1(stateNumVec[k][0], stateNumVec[k][1]) = pfi->d_b[PS[k]][1];
      Qstar1(stateNumVec[k][0], stateNumVec[k][2]) = pfi->d_b[PS[k+1]][0];
      Qstar1(stateNumVec[k][1], stateNumVec[k+1][0]) = pfi->d_b[PS[k+1]][0];
      Qstar1(stateNumVec[k][2], stateNumVec[k+1][0]) = pfi->d_b[PS[k]][1];

      if(stateNumVec[k].size() > 3){
        for (int i = 2; i < stateNumVec[k].size()-1; ++i){
          Qstar1(stateNumVec[k][i], stateNumVec[k][i+1]) = pfi->d_b[PS[n-count+i]][0]; // need a enumerate of jobs until last one
          Qstar1(stateNumVec[k][i+1], stateNumVec[k+1][i]) = pfi->d_b[PS[k]][1];
        }
      }
    }
    Qstar1(states-2, states-1)= pfi->d_b[PS[n-1]][1];
    Qstar1(0, 1) = pfi->d_b[PS[0]][0];
    
    
    for (int i = 0; i < states; ++i){
        for (int j = i+1; j < states; ++j){
            Qstar1(i,i) += (-1 * Qstar1(i,j));
        }
    }
    Qstar = Qstar1.topLeftCorner(states-1,states-1);
}

double Scheduled::calObjDiscrete(std::vector<int> PS, const PFSInstance *pfi, const Bob::Permutation &per, double x1LB, double x2UB){
  double alpha =0.9;
  MatrixXd Qstar1;
  MatrixXd &Qstar = Qstar1;
  structureCon(PS, pfi, Qstar);
  VectorXd a(Qstar.rows());
  for (int i = 1; i < Qstar.rows(); ++i){
        a[i]=0;
  }
  a[0] = 1.0;
  double VaR_value = bisection( Qstar,  a,  alpha, x1LB, x2UB);
  
  return VaR_value;
}

std::vector<std::vector<int> > Scheduled::getstateNum(int n){
    // get the state numbers for blocks
  // if >1
  std::vector<std::vector<int> >stateNum;
  
  if(n>2){
    std::vector<int>stateNumVec(n-1, 0);
    for (int k = 1; k < n-1; ++k){
      // n-2-(k-1), (k-1) block adds number of states
      int add =0;
        if(n-k-1>0){
            add = n-k-1;
        }
      stateNumVec[0]=1;
      stateNumVec[k]=stateNumVec[k-1]+add+3;  // get the start index number of this block
    }

    for (int i = 0; i < n-1; ++i){
      std::vector<int>states;
      if(i<n-2){
        for (int j = stateNumVec[i]; j < stateNumVec[i+1]; ++j){
          states.push_back(j);
        }
      }else{
        for (int j = stateNumVec[i]; j < stateNumVec[i]+3; ++j){
          states.push_back(j);
        }
      }
      stateNum.push_back(states);
      states.resize(0);
    }
    std::vector<int>states2;
    states2.push_back(stateNum[n-2].back()+1);
    stateNum.push_back(states2);
    states2.resize(0);
    stateNumVec.resize(0);
  }
  else{
    // if n==2, the states block [[1,2,3],[4]]
    std::vector<int>states1;
    states1.push_back(1);
    states1.push_back(2);
    states1.push_back(3);
    stateNum.push_back(states1);
    std::vector<int>states3;
    states3.push_back(4);
    stateNum.push_back(states3);

    states1.resize(0);
    states3.resize(0);
  }

  return stateNum;
}


// ==============================================================================
// Calculate VaR from cdf, several alternative approaches
// ==============================================================================
#define EP 1  
double Scheduled::bisection(MatrixXd Qstar,  VectorXd a,double VaRalpha, double x1, double x2) {
    double const t = x1;
    double k1 = cdfCal(Qstar, a, x1)-VaRalpha;
    double k2 = cdfCal(Qstar, a, x2)-VaRalpha;
    // int count = 2;
    // std::cout << Qstar << std::endl;

    if ((k1 * k2 <= 0) && (x2-t >=EP)) {
        double c = x1;
        while ((x2-x1) >= EP) {
            // count++;
            c = (x1+x2)/2;
            double k3 = cdfCal(Qstar, a, c)-VaRalpha;
            if(abs(k3) <= 1e-3){
                x2 = c;
                break;
            }else{
                if (k3* k1 <= 0){
                    x2 = c;
                    k2 =k3;
                }
                else{
                    x1 = c;
                    k1 =k3;
                }    
            }
        }
    }
    // if (x2<= t){
    //     x2= t;
    // }
    // std::cout<< count <<std::endl;
    
    return x2;
}

double f(MatrixXd A,  VectorXd v, double x, double value) {
    return (Scheduled::cdfCal(A, v, x) - value);
}

class tolerance {
public:
    tolerance(double eps) :
        _eps(eps) {
    }
    bool operator()(double a, double b) {
        return (fabs(b - a) <= _eps);
    }
private:
    double _eps;
};


double Scheduled::boost_Bisect(MatrixXd Qstar,  VectorXd a,double x1lb,  double x2ub, double VaRalpha){
    double root = x2ub;
    const boost::uintmax_t maxit2 = 50;
    boost::uintmax_t it2 = maxit2;
    tolerance tol = 1;
    // catch error if root not in this interval
    try{
        std::pair<double, double> found1 = boost::math::tools::bisect(
        [Qstar, a, VaRalpha](double x){return f(Qstar,a, x, VaRalpha);}, 
        x1lb, 
        x2ub, 
        tol,
        it2);

        root = found1.second;
    }catch(...){
        root = x2ub;
        it2 = 2;
    }
    if (root < x1lb+2){
        root= x1lb+2;
    }
    std::cout << it2 << std::endl;
  return  root;
}


double Scheduled::boost_Bracket(MatrixXd Qstar,  VectorXd a,double x1lb,  double x2ub, double VaRalpha){
    double root = x2ub;
    const double guess = x2ub;//x1lb+0.5; 
    double factor = 2;
    const boost::uintmax_t maxit = 50;
    boost::uintmax_t it = maxit;
    bool is_rising = true;

    tolerance tol = 1.0;

    if(x2ub - x1lb - 2 < EP){
        std::cout << 0 << std::endl;
    }else{
        // before findind the root, check if the f(x2ub) <=0, if so, return x2ub and finish
        if (f( Qstar,  a, x2ub, VaRalpha) <=0 ){
            std::cout << 1 << std::endl;
        }else{
            std::pair<double, double> found1 = boost::math::tools::bracket_and_solve_root(
                [Qstar, a, VaRalpha](double x){return f(Qstar,a, x, VaRalpha);}, 
                guess,
                factor,
                is_rising, 
                tol,
                it);

            root = found1.second;
            std::cout << it << std::endl;
        }
        if (root < x1lb+2){
            root= x1lb+2;
        }
    }
    return root;
}


//  false position method to iteration
double Scheduled::false_pos(MatrixXd Qstar, double VaRalpha, VectorXd a,double x1, double x2) {
    double const t = x1+2;
    double k1 = cdfCal(Qstar, a, x1)-VaRalpha;
    double k2 = cdfCal(Qstar, a, x2)-VaRalpha;

    int count = 2;

    if ((k1 * k2 <= 0) && (x2-t >=EP)) {
        double c = x1;
        while ((x2-x1) >= EP) {
            count++;
            c = x2 - k2*((x2-x1)/(k2-k1));
            double k3 = cdfCal(Qstar, a, c)-VaRalpha;
            if(abs(k3) <= 1e-3){
                x2 = c;
                break;
            }else{
                if (k3* k1 <= 0){
                    x2 = c;
                    k2 =k3;
                }
                else{
                    x1 = c;
                    k1 =k3;
                }    
            }
        }
    }
    if (x2<= t){
        x2= t;
    }
    std::cout<< count <<std::endl;
    

    return x2;
}





// Illinois Algorithm : improved false-postion method
double Scheduled::Illinois(MatrixXd Qstar,  double VaRalpha, VectorXd a,double x1,  double x2){
    double const t = x1+2;

    double k1 = cdfCal(Qstar, a, x1)-VaRalpha;
    double k2 = cdfCal(Qstar, a, x2)-VaRalpha;

    int count = 2;

    if ((k1 * k2 <= 0) && (x2-t >=EP)) {
        double c = x1;
        while ((x2-x1) >= EP) {
            count++;
            // c = x2 - k2*((x2-x1)/(k2-k1));
            // c = (x1*(k2/2)- x2*k1)/(k2/2-k1);
            c = (x1*k2- x2*(k1/2))/(k2-(k1/2));
            double k3 = cdfCal(Qstar, a, c)-VaRalpha;
            if(abs(k3) <= 1e-3){
                x2 = c;
                break;
            }else{
                if (k3* k1 <= 0){
                    x2 = c;
                    k2 =k3;
                }
                else{
                    x1 = c;
                    k1 =k3;
                }    
            }
        }
    }
    if (x2<= t){
        x2= t;
    }
    std::cout<< count <<std::endl;
    

    return x2;
}





// ==============================================================================
// Calculate cdf from matrix, several alternative approaches
// ==============================================================================

// Calculate cdf  1st approach - Eigen M.exp()
double Scheduled::cdfCal(MatrixXd Qstar, VectorXd a, double x1){
  MatrixXd mat = x1 * Qstar;
  double cdf = 1-((a.transpose() * mat.exp()).sum());
  return cdf;
}


// Calculate cdf  ast approach - Krylov appraoch

/**
 * Arnoldi method to compute Krylov approximation of a matrix
 * @param A nxn matrix to approximate
 * @param v nx1 initial vector
 * @param k number of Krylov steps (size of resulting basis)
 * @param V output matrix (n x k) of orthogonal vectors
 * @param H output matrix (k+1 x k) containing Krylov approximation of A
 */
// template<class DerivedMatrix, class DerivedVector>

// void arnoldi( MatrixXd A,  VectorXd v, int k, MatrixXd &V, MatrixXd &H){
//     int n = A.cols();
//     VectorXd vt(n);
//     V.col(0) = v/v.norm();
//     for (unsigned int m=0; m<k; m++) {
//         vt.noalias() = A*V.col(m); // noalias can speed up the matrix product?
//         for (unsigned int j=0; j<m+1; j++) {
//             H(j,m) = vt.dot(V.col(j));
//             vt = vt - H(j,m)*V.col(j);
//         }
//         H(m+1,m) = vt.norm();
//         if (m != k-1)
//             V.col(m+1) = vt/H(m+1,m);
//     }
// }


// Calculate cdf  ast approach - CAM appraoch

// void balance_matrix(Eigen::MatrixXd &A, Eigen::MatrixXd &Aprime, Eigen::MatrixXd &D) {
//     // https://arxiv.org/pdf/1401.5766.pdf (Algorithm #3)
//     const int p = 2;
//     double beta = 2; // Radix base (2?)
//     Aprime = A;
//     D = Eigen::MatrixXd::Identity(A.rows(), A.cols());
//     bool converged = false;
//     do {
//         converged = true;
//         for (Eigen::Index i = 0; i < A.rows(); ++i) {
//             double c = Aprime.col(i).lpNorm<p>();
//             double r = Aprime.row(i).lpNorm<p>();
//             double s = pow(c, p) + pow(r, p);
//             double f = 1;
//             while (c < r / beta) {
//                 c *= beta;
//                 r /= beta;
//                 f *= beta;
//             }
//             while (c >= r*beta) {
//                 c /= beta;
//                 r *= beta;
//                 f /= beta;
//             }
//             if (pow(c, p) + pow(r, p) < 0.95*s) {
//                 converged = false;
//                 D(i, i) *= f;
//                 Aprime.col(i) *= f;
//                 Aprime.row(i) /= f;
//             }
//         }
//     } while (!converged);
// }


// void calAlpha(MatrixXd A, std::vector<int> mList, std::vector<double> thetaList, int &sNum, int &numM){

//   for (int p = 2; p < 9; ++p){
//     // cal the alpha(A) first
//     double d1 = pow(A.pow(p).lpNorm<1>(), (1.0/p));
//     double d2 = pow(A.pow(p+1).lpNorm<1>(), (1.0/(p+1)));
//     double alphaA = std::max(d1, d2);
//     int index =0;

//     for (int i = 0; i < mList.size(); ++i){
//       if(mList[i] * ceil(alphaA/thetaList[i]) <  numM * ceil(alphaA/thetaList[index])){
//         index = i;
//         numM = mList[i];
//         sNum = std::max(int(ceil(alphaA/thetaList[index])), 1);
//        }
//      }
//     }
// }

// // function to calculate number s and number m
// void sANDm(int &sNum, int &numM, MatrixXd A){
//   // These are prepared based on the precision, no need to change.
//   std::vector<int> mList {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55};
//   std::vector<double> thetaList  {0.13, 1.0, 2.2, 3.6, 4.9, 6.3, 7.7, 9.1, 11.0, 12.0, 13.0};
  
//   double normA = A.lpNorm<1>();
//   int index = 0;

//   // this number 41.6 is predefined
//   if (normA <= 41.6){
//     for (int i = 1; i < mList.size(); ++i){
//       if(mList[i] * int(ceil(normA/thetaList[i])) <  numM * int(ceil(normA/thetaList[index]))){
//         index = i;
//         numM = mList[i];
//       }
//     }
//     sNum = int(ceil(normA/thetaList[index]));
//   }
//   else{ // NOT validate yet
//     calAlpha( A,  mList, thetaList,  sNum,  numM);
//   }
// }

// void CAM_algo(MatrixXd &A, VectorXd &v, double t, VectorXd &F){
//    // balance and get Aprime, D, D^-1,
//    MatrixXd Aprime = A;
//    MatrixXd D = Eigen::MatrixXd::Identity(A.rows(), A.cols());
//    balance_matrix(A, Aprime, D);
   
//    // compare the norm, and get B
//    if(Aprime.lpNorm<1>()<A.lpNorm<1>()){
//     A = Aprime;
//     v = D.inverse()*v;
//    }
//    double traceVal = A.trace()/A.rows();
//    A = A - traceVal*Eigen::MatrixXd::Identity(A.rows(), A.cols());

//    // calculate s, m
//    int m= 5;
//    int s = 1;
//    sANDm(s, m, t*A);

//    // begin F algorithm
      
//    F = v;
//    double ita = exp(t*traceVal/s);
//    for (int i = 1; i < s+1; ++i){
//      double c1 = v.lpNorm<Infinity>();
//      for (int j = 1; j < m+1; ++j){
//        v = (t*(A*v))/(s*j);
//        double c2 = v.lpNorm<Infinity>();
//        F = F+v;
//        if(c1+c2 <= 6e-8 * F.lpNorm<Infinity>()){
//         break;
//        }
//        c1 = c2;
//      }
//      F = ita * F;
//      v = F;
//    }
//    F = D*F;
// }


// ==============================================================================




/*
*
* Class PFSNode
*
*/

PFSNode::PFSNode(const PFSInstance *pi,const Bob::Permutation &per):Bob::BBDoubleMinNode(0.0),nbj(pi->nbj),nbm(pi->nbm),per(nbj),sc(nbm) { 
   for (int j=0;j<nbj;j++ ) {
      FixeR(per.geti(j),*pi);
      dist()++;
   }
   const PFSInstance &pfi = *pi;
   // setEval(sc.getCost(pfi, per));
}

void PFSNode::FixeR(int j,const PFSInstance &pfi) {
   per.fixeR(j);
   if (per.isallfixed() ) {
      setEval(sc.getCost(pfi, per, x1LB));
      setSol();   // set this node as a solution
   }
}

void PFSNode::eval(const PFSInstance *pfi, double minCost,double x1LB, double x2UB) {
  if(minCost ==-1){
    setEval(0);
    
  }else{
    double lb1 = pfi->lb1m->LowerBound(pfi,per,sc,x1LB,x2UB);
    setEval(lb1);
  }
}


/*
*
* branch scheme
* Class PFSGenChild
*
*/
bool PFSGenChild::operator()(PFSNode *p) {
   for (int j=0;j<inst->nbj;j++) {
      if ( p->perm().isfree(j) )  {         
         PFSNode *nf=new PFSNode(p);
         nf->FixeR(j,*inst);
         if ( nf->isSol() ) {
            std::cout << "***********Solution :"<<nf->getEval()<<std::endl;
            nf->Prt(std::cout); // output in terminal
         } else {
            // get the statistics
            algo->start_eval(nf,p);
            nf->eval(inst,algo->getGoal()->getBest(),nf->x1LB,algo->getGoal()->getBest());
            algo->end_eval(nf);
            std::cout << " Generate:"<<j<<" on "<<nf->dist()<<std::endl; // nf->dist is location index
            nf->Prt(std::cout);
            nf->dist()++;
         }
         algo->Search(nf);
      } 
   }
   // std::cout << "------Fin GenChild---------------------------------------------\n";
}