
#ifndef __BOB_STRING_HPP__
#define __BOB_STRING_HPP__

#include <iostream>

namespace Bob {


/** The Bob++ strbuff class
  * \ingroup BOB_UTIL_GRP
  *
  * This class has been implemented to avoid the use of the std::string
  * in the code. The std::string use many memory allocations, then in a 
  * multithreaded environment, malloc calls yield to poor performances.
  *
  * The size of the internal buffer is given using the template parameter.
  * The default size is 1024.
  *
  * We have also added conversion methods from other basic types.
  */
template < int n = 1024 >
class strbuff {
  char *e;
  char b[n+1];

  void tcapa(int i) {
    if ((e - b) >= n) {
      std::cerr << "N:" << (e - b) << " a " << i << "\n";
      e = b + n;
      *e = 0;
      std::cerr << "Warning Capacity :(" << (e - b) << "/" << n << ":|" << b << "|\n";
    }
  }
public:
  /** Constructor
    */
  strbuff() : e(b) {
    *b = 0;
  }
  /** Constructor
    * @param s a char * to initialize s.
    */
  strbuff(const char s): e(b) {
    add(s);
  }
  /** Constructor
    * @param s a char * to initialize s.
    */
  strbuff(const char *s): e(b) {
    add(s);
  }

  /** Constructor
    * @param s an strbuf<n>
    */
  strbuff(const strbuff<n> &s): e(b) {
    add(s);
  }
  /** Constructor
    * @param s a char * to initialize s.
    */
  strbuff(const std::string &s): e(b) {
    add(s.c_str());
  }

  /** returns the data in a char * type.
    */
  const char *str() const {
    return b;
  }
  /** return the size of the internal data
    */
  int size() const {
    return e -b;
  }
  /** add method
    * @param s the string to add to the strbuff
    */
  void add(const  std::string s) {
    strcpy(e, s.c_str());
    e += strlen(s.c_str());
    tcapa(1);
  }
  /** add method
    * @param s a char * to add to the strbuff
    */
  void add(const  char *s) {
    strcpy(e, s);
    e += strlen(s);
    tcapa(2);
  }
  /** add method
    * @param c a char to add to the strbuff
    */
  void add(const  char c) {
    *e = c;
    e++;
    *e = 0;
    tcapa(3);
  }
  /** add method
    * @param i the int to add to the strbuff
    */
  void add(const  int i) {
    sprintf(e, "%d", i);
    e += strlen(e);
    tcapa(4);
  }
  /** add method
    * @param l the long to add to the strbuff
    */
  void add(const  long l) {
    sprintf(e, "%ld", l);
    e += strlen(e);
    tcapa(5);
  }
  /** add method
    * @param l the unsigned long to add to the strbuff
    */
  void add(const unsigned long l) {
    sprintf(e, "%lu", l);
    e += strlen(e);
    tcapa(5);
  }

  /** add method
    * @param l the long long to add to the strbuff
    */
  void add(const long long l) {
    sprintf(e, "%lld", l);
    e += strlen(e);
    tcapa(10);
  }
  /** add method
    * @param l the unsigned long long to add to the strbuff
    */
  void add(const unsigned long long l) {
    sprintf(e, "%llu", l);
    e += strlen(e);
    tcapa(6);
  }
  /** add method
    * @param f the float to add to the strbuff
    */
  void add(const  float f) {
    sprintf(e, "%f", f);
    e += strlen(e);
    tcapa(7);
  }
  /** add method
    * @param d the double to add to the strbuff
    */
  void add(const  double d) {
    sprintf(e, "%lf", d);
    e += strlen(e);
    tcapa(8);
  }
  /** add method
    * @param s an strbuff to add to string
    */
  template<int nt>
  void add(strbuff<nt> const &s) {
    strcpy(e, s.str());
    e += s.size();
    tcapa(9);
  }
  /** copy method
    * @param s an strbuff to copy
    */
  template<int nt>
  void copy(strbuff<nt> const &s) {
    e=b;strcpy(e, s.str());
    e += s.size();
    tcapa(10);
  }
  /** copy method
    * @param s an strbuff to copy
    */
  void copy(const char *s) {
    e=b;strcpy(e, s);
    e += strlen(s);
    tcapa(10);
  }

  /** operator<<
    * @param t a templatized variable to add to the strbuff.
    */
  template<class T>
  strbuff<n> & operator<<(const T&t) {
    add(t);
    return *this;
  }

  /** the classical operator to output a strbuff
    * @param os the output stream
    * @param s the strbuff to be displayed
    */
  friend std::ostream &operator<<(std::ostream &os, const strbuff<n> &s) {
    return os << s.b;
  }

  /** flush method
    * method to empty the buffer.
    */
  void flush() {
    *b = 0;
    e = b;
  }
  /** Pack method to serialize a strbuff
    * @param bs the Bob::Serialize object use to pack
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack a strbuf");
    Bob::Pack(bs, b);
  }
  /** Pack method to serialize a strbuf
    * @param bs the Bob::DeSerialize object use to unpack
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack a strbuf");
    Bob::UnPack(bs, b);
  }

};


};

#endif
