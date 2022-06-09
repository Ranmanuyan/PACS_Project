
#include <bobpp/bobpp>
#include <cstdio>
#include <cstdlib>

const int MaxField=10;

double getx(char *line) ;

struct Data {
   double x;
   long nb;
   double time;

   void printn() { 
      std::cout << x << " "<< nb<<"\n";
   }
   void printt() { 
      std::cout << x << " "<< time<<"\n";
   }
};

struct Curve {
  int yi;
  int cd;
  std::string pat;
  std::string label;
  std::vector<Data> vd;
  std::vector<std::string> sel;
  int nbsel;
  bool time;
  bool y2;

  Curve() : yi(0),cd(0),pat(""),label(""),vd(),sel(MaxField),nbsel(0),time(false),y2(false) {}
  Curve(int _yi) : yi(_yi),cd(0),pat(""),label(""),vd(),sel(MaxField),nbsel(0),time(false),y2(false) {}
  Curve(const Curve &c) : yi(c.yi),cd(c.cd),pat(c.pat),label(c.label),vd(c.vd),sel(c.sel),nbsel(c.nbsel),time(c.time),y2(c.y2) {}

  void init(int _yi) { 
    int i;
    yi=_yi; 
    for (i=0;i<MaxField && sel[i].length()>0 ;i++ );
    nbsel = i;
  }
  bool is_sel(char *line) {
    for (int i=0;i<nbsel ;i++ ) {
       if ( strstr(line,sel[i].c_str())!=NULL ) return true;
    }
    return nbsel==0;
  }
  void getval(char *line) {
    char *s;
    char *da;
    char *f;
    if ( pat.length()==0 ) { return; }
    if ( !is_sel(line) ) { return; }

    da = strchr(line,'}');
    if ( da==NULL ) {
      std::cerr << " There is not data part (found with the '}' char)"<<std::endl;
      exit(1);
    }
    if ( (s=strstr(da, pat.c_str()))==NULL ) {
      std::cerr << " Field " << pat << "not found"<<std::endl;
      exit(1);
    }
    while (!isdigit(*s) && *s!=0) s++;
    std::cout << "Int:"<<s<<std::endl;
    vd.resize(cd+1);
    vd[cd].nb = atoi(s);
    f = strchr(s,',');
    if ( f==NULL ) {
      if ( !time) {vd[cd].time=0.0; return; }
      else { 
        std::cerr << "You specifiy a time field but only count field exists !"<<std::endl;
        exit(1);
      }
    }
    std::cout << "F:"<<f+1<<std::endl;
    vd[cd].time = atof(f+1);
    vd[cd].x =getx(line);
    if ( vd[cd].x==-1 ) vd[cd].x=cd;
    cd++;
  }
  void print() { 
      for (unsigned int i=0; i < vd.size() ;i++ ) {
          if ( time ) vd[i].printt();
          else vd[i].printn();
      }
      std::cout << "\n";
  }
  void fillbuff(std::ofstream &s) { 
      for (unsigned int i=0; i < vd.size() ;i++ ) {
          if ( time ) {
              s << vd[i].x << " " << vd[i].time<< "\n";
          } else {
              s<< vd[i].x << " "<< vd[i].nb<<"\n";
          }
      }
      s << "e\n";
      //std::cout << s;
  }

};

class Option {
public:
   static Bob::Opt opt;
   static std::string filename;
   static std::string outfilename;

   static bool log;
   static bool notxt;
   static bool x11;
   static bool postscript;
   static bool beamer;

   static bool hasy2;
   static int nbcrv;
   static std::vector<Curve> crv;
   static std::string x_field;
   static std::string x_label;
   static int nbls;
   static std::vector<std::string> line_select;

   static void init(int n,char **v) {
      opt.add(std::string("--"), Bob::Property("-help", "Display the help"));
      opt.add(std::string("--"), Bob::Property("-f", "the filename","collect.txt",&Option::filename));

      opt.add(std::string("--out"), Bob::Property("-f", "basename of the ouput file","out",&Option::outfilename));
      opt.add(std::string("--out"), Bob::Property("-log", "set logscale to the y axe",&Option::log));
      //opt.add(std::string("--out"), Bob::Property("-nt", "no text file",&Option::notxt));
      //opt.add(std::string("--out"), Bob::Property("-x11", "X11 output (gnuplot)",&Option::x11));
      //opt.add(std::string("--out"), Bob::Property("-p", "postscript (gnuplot)",&Option::postscript));
      //opt.add(std::string("--out"), Bob::Property("-b", "beamer",&Option::beamer));
      for (int i=0;i<MaxField; i++ ) {
         Bob::strbuff<6> s="--c";
         s.add(i);
         opt.add(std::string(s.str()), Bob::Property("-y2", 
                "This curve has its y coords on the second y axe y2", &crv[i].y2));
         opt.add(std::string(s.str()), Bob::Property("-p", 
                "field value for this curve", "",&crv[i].pat));
         opt.add(std::string(s.str()), Bob::Property("-t", 
                "Time value field on y", &crv[i].time));
         opt.add(std::string(s.str()), Bob::Property("-l", 
                "Label for this curve", "",&crv[i].label));
         for (int j=0;j<MaxField; j++ ) {
            Bob::strbuff<6> se="-sel";
            se.add(j);
            opt.add(std::string(s.str()), Bob::Property(se.str(), 
                "a selection pattern for this curve","", &crv[i].sel[j]));
         }
      }
      for (int i=0;i<MaxField; i++ ) {
         Bob::strbuff<6> s="-p";
         s.add(i);
         opt.add(std::string("--sel"), Bob::Property(s.str(), 
                "run or line to select", "",&line_select[i]));
      }
      opt.add(std::string("--x"), Bob::Property("-p","field on x", "",&x_field));
      opt.add(std::string("--x"), Bob::Property("-l","label on x", "",&x_label));
      opt.parse(n,v);
      if (opt.BVal("--", "-help")) {
         opt.Usage();
         exit(0);
      }

   }
};

