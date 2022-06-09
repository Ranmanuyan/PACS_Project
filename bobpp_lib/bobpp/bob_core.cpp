
#include <bobpp>
#include <string>
#include <vector>
#include <signal.h>

namespace Bob {

#ifdef BOBPP_ENABLE_DEBUG
bool debug_config::mem = false;
bool debug_config::user = false;
bool debug_config::algo = false;
bool debug_config::env = false;
bool debug_config::pack = false;

void debug_config::opt() {
  core::opt().add(std::string("--dbg"), Property("-mem", "print information on memory managment", &debug_config::mem));
  core::opt().add(std::string("--dbg"), Property("-al", "print information on algorithm execution", &debug_config::algo));
  core::opt().add(std::string("--dbg"), Property("-user", "print information generate by the user side", &debug_config::user));
  core::opt().add(std::string("--dbg"), Property("-env", "print information generate by the environment side", &debug_config::env));
  core::opt().add(std::string("--dbg"), Property("-pck", "print information generate by the pack and unpack operations", &debug_config::pack));
}
#else
void debug_config::opt() { }

#endif

void handle_stop_and_save(int v) {
   core::stop_and_save();
}

void handle_stat(int v) {
   core::disp_stat();
}

void handle_stop(int v) {
    core::stop();
}

std::string core::SerialFileName;
string core::logfile = "file.log";
bool core::verb = false;
bool core::spacesearch= false;
bool core::collect= false;
std::string core::collectfile = "collect.txt";
Opt core::theopt;
Log core::thelog;
long core::reft;
double core::reftd;
int core::btdthreshold;
int core::depth_rec;
long core::time_limt;
node_id core::nid = 1;
bool core::dispbestval = false;
BaseAlgoEnvProg *core::env_prog=0;

/*------------------ Core methods ----------------------*/

void core::Config(int n, char **v) {

  opt().add(std::string("--"), Property("-help", "Display the help"));
  opt().add(std::string("--"), Property("-t", "Depth threshold for BestThenDepth priority", 10, &btdthreshold));
  opt().add(std::string("--"), Property("-c", "collect results", &collect));
  opt().add(std::string("--"), Property("-cf", "file to collect the results", "collect.txt", &collectfile));
  opt().add(std::string("--"), Property("-lf", "log to file", &verb));
  opt().add(std::string("--"), Property("-of", "log output file", "file.log", &logfile));
  opt().add(std::string("--"), Property("-sp", "log the space search", &spacesearch));
  opt().add(std::string("--"), Property("-s", "display current best solution", &dispbestval));
  opt().add(std::string("--"), Property("-tl", "time limt", -1, (int *)&time_limt));
  opt().add(std::string("--"), Property("-dr", "depth to pass to recursive search", 10, (int *)&depth_rec));
#ifndef WIN32
  opt().add(std::string("--"), Property("-ln", "log to network host"));
  opt().add(std::string("--"), Property("-on", "log network host, format = address[:port]", "localhost:2222"));
#endif
#ifdef BOBPP_ENABLE_FT
  opt().add(std::string("--"), Property("-ft_file", "file to store", "bobpp.store"));
  opt().add(std::string("--"), Property("-ft_rs", "restoring from file"));
#endif
  debug_config::opt();
  opt().parse(n, v);
#ifndef WIN32
  verb = verb || opt().BVal("--", "-ln")
#endif
         ;
  reft = Time();
  reftd = dTime();

  if (opt().BVal("--", "-help")) {
    opt().Usage();
    exit(0);
  }
  if (isVerb()) {
#ifndef WIN32
    if (opt().BVal("--", "-ln")) {
      std::string host = opt().SVal("--", "-on");
      string::size_type pos = host.rfind(":", host.size());
      int port;
      char s[256];
      if (pos == string::npos)
        port = 2222;
      else {
        std::string port_str = host.substr(pos + 1);
        std::istringstream iss(port_str);
        iss >> std::dec >> port;
        host = host.substr(0, pos);
      }
      log().setLog(new Log_net(host, port));
      sprintf(s, "net://%s:%d", host.c_str(), port);
      logfile = s;
    } else
#endif
    log().setLog(new Log_file(logfile));
    std::cout << " Bob++ Version "<< BOBPP_PACKAGE_VERSION << " © B. Le Cun & F. Galea 2006-2009" << std::endl;
    std::cout << " Verbose Mode on " << std::endl;
    std::cout << " LogFile :" << logfile << std::endl;
  }
#ifndef WIN32
  if ( time_limt>0 ) {
     alarm(time_limt);
     signal(SIGALRM,handle_stop);
  }
  signal(SIGQUIT,handle_stop_and_save);
  signal(SIGUSR1,handle_stat);
#endif
}

/// Get a BobSerialize generally this is the File version
/*
std::string core::SerialFileName() {
	ostringstream os;
	os << "bobpp_"<<getpid()<<s;
	return os.rdbuf()->str();
}*/

void core::End() {
  double t=dTime();
  std::cout << "Core Time :"<< t << std::endl;
  opt().clear();
}

long core::Time(long b) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (t.tv_sec) - (reft + b);
}
double core::dTime(double b) {
  struct timeval t;
  gettimeofday(&t, NULL);
  double d = t.tv_sec + 1e-6 * t.tv_usec;
  return d -(reftd + b);
}
string core::sTime(long b) {
  char buf[120];
  sprintf(buf, "%02ldh:%02ldm:%02lds", b / (3600), (b / 60) % 60, b % 60);
  return string(buf);
}

node_id core::get_new_id(long v) {
  return __sync_add_and_fetch(&nid, 1);
}

void core::stop() {
   std::cout <<" Stopping the environment\n";
   if ( env_prog!=0 ) {
      env_prog->stop();
   }
}

void core::stop_and_save() {
   std::cout <<" Stopping the environment\n";
   if ( env_prog!=0 ) {
      env_prog->stop_and_save();
   }
}

void core::disp_stat() {
   if ( env_prog!=0 ) {
      env_prog->display();
   }
}

void core::register_env_prog(BaseAlgoEnvProg *env) {
   if ( env_prog==0 ) env_prog=env;
}


};
