
#include <bobpp>
#include <signal.h>
#include <iomanip>
#include <iostream>
#include <sstream>


namespace Bob {

void stat_timer::start() {
  dt = 0L;
  nbr++;
  st = core::dTime();
  core::log().out(str(), 's', nbr,cumt);
}

/// Set the end time
void stat_timer::end() {
  dt = core::dTime(st);
  cumt += dt;
  if ( is_logged() ) core::log().out(str(), 'e', nbr,cumt);
}

void stat_timer::start(node_id id) {
  dt = 0L;
  nbr++;
  st = core::dTime();
  if ( is_logged() ) core::log().out(str(), 's', nbr,cumt,id);
}

/// Set the end time
void stat_timer::end(node_id id) {
  dt = core::dTime(st);
  cumt += dt;
  if ( is_logged() ) core::log().out(str(), 'e', nbr,cumt,id);
}

void stat_counter::update_log() {
  update_mix();
  if ( is_logged() ) core::log().out(str(), nb);
}
void stat_counter::update_log(const char *ni) {
  update_mix();
  if ( is_logged() ) core::log().out(str(), nb,ni);
}
void stat_counter::update_log(node_id ni) {
  update_mix();
  if ( is_logged() ) core::log().out(str(), nb,ni);
}

void stat_counter::update_mix() {
  if (nb > max) max = nb;
}

void stat::reset() {
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    mc[i->first].reset();
  }
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    mt[i->first].reset();
  }
}

void stat::merge(const stat &s) {
  DBGAFF_PCK("stat::merge", "merge all the stats");
  for (MapCount::const_iterator i = s.mc.begin(); i != s.mc.end() ; i++) {
    mc[i->first].merge(i->second);
  }
  for (MapTime::const_iterator i = s.mt.begin(); i != s.mt.end() ; i++) {
    mt[i->first].merge(i->second);
  }
}

#define COUN_SP 6
#define TIME_SP 10

/* Dispaly of stat counters */
void format_label(std::stringstream &os, const stat_counter &st) {
  os << std::setw(COUN_SP) << st.id() << " ";
}

void format_data(std::stringstream &os, const stat_counter &st) {
  os << std::setw(COUN_SP) << st.get() << " ";
}

void format_label(std::stringstream &os, const MapCount &mc) {
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    format_label(os, i->second);
  }
}
void format_data(std::stringstream &os, const MapCount &mc) {
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    format_data(os, i->second);
  }
}

/* Dispaly of stat timers */
void format_label(std::stringstream &os, const stat_timer &st) {
  os << std::setw(TIME_SP) << st.id() << " ";
}

void format_data(std::stringstream &os, const stat_timer &st) {
  os << std::fixed << std::setprecision(0) << std::setw(5) << st.getn()
  << ":" << std::setprecision(2) << st.getc() << " ";
}

void format_label(std::stringstream &os, const MapTime &mt) {
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    format_label(os, i->second);
  }
}
void format_data(std::stringstream &os, const MapTime &mt) {
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    format_data(os, i->second);
  }
}

void stat::dump(strbuff<> &s) const {
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    i->second.dump(s);
  }
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    i->second.dump(s);
  }


}

void stat::update(const Id &id, int disp) {
  for (MapCount::iterator i = mc.begin(); i != mc.end() ; i++) {
    i->second.set(id,disp);
  }
  for (MapTime::iterator i = mt.begin(); i != mt.end() ; i++) {
    i->second.set(id,disp);
  }

}

void stat::log_header() const {
  if ( !is_logged()) return; 
  buff_st os; 
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    //std::cout << "mc:"<<i->first<<"\n";
    if ( i->second.is_logged() ) i->second.log_header(os);
  }
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    //std::cout << "mt:"<<i->first<<"\n";
    if ( i->second.is_logged() ) i->second.log_header(os);
  }
  core::log().log_header(os);
}

void stat::display_title(ostream &os) {
  os << "-------Stats for : " << id() << std::endl;
}

void stat::display_label(ostream &os,const char *lib,int size) {
  std::stringstream ss(stringstream::in | stringstream::out);
  format_label(ss, (const MapTime &)mt);
  format_label(ss, (const MapCount &)mc);
  ss << std::endl;
  os << ss.str();
}

void stat::display_data(ostream &os,const char *lib,int size) {
  std::stringstream ss(stringstream::in | stringstream::out);
  format_data(ss, (const MapTime &)mt);
  format_data(ss, (const MapCount &)mc);
  ss << std::endl;
  os << ss.str();
}


void stat::Pack(Serialize &bs) const {
  int nb;
  DBGAFF_PCK("stat::Pack", "Packing stat");
  nb = mc.size();
  bs.Pack(&nb, 1);
  for (MapCount::const_iterator i = mc.begin(); i != mc.end() ; i++) {
    bs.Pack(&i->first, 1);
    i->second.Pack(bs);
  }
  nb = mt.size();
  bs.Pack(&nb, 1);
  for (MapTime::const_iterator i = mt.begin(); i != mt.end() ; i++) {
    bs.Pack(&i->first, 1);
    i->second.Pack(bs);
  }
}

void stat::UnPack(DeSerialize &bs)  {
  DBGAFF_PCK("stat::UnPack", "UnPacking stat");
  char s;
  stat_counter sc;
  stat_timer st;
  int i, nb;
  bs.UnPack(&nb, 1);
  for (i = 0; i < nb ; i++) {
    bs.UnPack(&s, 1);
    mc[s].UnPack(bs);
  }
  bs.UnPack(&nb, 1);
  for (i = 0; i < nb ; i++) {
    bs.UnPack(&s, 1);
    mt[s].UnPack(bs);
  }
}


};
