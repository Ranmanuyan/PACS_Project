
/*
 * bob_opt.h
 *
 *  These classes are used to handle the application option in Bob++
 *
 *
 */

#ifndef BOBOPT_HEADER
#define BOBOPT_HEADER


/*------------------ Option Classes -------------*/
#include<cstdio>
#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<exception>
#include"bob_error.h"

namespace Bob {

/**
 *  @defgroup OptGRP The Options
 *  @brief  this module describes how to manage and to handle the option and binary parameters.
 *  @ingroup CoreGRP
 *  @{
 *  \todo adds a BOTTP_FILE handled by the library.
 */

/** \page OptPage Managing The Options.
 *
 *  All Option classes are also include in the Module @ref OptGRP.
 *
 *
 *  \section OptIntro Introduction

Bob++ has its own options/parameters. Generally, each
application need parameters to know wich are the file where find the data,
if we use this or this feature in my Algo. So the general structure
of the options or parameters is the following :

- Option: sign - followed by a letter or a number (Unix systems)
- Parameters : a string or a number.

To separate the default Bob++ options from a specific Algo
options/parameters each Algo has a default string preceed by '--' to
identify them. Between two --XX options, are the options of the Algorithms
identified by XX.
\verbatim
$ a.out -p 4 --BB -V para1 para2 124
\endverbatim

The previous line is a command line for a Branch and Bound application
(named a.out) that admits three parameters, the first one and the second one
are strings and the third one is a number, and an option '-V',
the -p 4 option is to run the application in parallel on 4 processors.

\section OptDefault Default Bob++ Option

 - -l : log or not log
 - -help : Display the help, the list of the parameters
 - -ol \<file\>: the logging ouput file

\section OptAdd Defining options and parameters in your Algorithms

The example directory stores an example called opt1.cpp \include opt1.cpp

The idea is to add properties in the Bob++ global options
(obtain using Bob::core::opt() ) using the add() method.

Then after the call to the Bob::core::Config(...) method, you could call
the Bob::core::opt().NVal() or the Bob::core::opt().DVal() to get the
associated value.

 */
/** @}
 */

/** The Property class store the value of an optional parameter.
  * A Property stores a string which is the option i.e. "-an_option",
  * a description of the option and the value of the option.
  * The Option Value is typed Boolean, Integer, Double or String.
  *
  * a Property could store the vlaue by itself, or point on a
  * user space variable.
  *
  *  @ingroup  OptGRP
  */
class Property {

public:
  /** Type of Property
   */
  enum PropType {BOOL, /*!< the property choice is a boolean */
                 INTEGER,/*!< the property choice is a integer */
                 DOUBLE ,/*!< the property choice is a double*/
                 STRING /*!< the property choice is a string*/
                };
  /** @name the Constructors
   *@{ */
  /** Constructor for an empty property
   */
  Property();
  /** Constructor to copy a property
   */
  Property(const Property &prop);
  /** Constructor for a boolean property
   * @param n the name of the property
   * @param c the comment
   */
  Property(const std::string &n, const std::string &c);
  /** Constructor for a boolean property
   * @param n the name of the property
   * @param c the comment
   * @param v the variable that stores the result
   */
  Property(const std::string &n, const std::string &c, bool *v);
  /** Constructor for a string property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   */
  Property(const std::string &n, const std::string &c, const std::string &def);
  /** Constructor for a string property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   * @param v the variable that stores the result
   */
  Property(const std::string &n, const std::string &c, const std::string &def, std::string *v);
  /** Constructor for a long property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   */
  Property(const std::string &n, const std::string &c, int def);
  /** Constructor for a long property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   * @param v the variable that stores the result
   */
  Property(const std::string &n, const std::string &c, int def, int *v);
  /** Constructor for a double property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   */
  Property(const std::string &n, const std::string &c, double def);
  /** Constructor for a double property
   * @param n the name of the property
   * @param c the comment
   * @param def the default value of the property
   * @param v the variable that stores the result
   */
  Property(const std::string &n, const std::string &c, double def, double *v);

