
#include<bobpp/ilp/bob_lpbbalgo.h>

namespace Bob {

/*GenericBranchingSet::GenericBranchingSet() {
    add_calc_branch(new MostFracBranching());
    add_calc_branch(new StrongBranching());
    add_calc_branch(new EntropyBranching());
    add_calc_branch(new StrongThenMostFracBranching(Bob::core::opt().NVal("--mip", "-s")));
    std::string rule = Bob::core::opt().SVal("--mip", "-b");
    type = -1;
    for( unsigned int i=0;i<gbs.size();i++ ) {
         if ( gbs[i]!=0 && gbs[i]->is_same(rule.c_str()) ) { type= i; return; }
    }
    std::cerr << "Error Branching strategy not found :"<<rule<<"\n";
    exit(1);
}

int GenericBranchingSet::branch(LP *lp,BranchingInfo *i,LPStat &st,int dist) {
   return gbs[type]->branch(lp,i,st,dist);
}*/

};

