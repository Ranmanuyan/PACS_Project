/*
 * =====================================================================================
 *
 *       Filename:  reader.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.09.2008 10:09:01
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */



struct VrpData {
      std::string name;
      int type;
      int dime;
      int edge;
      int capa;
      int veh;
      Bob::pvector<Coord> coord;
      int **weight;
      int *demand;
      int depot;

      void init() ;
      void Prt(std::ostream &os) {
         os << "type: "<<type<<"\n";
         os << "dime: "<<dime<<"\n";
         os << "edge: "<<edge<<"\n";
         os << "capa: "<<capa<<"\n";
         os << "veh : "<<veh <<"\n";
         os << "Coord : \n";
         for (int i=0;i<dime;i++) {
            os << coord[i].getx()<<","<<coord[i].gety()<<"\n";
         }
         os << "Demand : \n";
         for (int i=0;i<dime;i++) {
            os << demand[i]<<"\n";
         }
      }
};

#define TOK_NAME 1000
#define TOK_COMM 1001
#define TOK_TYPE 1002
#define TOK_DIME 1003
#define TOK_EDGEWT 1004
#define TOK_CAPA 1005
#define TOK_VEHI 1006
#define TOK_ND_COORD 1007
#define TOK_ND_SECTION 1008
#define TOK_DEMA 1009
#define TOK_DEPOT 1010
#define TOK_VEOF 1011

#define TOK_TYPE_CVRP 10021
#define TOK_TYPE_LAST 10022

#define TOK_EDGEWT_EUC2D 10041
#define TOK_EDGEWT_EXPLI 10042
#define TOK_EDGEWT_LAST 10043

class VrpReader : public Bob::parser {
  public:
      VrpReader(const char *s) : Bob::parser(new Bob::stream_reader(s)) { 
        set_crsep(false);
        add("NAME",TOK_NAME);
        add("COMMENT",TOK_COMM);
        add("TYPE",TOK_TYPE);
        add("CVRP",TOK_TYPE_CVRP);
        add("DIMENSION",TOK_DIME);
        add("EDGE_WEIGHT_TYPE",TOK_EDGEWT);
        add("EUC_2D",TOK_EDGEWT_EUC2D);
        add("EXPLICITE",TOK_EDGEWT_EXPLI);
        add("CAPACITY",TOK_CAPA);
        add("VEHICLES",TOK_VEHI);
        add("NODE_COORD_SECTION",TOK_ND_COORD);
        add("DEMAND_SECTION",TOK_DEMA);
        add("DEPOT_SECTION",TOK_DEPOT);
        add("EOF",TOK_VEOF);
      }
      virtual ~VrpReader() { }
      int MatchIntegerLine(int tok) ;
      int MatchTokLine(int tok) ;
      void MatchStrLine(int tok,std::string &rs);
      void MatchMultStrLine(int tok) ;
      void go(VrpData &vd); 
};


CVRP_Graph *ConsCVRP_Graph(VrpData &vd);