  //@}
  /// Get the name of the property
  std::string getName() const;
  /** set the argument value
    * @param v the string that stores the argument
    */
  void setArg(const std::string &v);
  /// test if the propoerty has an argument or not
  bool hasArg() const;
  /// gets the boolean value of the parameter
  bool BVal() const;
  /// gets the int value of the parameter
  int NVal() const;
  /// gets the double value of the parameter
  double DVal() const;
  /// gets the string value of the parameter
  std::string SVal() const;
  /** Dump the property
    * Put the name of the property and its value on a strbuf
    * @param s the strbuff to fill.
    */
  void dump(strbuff<> &s) const;
  /// Usage method
  void Usage(std::ostream &o) const;
protected:
  /** Conversion methods from std::string to basic type
  */
  //@{
  /** Convert a string to a long
  */
  int String2Int(const std::string& s) const;
  /** Convert a int to a string
  */
  void Int2String(int l, std::string& s) const;

  /** Convert a string to a double
  */
  double String2Double(const std::string& s) const;
  /** Convert a double to a string
   */
  void Double2String(double d, std::string& s) const;

  //@}



  /// Name of the option
  std::string name;
  /// Type of property
  PropType pt;
  /// value stored as a string
  std::string val;
  /// boolean indicating if the property point on a external variable.
  bool ext;
  /// pointer on the external value.
  void *pval;
  /// Comments on this option.
  std::string com;
  /// Methods that test if the type of Property is the good one...
  bool test(bool set, PropType ptd) const;
};

/**
  * The Group option ie "--Truc" is just to identify the options for
  * the Kernel, or one of  a specific Algorithm
 *  @ingroup  OptGRP
  */
class GrpOpt {
public:
  /// typedef PropertySet which is an stl map with a string key and a property for the data
  typedef std::map<std::string, Property> PropertySet;
private :
  std::string s;
  std::string com;
  PropertySet opt;

public:
  /// Constructor
  GrpOpt() : s(""), com(""), opt() {
  }
  /// Constructor
  GrpOpt(const std::string &sp, const std::string &c) : s(sp), com(c), opt() {

  }
  /// Destructor
  virtual ~GrpOpt() {
    opt.clear();
  }
  /** Get the name (the reference of the Group Option
  */
  std::string &getRef() {
    return s;
  }
  /** Set the reference to this group option.
  * @param ref the key of the GrPOpt
  */
  void setRef(const std::string &ref) {
    s = ref;
  }
  /** Adds a unit option on this group
  * @param p the property to add.
   * @see OptGRP
  */
  void add(const Property &p) {
    opt[p.getName()] = p;
  }
  /** sets a comment for this GroupOption
  * @param c the comment (a string) for the help
  */
  void SetComments(const std::string &c) {
    com = c;
  }
  /** get the property stored in the GrpOpt associated to the key o
   * @param o the key of the property to get
   * @return the property (return 0 if the named property does not exist).
  */
  Property *get(const std::string &o) {
    PropertySet::iterator p = opt.find(o);
    if (p != opt.end()) {
      return &(p->second);
    }
    return 0;
  }
  /** Set the argument of a property stored in the GrpOpt
   * @param o the key of the property to get
   * @param v the value to set
  */
  void setArg(const std::string &o, const std::string &v) {
    PropertySet::iterator p = opt.find(o);
    if (p != opt.end()) {
      p->second.setArg(v);
    }
  }
  /** Get the boolean value of the property associated with the key s
  * @param s the key of the property
   * @return the boolean value
  */
  bool getBVal(const std::string &s) {
    PropertySet::iterator p = opt.find(s);
    if (p == opt.end()) {
      BOBEXCEPTION(OptionException, s, " non existant option");
    }
    return p->second.BVal();
  }

