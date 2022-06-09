
#include "bobpp.h"

namespace  Bob {

Property::Property() :
    name(""), pt(BOOL), val(""), ext(false), pval(0), com("") { }

Property::Property(const Property &prop) : name(prop.name), pt(prop.pt),
    val(prop.val), ext(prop.ext), pval(prop.pval), com(prop.com) { }

Property::Property(const std::string &n, const std::string &c) :
    name(n), pt(BOOL), val(""), ext(false), pval(0), com(c) { }

Property::Property(const std::string &n, const std::string &c, bool *v) :
    name(n), pt(BOOL), val(""), ext(true), pval(v), com(c) {  }

Property::Property(const std::string &n, const std::string &c, const std::string &def) :
    name(n), pt(STRING), val(def), ext(false), pval(0), com(c) { }

Property::Property(const std::string &n, const std::string &c, const std::string &def, std::string *v) :
    name(n), pt(STRING), val(def), ext(true), pval(v), com(c) {
  *v = def;
}

Property::Property(const std::string &n, const std::string &c, int def) :
    name(n), pt(INTEGER), val(""), ext(false), pval(0), com(c) {
  Int2String(def, val);
}

Property::Property(const std::string &n, const std::string &c, int def, int *v) :
    name(n), pt(INTEGER), val(""), ext(true), pval(v), com(c) {
  *v = def;
}

Property::Property(const std::string &n, const std::string &c, double def) :
    name(n), pt(DOUBLE), val(""), ext(false), pval(0), com(c) {
  Double2String(def, val);
}

Property::Property(const std::string &n, const std::string &c, double def, double *v) :
    name(n), pt(DOUBLE), val(""), ext(true), pval(v), com(c) {
  *v = def;
}


/// Get the name of the property
std::string Property::getName() const {
  return name;
}
/// try to set argument
void Property::setArg(const std::string &v) {
  bool *b;
  double *d;
  int *l;
  std::string *ps;
  std::cout << " Type:" << pt << ":" << v << std::endl;
  val = v;
  if (ext) {
    switch (pt) {
    case BOOL :
      b = (bool *)pval;
      *b = true;
      break;
    case DOUBLE :
      d = (double *)pval;
      *d = String2Double(v);
      break;
    case INTEGER:
      l = (int *)pval;
      *l = String2Int(v);
      break;
    case STRING:
      ps = (std::string *)pval;
      *ps = v;
    }
  }
}
/// gets the int value of the parameter
bool Property::BVal() const {
  if (test(false, BOOL)) {
    if (ext) return *((bool *)pval);
    return val.size() != 0;
  } else
    return false;
};
/// gets the int value of the parameter
int Property::NVal() const {
  if (test(false, INTEGER)) {
    if (ext) return *((int *)pval);
    return String2Int(val);
  } else
    return 0L;
};
/// gets the int value of the parameter
double Property::DVal() const {
  if (test(false, DOUBLE)) {
    if (ext) return *((double *)pval);
    return String2Double(val);
  } else
    return 0.0;
};
/// gets the string value of the parameter
std::string Property::SVal() const {
  if (test(false, STRING)) {
    if (ext) return *((std::string *)pval);
    return val;
  } else return std::string("");
}

void Property::dump(strbuff<> &s) const {
  s << '.'<< name.c_str()+1 << ':';
  switch (pt) {
    case BOOL: s  << (ext? ( *(bool *)pval?"t":"f"): (val.size() != 0?"t":"f")); break;
    case INTEGER : s << NVal(); break;
    case DOUBLE : s << DVal(); break;
    case STRING: s << SVal(); break;
  }

}

/// usage method
void Property::Usage(std::ostream &o) const {
  o << "Option :" << name;
  switch (pt) {
  case BOOL:
    o << " boolean (def not set) " << com << std::endl;
    break;
  case INTEGER :
    o << " arg : a int (def:" << NVal() << ") " << com << std::endl;
    break;
  case DOUBLE :
    o << " arg : a double (def:" << DVal() << ") " << com << std::endl;
    break;
  case STRING:
    o << " arg : a string (def:" << SVal() << ") " << com << std::endl;
    break;
  }
}

bool Property::hasArg() const {
  return pt != BOOL;
}

void Property::Int2String(int l, std::string& s) const {
  std::ostringstream tmp;
  tmp << l;
  s = tmp.str();
}

int Property::String2Int(const std::string& s) const {
  int v;
  std::istringstream tmp(s);
  tmp >> std::ws >> v;
  if (tmp.fail()) {
    std::cerr << "Warning : bad conversion for option " << name << " \"" << s << "\" to int" << std::endl;
    Usage(std::cerr);
    return 0;
  }
  return v;
}

void Property::Double2String(double d, std::string& s) const {
  std::ostringstream tmp;
  tmp << d;
  s = tmp.str();
}


double Property::String2Double(const std::string& s) const {
  double v;
  std::istringstream tmp(s);
  tmp >> std::ws >> v;
  if (tmp.fail()) {
    std::cerr << "Warning : bad conversion for option " << name << " \"" << s << "\" to double" << std::endl;
    Usage(std::cerr);
    return 0.0;
  }
  return v;
}

bool Property::test(bool set, PropType ptd) const {
  if (ptd != pt) {
    if (set) {
      std::cerr << "Bobpp: Warning you set a value of a" ;
      switch (pt) {
      case BOOL:
        std::cerr << " boolean ";
        break;
      case INTEGER :
        std::cerr << " integer ";
        break;
      case DOUBLE :
        std::cerr << " double ";
        break;
      case STRING:
        std::cerr << " string ";
        break;
      }
      std::cerr << " property with a " ;
      switch (ptd) {
      case BOOL:
        std::cerr << " boolean ";
        break;
      case INTEGER :
        std::cerr << " integer ";
        break;
      case DOUBLE :
        std::cerr << " double ";
        break;
      case STRING:
        std::cerr << " string ";
        break;
      }
      std::cerr << " property !" << std::endl;
    } else {
      std::cerr << "Warning you use a " ;
      switch (ptd) {
      case BOOL:
        std::cerr << "BVal() ";
        break;
      case INTEGER :
        std::cerr << " NVal() ";
        break;
      case DOUBLE :
        std::cerr << " DVal() ";
        break;
      case STRING:
        std::cerr << " SVal() ";
        break;
      }
      std::cerr << " method for a " ;
      switch (pt) {
      case BOOL:
        std::cerr << " boolean ";
        break;
      case INTEGER :
        std::cerr << " integer ";
        break;
      case DOUBLE :
        std::cerr << " double ";
        break;
      case STRING:
        std::cerr << " string ";
        break;
      }
      std::cerr << " property !" << std::endl;
    }
    return false;
  }
  return true;
}

void GrpOpt::dump(strbuff<> &b) const {
    PropertySet::const_iterator p;
    for (p = opt.begin() ; p != opt.end(); p++) {
      Property pr = p->second;
      b << ' '<< s.c_str()+2;
      pr.dump(b);
    }
}


/*------- Options -------------*/
void Opt::dump(strbuff<> &s) const {
    GrpOptSet::const_iterator p;
    for (p = lo.begin() ; p != lo.end(); p++) {
      GrpOpt g = p->second;
      if ( g.getRef().compare("--")!=0 && g.getRef().compare("--dbg")!=0 ) 
        g.dump(s);
    }
}


void Opt::parse(int &n, char ** &v) {
  int state = 0;
  std::string gs = "--";
  std::string o = "";
  int i = 1;
  GrpOptSet::iterator p;

  while (i < n) {
    char *s = v[i];
    if (s[0] == '-' && state <= 1) {
      if (s[1] == '-') {
        gs = s;
        p = lo.find(gs);
        if (p != lo.end()) {
          state = 1;
        }
        i++;
        continue;
      }
      o = s;
      p = lo.find(gs);
      if (p != lo.end()) {
        Property *pi = p->second.get(o);
        if (pi == 0) {
          i++;
          continue;
        }
        if (pi->hasArg()) {
          if (i + 1 >= n) BOBEXCEPTION(OptionException, s, " has argument");
          pi->setArg(v[i+1]);
          i++;
        } else {
          pi->setArg("1");
        }
        state = 1;
      }
      i++;
      continue;
    }
    i++;
  }
}

}// end of namespace
