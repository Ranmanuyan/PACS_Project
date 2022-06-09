
/*
 * Bob++/include/bob_core.h
 *
 * This source include the abstract data type for the application
 * core.
 */

#ifndef BOBCORE
#define BOBCORE

#include <sys/time.h>
#include <time.h>

namespace Bob {

class BaseAlgoEnvProg;

/*------------------ core Class-------------*/
/** @defgroup CoreClassGRP The Bob::core Class
 *  The core class contains static datas and methods, that are globally
 *  used by each of the Bob++ components. It stores
 *  -# the options objects that stores the properties (@ref OptGRP)
 *  -# some constants or value that are associated with options
 *  -# the Log object (see @ref IOGRP) used to log the activity of the different algorithms
 *  -# methods to get time as long, double, or string types.
 *
 *  @brief the core class stores many global features
 *  @ingroup CoreGRP
  * @{
  *
*/


/** the Core class
 */
class core {
private:
  static std::string SerialFileName;
protected:
  /// The file log for the execution
  static std::string logfile;
  /// The file log for the execution
  static std::string ft_file;
  /// Verbose level
  static bool verb;
  /// log the space search or not
  static bool spacesearch;
  /// The Option
  static Opt theopt;
  /// Interface for log information.
  static Log thelog;
  /// Collect the results in a file or not
  static bool collect;
  /// the file name to collect the results
  static std::string collectfile;
  /// the reference time.
  static long reft;
  /// the reference time with microsecond precision.
  static double reftd;
  /// Depth threshold for BestThenDepth priority
  static int btdthreshold;
  /// threshold to pass to recursive mode
  static int depth_rec;
  /// Time limt of the application.
  static long time_limt;
  /// the node identifier
  static node_id nid;
  /// display the value of the current best solution
  static bool dispbestval;
  /// the primary environment
  static BaseAlgoEnvProg *env_prog;
public:
  /// constructor
  core() {}
  /// Destructor
  ~core() {}
  /// Parse the internal option of Bob++
  static void Config(int n, char **v);
  /// Gets the verbose level
  static int isVerb() {
    return verb;
  }
  /// method to get the option
  static int BTDthreshold() {
    return btdthreshold;
  }
  /// method to get the option
  static int get_depth_rec() {
    return depth_rec;
  }

  /// if we need to display the best value at each goal update
  static bool dispBestVal() {
    return dispbestval;
  }
  /// if we need to log the space search (display the node identifier and their hierarchy 
  static bool space_search() {
    return spacesearch;
  }
  /// Gets the boolean to know if the run must put its statistics resume on the collection file.
  static bool iscollect() { 
    return collect; 
  }
  /// Put the line (the strbuf varaible) on the collection file
  static void docollect(strbuff<> s) { 
    if ( collect ) {
       std::ofstream f(collectfile.c_str(),std::ios_base::app);
       f << s <<std::endl;
       f.close();
    }
  }

  /// method to get the option
  static Opt &opt() {
    return theopt;
  }
  /// method to get the logging object.
  static Log &log() {
    return thelog;
  }
  /// End the core
  static void End();
  /// static method to get the time in seconds
  static long Time(long b = 0);
  /// static method to get the time in seconds with microsecond precision
  static double dTime(double b = 0.0);
  /// static method to get the time in a human redable way
  static std::string sTime(long b);
  /// static method to get a node identifier
  static node_id get_new_id(long v = 0);
  /// static method to stop the primary environment and save all the data
  static void disp_stat();
  /// static method to stop the primary environment
  static void stop();
  /// static method to stop the primary environment and save all the data
  static void stop_and_save();
  /// static method to register the primary environment.
  static void register_env_prog(BaseAlgoEnvProg *env);
};


/** RegistStatException. An object of this class is throwed
 * there is error in the Bob++ classes.
*/
class RegistStatException: public Exception {
public:
  /** Default constructor
       * @param msg is the message of the execption
  */
  RegistStatException(const std::string& msg)
      : Exception("Register Statistics Exception :", msg) {}
};


/**
 *  @}
 */
}
#endif

