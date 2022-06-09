/*
 * =====================================================================================
 *
 *       Filename:  bobtree.cpp
 *
 *    Description:  executable to generate a tree (with dot application) from the log file
 *                  bobtree must not be used any more, it has been replaced by, bobloggui.
 *
 *        Version:  1.0
 *        Created:  14.03.2008 15:52:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bertrand.Lecun@prism.uvsq.fr
 *        Company:  PRiSM Laboratory UVSQ
 *
 * =====================================================================================
 */

#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<getopt.h>


/* XPM */
#define NB_PAL 400

static char * pale_xpm[] = {
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

char *Color(double min,double v, double max) {
   int ind= (((v-min)*NB_PAL)/(max-min));
   //printf("I:%d x:%d delta:%d\n",ind,n->cost-Root_Init,Sol_Init-Root_Init);
   /*if ( (Sol_Init-Root_Init)<NB_PAL ) {
      ind = n->cost-Root
   }*/
   if ( ind <0 || ind >=NB_PAL ) return black;
   return pale_xpm[ind];
}

#define FILENAME 255
#define MAXLINE 255

typedef struct _opt {
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
} opt;

void init_opt(opt *o) {
   o->ranksep=0.1;
   o->nodesep=0.05;
   o->node_height=0.1;
   o->node_width=0.1;
   o->edge_width=0.1;
   o->value_max=-1.0;
   o->fl_max=0;
   o->wth_lab=0;
   strcpy(o->algo,"");
   strcpy(o->sfile,"file.log");
   strcpy(o->ofile,"file.dot");
}

void head(FILE *f,opt *o) {
   fprintf(f,"/* Create by the bobtree application for the dot application*/\n\n");
   fprintf(f,"graph bb {\n    ranksep=%1.2f\n    nodesep=%1.2f\n",o->ranksep,o->nodesep);
   fprintf(f,"    node [fixedsize=true];\n");
   fprintf(f,"    node [style=filled];\n");
   if ( !o->wth_lab ) {
      fprintf(f,"    node [height=%1.2f];\n    node [width=%1.2f];\n",o->node_height,o->node_width);
      fprintf(f,"    node [label=\"\"];\n");
   }
   fprintf(f,"    edge [width=%1.2f];\n", o->edge_width);
}

void tail(FILE *f) {
   fprintf(f,"}\n");
}

#define TOK_EOF 256
#define TOK_INT 257
#define TOK_REAL 258
#define TOK_ID 259
#define TOK_STRING 260
char str_eof[]="end of file";
char str_int[]="integer value";
char str_real[]="real value";
char str_id[]="identifier";
char str_string[]="string";
char str_nl[]="new line";
char str_notknow[]="char not known";

char *tostr(int tok) {
   static char t[30];
   switch(tok) {
      case TOK_EOF : return str_eof;
      case TOK_INT : return str_int;
      case TOK_REAL : return str_real;
      case TOK_ID : return str_id;
      case TOK_STRING :return str_string;
      case '\n' : return str_nl;
      default : return str_notknow;
   }
   sprintf(t,"%c",tok);
   return t;
}

int LookHead;
int TokenVal;
double TokenDVal;
char TokenStr[80];

int OldTokenVal;
double OldTokenDVal;
char OldTokenStr[80];
FILE *fh;
int nbLine=1;

int Lex() {
long c;
int i=0;
char *s = TokenStr;

   strcpy(OldTokenStr,TokenStr);
   OldTokenVal = TokenVal;
   OldTokenDVal = TokenDVal;
   c = fgetc(fh);
   while ( c==' ' || c=='\t' ) {
        c = fgetc(fh);
   }
   if ( isdigit(c) ) {
      TokenVal=0;
      while ( isdigit(c) ) {
         TokenVal = TokenVal*10 + c-'0';
         c = fgetc(fh);
      }
      if ( c!='.' ) {
         ungetc(c,fh);
         return TOK_INT;
      }
      TokenDVal = (double)TokenVal;
      i = 1;
      c = fgetc(fh);
      while ( isdigit(c) ) {
         TokenDVal = TokenDVal + (double)(c-'0')/i;
	      i*=10;
         c = fgetc(fh);
      }
      ungetc(c,fh);
      return TOK_REAL;
   }
   if ( isalpha(c) ) {
      while ( isalpha(c)||isdigit(c)||c=='_' ) {
         *s++ = c;
         c = fgetc(fh);
      }
      *s=0;
      ungetc(c,fh);
      /*i = 0;
      while ( strcmp(TokenStr,Type[i].Name) && i<(NBTOKEN-1) ) {i++;}
      TokenVal = Type[i].Token;*/
      return TOK_ID;
   }
   if ( c=='"' ) {
      c = fgetc(fh);
      while ( c!='"' ) {
         *s++ = c;
         c = fgetc(fh);
         if ( c=='\n' ) nbLine++;
      }
      *s=0;
      ungetc(c,fh);
      return TOK_STRING;
   }
   if ( c=='\n' ) {nbLine++; return '\n'; }
   if ( c==EOF ) return TOK_EOF;
   return (int)c;
}

void Match(int Tok) {
    if ( LookHead==Tok ) {
       LookHead = Lex();
       return;
    }
    fprintf(stderr,"Parse error on line %d : %c %s unexpected (has a %s)\n",nbLine,Tok,tostr(Tok),
      tostr(LookHead));
    exit(1);
}

void parse_file(FILE *s,FILE *d,opt *o) {
char algo[20];
double Min=0.0;
double v;
char sol;
int thr=0;
int parentnode;
int childnode;
   fh=s;
   LookHead=Lex();
   while (LookHead!='}' ) {
      Match(LookHead);
   }
   Match('}');
   Match('\n');
   while ( LookHead!=TOK_EOF ) {
      Match(TOK_REAL);
      if ( LookHead=='\n' ) {
         while (LookHead!='}' ) {
            Match(LookHead);
         }
         Match('}');
         Match('\n');
         continue;
      }
      if ( LookHead==TOK_INT || LookHead=='*' || LookHead=='-' ) {
         if ( LookHead==TOK_INT ) {
            thr = TokenVal;
         } else thr=-1;
         Match(LookHead);
      }
      Match('-');
      //fprintf(stderr,"Ok real int :%d\n",nbLine);
      Match(TOK_ID);
      strcpy(algo,OldTokenStr);
      Match(':');
      if ( LookHead==TOK_ID ) {
         Match(TOK_ID);
      } else {
         Match(TOK_INT);
      }
      Match(':');
      Match(TOK_ID);
      if ( LookHead==':' ) {
         Match(':');
         Match(TOK_ID);
      } 
      Match(TOK_INT);
      if ( LookHead==TOK_ID ) {
         Match(TOK_ID);
         Match(TOK_REAL);
      }
      //fprintf(stderr," id:id %d\n",nbLine);
      if ( LookHead=='(' ) {
         Match('(');
         Match(TOK_INT);
         childnode = OldTokenVal;
         if ( LookHead==':' ) {
            Match(':');
            Match(TOK_INT);
            parentnode = OldTokenVal;
         } else if ( LookHead==',' ) {
            Match(',');
            Match(TOK_INT);
            Match(',');
            Match(TOK_ID);
            sol=*OldTokenStr;
            Match(',');
            if ( LookHead==TOK_INT) { Match(TOK_INT); v=OldTokenVal; }
            else { Match(TOK_REAL); v=OldTokenDVal; }
            if ( LookHead==',' ) { Match(','); Match(TOK_STRING); }
         }
         Match(')');
         if ( !*(o->algo) || !strcmp(o->algo,algo) ) {
            if ( parentnode==-1 ) { 
               Min=v; 
               if ( o->fl_max ) {
                  fprintf(d,"n%d [fillcolor=\"%s\"]\n",childnode, Color(Min,v,o->value_max));
               } 
            } else {
               if ( o->fl_max ) {
                  if ( sol=='n' ) {
                     fprintf(d,"n%d -- { n%d [color=\"%s\" style=filled fillcolor=\"%s\"] }\n",parentnode,childnode,
                     Color(0,100*thr,399),Color(Min,v,o->value_max));
                  } else {
                     fprintf(d,"n%d -- { n%d [color=\"%s\" style=filled fillcolor=\"%s\" shape=box] }\n",parentnode,childnode,
                     Color(0,100*thr,399),Color(Min,v,o->value_max));
                  }
               } else {
                  if ( sol=='n' ) {
                     fprintf(d,"n%d -- { n%d }\n",parentnode,childnode);
                  } else {
                     fprintf(d,"n%d -- { n%d [shape=box] }\n",parentnode,childnode);
                  }
               }
            }
         }
      } else {
         //Match(':');
         //Match(TOK_ID);
         while (LookHead!='\n' ) {
            Match(LookHead);
         }
      }
      Match('\n');
   }
}

void usage(char *s, opt *o) {
   fprintf(stderr,"Usage : %s [-l ] [-r value] [-n value] [-h value] [-w value ] [-s value] [-o filename] <file>\n",s);
   fprintf(stderr,"\t\t-l       : with label on nodes (default no label)\n");
   fprintf(stderr,"\t\t-a str   : with label on nodes (default no label)\n");
   fprintf(stderr,"\t\t-r value : the rank separation length (default %1.2f)\n",o->ranksep);
   fprintf(stderr,"\t\t-n value : the node separation length (default %1.2f)\n",o->nodesep);
   fprintf(stderr,"\t\t-h value : height of the node (default %1.2f)\n",o->node_height);
   fprintf(stderr,"\t\t-w value : width of the node (default %1.2f)\n",o->node_width);
   fprintf(stderr,"\t\t-s value : width of the edge (default %1.2f)\n",o->edge_width);
   fprintf(stderr,"\t\t-m value : maximum value for the bound (no default)\n");
   fprintf(stderr,"\t\t-o value : file name to create (default %s)\n",o->ofile);
   fprintf(stderr,"\t\t<file> : the filename to read (default %s)\n",o->sfile);
}

int main(int n, char **v) {
   opt o;
   FILE *src,*dest;
   int oc;

         init_opt(&o);
          while ((oc = getopt(n, v, "la:r:n:h:w:s:o:m:")) != -1) {
               switch (oc) {
               case 'l': o.wth_lab=1; break;
               case 'r': sscanf(optarg,"%f",&o.ranksep); break;
               case 'n': sscanf(optarg,"%f",&o.nodesep); break;
               case 'h': sscanf(optarg,"%f",&o.node_height); break;
               case 'w': sscanf(optarg,"%f",&o.node_width); break;
               case 's': sscanf(optarg,"%f",&o.edge_width); break;
               case 'm': sscanf(optarg,"%f",&o.value_max); o.fl_max=1;break;
               case 'a': strcpy(o.algo,optarg); break;
               case 'o': strcpy(o.ofile,optarg);break;
               default: /* '?' */
                   usage(v[0],&o);
                   exit(1);
               }
           }
           if ( optind<n ) {
               strcpy(o.sfile,v[optind]);
            }
   if ( (src = fopen(o.sfile,"r"))==NULL ) {
      perror(o.sfile);
      exit(2);
   }
   if ( (dest = fopen(o.ofile,"w"))==NULL ) {
      perror(o.ofile);
      exit(3);
   }
   if ( !o.fl_max ) {
      fprintf(stdout,"Warning : This version do not compute the Max value of the node\n");
      fprintf(stdout,"          then we can not generate colored nodes\n");
   }
   head(dest,&o);
   parse_file(src,dest,&o);
   
   tail(dest);
   fclose(dest);
   fclose(src);
   return 0;
}

