/*
 * =====================================================================================
 *
 *       Filename:  genchild.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.09.2008 10:53:55
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */

class VrpGenChild : public Bob::BBLPGenChild<VrpTrait> {
   public:
      VrpGenChild(const VrpInstance* vi, Bob::BBAlgo<VrpTrait>* ai):Bob::BBLPGenChild<VrpTrait> (vi,ai) {}
};
