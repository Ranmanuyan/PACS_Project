#include"boblog.hpp"

/* XPM */

static const char * pale_xpm[] = {
"#FF0000", "#FF0400", "#FF0800", "#FF0B00", "#FF1000", "#FF1300", "#FF1700",
"#FF1B00", "#FF1F00", "#FF2200", "#FF2700", "#FF2A00", "#FF2E00", "#FF3100",
"#FF3600", "#FF3A00", "#FF3E00", "#FF4200", "#FF4500", "#FF4900", "#FF4D00",
"#FF5100", "#FF5500", "#FF5900", "#FF5C00", "#FF6000", "#FF6300", "#FF6800",
"#FF6B00", "#FF7000", "#FF7300", "#FF7700", "#FF7B00", "#FF7F00", "#FF8300",
"#FF8600", "#FF8A00", "#FF8E00", "#FF9100", "#FF9500", "#FF9900", "#FF9D00",
"#FFA100", "#FFA500", "#FFA800", "#FFAD00", "#FFB000", "#FFB400", "#FFB800",
"#FFBC00", "#FFC000", "#FFC300", "#FFC800", "#FFCB00", "#FFCF00", "#FFD300",
"#FFD700", "#FFDB00", "#FFDE00", "#FFE200", "#FFE600", "#FFEA00", "#FFED00",
"#FFF200", "#FFF600", "#FFF900", "#FFFD00", "#FDFF00", "#F9FF00", "#F5FF00",
"#F2FF00", "#EDFF00", "#EAFF00", "#E6FF00", "#E3FF00", "#DEFF00", "#DBFF00",
"#D6FF00", "#D3FF00", "#CFFF00", "#CBFF00", "#C7FF00", "#C4FF00", "#BFFF00",
"#BCFF00", "#B8FF00", "#B4FF00", "#B0FF00", "#ACFF00", "#A8FF00", "#A5FF00",
"#A1FF00", "#9DFF00", "#99FF00", "#96FF00", "#92FF00", "#8EFF00", "#8AFF00",
"#86FF00", "#83FF00", "#7EFF00", "#7BFF00", "#77FF00", "#73FF00", "#6FFF00",
"#6BFF00", "#68FF00", "#64FF00", "#60FF00", "#5CFF00", "#58FF00", "#55FF00",
"#50FF00", "#4CFF00", "#48FF00", "#45FF00", "#41FF00", "#3DFF00", "#39FF00",
"#35FF00", "#32FF00", "#2EFF00", "#2BFF00", "#27FF00", "#22FF00", "#1FFF00",
"#1AFF00", "#17FF00", "#13FF00", "#0FFF00", "#0BFF00", "#08FF00", "#04FF00",
"#00FF00", "#00FF04", "#00FF08", "#00FF0C", "#00FF0F", "#00FF13", "#00FF17",
"#00FF1B", "#00FF1E", "#00FF22", "#00FF26", "#00FF2A", "#00FF2E", "#00FF32",
"#00FF36", "#00FF39", "#00FF3D", "#00FF42", "#00FF45", "#00FF49", "#00FF4D",
"#00FF51", "#00FF55", "#00FF58", "#00FF5C", "#00FF60", "#00FF64", "#00FF68",
"#00FF6C", "#00FF70", "#00FF73", "#00FF77", "#00FF7B", "#00FF7E", "#00FF82",
"#00FF86", "#00FF8A", "#00FF8E", "#00FF91", "#00FF95", "#00FF99", "#00FF9D",
"#00FFA1", "#00FFA4", "#00FFA9", "#00FFAC", "#00FFB1", "#00FFB4", "#00FFB8",
"#00FFBC", "#00FFBF", "#00FFC3", "#00FFC8", "#00FFCB", "#00FFCF", "#00FFD3",
"#00FFD6", "#00FFDB", "#00FFDE", "#00FFE2", "#00FFE6", "#00FFEA", "#00FFEE",
"#00FFF2", "#00FFF6", "#00FFF9", "#00FFFD", "#00FDFF", "#00F9FF", "#00F5FF",
"#00F1FF", "#00EEFF", "#00EAFF", "#00E6FF", "#00E2FF", "#00DEFF", "#00DBFF",
"#00D7FF", "#00D2FF", "#00CFFF", "#00CCFF", "#00C8FF", "#00C4FF", "#00C0FF",
"#00BCFF", "#00B8FF", "#00B4FF", "#00B0FF", "#00ACFF", "#00A9FF", "#00A5FF",
"#00A1FF", "#009EFF", "#009AFF", "#0096FF", "#0091FF", "#008EFF", "#008AFF",
"#0086FF", "#0083FF", "#007FFF", "#007BFF", "#0077FF", "#0073FF", "#006FFF",
"#006BFF", "#0067FF", "#0064FF", "#0060FF", "#005CFF", "#0059FF", "#0054FF",
"#0051FF", "#004CFF", "#0049FF", "#0045FF", "#0041FF", "#003DFF", "#0039FF",
"#0035FF", "#0031FF", "#002EFF", "#002AFF", "#0026FF", "#0022FF", "#001FFF",
"#001BFF", "#0017FF", "#0013FF", "#000FFF", "#000CFF", "#0008FF", "#0004FF",
"#0000FF", "#0300FF", "#0700FF", "#0C00FF", "#0F00FF", "#1300FF", "#1700FF",
"#1B00FF", "#1F00FF", "#2200FF", "#2600FF", "#2A00FF", "#2E00FF", "#3200FF",
"#3600FF", "#3A00FF", "#3E00FF", "#4100FF", "#4500FF", "#4900FF", "#4D00FF",
"#5100FF", "#5400FF", "#5800FF", "#5C00FF", "#6000FF", "#6400FF", "#6700FF",
"#6B00FF", "#6F00FF", "#7300FF", "#7700FF", "#7B00FF", "#7E00FF", "#8200FF",
"#8700FF", "#8A00FF", "#8E00FF", "#9100FF", "#9500FF", "#9900FF", "#9D00FF",
"#A100FF", "#A500FF", "#A900FF", "#AC00FF", "#B100FF", "#B500FF", "#B800FF",
"#BC00FF", "#C000FF", "#C400FF", "#C700FF", "#CB00FF", "#CF00FF", "#D300FF",
"#D700FF", "#DA00FF", "#DE00FF", "#E200FF", "#E600FF", "#EA00FF", "#EE00FF",
"#F100FF", "#F600FF", "#F900FF", "#FD00FF", "#FF00FD", "#FF00F9", "#FF00F5",
"#FF00F2", "#FF00ED", "#FF00EA", "#FF00E6", "#FF00E2", "#FF00DE", "#FF00DA",
"#FF00D7", "#FF00D3", "#FF00CF", "#FF00CC", "#FF00C7", "#FF00C3", "#FF00C0",
"#FF00BC", "#FF00B8", "#FF00B5", "#FF00B0", "#FF00AC", "#FF00A8", "#FF00A5",
"#FF00A1", "#FF009D", "#FF0099", "#FF0096", "#FF0092", "#FF008E", "#FF008A",
"#FF0086", "#FF0083", "#FF007E", "#FF007A", "#FF0077", "#FF0073", "#FF006F",
"#FF006B", "#FF0068", "#FF0064", "#FF0060", "#FF005C", "#FF0058", "#FF0054",
"#FF0050", "#FF004D", "#FF0049", "#FF0045", "#FF0041", "#FF003D", "#FF003A",
"#FF0036", "#FF0032", "#FF002E", "#FF002A", "#FF0027", "#FF0023", "#FF001F",
"#FF001B", "#FF0017", "#FF0013", "#FF000F", "#FF000C", "#FF0007", "#FF0004",
"#FF0001",
};

