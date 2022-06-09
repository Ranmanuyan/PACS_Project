/*
 *        BOBO version 1.0:  Branch and Bound Optimization LiBrary
 *                    Opale Team of PRiSM laboratory
 *             University of Versailles St-Quentin en Yvelines.
 *                     Authors:  B. Le Cun F. Galea
 *                    (C) 2008 All Rights Reserved
 *
 *                              NOTICE
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted
 * provided that the above copyright notice appear in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * Neither the institutions (Versailles University, PRiSM Laboratory,
 * the Opale Team), nor the Authors make any representations about the
 * suitability of this software for any purpose.  This software is
 * provided ``as is'' without express or implied warranty.
 *
 */

/*
 *  File   : bob_misc.hpp
 *  Author : B. Le Cun 
 *  Date   : Nov 11 2007.
 *  Comment: Misc Source file  which propose different data structures 
 *  very hepfull to develop Bobpp applications.
 */


#ifndef BOBMISCHEADER
#define BOBMISCHEADER

#include<vector>

namespace Bob {


/** Template class which inherits std::vector
 * the goal is to add the pack and unpack methods
 * \ingroup BOB_UTIL_GRP
 *
 */
template <class T>
class pvector : public std::vector<T> {
public:
  /** Constructor
    */
  pvector() : std::vector<T>() {}
  /** Constructor
    * @param s the size of the pvector
    * @param t the default value
    */
  pvector(int s, T t = T()) : std::vector<T>(s, t) {}
  /** Copy Constructor
    * @param pt the pvector to copy
    */
  pvector(const pvector<T> &pt) : std::vector<T>(pt) { }
  /** Destructor
    */
  virtual ~pvector() {}

  /** copy method
    * @param p the vector to copy
    */
  virtual void copy(const pvector<T> &p)  {
    *this = p;
  }
  /** method to display a pvector
    * @param os the ostream 
    */
  virtual void Prt(std::ostream &os) const {
    os << "v :";
    for (unsigned int i = 0;i < std::vector<T>::size();i++) {
      os << std::vector<T>::at(i) << ",";
    }
    os << std::endl;
  }