  /** Get the double value of the property associated with the key s
  * @param s the key of the property
   * @return the double value
  */
  double getDVal(const std::string &s) {
    PropertySet::iterator p = opt.find(s);
    if (p == opt.end()) {
      BOBEXCEPTION(OptionException, s, " non existant option");
    }
    return p->second.DVal();
  }
  /** Get the long value of the property associated with the key s
  * @param s the key of the property
   * @return the int value
  */
  int getNVal(const std::string &s) {
    PropertySet::iterator p = opt.find(s);
    if (p == opt.end()) {
      BOBEXCEPTION(OptionException, s, " non existant option");
    }
    return p->second.NVal();
  }
  /** Get the string value of the property associated with the key s
  * @param s the key of the property
   * @return the string value
  */
  std::string getSVal(const std::string &s) {
    PropertySet::iterator p = opt.find(s);
    if (p == opt.end()) {
      BOBEXCEPTION(OptionException, s, " non existant option");
    }
    return p->second.SVal();
  }
  /** Method to parse the parameters. Parameters given
  * in v, that does not exist in the Bob++ options set, are ignored.
  * @param n the number of parameters
  * @param v the array of parameters
  */
  int Handle(int n, char **v);
  /** Dump the group of Properties
    * @param s the strbuff to fill.
    */
  void dump(strbuff<> &s) const;

  /** the usage method.
  * @param err a std::ostream to display the options usage.
  */
  void Usage(std::ostream &err = std::cerr) {
    PropertySet::iterator p;
    err << "Group Option : " << s << ":" << com << std::endl;
    for (p = opt.begin() ; p != opt.end(); p++) {
      Property pr = p->second;
      pr.Usage(err);
    }
  }

};

/**
  * The Option encapsulate all the group options of the application.
 *  @ingroup  OptGRP
  */
class Opt {
  typedef std::map<std::string, GrpOpt> GrpOptSet;
  std::string s;
  GrpOptSet lo;
public:

  /// Constructor of a Opt.
  Opt() : s("Bob Option") { }
  /** Constructor of a Opt.
    * @param sp is the name of the option
    */
  Opt(const std::string &sp) : s(sp) { }
  /// Destructor
  virtual ~Opt() {
    lo.clear();
  }
  /** Adds a propoerty p to the group option named _s
    * @param _s the name of the group option
    * @param p the property to add
    */
  /*void add(const std::string &_s,Property p) {
    //std::cout<< "Grp:"<<_s<<std::endl;
    lo[_s].setRef(_s);
    lo[_s].add(p);
    //lo[_s].Usage(std::cout);
  }*/

  /** Adds a propoerty p to the group option named _s
    * @param _s the name of the group option
    * @param p the property to add
    */
  void add(const std::string &_s, const Property &p) {
    //std::cout<< "Grp:"<<_s<<std::endl;
    lo[_s].setRef(_s);
    lo[_s].add(p);
    //lo[_s].Usage(std::cout);
  }
  /** Gets the bool value of an option in the named group option
    * @param go the name of the group option
    * @param so the name of the option
    * @return the long value
    */
  bool BVal(const std::string &go, const std::string &so) {
    return lo[go].getBVal(so);
  }

  /** Gets the long value of an option in the named group option
    * @param go the name of the group option
    * @param so the name of the option
    * @return the long value
    */
  int NVal(const std::string &go, const std::string &so) {
    return lo[go].getNVal(so);
  }
  /** Gets the double value of an option in the named group option
    * @param go the name of the group option
    * @param so the name of the option
    * @return the double value
    */
  double DVal(const std::string &go, const std::string &so) {
    return lo[go].getDVal(so);
  }
  /** Gets the string value of an option in the named group option
    * @param go the name of the group option
    * @param so the name of the option
    * @return the string value
    */
  std::string SVal(const std::string &go, const std::string &so) {
    return lo[go].getSVal(so);
  }
  /** Parse the usual options (argc argv) according to the registring Options.
    * @param n the number of parameters
    * @param v the array of char * that stores the parameters.
    */
  void parse(int &n, char ** &v);
  /// clears all the options
  void clear() {
    lo.clear();
  }
  /** Dump the group of Properties
    * @param s the strbuff to fill.
    */
  void dump(strbuff<> &s) const;

  /** Usage method displaying the given String error.
    * @param err a stream to display the usage
    */

  void Usage(std::ostream &err = std::cerr) {
    GrpOptSet::iterator p;
    for (p = lo.begin() ; p != lo.end(); p++) {
      GrpOpt g = p->second;
      g.Usage(err);
    }
  }
};

} // end of namspace

#endif
