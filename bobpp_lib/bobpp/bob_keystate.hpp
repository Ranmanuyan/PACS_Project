
/*
 * Bobpp/src/bob_keystate.h
 *
 * This source include the abstract data type for the Algo level where
 */

#include <string>


#ifndef BOBKEYSTATE_HEADER
#define BOBKEYSTATE_HEADER
namespace Bob {


/*---------------------------------------------------------------------*/
/** @defgroup KeyFunctGRP Keys class.
 *  @ingroup SimSpAlgoGRP
 *  @brief class to store a key associated to a node.
 * @{
 *  In the simulation space algorithm, we need to identify a node. 
 *  We propose a Key class use to identify the nodes.
 */
/*---------------------------------------------------------------------*/


/** Class that represents a Key to identify the state 
  *  node.
  */
class Key {

public:
  /** Constructor
    */
  Key() { }
  /** Copy constructor
    */
  Key(const Key &c) { }
  /** Destructor
    */
  ~Key() { }
  /** method to convert the key to a long type
    */
  virtual long to_long() const =0; 
  /** method to check the two keys are comparable
    */
  virtual bool is_comparable( const Key &k)  const =0;
  /** Copy method 
    */
  virtual void copy(const Key &k)=0;
  /** Pack method to Serialize the Key
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs)  const =0;
  /** Unpack method to deserialize the Key 
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  =0;
};


/** Simple key which is represents by a long int
  *  
  */
class LongKey {
protected:
 /** the long int storing the key
   */
 long l;
public:
  /** Constructor
    */
  LongKey(): l(0) { }
  /** Constructor
    */
  LongKey(long _l): l(_l) { }
  /** Copy constructor
    */
  LongKey(const LongKey &k): l(k.l) { }
  /** Destructor
    */
  ~LongKey() { }
  /** method to convert the key to a long type
    */
  virtual long to_long() const {return l; } 
  /** set the Key value.
    */
  virtual void set(long _l) { l=_l; }
  /** method to check the two keys are comparable
    */
  virtual bool is_comparable( const LongKey &k) const { return true; }
  /** Copy method 
    */
  virtual void copy(const LongKey &k) { l=k.l; }
  /** Pack method to Serialize the LongKey
   * @param bs the DeSerialize object
   */
  virtual void Pack(Serialize &bs)  const {
    DBGAFF_PCK("LongKey::Pack", "Packing LongKey");
    bs.Pack(&l,1);
  }
  /** Unpack method to deserialize the LongKey
   * @param bs the DeSerialize object
   */
  virtual void UnPack(DeSerialize &bs)  {
    DBGAFF_PCK("LongKey::UnPack", "Unpacking LongKey");
    bs.UnPack(&l,1);
  }

};

/** @}
  */
}
#endif