static char black[]="#000000";

const char *Color(double min,double v, double max) {
   int ind= (((v-min)*NB_PAL)/(max-min));
   //printf("I:%d x:%d delta:%d\n",ind,n->cost-Root_Init,Sol_Init-Root_Init);
   /*if ( (Sol_Init-Root_Init)<NB_PAL ) {
      ind = n->cost-Root
   }*/
   if ( ind <0 || ind >=NB_PAL ) return black;
   return pale_xpm[ind];
}

void TreeDotFile::gene(const opt &o) {
    std::cout << "Generated dot file"<< lgs.nll.size() <<"\n";
    for (unsigned int i=0; i < lgs.nll.size() ; i++ ) {
       if ( lgs.nll[i].nid>=0 && lgs.nll[i].type=='c' ) {
          if ( lgs.nll[i].pid<0 ) {
            fprintf(f,"n%ld [fillcolor=\"%s\"]\n",lgs.nll[i].nid, Color(lgs.min_val,lgs.nll[i].eval,lgs.max_val));
          } else {
             if ( lgs.nll[i].n_type=='n' ) {
               fprintf(f,"n%ld-- { n%ld [color=\"%s\" style=filled fillcolor=\"%s\"]}\n",lgs.nll[i].pid,lgs.nll[i].nid,Color(0,10*lgs.nll[i].thr,399),Color(lgs.min_val,lgs.nll[i].eval,lgs.max_val));
             } else {
               fprintf(f,"n%ld-- { n%ld [color=\"%s\" style=filled fillcolor=\"%s\" shape=box]}\n",lgs.nll[i].pid,lgs.nll[i].nid,Color(0,100*lgs.nll[i].thr,399),Color(lgs.min_val,lgs.nll[i].eval,lgs.max_val));
             }
          }
       }
    }
}


