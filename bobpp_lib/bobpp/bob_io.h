/*
 * Bob++/bob_io.h
 *
 *  These classes are used  as input/ouput classes for Bob++.
 */
#ifndef BOBIO_ABSTRACT
#define BOBIO_ABSTRACT

#include <sstream>



namespace Bob {

/** @defgroup IOGRP The Bob++ Input/Ouput classes
 *  @brief The  IO classes are used to handle input/ouput features of the Bob++ library
 *  We have defined several classes for input/ouput functionnalities.
 *  We have classified in two category
 *    -# the class used to pack or unpack datas, or serialized the datas.
 *       The serialization could be made on file or on buffer to be sent.
 *    -# the classes used to log the execution.
 *
 *  @ingroup CoreGRP
 *  @{
 */

/** @defgroup BOBOSERIALIZEGRP The Bob++ classes to serialize datas
 *  @brief The Bob++ classes used to serialized or deserialized the Bob++ Classes.
 *
 *  The goal is not to have persistent object, but to have the possibility to write or save
 *  all the data during an execution of an application, to perform stop or resume operation.
 *
 *  Now you can use the global functions Pack and Unpack for most of the types.
 *  The prototypes are
 *    - void Bob::Pack(Bob::Serialize &s,const Type *t,int nb=1);
 *    - void Bob::UnPack(Bob::DeSerialize &s,Type *t,int nb=1);
 *
 *  If you define a class (say Foo) that must be serialized, you have to define both
 *  Pack and UnPack methods.
 *  \code
 *   class Foo {
 *       public:
 *          Foo() { ... }
 *          void Pack(Bob::Serialize &s) const {
 *             s.Pack(&j,1);
 *             Pack(s,&j,1);
 *             Pack(s,&j);
 *             Pack(s,&cost);
 *          }
 *          void UnPack(Bob::DeSerialize &s) {
 *             s.UnPack(&j,1);
 *             UnPack(s,&j,1);
 *             UnPack(s,&j);
 *             UnPack(s,&cost);
 *          }
 *       protected:
 *          int j;
 *          float cost;
 *   }
 *  \endcode
 *
 *  In an other class or code you can also use the form you want.
 *  \code
 *   class BigFoo {
 *       public:
 *          BigFoo() { ... }
 *          void Pack(Bob::Serialize &s) const {
 *             f.Pack(s);
 *             Pack(s,&f);
 *             Pack(s,&f);
 *          }
 *          void UnPack(Bob::DeSerialize &s) {
 *             f.UnPack(s);
 *             UnPack(s,&f);
 *             UnPack(s,&f);
 *          }
 *       protected:
 *          Foo f;
 *   }
 *  \endcode
 *  The goal was to unify the call to pack/unpack, for scalar types and classes.
 *
 *  @ingroup IOGRP
 *  @{
 */

/** The abstract struture to serialize data
 */
struct Serialize {
public:
  /// Constructor
  Serialize() { }
  /// Destructor
  virtual ~Serialize() { }
  /// Pack method for array of bool
  virtual int Pack(const bool *n, int nb) = 0;
  /// Pack method for array of int
  virtual int Pack(const int *n, int nb) = 0;
  /// Pack method for array of unsigned int
  virtual int Pack(const unsigned int *n, int nb) = 0;
  /// Pack method for array of short
  virtual int Pack(const short *s, int nb) = 0;
  /// Pack method for array of unsigned short
  virtual int Pack(const unsigned short *s, int nb) = 0;
  /// Pack method for array of long long
  virtual int Pack(const long long *n, int nb) = 0;
  /// Pack method for array of unsigned long long
  virtual int Pack(const unsigned long long *n, int nb) = 0;
  /// Pack method for array of long
  virtual int Pack(const long *n, int nb) = 0;
  /// Pack method for array of unsigned long
  virtual int Pack(const unsigned long *n, int nb) = 0;
  /// Pack method for array of float
  virtual int Pack(const float *n, int nb) = 0;
  /// Pack method for array of double
  virtual int Pack(const double *n, int nb) = 0;
  /// Pack method for array of char
  virtual int Pack(const char *s, int nb) = 0;
  /// Pack method for array of string
  virtual int Pack(const std::string *s, int nb) {
    int i, len;
    for (i = 0;i < nb;i++) {
      len = (const int)s[i].length();
      Pack(&len, 1);
      Pack(s[i].c_str(), len);
    }
    return nb;
  }
};

/** The abstract struture to deserialize data
 */
struct DeSerialize {
public:
  /// Constructor
  DeSerialize() { }
  /// Destructor
  virtual ~DeSerialize() { }
  /// Is Empty
  virtual bool IsEmpty() {
    return true;
  }
  /// UnPack method for array of bool
  virtual void UnPack(bool *n, int nb) = 0;
  /// UnPack method for array of int
  virtual void UnPack(int *n, int nb) = 0;
  /// UnPack method for array of unsigned int
  virtual void UnPack(unsigned int *n, int nb) = 0;
  /// UnPack method for array of short
  virtual void UnPack(short *s, int nb) = 0;
  /// UnPack method for array of unsigned short
  virtual void UnPack(unsigned short *s, int nb) = 0;
  /// UnPack method for array of long
  virtual void UnPack(long *n, int nb) = 0;
  /// UnPack method for array of unsigned long
  virtual void UnPack(unsigned long *n, int nb) = 0;
  /// UnPack method for array of long long
  virtual void UnPack(long long *n, int nb) = 0;
  /// UnPack method for array of unsigned long long
  virtual void UnPack(unsigned long long *n, int nb) = 0;
  /// UnPack method for array of float
  virtual void UnPack(float *n, int nb) = 0;
  /// UnPack method for array of double
  virtual void UnPack(double *n, int nb) = 0;
  /// UnPack method for array of char
  virtual void UnPack(char *s, int nb) = 0;
  /// UnPack method for array of string
  virtual void UnPack(std::string *s, int nb) {
    int i, len;
    for (i = 0;i < nb;i++) {
      UnPack(&len, 1);
      char buf[len];
      UnPack(buf, len);
      s[i].erase();
      s[i].insert(0, buf, len);
    }
  }
};

/** Global template function to pack a class.
 *  The template parameter class must have the const Pack(Serialize &s)  method.
 *  @param s the serialize object
 *  @param t the objet to serialize.
 */
template<class T>
void Pack(Serialize &s, const T *t) {
  t->Pack(s);
}
/** redefinition of the Pack function for bool type
 */
void Pack(Serialize &s, const bool *t, int nb = 1);
/** redefinition of the Pack function for int type
 */
void Pack(Serialize &s, const int *t, int nb = 1);
/** redefinition of the Pack function for unsigned int type
 */
void Pack(Serialize &s, const unsigned int *t, int nb = 1);
/** redefinition of the Pack function for short type
 */
void Pack(Serialize &s, const short *t, int nb = 1);
/** redefinition of the Pack function for unsigned short type
 */
void Pack(Serialize &s, const unsigned short *t, int nb = 1);
/** redefinition of the Pack function for long type
 */
void Pack(Serialize &s, const long *t, int nb = 1);
/** redefinition of the Pack function for unsigned long type
 */
void Pack(Serialize &s, const unsigned long *t, int nb = 1);
/** redefinition of the Pack function for long long type
 */
void Pack(Serialize &s, const long long *t, int nb = 1);
/** redefinition of the Pack function for unsigned long long type
 */
void Pack(Serialize &s, const unsigned long long *t, int nb = 1);
/** redefinition of the Pack function for double type
 */
void Pack(Serialize &s, const double *t, int nb = 1);
/** redefinition of the Pack function for float type
 */
void Pack(Serialize &s, const float *t, int nb = 1);
/** redefinition of the Pack function for char * type
 */
void Pack(Serialize &s, const char *t, int nb = 1);
/** redefinition of the Pack function for std::string type
 */
void Pack(Serialize &s, const std::string *t, int nb = 1);

/** Global functions to unpack a class
 *  The template parameter class must have the const Pack(Serialize &s)  method.
 * @param s the Deserialze object.
 * @param t the pointer on the object to deserialize.
 */
template<class T>
void UnPack(DeSerialize &s, T *t) {
  t->UnPack(s);
}
/** redefinition of the UnPack function for bool type
 */
void UnPack(DeSerialize &s, bool *t, int nb = 1);
/** redefinition of the UnPack function for int type
 */
void UnPack(DeSerialize &s, int *t, int nb = 1);
/** redefinition of the UnPack function for unsigned int type
 */
void UnPack(DeSerialize &s, unsigned int *t, int nb = 1);
/** redefinition of the UnPack function for short type
 */
void UnPack(DeSerialize &s, short *t, int nb = 1);
/** redefinition of the UnPack function for unsigned short type
 */
void UnPack(DeSerialize &s, unsigned short *t, int nb = 1);
/** redefinition of the UnPack function for long type
 */
void UnPack(DeSerialize &s, long *t, int nb = 1);
/** redefinition of the UnPack function for unsigned long type
 */
void UnPack(DeSerialize &s, unsigned long *t, int nb = 1);
/** redefinition of the UnPack function for long long type
 */
void UnPack(DeSerialize &s, long long *t, int nb = 1);
/** redefinition of the UnPack function for unsigned long long type
 */
void UnPack(DeSerialize &s, unsigned long long *t, int nb = 1);
/** redefinition of the UnPack function for double type
 */
void UnPack(DeSerialize &s, double *t, int nb = 1);
/** redefinition of the UnPack function for float type
 */
void UnPack(DeSerialize &s, float *t, int nb = 1);
/** redefinition of the UnPack function for char * type
 */
void UnPack(DeSerialize &s, char *t, int nb = 1);
/** redefinition of the UnPack function for std::string type
 */
void UnPack(DeSerialize &s, std::string *t, int nb = 1);


/** The structure to serialize data on a file (ofstream)
 */
struct SerialFile : public Serialize {
public:
  /// Constructor
  SerialFile() : Serialize(), f() {
  }
  /** Constructor
    * @param s the name of the file
    */
  SerialFile(std::string const &s) : Serialize(), f(s.c_str()) {
  }
  /// Destructor
  ~SerialFile() {
    f.close();
  }
  /** Open the internal stream
    * @param s the name of the file
    */
  void open(std::string const &s) { f.open(s.c_str()); }
  /// close the internal stream
  void close() { f.close(); }

