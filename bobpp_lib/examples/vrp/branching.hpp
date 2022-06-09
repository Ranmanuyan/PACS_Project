/*
 * =====================================================================================
 *
 *       Filename:  branching.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  22.09.2008 15:25:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */

template<class Trait>
class vrp_branching_info : public Bob::Generic_BranchingInfo<Trait> {
  int var;
  double val;
public :
  /// Type definition of the LP.
  typedef typename Trait::LP TheLP;
  typedef typename Trait::Node TheNode;

  /// Constructor
  vrp_branching_info(): var(-1) { }
  /** Constructor
   * @param _v the index of the variable to branch
   * @param _d the value of the variable.
   */
  vrp_branching_info(int _v, double _d): var(_v) { }
  /** Destructor
    */
  virtual ~vrp_branching_info() {}
  /** returns the index of the variable to branch
    * @return the index of the variable on the LP
    */
  int get_var() {
    return var;
  }
  /** Sets the index of the variable to branch
    * @param _v the index of the variable on the LP
    */
  void set_var(int _v) {
    var = _v;
  }
  /** Sets the value of the variable to branch
    * @param _v the index of the variable on the LP
    */
  void set_val(double _v) {
    val = _v;
  }

  /** method that performs the ith branch on a LP.
    * @param i the index of the branch
    * @param n the node to branch
    */
  virtual void do_branch(int i, TheNode *n) {
    DBGAFF_USER("MipNode::addCut()", "Called");
    TheLP *lp=n->Lp();
    double bd;
    if (i == 0) {
      bd = 1;
      lp->set_col_bounds(&bd, NULL, var, var);
      cout<<"do_branch: "<<var<<" > "<<bd<<endl;
    } else {
      bd = 0;
      lp->set_col_bounds(NULL, &bd, var, var);
      cout<<"do_branch: "<<var<<" < "<<bd<<endl;
    }
  }
  /** Here this method returns 2.
    * @return the number of children.
    */
  virtual int nbChildren() {
    return 2;
  }
  /** Method to pack the information
    * @param bs the Bob::Serialize object
    */
  virtual void Pack(Bob::Serialize &bs) const {
    bs.Pack(&var, 1);
  }
  /** Method to unpack the information
    * @param bs the Bob::DeSerialize object
    */
  virtual void UnPack(Bob::DeSerialize &bs)  {
    bs.UnPack(&var, 1);
  }

};
