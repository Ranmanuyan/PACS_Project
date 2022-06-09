
#include<ctype.h>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<getopt.h>
#include <map>

#include<bobpp/bobpp>

#define FILENAME 50

struct opt {
   char sfile[FILENAME];
   char ofile[FILENAME];
   /** Constructor with the default value of the options
   */
   opt() {
     strcpy(sfile,"seq-collect.txt");
     strcpy(ofile,"collect.csv");
   }
};


struct StatData {
   int has_time;
   long nb;
   double time;
   
   StatData():has_time(false),nb(-1),time(-1.0) { }
   StatData(bool ht,long _n,double _t):has_time(ht),nb(_n),time(_t) { }
   StatData(const StatData &sd):has_time(sd.has_time),nb(sd.nb),time(sd.time) { }
   StatData(long _n,double _t):nb(_n),time(_t) { 
      if (!has_time ) std::cerr << "Strange StatData : has not time, but i receive one ! ignored"<<std::endl; 
   }
   StatData(long _n):has_time(false),nb(_n),time(-1.0) { 
      if (has_time ) std::cerr << "Strange StatDat has time, but i do not receive one ! ignored"<<std::endl; 
      time = 0.0;
   }
   void init(bool ht,long _n,double _t) {
      has_time=ht;
      nb = _n;
      time = _t;
   }

   void init(long _n,double _t) {
      nb = _n;
      time = _t;
      if (!has_time ) std::cerr << "Strange StatData : has not time, but i receive one ! ignored"<<std::endl; 
   }
   void init(long _n) {
      nb = _n;
      time = 0.0;
      if (has_time ) std::cerr << "Strange StatDat has time, but i do not receive one ! ignored"<<std::endl; 
   }
   StatData& operator=(const StatData &s2) {
      has_time = s2.has_time;
      nb= s2.nb;
      time= s2.time;
      return *this;
   }

};


typedef std::map<std::string,int> Titles;
typedef std::pair<std::string,int> pairt;
typedef std::vector<std::string> Options;
typedef std::vector<StatData> Statistics;

class CollectParse  : public Bob::parser {
  protected:
    int nbo;
    Titles tio;
    Options op;
    int nbs;
    Titles tis;
    Statistics stat;

  public:
    CollectParse(Bob::data_reader *dr) : Bob::parser(dr),nbo(0),tio(),op(),nbs(0),tis(),stat() { set_crsep(false); set_match_sep(true); }
    virtual ~CollectParse() { }

    void ParsePara(bool first) {
       std::string para("");
       Bob::strbuff<256> pv("");
       Match(TOK_ID);
       para+= get_oldvalue().sval;
       while ( lookhead()!=':' ) {
           Match('.');
           Match(TOK_ID);
           para+= get_oldvalue().sval;
       }
          
       Match(':');
       while (lookhead()!=TOK_SEPARA ) {
          switch(lookhead()) {
              case TOK_ID : Match(lookhead()); pv.add(get_oldvalue().sval.c_str());break;
              case TOK_INT : Match(lookhead()); pv.add(get_oldvalue().dval);break;
              case TOK_REAL : Match(lookhead()); pv.add(get_oldvalue().fval);break;
              default : pv.add((char)lookhead()); Match(lookhead());
          }
       }
       Match(TOK_SEPARA);
       if ( first ) {
         tio.insert(pairt(para,nbo)); 
         op.resize(nbo+1);
         op[nbo] = std::string(pv.str());
         nbo++;
       } else {
         op[tio[para]]=std::string(pv.str()); 
         std::cout << "Stocké:"<<op[tio[para]]<<" a stocké:"<<std::string(pv.str())<<"\n";
       }
    }
    void ParseStat(bool first) {
       std::string stt("");
       long nb;
       double time=-1.0;
       bool has_time=false;
       Match(TOK_ID);
       stt= get_oldvalue().sval;
       Match(':');
       Match(TOK_INT);
       nb= get_oldvalue().dval;
       if ( lookhead()==',' ) {
         Match(',');
         Match(TOK_REAL);
         time= get_oldvalue().fval;
         has_time=true;
       }
       if ( first )  {
         tis.insert(pairt(stt,nbs));
         stat.resize(nbs+1);
         if ( has_time) {
           stat[nbs].init(has_time,nb,time); 
         } else {
           stat[nbs].init(nb); 
         }
         nbs++;
       } else {
         if ( has_time) {
           stat[tis[stt]].init(has_time,nb,time); 
         } else {
           stat[tis[stt]].init(nb); 
         }
       }
       std::cout << "finish stat:"<<stt<<":"<<nb;
       if ( has_time) {
          std::cout << ","<<time<<"\n";
       } else {
          std::cout << "\n";
       }
       if ( stat[tis[stt]].nb !=nb ) {
          std::cout << "Different "<<tis[stt]<<" : "<<stat[tis[stt]].nb<<"\n";
       }
    }