  /** templatized  Pack method....
   * @param t the array to serialize
   * @param nb the number of elements to serialize
   */
  template<class T>
  int Pack(T *t,int nb) {
    f.write((char*)t, nb*sizeof(T));
    return nb;
  }
  /// Pack method for array of bool
  virtual int Pack(const bool *n, int nb) {
    f.write((char*)n, nb*sizeof(bool));
    return nb;
  }
  /// Pack method for array of int
  virtual int Pack(const int *n, int nb) {
    f.write((char*)n, nb*sizeof(int));
    return nb;
  }
  /// Pack method for array of unsigned int
  virtual int Pack(const unsigned int *n, int nb) {
    f.write((char*)n, nb*sizeof(unsigned int));
    return nb;
  }
  /// Pack method for array of short
  virtual int Pack(const short *n, int nb) {
    f.write((char*)n, nb*sizeof(short));
    return nb;
  }
  /// Pack method for array of unsigned short
  virtual int Pack(const unsigned short *n, int nb) {
    f.write((char*)n, nb*sizeof(unsigned short));
    return nb;
  }
  /// Pack method for array of long
  virtual int Pack(const long *n, int nb) {
    f.write((char*)n, nb*sizeof(long));
    return nb;
  }
  /// Pack method for array of unsigned long
  virtual int Pack(const unsigned long *n, int nb) {
    f.write((char*)n, nb*sizeof(unsigned long));
    return nb;
  }
  /// Pack method for array of long long
  virtual int Pack(const long long *n, int nb) {
    f.write((char*)n, nb*sizeof(long long));
    return nb;
  }
  /// Pack method for array of unsigned long long
  virtual int Pack(const unsigned long long *n, int nb) {
    f.write((char*)n, nb*sizeof(unsigned long long));
    return nb;
  }
  /// Pack method for array of float
  virtual int Pack(const float *n, int nb) {
    f.write((char*)n, nb*sizeof(float));
    return nb;
  }
  /// Pack method for array of double
  virtual int Pack(const double *n, int nb) {
    f.write((char*)n, nb*sizeof(double));
    return nb;
  }
  /// Pack method for array of char
  virtual int Pack(const char *s, int nb) {
    f.write((char*)s, nb*sizeof(char));
    return nb;
  }
protected:
  /// the std::ostream used to write.
  std::ofstream f;
};

/** The structure to deserialize data from a file
 */
struct DeSerialFile : public DeSerialize {
public:
  /// Constructor
  DeSerialFile() : DeSerialize(), f() {
  }
  /** Constructor
    * @param s the name of the file
    */
  DeSerialFile(std::string const &s) : DeSerialize(), f(s.c_str()) {
  }
  /// Destructor
  ~DeSerialFile() {
    f.close();
  }
  /// test if the internal stream is open.
  bool is_open() { return f.is_open(); }
  /** Open the internal stream
    * @param s the name of the file
    */
  void open(std::string const &s) { f.open(s.c_str()); }
  /// close the internal stream
  void close() { f.close(); }
  /// Is Empty
  virtual bool IsEmpty() {
    return f.eof();
  }
  /** templatized  UnPack method....
   * @param t the array to deserialize
   * @param nb the number of elements to deserialize
   */
  template<class T>
  void UnPack(T *t,int nb) {
    f.read((char*)t, nb*sizeof(T));
  }
  /// UnPack method for array of bool
  virtual void UnPack(bool *n, int nb) {
    f.read((char*)n, sizeof(bool)*nb);
  }
  /// Unpack method for array of int
  virtual void UnPack(int *n, int nb) {
    f.read((char*)n, sizeof(int)*nb);
  }
  /// Unpack method for array of unsigned int
  virtual void UnPack(unsigned int *n, int nb) {
    f.read((char*)n, sizeof(unsigned int)*nb);
  }
  /// Unpack method for array of short
  virtual void UnPack(short *n, int nb) {
    f.read((char*)n, sizeof(short)*nb);
  }
  /// Unpack method for array of unsigned short
  virtual void UnPack(unsigned short *n, int nb) {
    f.read((char*)n, sizeof(unsigned short)*nb);
  }
  /// Unpack method for array of long
  virtual void UnPack(long *n, int nb) {
    f.read((char*)n, sizeof(long)*nb);
  }
  /// Unpack method for array of unsigned long
  virtual void UnPack(unsigned long *n, int nb) {
    f.read((char*)n, sizeof(unsigned long)*nb);
  }
  /// Unpack method for array of long long
  virtual void UnPack(long long *n, int nb) {
    f.read((char*)n, sizeof(long long)*nb);
  }
  /// Unpack method for array of unsigned long long
  virtual void UnPack(unsigned long long *n, int nb) {
    f.read((char*)n, sizeof(unsigned long long)*nb);
  }
  /// Unpack method for array of float
  virtual void UnPack(float *n, int nb) {
    f.read((char*)n, sizeof(float)*nb);
  }
  /// Unpack method for array of double
  virtual void UnPack(double *n, int nb) {
    f.read((char*)n, sizeof(double)*nb);
  }
  /// Unpack method for array of char
  virtual void UnPack(char *s, int nb) {
    f.read((char*)s, sizeof(char)*nb);
  }
protected:
  /// the ifstream used to read the data.
  std::ifstream f;
};

/**
 * @}
 */


/**
 * @}
 */

};
#endif
