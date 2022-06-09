

class VrpNode :public Bob::BBLPNode<VrpTrait>, public FCVRP_Graph {
   public:
      typedef VrpTrait::LP TheLP;
      typedef VrpTrait::Instance TheInstance;
      typedef VrpTrait::Stat TheStat;
      typedef VrpTrait::Node TheNode;
      Bob::pvector<Path> paths;
      /** Constructor
        */
      VrpNode() :Bob::BBLPNode<VrpTrait>(),FCVRP_Graph(),paths()  { 
      }
      /** Constructor
        */
      VrpNode(const VrpNode &vn) :Bob::BBLPNode<VrpTrait>(vn),FCVRP_Graph(vn),paths(vn.paths)  { 
      }
      /** Constructor
        */
      VrpNode(const TheInstance *inst);
      /** Destructor
        */
      ~VrpNode() {}
      /** Performs a complete evaluation with cuts and variables generation
       * @param inst the Instance
       * @param st the statistics
       * @param max the value of the incubent.
       */
      virtual void eval(const TheInstance *inst, TheStat *st, double max); 
      /** Do the ith branches 
       * @param i the branch to compute.
       * @param st the statistics
       */
      virtual void do_branch(int i, TheStat *st) {
        std::cout <<"do branch of the vrp....\n";
        bi.do_branch(i, (TheNode *)this);
      }
      /** method to print the node data
        */
      virtual std::ostream &Prt(std::ostream &os) {
         Bob::BBLPNode<VrpTrait>::Prt(os);
         FCVRP_Graph::Prt(os);
         paths.Prt(os);
         return os;
      }

};

