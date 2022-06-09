
#include <bobpp/ilp/bob_lpbbalgo.h>

/*#define DEBUG_PACK*/

#ifdef DEBUG_PACK
#define GUP_SER(type,nb) std::cout<<"Gup_Ser "<<#type<<":"<<nb<<std::endl
#define GUP_DER(type,nb) std::cout<<"Gup_Der "<<#type<<":"<<nb<<std::endl
#else
#define GUP_SER(type,nb) 
#define GUP_DER(type,nb)
#endif

/** pack an array of char */
int gup_pack_char(gup_serialize * s, const char *p, int count) {
  GUP_SER(char,count);
  s->s->Pack(p, count);
  return count;
}

/** pack an array of int */
int gup_pack_int(gup_serialize * s, const int *p, int count) {
  GUP_SER(int,count);
  s->s->Pack(p, count);
  return count;
}

/** pack an array of double */
int gup_pack_double(gup_serialize * s, const double *p, int count) {
  GUP_SER(double,count);
  s->s->Pack(p, count);
  return count;
}

/** unpack an array of char */
int gup_unpack_char(gup_deserialize * s, char *p, int count) {
  GUP_DER(char,count);
  s->d->UnPack(p, count);
  return count;
}

/** unpack an array of int */
int gup_unpack_int(gup_deserialize * s, int *p, int count) {
  GUP_DER(int,count);
  s->d->UnPack(p, count);
  return count;
}

/** unpack an array of double */
int gup_unpack_double(gup_deserialize * s, double *p, int count) {
  GUP_DER(double,count);
  s->d->UnPack(p, count);
  return count;
}

namespace Bob {
/*bool vc_generator_set::gene(LP *lp) {
  bool b=false;
  for( std::vector<vc_generator *>::iterator iter = vcfs.begin(); iter != vcfs.end(); iter++ ) {
     b = b || (*iter)->gene(lp);
  }
  return b;
}*/
/// Initialize the Instance
void LP::read(const char *fname) {
  const char *ext;

  ext = fname + strlen(fname);
  if (!strcmp(ext - 4, ".mps"))
    read_mps(fname);
  else if (!strcmp(ext - 3, ".lp"))
    read_lpt((char*)fname);
  Normalize();
}


void LP::Normalize() {
  int i, ncols;
  set_verbosity(1);
  set_presolve(1);
  ncols = get_num_cols();
  if (get_obj_dir() == GSP_MAX) {
    // Maximization problem, we need to negate all objective coefficients
    double obj_coef[ncols];
    get_obj_coef(obj_coef, 0, ncols - 1);
    for (i = 0; i < ncols; i++)
      obj_coef[i] = -obj_coef[i];
    set_obj_coef(obj_coef, 0, ncols - 1);
    set_const_term(-get_const_term());
    set_obj_dir(GSP_MIN);
  }
  set_class(GSP_LP);
}

//int LP::nbLP=0;

LP::~LP() {
 //nbLP--;
 //std::cout <<" MEMORY LP Deleted"<<nbLP<<"\n";
}

/*int LP::solve(LPStat *stat,double bs) {
    int ret;
    DBGAFF_USER("LP::solve()", "Called");
    set_obj_upper_limit(bs);
    stat->lp_start();
    ret=ppglop::pb::solve();
    integer_done=false;
    stat->lp_end();
    if (get_status() == GSP_LP_NOFEAS) {
      std::cerr << " Non feasible Problem\n";
      exit(1);
    }
    return ret;
}*/

/*bool LPGeneVar::add_var(BXStat &stat) {
     bool b;
     if ( sv==0 ) return false;
     stat.cuts_start();
     b=sv->gene(this);
     stat.cuts_end();
     return b;
}

bool LPGeneCut::add_cut(BXStat &stat) {
     if ( sc==0 ) return false;
     stat.cuts_start();
     b =sc->gene(this);
     stat.cuts_end();
     return b;
}*/

/*int LP::solve(LPStat &stat) {
    //int nb_cuts;
    double obj, obj2,improvement;
    int ret;
    //bool gen_cut;
    //bool gen_var;
    improvement=0.0;

    ret=one_solve(stat);
    obj2 = get_obj_val();

    while ( add_varcut(stat) ) {
      obj = obj2;
      ret=one_solve(stat);
      obj2 = get_obj_val();
      improvement += obj2-obj;
      std::cout<<i<<" "<<improvement<<std::endl;
      if (obj2>obj)
        std::cout << "obj " << obj << " -> " << obj2 << std::endl;
      if ( (obj2-obj)<ACCURACY ) break;
    }
    return ret;
}
*/

bool LP::do_test_integer() {
  int i;
  double val, frc, bestfrc, bestvar, bestval;
  int ncols = get_num_cols();
  int kind[ncols];
  get_col_kind(kind, 0, ncols - 1);
  val = get_obj_val();

  bestvar = -1;
  bestfrc = 0.0;
  bestval = 0.0;
  for (i = 0; i < ncols; i++) {
    if (kind[i] != GSP_IV)
      continue;
    get_col_prim(&val, i, i);
    frc = fabs(val - nearbyint(val));
    if (frc >= ACCURACY && frc > bestfrc) {
      bestfrc = frc;
      bestvar = i;
      bestval = val;
    }
  }

  if (bestvar == -1) {
    std::cout << "Solution found: " << get_obj_val() << std::endl;
    integer = true;
    integer_done = true;
    return true;
  }
  integer_done = true;
  return false;
}

/*
void LPGeneCut::addCut(mip_cut &c) {
    if (c.var < 0)
      abort();
    //std::cout << "adding cut: x[" << c.var << "] "
    //	<< (c.direction_is_low?"> ":"< ") << c.bound << std::endl;
    static double one = 1.0;
    int dir = c.direction_is_low ? GSP_LO : GSP_UP;
    gsp_add_row(lp, 1, &c.var, &one, dir, c.bound, NULL);
}

void LPGeneCut::addVarCut(int var, double value, bool direction_is_low) {
    DBGAFF_USER("MipLP::addCut()", "Called");
    mip_cut c;
    c.var = var;
    c.bound = direction_is_low ? ceil(value) : floor(value);
    c.direction_is_low = direction_is_low;
    addCut(c);
    cuts.push_back(c);
}*/

};

