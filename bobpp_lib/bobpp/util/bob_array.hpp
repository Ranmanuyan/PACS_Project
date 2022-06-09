#ifndef __ARRAY_HPP__
#define __ARRAY_HPP__

#include <iostream>
#include <bobpp/bobpp.h>

//#define __USE_CELL

namespace Bob {

/** @defgroup BOB_UTIL_ARRAY A n dimensinal array
  * @brief This module contains an implemntation of n-dimensional array.
  * @ingroup BOB_UTIL_GRP 
  *
  * This module describes an easy to use n-dimension array, templatized by the
  * type of the data to store.
  * The Bob::array class is the main container. This class must be use to allocate
  * a n-dimension array (8 is the maximal number of dimensions).
  * We have also implented the Bob::view class which allows to get a reduced 
  * view of an array without copy.
  * @{
  */

/** class that represent an view of an array.
  */
template <class type>
class view {
  view(type *_data, unsigned long *_count) : data(_data), count(_count) {
    //std::cout << "view::view data=" <<data << " count="<<count
  }
public:
  /** Constructor
    */
  view() : data(0), count(0) {}
  /** Destructor
    */
  virtual ~view() {}
  /** operator[] to get the value at a given position.
    * @param i the position
    */
  type &operator[](int i) {
    return data[i];
  }
  /** functor to get the value at a given position for 1 dimensions.
    * @param i the position in the first dimension
    */
  type &operator()(int i) {
    return data[i];
  }

  /** functor to get the value at a given position for 2 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    */
  type &operator()(int i, int j) {
    return data[count[0]*i + j];
  }
  /** functor to get the value at a given position for 3 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    */
  type &operator()(int i, int j, int k) {
    return data[count[0]*i + count[1]*j + k];
  }
  /** functor to get the value at a given position in 4 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    */
  type &operator()(int i, int j, int k, int l) {
    return data[count[0]*i + count[1]*j + count[2]*k + l];
  }
  /** functor to get the value at a given position in 5 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    */
  type &operator()(int i, int j, int k, int l, int m) {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + m];
  }
  /** functor to get the value at a given position in 6 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n) {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + n];
  }
  /** functor to get the value at a given position in 7 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    * @param o the position in the seventh dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n, int o) {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + count[5]*n + o];
  }
  /** functor to get the value at a given position in 8 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    * @param o the position in the seventh dimension
    * @param p the position in the height dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n, int o, int p) {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + count[5]*n + count[6]*o + p];
  }

  /** functor to get the value at a given position.
    * @param i the position
    */
  type &operator()(int i) const {
    return data[i];
  }
  /** functor to get the value at a given position for 2 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    */
  type &operator()(int i, int j) const {
    return data[count[0]*i + j];
  }
  /** functor to get the value at a given position for 3 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    */
  type &operator()(int i, int j, int k) const {
    return data[count[0]*i + count[1]*j + k];
  }
  /** functor to get the value at a given position in 4 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    */
  type &operator()(int i, int j, int k, int l) const {
    return data[count[0]*i + count[1]*j + count[2]*k + l];
  }
  /** functor to get the value at a given position in 5 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    */
  type &operator()(int i, int j, int k, int l, int m) const {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + m];
  }
  /** functor to get the value at a given position in 6 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n) const {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + n];
  }
  /** functor to get the value at a given position in 7 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    * @param o the position in the seventh dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n, int o) const {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + count[5]*n + o];
  }
  /** functor to get the value at a given position in 8 dimensions.
    * @param i the position in the first dimension
    * @param j the position in the second dimension
    * @param k the position in the third dimension
    * @param l the position in the fourth dimension
    * @param m the position in the fifth dimension
    * @param n the position in the sixth dimension
    * @param o the position in the seventh dimension
    * @param p the position in the height dimension
    */
  type &operator()(int i, int j, int k, int l, int m, int n, int o, int p)
  const {
    return data[count[0]*i + count[1]*j + count[2]*k + count[3]*l + count[4]*m
                + count[5]*n + count[6]*o + p];
  }
  /** method to get a sub view (dimension n-1) from the view (dimension n).
    * @param i the index in the first dimension
    */ 
  view<type> getview(int i) const {
    return view<type>(view<type>::data + view<type>::count[0]*i,
                      view<type>::count + 1);
  }
  /** method to get a sub view (dimension n-2) from the view (dimension n).
    * @param i the index in the first dimension
    * @param j the index in the second dimension
    */ 
  view<type> getview(int i, int j) const {
    return view<type>(view<type>::data + view<type>::count[0]*i
                      + view<type>::count[1]*j, view<type>::count + 2);
  }
  /** method to get a sub view (dimension n-3) from the view (dimension n).
    * @param i the index in the first dimension
    * @param j the index in the second dimension
    * @param k the index in the third dimension
    */ 
  view<type> getview(int i, int j, int k) const {
    return view<type>(view<type>::data + view<type>::count[0]*i
                      + view<type>::count[1]*j + view<type>::count[2]*k, view<type>::count + 3);
  }
  /** method to get a sub view (dimension n-4) from the view (dimension n).
    * @param i the index in the first dimension
    * @param j the index in the second dimension
    * @param k the index in the third dimension
    * @param l the index in the fourth dimension
    */ 
  view<type> getview(int i, int j, int k, int l) const {
    return view<type>(view<type>::data + view<type>::count[0]*i
                      + view<type>::count[1]*j + view<type>::count[2]*k
                      + view<type>::count[3]*l, view<type>::count + 4);
  }
  /** method to display a view 
    * @param os the ostream 
    */ 

  virtual void Prt(std::ostream &os) const {
    int i = 0;
    os << "data=" << data << "\ncount=" << count << "\n";
    if ( count!=0 ) {
      while (count[i] != 0) {
         os << "count[" << i << "]=" << count[i] << " ";
         i++;
      }
      os << std::endl;
    }
  }
