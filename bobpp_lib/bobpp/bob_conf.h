
#ifndef BOBCONF
#define BOBCONF

namespace Bob {

/*------- Class to trace the executions------------------------------*/

/** @defgroup DEBUG The Debug mode
 *  The underlying execution of a Bob++ application could appear to new user
 *  as a difficult task to understand. Then the debugging phase could represent
 *  a difficult task.
 *
 *  For this reason, we have instrumented many of the Bob++ methods
 *  to display trace information.
 *  By default, the compilation of Bob++ library does not integrate this
 *  functionnality. To activate it, you have to do it during the
 *  configuration phase (see @ref InstMan or a complete explaination
 *  of the option of the ./configure).
 *
 *  To activate the debug mode, you have to add the --enable-debug to the
 *  configure phase.
 *  \code
 *     > ./configure --prefix=installpath --enable-debug
 *  \endcode
 *
 *  The Debug trace are divided in 4 parts according to the part of
 *  the library which display it.
 *  The parts are the Memory, the algorithm, the user, the environment.
 *
 *  When an application is compiled with the Bob++ library with the Debug
 *  mode activated, each part could display its trace information
 *  using the command line option of the application
 *
 *  For example, if an application named bobapp has been generated
 *  \code
 *    > ./bobapp --dbg -mem
 *  \endcode
 *  the memory part displays its trace information.
 *  <ul>
 *  <li> The Memory part (-mem to enable) traces the constructor and destructor of the Bob++ classes or structures.</li>
 *  <li> The Algorithm part (-al to enable) traces the call of the different methods of the classes or structures belonging to the Bob++ algorithms.</li>
 *  <li> The user part (-user to enable) display the information that the user has been added to its code using the macro DBGAFF_USER().</li>
 *  <li> The environment part (-env to enable) traces execution of the environment part you have decide to use (sequential, pthread, athapascan).</li>
 *  </ul>
 *
 *  @brief this module show how to use the debug mode in Bob++
 *  @ingroup CoreGRP
 *  @{
 */

/** Structure that stores the flag for the debuggging information
 */
struct debug_config {
#ifdef BOBPP_ENABLE_DEBUG
  /** Boolean associated with the memory debugging.
    */
  static bool mem;
  /** Boolean associated with the user debugging.
    */
  static bool user;
  /** Boolean associated with the algorithm debugging.
    */
  static bool algo;
  /** Boolean associated with the environment debugging.
    */
  static bool env;
  /** Boolean associated with the pack/unpack debugging.
    */
  static bool pack;
#endif
  /** static method which insert or not the Debug Properties.
    */
  static void opt();
};

#ifdef BOBPP_ENABLE_DEBUG
#define AFFDEBUG(t,m,mess) if ( t ) {std::cout<<m<<"("<<__FILE__<<", Line:"<<__LINE__<<"):"<< mess<< std::endl;}
#define AFFDEBUG_THIS(t,m,mess) if ( t ) {std::cout<<m<<"("<<__FILE__<<", Line:"<<__LINE__<<")"<<(int *)this<<":"<< mess<< std::endl;}

/** Macro to trace the memory operations.
 * This macro is called in the constructor and the destructor of the
 * Bob++ classes
 */
#define DBGAFF_MEM(m,mess) AFFDEBUG_THIS(Bob::debug_config::mem,m,mess)
/** Macro to trace the Bob++ algorithms.
 * This macro is called in the different methods of the
 * Bob++ classes related to the algorithms
 */
#define DBGAFF_ALGO(m,mess) AFFDEBUG(Bob::debug_config::algo,m,mess)

/** Macro to trace user operations.
 * This macro could be called in the user code to trace
 * the execution
 */
#define DBGAFF_USER(m,mess) AFFDEBUG(Bob::debug_config::user,m,mess)

/** Macro to trace the environment operations.
 * This macro is called in Environment side to trace the execution
 */
#define DBGAFF_ENV(m,mess) AFFDEBUG(Bob::debug_config::env,m,mess)
/** Macro to trace the environment operations.
 * This macro is called in Environment side to trace the pack and unpack operations
 */
#define DBGAFF_PCK(m,mess) AFFDEBUG(Bob::debug_config::pack,m,mess)
#else
/** Macro to trace the memory operation.
 * This macro is called in the constructor and the destructor of the
 * Bob++ classes
 */
#define DBGAFF_MEM(m,mess)
/** Macro to trace the Bob++ algorithms.
 * This macro is called in the different methods of the
 * Bob++ classes related to the algorithms
 */
#define DBGAFF_ALGO(m,mess)
/** Macro to trace user operations.
 * This macro could be called in the user code to trace
 * the execution
 */
#define DBGAFF_USER(m,mess)
/** Macro to trace the environment operations.
 * This macro is called in Environment side to trace the execution
 */
#define DBGAFF_ENV(m,mess)
/** Macro to trace the environment operations.
 * This macro is called in Environment side to trace the pack and unpack operations
 */
#define DBGAFF_PCK(m,mess)
#endif

/**
 *  @}
 */
};
#endif
