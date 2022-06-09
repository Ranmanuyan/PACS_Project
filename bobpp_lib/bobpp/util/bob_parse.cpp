
#include "bob_parse.hpp"

namespace Bob {

char str_eof[]="end of file";
char str_int[]="integer value";
char str_real[]="real value";
char str_id[]="identifier";
char str_string[]="string";
char str_separa[]="separator (blank)";
char str_nl[]="\\n";
char str_notknow[]="char not known";

/*------------------------------------------------------------------*/
int parser::Lex() {
long c;
int i=0;
char str[512];
char *s=&(str[0]);

   ov = cv;
   c = get_c();
   //std::cout <<"Lu :"<<c<<" crsep"<<cr_sep<<"\n";
   while ( c==' ' || c=='\t' || (cr_sep && c=='\n' ) ) {
        i=1;
        c = get_c();
   }
   if ( match_sep && i==1 ) {
      unget_c(c);
      return TOK_SEPARA;
   }
   i=0;
   if ( isdigit(c) ) {
      cv.dval=0;
      while ( isdigit(c) ) {
         cv.dval= cv.dval*10 + c-'0';
         c = get_c();
      }
      if ( c!='.' ) {
         unget_c(c);
         return TOK_INT;
      }
      cv.fval = (double)cv.dval;
      i = 10;
      c = get_c();
      while ( isdigit(c) ) {
        cv.fval= cv.fval+ (double)(c-'0')/i;
	      i*=10;
        c = get_c();
      }
      unget_c(c);
      return TOK_REAL;
   }
   if ( isalpha(c) ) {
      while ( isalpha(c)||isdigit(c)||c=='_' ) {
         *s++ = c;
         c = get_c();
      }
      *s=0;
      unget_c(c);
      i = 0;
      int tk = toks[std::string(str)].tok;
      cv.sval=str;
      if ( tk<1000 ) return TOK_ID;
      return tk;
   }
   if ( c=='"' ) {
      c = get_c();
      while ( c!='"' && c!='\n' ) {
         *s++ = c;
         c = get_c();
         if ( c=='\n' ) { 
           std::cerr<<"Error String not on several lines : "<<nbline<<"\n"; 
           BOBEXCEPTION(ParseException,"" , " Lex error");
         }
      }
      *s=0;
      cv.sval=str;
      //unget_c(c);
      return TOK_STRING;
   }

   if ( c=='\n' ) {nbline ++; return '\n';}
   if ( c<=0 ) 
      return TOK_EOF;
   return (int)c;
}


/*------------------------------------------------------------------*/
void parser::Match(int Tok) {
    if ( lh==Tok ) {
       lh= Lex();
       return;
    }
    std::cerr <<"Parse error on line "<<nbline<<" expected token \"";
    tostr(Tok,std::cerr);
    std::cerr <<"\" ("<< Tok <<") got token \"";
    tostr(lh,std::cerr);
    std::cerr << "\" ("<<lh;
    std::cerr <<")\n";
    BOBEXCEPTION(ParseException,"" , " Parse error");
}

std::ostream &parser::tostr(int tok,std::ostream &os) {
      if ( tok>255) {
        switch(tok) {
           case TOK_EOF : os <<str_eof; break;
           case TOK_INT : os <<str_int; break;
           case TOK_REAL : os <<str_real; break;
           case TOK_ID : os <<str_id; break;
           case TOK_STRING :os <<str_string; break;
           case TOK_SEPARA :os <<str_separa; break;
           default : 
               for (std::map<std::string,token>::iterator it=toks.begin();
                    it!=toks.end();it++) {
                  if ( it->second.tok==tok ) { os << it->second.st; return os;}
               }
               os <<str_notknow;
        }
        return os;
      }
      if ( tok=='\n' ) { os << str_nl; return os; }
      os << (char)tok; 
      return os;
   }

}