void LogParse::MatchStatId(opt *o,std::string &sid) {
   char t[10];
   bool fli=false;
   Match(TOK_ID);
   sid.clear();
   sid.append(get_oldvalue().sval);
   Match(':');
   sid.append(":");
   if ( lookhead()==TOK_ID ) {
      Match(TOK_ID);
      sid.append(get_oldvalue().sval);
   }
   else {
      Match(TOK_INT);
      sprintf(t,"%d",get_oldvalue().dval);
      sid.append(t);
      fli=true;
   }
   Match(':');
   sid.append(":");
   Match(TOK_ID);
   sid.append(get_oldvalue().sval);
   if ( fli ) {
     Match(':');
     sid.append(":");
     Match(TOK_ID);
     sid.append(get_oldvalue().sval);
   }
}

void LogParse::parse(opt *o,FILE *d) {
retins r;

   init();
   Match('{');
   Match('\n');
   while (lookhead()!='}' ) {
      logheader lh;
      Match(TOK_ID);
      if ( *(get_oldvalue().sval.c_str())=='g' ) lh.local=false;
      else lh.local=true;
      MatchStatId(o,lh.id);
      Match(':');
      Match(TOK_ID);
      if ( strcmp("COUNT",get_oldvalue().sval.c_str()) == 0 ) {
            lh.type_op='c';
      } else if ( strcmp("TIME",get_oldvalue().sval.c_str()) == 0 ) {
            lh.type_op='t';
      }
      Match(':');
      lh.libelle[0]=0;
      while ( lookhead()!='\n' ) {
         Match(TOK_ID);
         strcat(lh.libelle,get_oldvalue().sval.c_str());
         strcat(lh.libelle," ");
      }
      Match('\n');
      r=lm.insert(lmpair(lh.id,lh));
      if (r.second==false) {
         cout << "element already existed"<< r.first->second << endl;
      }
   }
   Match('}');
   std::cout << "Finish to parse header\n";
   std::cout << lm;
   Match('\n');
   while ( lookhead()!=TOK_EOF ) {
      logline lg;
      Match(TOK_REAL);
      lg.time = get_oldvalue().fval;
      Match(TOK_INT);
      lg.thr = get_oldvalue().dval;
      Match('-');
      MatchStatId(o,lg.id);
      Match(TOK_INT);
      lg.nb = get_oldvalue().dval;
      if ( lookhead()==TOK_ID ) {
        lg.type='t';
        Match(TOK_ID);
        lg.es = *(get_oldvalue().sval.c_str());
        if ( lg.es!='e'&& lg.es!='s' )  { 
          std::cerr<<"Parse error on line "<<nbline<<std::endl;
          BOBEXCEPTION(Bob::ParseException, "e or s", " are allowed for type");
        }
        Match(TOK_REAL);
        if ( lg.es=='e' ) lg.duration = get_oldvalue().fval;
      } else
        lg.type='c';
      if ( lookhead()=='(' ) {
        has_space=true;
        Match('(');
        Match(TOK_INT);
        lg.nid = get_oldvalue().dval;
        if ( lookhead()==',' ) {
           Match(',');
           Match(TOK_INT);
           lg.d = get_oldvalue().dval;
           Match(',');
           Match(TOK_ID);
           lg.n_type = *(get_oldvalue().sval.c_str());
           if ( lookhead()==',' ) {
              Match(',');
              if ( lookhead()==TOK_INT ) {
                Match(TOK_INT);
                lg.eval= get_oldvalue().dval;
                if ( lg.eval<lgs.min_val ) lgs.min_val=lg.eval;
                if ( lg.eval>lgs.max_val ) lgs.max_val=lg.eval;
              } else {
                Match(TOK_REAL);
                lg.eval= get_oldvalue().fval;
                if ( lg.eval<lgs.min_val ) lgs.min_val=lg.eval;
                if ( lg.eval>lgs.max_val ) lgs.max_val=lg.eval;
              }
              if ( lookhead()==',' ) {
                Match(',');
                Match(TOK_STRING);
                lg.ud = get_oldvalue().sval;
              }
           } 
        } else if ( lookhead()==':' ){
           Match(':');
           Match(TOK_INT);
           lg.pid = get_oldvalue().dval;
        }
        Match(')');
      } else lg.nid=-1;
      Match('\n');
      lgs.Ins(lg);
        //lg.Prt();
   }
   std::cout << lgs;
}