  /** Pack method to serialize the pvector  
    * @param bs the Bob::Serialize object use to pack the pvector object
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("pvector::Pack", "Pack a pvector");
    const int s = std::vector<T>::size();
    Bob::Pack(bs, &s, 1);
    for (int i = 0;i < s;i++) {
      const T *v = &((*this)[i]);
      Bob::Pack(bs, v);
    }
  }
  /** Pack method to serialize the pvector  
    * @param bs the Bob::DeSerialize object use to unpack the pvector object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("pvector::UnPack", "UnPack a pvector");
    int s;
    Bob::UnPack(bs, &s, 1);
    std::vector<T>::resize(s);
    for (int i = 0;i < s;i++) {
      T *v = &((*this)[i]);
      Bob::UnPack(bs, v);
    }
  }
};

/** template << operator to display a pvector
  * \ingroup BOB_UTIL_GRP
  * @param os the ostream object to use
  * @param p the pvector to display
  */
template <class T>
std::ostream &operator<<(std::ostream &os,const pvector<T> &p) {
    p.Prt(os);
    return os;
}

/** number of bit in a int
 */
#define NBITINT (8*sizeof(int))

/** Class bit_vector based on the pvector class
 * \ingroup BOB_UTIL_GRP
 */
class bit_vector : public pvector<int> {
  public :
    /** constructor
     */
    bit_vector() : pvector<int>() { }
    /** constructor
     * @param nbit the number of bits
     */
    bit_vector(int nbit) : pvector<int>((nbit+NBITINT-1)/NBITINT,0),number_of_bits(nbit) {
    }
    /** constructor
     * @param bv the number of bits
     */
    bit_vector(const bit_vector &bv) : pvector<int>(bv),number_of_bits(bv.number_of_bits) {
    }
    /** resize the array to increase the number of bits
     * @param nbit the number of bits
     */
    void resize(int nbit) { number_of_bits=nbit;std::vector<int>::resize((nbit+NBITINT-1)/NBITINT,0); }
    /** returns the number of bits
     * @returns the number of stored bits
     */
    int size() const { return number_of_bits; /*std::vector<int>::size()*NBITINT;*/ }
    /** Set the ith bit to the value v
     * @param i  the index of bit
     * @param v  the value to affect (0|1)
     */
    void set(int i,int v) { (*this)[i/NBITINT]|= (v&1)<<(i%NBITINT); }
    /** Set the ith bit
     * @param i 
     */
    void set(int i) { (*this)[i/NBITINT]|= 1<<(i%NBITINT); }
    /** Unset the ith bit
     * @param i 
     */
    void unset(int i) { (*this)[i/NBITINT]&= ~(1<<(i%NBITINT)); }
    /** Unset the ith bit
     * @param i 
     */
    int get(int i) const { return (((*this)[i/NBITINT]>>(i%NBITINT)) & 1) ; }
    /** test if the ith bit is set or not
     * @param i 
     */
    bool test(int i) const { return ((*this)[i/NBITINT]& (1<<(i%NBITINT)))!=0 ; }
   protected: 
    int number_of_bits;
};

/** operator to display the bit_vector
  */
inline std::ostream &operator<<(std::ostream &os,const bit_vector &b) {
    for (int i=0;i<b.size();i++) {
      os << (b.test(i)?1:0);
    }
    return os;
}

/** Permutation class
 * \ingroup BOB_UTIL_GRP
 * In many optimization problem the solution is stored as a permutation.
 * This class must be used to store this kind of solution.
 * We named i the facility to put on a location named j.
 * A facility could be assigned or non assigned, a location j could free or not.
 * The origin of this implementation was in a scheduling problem resolution. Then 
 * methods like fixeR resp. fixeD was to fixe a task according to its release
 * date resp. its due date. 
 */
class Permutation {
protected:
  /// size of the permutation
  int nb;
  /// number of free locations
  int nbfree;
  int r, /*!< indice of the first free locations */
      d; /*!< indice of the last free locations */
  /// pvector giving the location from the facility
  pvector<int> i2j;
  /// pvector giving the facilty from the location  
  pvector<int> j2i;
public:
  /** Constructor
    */
  Permutation() : nb(0), nbfree(0), r(0), d(0), i2j(), j2i() {  }
  /** Constructor
    * @param s the size of the permutation
    */
  Permutation(int s) : nb(s), nbfree(s), r(0), d(s - 1), i2j(s, -1), j2i(s, -1) {
  }
  /** Constructor
    * @param p the permutation to copy
    */
  Permutation(const Permutation &p): nb(p.nb), nbfree(p.nbfree), r(p.r), d(p.d), i2j(p.i2j), j2i(p.j2i) {
  }
  /** Destructor
    */
  virtual ~Permutation() {  }
  /** copy method
   * @param p the permutation to copy
   */
  virtual void copy(const Permutation &p)  {
    nb = p.nb;
    nbfree = p.nbfree;
    r = p.r;
    d = p.d;
    i2j.copy(p.i2j);
    j2i.copy(p.j2i);
  }
  /** size of the permutation
    */
  int size() const {
    return nb;
  }
  /** Method to initialize the permutation object
    * each facility is non assigned and each location is free.
    * @param s the size of the permutation
    */
  void init(int s) {
    nbfree = nb = s;
    r = 0;
    d = nb - 1;
    i2j.resize(nb, -1);
    j2i.resize(nb, -1);
  }
  /** Assign a facility i on the next free location
    * @param i the facility to put on the nex free location
    */
  void fixeR(int i) {
    fixe(i, r);
    r++;
  }
  /** Assign a facility i on the last free location
    * @param i the facilty to put on the last free location
    */
  void fixeD(int i) {
    fixe(i, d);
    d--;
  }
  /** get the last fixed facility at the beginning of the permutation
    */
  int getFacR() { if ( r>0 ) return j2i[r-1]; else return -1; }
  /** get the last fixed location at the beginning of the permutation
    */
  int getLocR() { if ( r>0 ) return r-1; else return -1; }
  /** get the last fixed facility at the beginning of the permutation
    */
  int getFacD() { if ( d<nb-1 ) return j2i[d+1]; else return -1; }
  /** get the last fixed location at the beginning of the permutation
    */
  int getLocD() { if ( d<nb-1 ) return d+1; else return -1; }