    void write_header(FILE *f) {
      for ( Titles::iterator it=tio.begin(); it!=tio.end(); it++ ) {
         fprintf(f,"%s ,",it->first.c_str());
      }
      for ( Titles::iterator it=tis.begin(); it!=tis.end(); it++ ) {
         if ( stat[it->second].has_time ) 
            fprintf(f,"# %s , time %s ,",it->first.c_str(),it->first.c_str());
         else
            fprintf(f,"%s ,",it->first.c_str());
      }
      fprintf(f,"\n");
    }
    void write_data(FILE *f) {
      for ( Titles::iterator it=tio.begin(); it!=tio.end(); it++ ) {
         fprintf(f,"%s ,",op[it->second].c_str());
      }
      for ( Titles::iterator it=tis.begin(); it!=tis.end(); it++ ) {
         if ( stat[it->second].has_time ) 
            fprintf(f,"%ld , %lf, ",stat[it->second].nb,stat[it->second].time);
         else
            fprintf(f,"%ld ,",stat[it->second].nb);
      }
      fprintf(f,"\n");
    }

    void parse(opt *o,FILE *dest) {
      bool first=true;
      init();
      while(lookhead()!=TOK_EOF ) {
        Match(TOK_INT);
        Match(TOK_SEPARA);
        Match('{');
        Match(TOK_SEPARA);
        while (lookhead()!='}' ) {
          ParsePara(first);
        }
        Match('}');
        std::cout << "finish para\n";
        if ( lookhead()==TOK_SEPARA ) 
          Match(TOK_SEPARA);
        while (lookhead()!='\n' ) {
          ParseStat(first);
          if ( lookhead()==TOK_SEPARA ) 
             Match(TOK_SEPARA);
        }
        Match('\n');
        std::cout << "finish stat\n";
        if ( first ) write_header(dest);
        write_data(dest);
        fflush(dest);
        std::cout << "Finish line\n";
        first=false;
      }

    }
};


void usage(char *s, opt *o) {
   fprintf(stderr,"Usage : %s  [-o filename] <file>\n",s);
   fprintf(stderr,"\t\t-o value : file name to create (default %s)\n",o->ofile);
   fprintf(stderr,"\t\t<file> : the filename to read (default %s)\n",o->sfile);
}

int main(int n, char **v) {
   opt o;
   FILE *dest;
   int oc;

   while ((oc = getopt(n, v, "gdla:r:n:h:w:s:o:m:")) != -1) {
     switch (oc) {
       case 'o': strcpy(o.ofile,optarg);break;
       default: /* '?' */
         usage(v[0],&o);
         exit(1);
     }
   }
   if ( optind<n ) {
      strcpy(o.sfile,v[optind]);
   }
   Bob::file_reader sr(o.sfile);
   if ( (dest = fopen(o.ofile,"w"))==NULL ) {
      perror(o.ofile);
      exit(3);
   }
   CollectParse cp(&sr);
   cp.parse(&o,dest);
   fclose(dest);

}
