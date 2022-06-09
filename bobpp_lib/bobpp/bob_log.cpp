#include <bobpp>
#include <string>

#include <sys/types.h>
#ifndef WIN32
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace Bob {
/********************************** Log_base *******************************/
  /// constructor
Log_base::Log_base() : ei(new SeqId()) {}
  /// destructor
Log_base::~Log_base() { 
    if ( ei ) delete ei;
}

/// Set the log object
void Log_base::setEId(EnvId *_ei) {
  if (ei != 0) delete ei;
  ei = _ei;
}


/********************************** Log_real *******************************/

/// display the id
void Log_real::out(strbuff<> const &lm) {
  lock();
  nolock_out(lm);
  unlock();
}
/// display the log line counter
void Log_real::out(const char *what, node_id  c, const char * ni) {
  strbuff<> st;
  st << ei->id() << "-" << what << " " << c << " (" << ni << ")\n";
  out(st);
}

/// display the log line counter
void Log_real::out(const char *what, char w,node_id  c, double t,const char * ni) {
  strbuff<> st;
  st << ei->id() <<"-"<<what<<" "<<c <<" "<< w<<" "<<t<<" ("<<ni<<")\n";
  out(st);
}
/// display the log line counter
void Log_real::out(const char *what, node_id  c) {
  strbuff<> st;
  st << ei->id() << "-" << what << " " << c << "\n";
  out(st);
}

/// display the log line counter
void Log_real::out(const char *what, char w,node_id  c, double t) {
  strbuff<> st;
  st << ei->id() <<"-"<<what<<" "<<c<<" "<< w<<" "<<t<<"\n";
  out(st);
}

/// display the log line counter
void Log_real::out(const char *what, node_id  c, node_id  ni) {
  strbuff<> st;
  st << ei->id() << "-" << what << " " << c << " (" << ni << ")\n";
  out(st);
}

/// display the log line counter
void Log_real::out(const char *what, char w,node_id  c, double t,node_id ni) {
  strbuff<> st;
  st << ei->id() <<"-"<<what<<" "<<c<<" "<< w<<" "<<t<<" ("<<ni<<")\n";
  out(st);
}

/// display the log line counter
void Log_real::start_header() {
  strbuff<> st;
  st << "{\n";
  nolock_out_wot(st);
}
/// display the stat log header
void Log_real::log_header(const strbuff<> &st) {
  nolock_out_wot(st);
}

/// display the log line counter
void Log_real::end_header() {
  strbuff<> st;
  st << "}\n";
  nolock_out_wot(st);
}

/********************************** Log_file *******************************/
void Log_file::nolock_out(strbuff<> const &lm) {
  char s[16];
  sprintf(s, "%.04f ", core::dTime());
  os << s << lm;
}

void Log_file::nolock_out_wot(strbuff<> const &lm) {
  os << lm;
}

/********************************** Log_net*******************************/
#ifndef WIN32
Log_net::Log_net(std::string host, int port): Log_real(), os(-1) {
  struct sockaddr_in adr;
  struct hostent *hp;

  hp = gethostbyname(host.c_str());
  if (hp == NULL)
    herror("Log_net::Log_net"), exit(1);
  os = socket(PF_INET, SOCK_DGRAM, 0);
  if (os == -1)
    perror("Log_net::Log_net"), exit(1);
  adr.sin_family = AF_INET;
  adr.sin_port = htons(port);
  memcpy(&adr.sin_addr, hp->h_addr, hp->h_length);
  if (connect(os, (struct sockaddr*)&adr, sizeof(adr)) == -1)
    perror("Log_net::Log_net"), exit(1);
}


Log_net::~Log_net() {
  if (close(os))
    perror("Log_net::~Log_net"), exit(1);
}

/// display the message lm to the log
void Log_net::nolock_out(strbuff<> const &lm) {
  send(os, lm.str(), lm.size(), 0);
}
void Log_net::nolock_out_wot(strbuff<> const &lm) {
  send(os, lm.str(), lm.size(), 0);
}

#endif

/********************************** Log used in CORE *******************************/
Log::Log(): Log_base(),l(new Log_base()) {
};

Log::~Log() {
  delete l;
}
/// Set the log object
void Log::setLog(Log_base *lb) {
  if (l != 0) delete l;
  l = lb;
}


void Log::out(const char *what, node_id  c, const char *ni) {
     if ( core::space_search() ) {
       l->out(what,c,ni);
     } else {
       l->out(what,c);
     }
}
void Log::out(const char *what, char w, node_id  c, double t,const char *ni) {
     if ( core::space_search() ) {
       l->out(what,w,c,t,ni);
     } else {
       l->out(what,w,c,t);
     }
}
void Log::out(const char *what, node_id  c) { l->out(what,c);}

void Log::out(const char *what, char w, node_id  c, double t) {
     l->out(what,w,c,t);
  }
void Log::out(const char *what, node_id  c, node_id ni) {
     if ( core::space_search() ) {
        l->out(what,c,ni);
     } else {
        l->out(what,c);
     }
}
void Log::out(const char *what, char w, node_id  c, double t,node_id ni) {
     if ( core::space_search() ) {
       l->out(what,w,c,t,ni);
     } else {
       l->out(what,w,c,t);
     }
}




};