Bob::Opt Option::opt;
std::string Option::filename("collect.txt");
std::string Option::outfilename="out.txt";

bool Option::log=false;
bool Option::notxt=false;
bool Option::x11=false;
bool Option::postscript=false;
bool Option::beamer=false;
int Option::nbcrv = 0;
bool Option::hasy2= false;
std::vector<Curve> Option::crv(MaxField);
int Option::nbls = MaxField;
std::vector<std::string > Option::line_select(MaxField);
//std::string  Option::line_label;
std::string  Option::x_field;
std::string  Option::x_label;

bool is_selected(char *b) {
    //if ( strstr(b,Option::x_field.c_str())==NULL ) return false;
    for (int i=0;i<Option::nbls;i++ ) {
       if ( Option::line_select[i].length()!=0 && 
             strstr(b,Option::line_select[i].c_str())==NULL ) return false;
    }
    return true;
}


double getx(char *line) {
    char *s;
    char *da;
    double x;
    da = line;
    /*if ( da==NULL ) {
      std::cerr << " There is not data part (found with the '}' char)"<<std::endl;
      exit(1);
    }*/
    if ( Option::x_field.length()==0 )  {
      std::cerr << " x Field is empty"<<std::endl;
      return -1;
    }
    if ( (s=strstr(da, Option::x_field.c_str()))==NULL ) {
      std::cerr << " Field " << Option::x_field << "not found"<<std::endl;
      exit(1);
    }
    while (!isdigit(*s) && *s!=0) s++;
    //std::cout << "Int:"<<s<<std::endl;
    x = atof(s);
    return x;
}

void count_nb_crv() {
   Option::nbcrv=0;
   for (int i=0; i < MaxField ; i ++ ) {
       if ( Option::crv[i].pat.length()>=1 ) {
          Option::nbcrv++;
          std::cout << "y"<<i<<":";
          //Option::crv[i].print();
          if ( Option::crv[i].y2) Option::hasy2=true;
       }
   }
}

void gene_gnuplot(char *v) {
   if ( Option::nbcrv==0 ) {
      std::cerr << v<< " No curve data to generate to gnuplot" <<std::endl;
      exit(1);
   }
   std::ofstream f(Option::outfilename.c_str());
   if ( !f.is_open() ) {
      std::cerr << v<< " Error : cannot create "<<Option::outfilename<<" file" <<std::endl;
      exit(1);
   }
   if ( Option::log ) {
      f << "set logscale y\n";
   }
   if ( Option::hasy2 ) {
      f << "set y2tics\n";
   }
   f << "set term postscript eps 12 colour\nset output \"res.eps\"\n";
   f << "plot \"-\" axes "<< (Option::crv[0].y2 ?"x1y2":"x1y1")<< " smooth unique title \"";
   f << Option::crv[0].label;
   f << "\" with linespoints";
   for (int i=1; i < Option::nbcrv ; i ++ ) {
      f << ", \"-\" axes "<< (Option::crv[i].y2 ?"x1y2":"x1y1") <<" smooth unique title \"";
      f << Option::crv[i].label;
      f << "\" with linespoints";
   } 
   f << "\n";
   for (int i=0; i < Option::nbcrv ; i ++ ) {
       Option::crv[i].fillbuff(f);
   }
   f.close();
}

int main(int n, char **v) {
   FILE *f;
   char line[512];

   Option::init(n,v);
   if ( (f=fopen(Option::filename.c_str(),"r"))==NULL ) {
      std::cerr << v[0]<< " Error : filename "<<Option::filename<<" not found" <<std::endl;
   }
   while ( !feof(f) ) {
      fgets(line,512,f);
      if ( is_selected(line) ) {
          std::cout << " Selection "<<line <<std::endl;
          for (int i=0; i < MaxField ; i ++ ) {
             Option::crv[i].init(i);
             Option::crv[i].getval(line);
          }
      }
   }
   fclose(f);
   count_nb_crv();
   gene_gnuplot(v[0]);
}
