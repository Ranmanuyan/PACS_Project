/*
 * Bob++/include/bob_id.h
 *
 *  These classes are used to store all the identfiers
 */
#ifndef BOBIDENTIF
#define BOBIDENTIF

#include <limits.h>

namespace Bob {

/**
 *  @defgroup IdGRP The Bob++ identifiers
 *  @ingroup CoreGRP
 *  @brief the different opject identifiers used in Bob++
 *  @{
 *  To be able to print some human readable statistics/loggin information,
 *  Bob++ defines several forms of identifiers.
 *  The EnvId class is used to get a char * that named a computing ressource
 *  as threaded, process...
 *
 *  The Id is more general and is used to named the different statistics of
 *  algorithms or data structures operations
 *  One specific user algorithm has its statistics, but also the genchild,
 *  and the different data structure as Priority Queue and Goal.
 */


/** The EnvId is an abstract class to print the identifier of
 * a computing ressource in a environment.
 *
 * An object of this class must be redefined in each environment and must
 * be set in the Bobpp log using the core::log().setEid()
 *
 */
class EnvId {
public:
  /** Constructor
    */
  EnvId() {}
  /** Destructor
    */
  virtual ~EnvId() {}
  /** method that returns the id of the thread/process according to
    * the environment.
    */
  virtual long id() = 0;
  /** method that returns the number of the thread/process according to
    * the environment.
    */
  virtual long nb() = 0;
};

/** The SeqId is the default id stored in the Log().
 */
class SeqId : public EnvId {

public:
  /** Constructor
    */
  SeqId() : EnvId() {}
  /** Destructor
    */
  virtual ~SeqId() { }
  /** Here the method simply returns 0
    */
  virtual long id() {
    return 0L;
  }
  /** Here the method simply returns 1
    */
  virtual long nb() {
    return 1L;
  }
};

/** typedef name_id is a strbuf with a fixed size (15)
  * This type is used to store an identifier as a string
  */ 
typedef strbuff<15> name_id;

/** The abstract class for an identifier
 */
class Id {
protected:
  /// Buffer used to store the name, returns by the str() method.
  name_id st;
public:
  /// Constructor
  Id() : st() { }
  /// Constructor
  Id(const Id &i) : st(i.st) { }
  /// destructor
  virtual ~Id() {}
  /// Method to get the complete string identifier.
  virtual const char *str() const = 0;
  /** Pack method to serialize a Id
    * @param bs the Bob::Serialize object use to pack
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack an Id");
    Bob::Pack(bs, &st);
  }
  /** Pack method to serialize a strbuf
    * @param bs the Bob::DeSerialize object use to unpack
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack an Id");
    Bob::UnPack(bs, &st);
  }

};


/** The class for a char identifier with a parent identifier
 */
class CId : public Id {
protected:
  /// Buffer used to store the name, returns by the id() method.
  char n;
  /** an Id pointer to know the Id parent of this Id.
    * This pointer could be equal to 0 if no parent is defined.
    */
  name_id parent;
public:
  /// Constructor
  CId(): Id(), n(), parent() {
    //DBGAFF_MEM("CId::CId()","Constructor");
  }
  /// Constructor
  CId(char const _n, const Id &f): Id(), n(_n), parent(f.str()) {
    update();
    //DBGAFF_MEM("CId::CId(char const,Id*)","Constructor");
  }
  /// Constructor
  CId(char const _n): n(_n), parent() {
    update();
    //DBGAFF_MEM("CId::CId(char )","Constructor");
  }
  /// Constructor
  CId(const CId &cid): Id(cid), n(cid.n), parent(cid.parent) {
    update();
    //DBGAFF_MEM("CId::CId(CId &)","Constructor");
  }
  /// destructor
  virtual ~CId() {
    //DBGAFF_MEM("CId::~CId","Destructor");
  }
  /// get th identifier of the stat (a char)
  const char get() const {
    return n;
  }
  /// set the name of the Identificator and the parent identificateur
  void set(char _n, const Id &f) {
    n = _n;
    parent.copy(f.str());
    update();
  }
  /// set the name of the Identificator
  void set(char _n) {
    n =_n;
    update();
  }
  /// set the name of the parent Identificator
  void set(const Id &i) {
    parent.copy(i.str());
    update();
  }
  /// Update the internal strbuff.
  void update() {
    st.copy(parent);
    st << ":"<< n;
  }
  /// Method to put the CId on a strbuf
  virtual const char *str() const {
    return st.str();
  }
  /// Method to put the Id on a strbuf
  virtual const char id() const {
    return n;
  }
  /** Pack method to serialize a CId
    * @param bs the Bob::Serialize object use to pack
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack a CId");
    Id::Pack(bs);
    Bob::Pack(bs,&n);
    Bob::Pack(bs,&parent);
  }
  /** Pack method to serialize a strbuf
    * @param bs the Bob::DeSerialize object use to unpack
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack a CId");
    Id::UnPack(bs);
    Bob::UnPack(bs,&n);
    Bob::UnPack(bs,&parent);
  }

};


/** The class for a int identifier with a parent identifier
 */
class NId : public Id {
protected:
  /// Buffer used to store the name, returns by the id() method.
  int n;
  /** an Id pointer to know the Id parent of this Id.
    * This pointer could be equal to 0 if no parent is defined.
    */
  name_id parent;
public:
  /// Constructor
  NId(): Id(), n(), parent() {
    //DBGAFF_MEM("NId::NId()","Constructor");
  }
  /// Constructor
  NId(int const _n, const Id &f): Id(), n(_n), parent(f.str()) {
    update();
    //DBGAFF_MEM("NId::NId(char const,Id*)","Constructor");
  }
  /// Constructor
  NId(int const _n): n(_n), parent() {
    update();
    //DBGAFF_MEM("NId::NId(char )","Constructor");
  }
  /// Constructor
  NId(const NId &nid): Id(nid), n(nid.n), parent(nid.parent) {
    update();
    //DBGAFF_MEM("NId::NId(NId &)","Constructor");
  }
  /// destructor
  virtual ~NId() {
    //DBGAFF_MEM("NId::~NId","Destructor");
  }
  /// get th identifier of the stat (a char)
  const char get() const {
    return n;
  }
  /// set the name of the Identificator and the parent identificateur
  void set(char _n, const Id &f) {
    n = _n;
    parent.copy(f.str());
    update();
  }
  /// set the name of the Identificator
  void set(char _n) {
    n =_n;
    update();
  }
  /// set the name of the parent Identificator
  void set(const Id &i) {
    parent.copy(i.str());
    update();
  }
  /// Update the internal strbuff.
  void update() {
    st.copy(parent);
    st << ":"<< n;
  }
  /// Method to put the NId on a strbuf
  virtual const char *str() const {
    return st.str();
  }
  /// Method to put the Id on a strbuf
  virtual const int id() const {
    return n;
  }
  /** Pack method to serialize a NId
    * @param bs the Bob::Serialize object use to pack
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack a NId");
    Id::Pack(bs);
    Bob::Pack(bs,&n);
    Bob::Pack(bs,&parent);
  }
  /** Pack method to serialize a strbuf
    * @param bs the Bob::DeSerialize object use to unpack
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack a NId");
    Id::UnPack(bs);
    Bob::UnPack(bs,&n);
    Bob::UnPack(bs,&parent);
  }

};

/** The class for a string identifier with a parent identifier
 */
class SId : public Id {
protected:
  /// Buffer used to store the name returns by the id() method.
  strbuff<10> n;
  /** an Id pointer to know the Id parent of this Id.
    * This pointer could be equal to 0 if no parent is defined.
    */
  name_id parent;
  /** method used to update the internal buffer returns by
    * the id() or str() methods.
    */
  void update() {
    if ( parent.size()!=0 ) {
      st.copy(parent);
      st << ":";
    }
    st << n;
  }

public:
  /// Constructor
  SId(): n(""), parent() {
    //DBGAFF_MEM("Sid::Sid()","Constructor");
  }
  /// Constructor
  SId(std::string const &_n): n(_n), parent() {
    update();
    //DBGAFF_MEM("Sid::Sid(string n)","Constructor");
  }
  /// Constructor
  SId(std::string const &_n, const Id &f): n(_n), parent(f.str()) {
    update();
    //DBGAFF_MEM("Sid::Sid(string n,Id*)","Constructor");
  }
  /// Constructor
  SId(char *const &_n): n(_n), parent() {
    update();
    //DBGAFF_MEM("Sid::Sid(char *)","Constructor");
  }
  /// Constructor
  SId(char *const &_n, const Id &f): n(_n), parent(f.str()) {
    update();
    //DBGAFF_MEM("Sid::Sid(char *,Id*)","Constructor");
  }
  /// Constructor
  SId(const SId &sid): n(sid.n), parent(sid.parent) {
    update();
    //DBGAFF_MEM("Sid::Sid(Sid &)","Constructor");
  }
  /// destructor
  virtual ~SId() {
    //DBGAFF_MEM("Sid::~Sid","Destructor");
  }
  /// set the name of the Identificator and the parent identificateur
  void set(std::string &_n, const Id &f) {
    n.flush();
    n.add(_n.c_str());
    parent.copy(f.str());
    update();
  }
  /// set the name of the Identificator
  void set(std::string &_n) {
    n.flush();
    n.add(_n.c_str());
    parent.flush();
    update();
  }
  /// set the name of the Identificator
  void set(char *_n) {
    n.flush();
    n.add(_n);
    parent.flush();
    update();
  }
  /// set the name of the Identificator and the parent identificateur
  void set(char *_n, const Id &f) {
    n.flush();
    n.add(_n);
    parent.copy(f.str());
    update();
  }
  /// set the name of the parent Identificator
  void set(const Id &i) {
    parent.copy(i.str());
    update();
  }
  /// Method to put the CId on a strbuf
  virtual const char *str() const {
    return st.str();
  }
  /// Method to put the Id on a strbuf
  virtual const char * id() const {
    return n.str();
  }
  /** Pack method to serialize a CId
    * @param bs the Bob::Serialize object use to pack
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack a SId");
    Id::Pack(bs);
    Bob::Pack(bs,&n);
    Bob::Pack(bs,&parent);
  }
  /** Pack method to serialize a strbuf
    * @param bs the Bob::DeSerialize object use to unpack
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack a SId");
    Id::UnPack(bs);
    Bob::UnPack(bs,&n);
    Bob::UnPack(bs,&parent);
  }

};

/**
  * @}
  */
}; // end of namspace



#endif
