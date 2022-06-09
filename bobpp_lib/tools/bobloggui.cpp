
#include"boblog.hpp"
//#include"boblogview.hpp"


logheadermap lm;
LoglineSet lgs;

/** operator to display a line header on a std::ostream
  */
std::ostream &operator<<(std::ostream &os, const logheader &lh) {
  lh.Prt(os);
  return os;
}

std::ostream &operator<<(std::ostream &os, const logheadermap &lm) {
  for(logheadermap::const_iterator it=lm.begin(); it!=lm.end(); it++ ) {
     os << (*it).second;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const logline &lh) {
  lh.Prt(os);
  return os;
}
std::ostream &operator<<(std::ostream &os, const LoglineSet &lm) {
  lm.Prt(os);
  return os;
}


void usage(char *s, opt *o) {
   fprintf(stderr,"Usage : %s [-l ] [-r value] [-n value] [-h value] [-w value ] [-s value] [-o filename] <file>\n",s);
   fprintf(stderr,"\t\t-g       : gtk visualization\n");
   fprintf(stderr,"\t\t-d       : generate dot file\n");
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
   FILE *dest;
   int oc;

   while ((oc = getopt(n, v, "gdla:r:n:h:w:s:o:m:")) != -1) {
     switch (oc) {
       case 'g': o.gtk=1; break;
       case 'd': o.gene_dot=1; break;
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
   Bob::file_reader sr(o.sfile);
   if ( (dest = fopen(o.ofile,"w"))==NULL ) {
      perror(o.ofile);
      exit(3);
   }
   if ( !o.fl_max ) {
      fprintf(stdout,"Warning : This version do not compute the Max value of the node\n");
      fprintf(stdout,"          then we can not generate colored nodes\n");
   }
   LogParse lp(&sr);
   lp.parse(&o,dest);
   
   if ( o.gene_dot ) {
      TreeDotFile tdf(dest);
      tdf.head(o);
      tdf.gene(o);
      tdf.tail();
      fclose(dest);
   }
   if ( o.gtk ) {
     //gtkinter::init(n,v);
   }
   return 0;
}

