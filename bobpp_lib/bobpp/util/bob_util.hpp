/*
 * =====================================================================================
 *
 *       Filename:  bob_util.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.09.2008 11:08:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */

#ifndef BOBUTILHEADER
#define BOBUTILHEADER

namespace Bob {
/** @defgroup BOB_UTIL_GRP Utilities
  *  @brief This module describe all classes very usefull to implement Bobpp algorithms.
  *  @ingroup  CoreGRP
  *  @{
  *   
  *   Along the time, different algorithm use same data structures.
  *   Then we group these data on this module.
  *   You can find :
  *     - simple fixed sized string buffer (@ref strbuff) 
  *     - pvector a class that represents a vector which can be instancied 
  *       with all type and have the Pack and UnPack method.
  *     - bit_vector class implemented with pvector.
  *     - Permutation class.
  *     - Multi-dimension array (@ref BOB_UTIL_ARRAY).
  *     - ll(1) parser class (@ref BOB_UTIL_PARSER).
  *   @}
  */

}
#include <bobpp/util/bob_string.hpp>
#include <bobpp/util/bob_misc.hpp>
#include <bobpp/util/bob_array.hpp>
#include <bobpp/util/bob_parse.hpp>

#endif
