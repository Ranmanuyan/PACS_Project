
#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<map>
#include<iostream>
#include<fstream>
#include<bobpp/bob_error.h>

namespace Bob {


/** @defgroup BOB_UTIL_PARSER Parse module 
  * This module implements a simple ll(1) parser.
  * @ingroup BOB_UTIL_GRP 
  * the classes proposed are
  *     - the classical token class, used to store
  *       the tokens of your language.
  *     - the value struct to store the different values read i.e. 
  *       string, double or int value.
  *     - a data_reader to read the char form a file or from a string
  *     - parser class which must be derived to implement your parser.
  * @{
  */


/// token for the end of file.
#define TOK_EOF 256
/// token for a integer
#define TOK_INT 257
/// token for a real
#define TOK_REAL 258
/// token for an identifier 
#define TOK_ID 259
/// token for a string 
#define TOK_STRING 260
/// token for a string 
#define TOK_SEPARA 261





/** structure that stores the token information
 */
struct token {
   /// the token stored in a string
   std::string st;
   /// the id of the token.
   int tok;

   /** constructor
     */
   token() : st(""),tok(-1) { }
   /** constructor
     */
   token(const std::string &_st,int tt) : st(_st),tok(tt) { }
};

/** structure that stores different value.
 */
struct value {
   /// integer value
   int dval;
   /// float value
   double fval;
   /// string value
   std::string sval;

   /** Constructor
     */
   value() : dval(0),fval(0.0),sval("") {}
   /** Constructor
     */
   value(const value &v) : dval(v.dval),fval(v.fval),sval(v.sval) {}
   /** Destructor
     */
   ~value() {}
};


/** abstract data reader
 */
class data_reader {
   public:
   /** constructor
     */
   data_reader() {}
   /** destructor
     */
   virtual ~data_reader() {}
   /** return  a char
     */
   virtual int get_c()=0;
   /** unget a char.
     */
   virtual void unget_c(int c)=0;
};

/** data reader from the old C-style FILE *
 */
class file_reader : public data_reader {
   FILE *fh;
   public:
   /** Constructor
     */
   file_reader(const char *s): data_reader() {
      fh = fopen(s,"r");
      if (fh == NULL ) {
         BOBEXCEPTION(ParseException, s, " file not opened");
      }
   }
   /** Destructor
     */
   virtual ~file_reader() { fclose(fh); }
   /** return  a char
     */
   virtual int get_c() { return fgetc(fh); }
   /** unget a char.
     */
   virtual void  unget_c(int c) { ungetc(c,fh); }
};

/** data reader from a std::istream.
 */
class stream_reader: public data_reader {
   int c;
   std::ifstream os;
   public:
   /** Constructor
     */
   stream_reader(const char *s) : data_reader(),c(-1),os(s,std::ios::binary) {       
      if (!os.is_open()) {
         BOBEXCEPTION(ParseException, s, " file not opened");
      }
   }
   /** Destructor
     */
   virtual ~stream_reader()  { os.close(); }
   /** return  a char
     */
   virtual int get_c() { 
       int cc;
       char tc;
       if ( c<=0 ) {
           os.read(&tc,1); 
           if ( os.eof() ) { tc=-1; }
           cc = tc;
       } else {
           cc = c; c=-1;
       }
       //std::cout <<"SR Lu:"<<(char)cc<<"\n";
       return cc;
   }
   /** unget a char.
     */
   virtual void unget_c(int _c) {  
       if ( c<=0 ) {
          c = _c;
       } else {
         BOBEXCEPTION(ParseException, "", " unget null char strange !!!");
       }
   }
};



/** Parser class.
 */
class parser {
   protected:
      /// the current value
      value cv;
      /// the old value
      value ov;
      /// a map storing the tokens of your language.
      std::map<std::string,token> toks;
      /// a number of lines.
      int nbline;
      /// the lookhead.
      int lh;
      /// the data_reader to get the char form a file or from a string.
      data_reader *rd;
      /// Is the carriage return is a separator or a token.
      bool cr_sep;
      /// the matching of space is explicit or not.
      bool match_sep;
      /// Internal method to test if the reader is ok.
      void testrd() {
         if (rd==0 ) { BOBEXCEPTION(ParseException, "","Error parser has no reader");}
      }
   public:
   /** Constructor
     */
   parser():cv(),ov(),toks(),nbline(0),lh(-1),rd(0),cr_sep(false),match_sep(false) { }
   /** Constructor
     * @param _rd the data_reader.
     */
   parser(data_reader *_rd):cv(),ov(),toks(),nbline(0),lh(-1),rd(_rd),cr_sep(false),match_sep(false) { }
   /** Destructor
     */
   virtual ~parser() {}
   /** Add a token
     * @param t a token to add to the list of token.
     */
   void add(const token &t) { toks[t.st]=t; }
   /** Add a token
     * @param st the std::string storing the token.
     * @param tok_type the type of the token.
     */
   void add(std::string &st, int tok_type) {
         add(token(st,tok_type));
   }
   /** Add a token
     * @param st a const char * storing the token.
     * @param tok_type the type of the token.
     */
   void add(const char *st, int tok_type) {
         add(token(st,tok_type));
   }

   /** initialize the parsing
    */
   void init() {
      lh=Lex();
   }
   /** returns the lookhead
    */
   int lookhead() { return lh; }
   /** option that consider if the cr char is a separator or not
     */
   void set_crsep(bool b) { cr_sep=b; }
   /** method to set if the separator "blank" must be matched explicitly
     */
   void set_match_sep(bool b) { match_sep=b; }

   /** get a char
     */
   int get_c() { testrd();return rd->get_c(); }
   /** unget a char
     */
   void unget_c(int c) { testrd();rd->unget_c(c); }
   /** do the lexical analysis.
     */
   int Lex();
   /** the match the given token.
     * @param tok the token to match
     */
   void Match(int tok);
   /** method to convert th token in string.
     */
   std::ostream &tostr(int tok,std::ostream &os) ;
   /** method to get the old value
    */
   value &get_oldvalue() { return ov;}
   /** method to get the current value
    */
   value &get_value() { return cv;}
};

/** 
  * @}
  */
};
