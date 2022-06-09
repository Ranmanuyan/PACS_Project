/*
* @Author: Lei Liu
* @Date:   2021-05-27 15:57:26
* @Last Modified by:   Lei Liu
* @Last Modified time: 2022-06-09 17:18:16
*/



#pragma once
#include <bobpp/bobpp>
#include"pfs.h"
#include"lb.h"

#include <fstream> 
#include <iomanip>
#include <iostream>
#include <string>

#define Threaded

#ifdef Threaded
#  include <bobpp/thr/thr.h>
#endif

using namespace std; 




// solve one instance
int BBoneIns(int n, char ** v, std::string filename, int line);
void write_string_to_file_append(const std::string & file_string, const std::string str);


// solve all the instances in the file
int main(int m, char ** v) {
  std::cout << "Please enter the data file path : ";
  std::string filename;
  std::cin >> filename;

  // output result header
  std::ostringstream ss1;
  ss1 << "InstanceID" << "\t";
  ss1 << "Job_number" << "\t";
  ss1 << "alpha" << "\t";
  ss1 << "Threads_num" << "\t";
  ss1 << "VaR_solution" << "\t";
  ss1 << "Core_time" << "\t";
  ss1 << "Evaluated Nodes" << "\t";
  write_string_to_file_append("../result.txt", ss1.str());

  for(int line=0; line <5; line++){
      int l = BBoneIns(m, v, filename, line);
  }

  exit(0);
}


 
void write_string_to_file_append(const std::string & file_string, const std::string str ){
  
  std::ofstream OsWrite(file_string, std::ofstream::app);
  OsWrite<<str;
  OsWrite<<std::endl;
  OsWrite.close();
}



int BBoneIns(int n, char ** v, std::string filename, int line){
  Bob::core::opt().add(std::string("--pfs"), Bob::Property("-f", "Problem File", filename));
  Bob::core::opt().add(std::string("--pfs"), Bob::Property("-lb", "Lower bound", 0));
  Bob::core::opt().add(std::string("--pfs"), Bob::Property("-line", "Instance index", line));   

#ifdef Threaded
  Bob::ThrBBAlgoEnvProg<PFSTrait> env;
  Bob::ThrEnvProg::init();
  Bob::core::Config(n, v);
  Bob::ThrEnvProg::start();
#else
  Bob::SeqBBAlgoEnvProg<PFSTrait> env;
  Bob::core::Config(n, v);
#endif

  double coreTime1 = 0.0;
  coreTime1 = Bob::core().dTime();

  PFSInstance *Instance = new PFSInstance(Bob::core::opt().SVal("--pfs", "-f"),Bob::core::opt().NVal("--pfs", "-lb"),Bob::core::opt().NVal("--pfs", "-line"));
  std::cout << "-------- Start to solve the entire tree" << std::endl;
  env(Instance);


#ifdef Threaded
  Bob::ThrEnvProg::end();
#endif
Bob::core::End();


// Collect the stat_array of each instance
  int nbj = Instance->nbj;
  double alpha = 0.99;
  int threads_num = Bob::ThrEnvProg::n_thread();
  double bestG = 0.0;
  bestG  = env.goal()->getBest();
  double coreTime2 = 0.0;
  coreTime2 = Bob::core().dTime();
  double coreTime = 0.0;
  coreTime = coreTime2 - coreTime1;

  int  small_c =0;
  for (int i=0; i<Bob::ThrEnvProg::n_thread()-1; ++i){
    small_c += env.stat_array()[i]->get_counter('c').get();
  }
  

  int instanceID = line+1;

// Instance ID, Job_number, Threads_num, VaR_solution, Core_time, small_c, Big_C, Big_D, Big_E, Big_T, small_i, small_p,small_d,small_g
  std::ostringstream ss;
  ss << instanceID << "\t";       // Instance ID
  ss << nbj << "\t";              // Job_number
  ss << alpha << "\t";            // alpha
  ss << threads_num << "\t";      // Threads_num
  ss << bestG << "\t";            // VaR_solution
  ss << coreTime << "\t";         // Core time of this Instance
  ss << small_c << "\t";          // number of generated nodes
  
  
  write_string_to_file_append("../result.txt", ss.str());

  delete Instance;
  return 0;
}