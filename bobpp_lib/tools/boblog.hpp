
#include<ctype.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<getopt.h>
#include <map>

#include<bobpp/bobpp>

#define NB_PAL 400


#define FILENAME 255
#define MAXLINE 255

/** Structure that stores the options of the application
  */
struct opt {
   int gtk;
   int gene_dot;
   float ranksep;
   float nodesep;
   float node_height;
   float node_width;
   float edge_width;
   float value_max;
   char algo[FILENAME];
   int fl_max;
   int wth_lab;
   char sfile[FILENAME];
   char ofile[FILENAME];

   /** Constructor with the default value of the options
     */
   opt() {
     gtk=0;
     gene_dot=0;
     ranksep=0.1;
     nodesep=0.05;
     node_height=0.1;
     node_width=0.1;
     edge_width=0.1;
     value_max=-1.0;
     fl_max=0;
     wth_lab=0;
     strcpy(algo,"");
     strcpy(sfile,"file.log");
     strcpy(ofile,"file.dot");
   }
};

/** structure that stores the header data of a Bobpp log file 
  * one line of the header represents a bobpp statistic (counter or timer).
  * the stat could be global or local to a thread or process.
  * a stat is identified by the application (app), an id for an algorithm
  * (mainly 'B' for Branch and Bound), hierarchy of operation.
  * and a full text that explain the goal of the stat.
  */
struct logheader {
  bool local;
  std::string id;
  char type_op;
  char libelle[30];
  logheader() : id(),type_op('c') { 
    *libelle=0;
  }
  logheader(const logheader &lh): local(false),id(lh.id),type_op(lh.type_op) {
    strcpy(libelle,lh.libelle);

  }
  void Prt(std::ostream & os=std::cout) const {
     os << "Stat:("<<id<<")=";
     os << (local?'l':'g')<<':'<<type_op<<":"<<libelle<<std::endl;
  }
};

/** operator to display a line header on a std::ostream
  */
extern std::ostream &operator<<(std::ostream &os, const logheader &lh);

/** each  line header is stored on a map according to the its id.
  */
typedef std::map<std::string,logheader> logheadermap;
/** the map that store all line header
  */
extern logheadermap lm;
/** type to insert a new line header on a map.
  */
typedef std::pair<std::string,logheader> lmpair;
/** type for the return of an insert to the map.
  */
typedef pair<map<std::string,logheader>::iterator,bool> retins;

/** operator to display a line header on a std::ostream
  */
extern std::ostream &operator<<(std::ostream &os, const logheadermap &lm);


/** the structure that stores a line of stat.
  */
struct logline {
  double time;
  double duration;
  char type; // c, t
  char es; // End or Start time.
  long nb;
  int thr;
  std::string id;
  long nid;
  long pid;
  int d;
  char n_type;
  double eval;
  std::string ud;

  logline() : time(0.0), duration(0.0), type('C'),es('S'),nb(0),thr(0),id(),nid(-2),pid(-2),d(-1),n_type('n'),eval(0.0),ud("") {}
  virtual ~logline() { }
  void Prt(std::ostream & os=std::cout) const {
    os << thr <<":("<<id<<")="<<nb<<"@";
    if ( type=='C' ) os <<time;
    else  { 
      os <<"["<<time<<":"<<duration<<"]";
    }
    if ( nid>-1 ) {
      os << "(n:"<<nid;
      if ( pid>-1 ) { os << ",p:"<< pid; }
      if ( d>-1 ) os << ","<<d<<","<<n_type<<","<<eval<<","<<ud;
      os <<")";
    }
    os <<std::endl;
  }
};

/** operator to display a log line on a std::ostream
  */
extern std::ostream &operator<<(std::ostream &os, const logline &lh);

typedef std::map<double,logline> tmloglinemap;
typedef std::vector<tmloglinemap> thtmloglinemap;
typedef std::vector<logline> Nodelogline;

/** type to insert a new log line on a map.
  */
typedef std::pair<double,logline> lgmpair;
/** type for the return of an insert to the map.
  */
typedef pair<map<std::string,logheader>::iterator,bool> retins;

/** type for logline sorted first by threads
  */
//typedef std::vector<std::map<std::string,logline> > ologlinemap;

struct LoglineSet {
   thtmloglinemap lgm;
   Nodelogline nll;
   int nbthr;
   double max_time;
   double min_val;
   double max_val;
   /** Constructor
     */
   LoglineSet() : lgm(1), nll(), nbthr(1), max_time(0.0) ,min_val(std::numeric_limits<float>::max()),max_val(std::numeric_limits<float>::min()) { }
   /** Destructor
     */
   virtual ~LoglineSet() { }
   /** insert a logline in the set.
     */
   void Ins(logline &lg);
   /** display the loglineset.
     */
   void Prt(std::ostream & os=std::cout) const {
     for(int i=0;i<nbthr;i++) {
        os << "-----------------Thread : "<<i<<"-------------\n";
        for(tmloglinemap::const_iterator it=lgm[i].begin(); it!=lgm[i].end(); it++ ) {
           os << (*it).second;
        }
     }
     os << "-----------------Node "<<nll.size()<<"-------------\n";
     for (unsigned int i=0; i < nll.size() ; i++ ) {
       if ( nll[i].nid>=0 ) {
           os << "i:"<<i<<":"<<nll[i];
       }
     }
   }

};

extern LoglineSet lgs;

extern std::ostream &operator<<(std::ostream &os, const LoglineSet &lm);


/** strcture to generate a dot file
  */
struct TreeDotFile {
  FILE *f;

  TreeDotFile(FILE *file) : f(file) {
  }
  void head(const opt &o) {
    fprintf(f,"/* Create by the bobloggui application for the dot application*/\n\n");
    fprintf(f,"graph bb {\n    ranksep=%1.2f\n    nodesep=%1.2f\n",
              o.ranksep,o.nodesep);
    fprintf(f,"    node [fixedsize=true];\n");
    fprintf(f,"    node [style=filled];\n");
    if ( !o.wth_lab ) {
      fprintf(f,"    node [height=%1.2f];\n    node [width=%1.2f];\n",
                      o.node_height,o.node_width);
      fprintf(f,"    node [label=\"\"];\n");
    }
    fprintf(f,"    edge [width=%1.2f];\n", o.edge_width);
  }
  void gene(const opt &o) ;
  void tail() {
    fprintf(f,"}\n");
  }
};


class LogParse  : public Bob::parser {

  public:
    bool has_space;
    LogParse(Bob::data_reader *dr) : Bob::parser(dr),has_space(false) { }
    virtual ~LogParse() { }
    void parse(opt *o,FILE *) ;
    void MatchStatId(opt *o,std::string &sid) ;
};

//#include "boblogview.hpp"
