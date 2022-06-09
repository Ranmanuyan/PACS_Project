/*
 * =====================================================================================
 *
 *       Filename:  reader.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.09.2008 10:33:29
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  first_name last_name (fl), fl@my-company.com
 *        Company:  my-company
 *
 * =====================================================================================
 */

#include "vrp.hpp"

void VrpData::init() {
         //Prt(std::cout);
         if ( type<TOK_TYPE_CVRP || type>=TOK_TYPE_LAST ) { 
            std::cerr <<" Type not supported \n";
            exit(1);
         } 
         if ( edge<TOK_EDGEWT_EUC2D || edge>=TOK_EDGEWT_LAST ) { 
            std::cerr <<" EDGE_WEIGHT_TYPE  not supported \n";
            exit(2);
         } 
         if ( edge== TOK_EDGEWT_EUC2D ) {
            std::cout << "Alloc coord\n";
            coord.resize(dime);
         } 
         weight=0;
         demand = new int[dime];
}

int VrpReader::MatchIntegerLine(int tok) {
         int val;
         Match(tok); 
         Match(':');
         Match(TOK_INT);
         val = ov.dval;
         Match('\n');
         return val;
}
int VrpReader::MatchTokLine(int tok) {
         Match(tok); 
         Match(':');
         int t=lookhead();
         Match(lookhead());
         Match('\n');
         return t;
}
void VrpReader::MatchStrLine(int tok,std::string &rs) {
         Match(tok); 
         Match(':');
         Match(TOK_ID);
         rs = ov.sval;
         Match('\n');
}
void VrpReader::MatchMultStrLine(int tok) {
         Match(tok); 
         Match(':');
         while (lookhead()!='\n') { Match(lookhead()); }
         Match('\n');
}

void  VrpReader::go(VrpData &vd) {
         int i;
         init();
         MatchMultStrLine(TOK_NAME);
         MatchMultStrLine(TOK_COMM);
         vd.type = MatchTokLine(TOK_TYPE);
         vd.dime=MatchIntegerLine(TOK_DIME);
         vd.edge = MatchTokLine(TOK_EDGEWT);
         vd.capa=MatchIntegerLine(TOK_CAPA);
         vd.veh=MatchIntegerLine(TOK_VEHI);
         vd.init();
         Match(TOK_ND_COORD); 
         Match('\n');
         while (lookhead()!=TOK_DEMA) { 
            Match(TOK_INT); 
            i = ov.dval-1;
            Match(TOK_INT); 
            vd.coord[i].setx(ov.dval);
            Match(TOK_INT); 
            vd.coord[i].sety(ov.dval);
            while (lookhead()!='\n') { Match(lookhead()); }
            Match('\n'); 
         }
         Match(TOK_DEMA); 
         Match('\n');
         while (lookhead()!=TOK_DEPOT) { 
            Match(TOK_INT); 
            i = ov.dval-1;
            Match(TOK_INT); 
            vd.demand[i] = ov.dval;
            Match('\n'); 
         }
         Match(TOK_DEPOT); Match('\n');
         Match(TOK_INT);Match('\n');
         Match(TOK_VEOF);
         if ( lookhead()=='\n' ) Match('\n');
      }


CVRP_Graph *ConsCVRP_Graph(VrpData &vd) {
     CVRP_Graph *cvg =new CVRP_Graph(Load(vd.capa),vd.dime+1);

     std::cout << "Dimension:"<<vd.dime << "\n";
     for (int i=0;i<vd.dime;i++ ) {
         cvg->set(CVRP_Vertex(i,Load(vd.demand[i])));
     } 
     cvg->set(CVRP_Vertex(vd.dime,Load(vd.demand[0])));
     for (int i=0;i<vd.dime;i++ ) {
         for (int j=0;j<vd.dime;j++ ) {
            if ( i!=j ) {
                long dx,dy;
                dx = vd.coord[i].getx()-vd.coord[j].getx();
                dy = vd.coord[i].gety()-vd.coord[j].gety();
                long d=sqrt(dx*dx+dy*dy);
                cvg->set(CVRP_Edge(i,j,Dist(d)));
            }
         }
     } 
     for (int j=0;j<vd.dime;j++ ) {
       long dx,dy;
       dx = vd.coord[j].getx()-vd.coord[0].getx();
       dy = vd.coord[j].gety()-vd.coord[0].gety();
       long d=sqrt(dx*dx+dy*dy);
       cvg->set(CVRP_Edge(j,vd.dime,Dist(d)));
     }

     return cvg;
}
