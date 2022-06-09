
/*------- Class for Errors Handling ------------------------------*/
#ifndef BOBERROR
#define BOBERROR

#include <iostream>
#include <sstream>
#define BOBUSEDEBUG

namespace Bob {

/** @defgroup ErrGRP The error handling
 *  @brief this module stores the Base exception class  to handle Bob++ error.
  * @ingroup CoreGRP
 *  @{
 *
  */


/** Exception: base class for exception hierarchy
Base class for hierarchy of error classes.
Match the ANSI/C++ proposal for the hierarchy.
*/
class Exception {
public:
  /** Default constructor
  * @param _s is the string that represents type of the exception class
  * @param _m is the message of the execption
   */
  Exception(const std::string &_s, const std::string &_m) : str(_s), m(_m) {}
  /** Destructor
   */
  virtual ~Exception() {}

  /** Output message to stream
      @param o an output stream
   */
  virtual std::ostream& print(std::ostream& o) const;

  /** Return an human readable string for the error message
   */
  virtual const char* what() const;


protected:
  /// the string that stores the type of the exception
  std::string str;
  /// the string that stores the message
  std::string m;
};



// --------------------------------------------------------------------
/** Option Exception. An object of this class is throwed
 * there is error in the Bob++ option related classes.
*/
class OptionException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  OptionException(const std::string& msg)
      : Exception("Option Exception :", msg) {}
};

/** Parse Exception. An object of this class is throwed
 * there is error in the Bob++ parse classes.
*/
class ParseException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  ParseException(const std::string& msg)
      : Exception("Parse Exception :", msg) {}
};


/** Facade Exception. An object of this class is throwed
 * when a facade has not its real implementation
*/
class FacadeException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  FacadeException(const std::string& msg)
      : Exception("Facade Exception :", msg) {}
};

/** Null pointer Exception. An object of this class is throwed
 * when a class has a null pointer on one of its members
*/
class NullPointerException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  NullPointerException(const std::string& msg)
      : Exception("Null pointer Exception:", msg) {}
};


/** State Exception. An object of this class is throwed
 * when a method is invocated on a class which is not in the good state
*/
class StateException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  StateException(const std::string& msg)
      : Exception("State Exception:", msg) {}
};

/** State Exception. An object of this class is throwed
 * when a method is invocated on a class which is not in the good state
*/
class NonMasterException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  NonMasterException(const std::string& msg)
      : Exception("Non Master Exception:", msg) {}
};

/** Reference Exception. An object of this class is throwed
 * when an object is deleted when its ref!=0 meaning that this object
 * is in used !
*/
class ReferenceException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  ReferenceException(const std::string& msg)
      : Exception(" Reference Exception:", msg) {}
};

/** User Exception. Simple exception class for the user side.
*/
class UserException: public Exception {
public:
  /** Constructor
  * @param msg the message to display
  */
  UserException(const std::string& msg)
      : Exception(" USER Exception:", msg) {}
};




/** Abort the running process
*/
extern void System_abort(const Exception& e);

/**@name Static methods to throw an exception.
    The purpose of this method is to be able to set a breakpoint on throw
    @param err a reference to an Exception object
    @exception the err object
*/
void __bob_Exception_throw(const Exception& err);
template<class T>
void Exception_throw(const T& ex) throw(T) {
  __bob_Exception_throw(ex);
  throw ex;
}


/// Assertion that could be used in all source. This could be a code or a message.
#define BOBASSERT_ALL(d,ex,msg1,msg2) if( !(d) ) { std::ostringstream str;\
           str << msg1 << msg2 << "\tFile:" << __FILE__ << ", Line:" << __LINE__;\
           Exception_throw(ex(str.str()));\
	}

#ifdef BOBUSETRACE
#define BOBTRACE(s) s
#define BOBUSEASSERT
#else
#define BOBTRACE(s)
#endif

#ifdef BOBUSEASSERT
#define BOBASSERT(d,ex,m1,m2) BOBASSERT_ALL(d,ex,m1,m2);
#else
#define BOBASSERT(d,ex,m1,m2)
#endif

#define BOBEXCEPTION(ex,msg1,msg2)  {std::ostringstream str;\
           str << msg1 << msg2 << "\tFile:" << __FILE__ << ", Line:" << __LINE__;\
           Exception_throw(ex(str.str()));\
	}


/**
 * @}
 */

}

#endif