void LoglineSet::Ins(logline &lg) {
       if ( (lg.thr)>=nbthr ) {nbthr=lg.thr+1;lgm.resize(nbthr); }
       if ( lg.type=='c' ) {
          lgm[lg.thr].insert(lgmpair(lg.time,lg));
       } else if ( lg.type=='t' ) {
          if ( lg.es=='e' ) {
            for(tmloglinemap::reverse_iterator it=lgm[lg.thr].rbegin(); it!=lgm[lg.thr].rend(); ++it ) { 
              if ( (*it).second.id.compare(lg.id)==0 && (*it).second.nb==lg.nb) {
                 (*it).second.duration= lg.duration; break;
              }
            }
          } else {
             lgm[lg.thr].insert(lgmpair(lg.time,lg));
          }
       } 
       if ( lg.nid >-1 ) {
          if ( ((unsigned int)lg.nid)>= nll.size() ) { nll.resize(lg.nid+50,logline()) ;}
          if ( nll[lg.nid].nid<0 ) { /*std::cout << "nid:"<<lg.nid<<std::endl;*/ nll[lg.nid]=lg; }
          if ( nll[lg.nid].pid<0 && lg.pid>-1 ) { /*std::cout << "Pid for nid:"<<lg.nid<<std::endl;*/ nll[lg.nid].pid=lg.pid;}
          if ( nll[lg.nid].d<0 && lg.d>-1 ) { 
              //std::cout << "D and ... for nid:"<<lg.nid<<std::endl; 
              nll[lg.nid].d=lg.d;
              nll[lg.nid].n_type=lg.n_type;
              nll[lg.nid].eval=lg.eval;
              nll[lg.nid].ud=lg.ud;
          }
          
       }

   }

