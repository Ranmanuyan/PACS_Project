/*
 * =====================================================================================
 *
 *       Filename:  instance.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12.09.2008 10:53:15
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Bertrand Le Cun (blc), Bertrand.Lecun@prism.uvsq.fr
 *        Company:  Universite de Versailles Saint-Quentin.
 *
 * =====================================================================================
 */


class VrpInstance : public Bob::BBLPInstance<VrpTrait> {
   std::string fname;
   CVRP_Graph *cvrp;
   public:
      /// Constructor
      VrpInstance() : Bob::BBLPInstance<VrpTrait> (),fname(),cvrp(0) { }
      /// Constructor
      VrpInstance(const std::string &_fn) : Bob::BBLPInstance<VrpTrait> (),
          fname(_fn),cvrp(0) { }
      /// Initialize the Instance
      void Init() {
         VrpReader vreader(VrpOpt::fname.c_str());
         VrpData vdata;
         vreader.go(vdata);
         vdata.Prt(std::cout);
         cvrp = ConsCVRP_Graph(vdata);
         cvrp->Prt(std::cout);
         lp = new TheLP();
         lp->add_ref();
         InitLP(cvrp,lp);

      }
      /*void InitAlgo(VrpTrait::Algo *al) const {
         std::cout << "je suis là\n";
      }*/
      const CVRP_Graph *getgraph() const { return cvrp; }
};