  /** Assign a facilty i to the location j
    * @param i facility to put on the j location
    * @param j the location
    */
  void fixe(int i, int j) {
    i2j[i] = j;
    j2i[j] = i;
    nbfree--;
  }
  /** get the facility at the given location
    * @param j the location
    * @return the facility, -1 if the location is free
    */
  int geti(int j) const {
    return j2i[j];
  }
  /** get the assigned location of a facilty
    * @param i the facility
    * @return the location assigned to the facility, -1 if tha facility is not assigned
    */
  int getj(int i) const {
    return i2j[i];
  }
  /** get the pvector to get the locations from the facilities.
    * @return a reference on the pvector.
    */
  const pvector<int> &get_i2j() const {
    return i2j;
  }
  /** get the pvector to get the facilities form the locations.
    * @return a reference on the pvector.
    */
  const pvector<int> &get_j2i() const {
    return j2i;
  }
  /** get the number of free facilities/locations.
    * @return int number of free location.
    */
  int nbFree() const {
    return nbfree;
  }
  /** test if the facility is assigned.
    * @param i the facility to test.
    * @return true if the facility is assigned.
    */
  bool isfixed(int i) const {
    return i2j[i] != -1;
  }
  /** test if the facility is not assign, is free.
    * @param i the facility to test.
    * @return true if the facility is free.
    */
  bool isfree(int i) const {
    return i2j[i] == -1;
  }
  /** test if the location is free.
    * @param j the location to test.
    * @return true if the location is not free.
    */
  bool isaffect(int j) const { return j2i[j]!=-1; }
  /** Test if this possible to fixe on the last location.
    * @return true a location is free.
    */
  bool fixeOnR() const {
    return r <= (nb - 1 - d);
  }
  /** test there exists free locations/values
    * @return true if each value has been fixed
    */
  bool isallfixed() const {
    return nbfree == 0;
  }
  /** Display a Permutation
    * @param os the ostream used to display.
    */
  virtual ostream &Prt(ostream &os) const {
    os << "values    :";
    for (int i = 0;i < nb;i++) {
      os << j2i[i] << ",";
    }
    os << std::endl;
    os << "locations :";
    for (int i = 0;i < nb;i++) {
      os << i2j[i] << ",";
    }
    os << std::endl;
    return os;
  }
  /** Pack method to serialize the permutation
    * @param bs the Serialize object 
    */
  virtual void Pack(Bob::Serialize &bs)  const {
    DBGAFF_PCK("Permutation::Pack", "Pack a permutation");
    bs.Pack(&nb, 1);
    bs.Pack(&nbfree, 1);
    bs.Pack(&r, 1);
    bs.Pack(&d, 1);
    i2j.Pack(bs);
    j2i.Pack(bs);
  }
  /** Unpack method to deserialize the permutation
    * @param bs the DeSerialize object 
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    DBGAFF_PCK("Permutation::UnPack", "UnPack a permutation");
    bs.UnPack(&nb, 1);
    bs.UnPack(&nbfree, 1);
    bs.UnPack(&r, 1);
    bs.UnPack(&d, 1);
    i2j.UnPack(bs);
    j2i.UnPack(bs);
  }

};

}

#endif