protected:
  /// the 1-dimension array storing the data.
  type *data;
  /// the 1-dimension array storing the size of each dimension.
  unsigned long *count;
};

/** The Array class
  * This class inherits form the view class in order to get each
  * method to manipulate the data.
  * The array class adds the initialization methods.
  * 
  */
template <class type, int align0=1, int align1=1, int align2=1>
class array : public view<type> {
  void alloc_data() {
    blk = new type[view<type>::count[0]*n0+align/sizeof(type)-1];
    view<type>::data =
        (type*) ((((unsigned long)blk)+align-1)&(-(unsigned long)align));
  }
  void free_data() {
    delete [] blk;
  }
public:
  /** Constructor
    */
  array() : view<type>(), blk(0), n0(0), align(1) {}
  /** Constructor for a n dimension array (8 is the maximum).
    */
  array(int a0, int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0, int a5 = 0,
        int a6 = 0, int a7 = 0): view<type>(), blk(0), n0(a0), align(1) {
    init(a0, a1, a2, a3, a4, a5, a6, a7);
  }
  /** constructor with copy
    */
  array(const array<type> &at) : view<type>() {
    copy(at);
  }
  /** Destructor
    */
  virtual ~array() {
    clear();
  }
  /** copy method
    */
  virtual void copy(const array &a)  {
    n0 = a.n0;
    align = a.align;
    if (n0 != 0) {
      view<type>::count = new unsigned long[8-1];
      memcpy(view<type>::count, a.count, 7*sizeof(unsigned long));
      alloc_data();
      memcpy(view<type>::data, a.data, view<type>::count[0]*n0*sizeof(type));
    }
  }
  /** init method to allocate a non-initialized array (8 dimension is the maximum).
    */
  void init(int a0, int a1 = 0, int a2 = 0, int a3 = 0, int a4 = 0, int a5 = 0, int a6 = 0,
            int a7 = 0) {
    int size[8];
    unsigned int aligns[3];
    int i, j, last;
    unsigned int g;
    unsigned long num = 1;
    unsigned long *&count = view<type>::count;
    size[0] = a0;
    n0 = a0;
    size[1] = a1;
    size[2] = a2;
    size[3] = a3;
    size[4] = a4;
    size[5] = a5;
    size[6] = a6;
    size[7] = a7;
    aligns[0] = align0;
    aligns[1] = align1;
    aligns[2] = align2;
    align = sizeof(type);
    for(i=0; i<3; i++)
      if (align < aligns[i])
        align = aligns[i];
    last=7;
    while (size[last] == 0)      // look for last dimension
      last--;
    size[last] *= sizeof(type);
    count = new unsigned long[8-1];
    j = 0;
    for (i = last; i > 0; i--) {
      num *= size[i];
      if (j < 3) {
        g = aligns[j++];
        num = (num+g-1)&-g;
      }
      count[i-1] = num / sizeof(type);
    }
    alloc_data();
  }
  /** clear and delete each internal data
    */
  void clear() {
    if (view<type>::data != 0) {
      free_data();
      delete [] view<type>::count;
    }
    view<type>::data = 0;
    view<type>::count = 0;
    n0 = 0;
  }
  /** Test if the array is allocated or not.
    * @return true if the array is already allocated.
    */
  bool isEmpty() {
    return view<type>::data == 0;
  }

  /// Pack method to serialize the array
  virtual void Pack(Bob::Serialize &bs) const {
    bs.Pack(&n0, 1);
    bs.Pack(&align, 1);
    if (n0 != 0) {
      bs.Pack(view<type>::count, 7);
      bs.Pack(view<type>::data, view<type>::count[0]*n0);
    }
  }
  /// Unpack method to deserialize the array
  virtual void UnPack(Bob::DeSerialize &bs)  {
    bs.UnPack(&n0, 1);
    bs.UnPack(&align, 1);
    if ( n0!=0 ) {
       view<type>::count = new unsigned long[7];
       bs.UnPack(view<type>::count, 7);
       alloc_data();
       bs.UnPack(view<type>::data, view<type>::count[0]*n0);
    } else {
      view<type>::count = 0;
      view<type>::data = 0;
    }
  }
protected:
  /// the pointer to the array that store the data.
  type *blk;
  /// the size of the first dimension array.
  int n0;
  /// offset to align array in block (for SSE operations).
  unsigned int align;
};

  /** @}
  */
}

#endif
