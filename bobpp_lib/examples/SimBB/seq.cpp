
#include<bobpp/bobpp>

class MyProblem;
class MyInstance;
class MyGenChild;
class MyNode;


class MyTrait {
public:
    typedef MyNode Node;
    typedef MyInstance Instance;
    typedef MyGenChild GenChild;
    typedef Bob::BBAlgo<MyTrait> Algo;
    typedef Bob::BBGoalBest<MyTrait> Goal;
    typedef Bob::BestEPri<MyNode> PriComp;
    typedef Bob::BBStat Stat;
};

class MyNode : public Bob::BBIntMinNode {
    unsigned int seed;
public:
    MyNode() : Bob::BBIntMinNode(0), seed(0) {
        DBGAFF_USER("MyNode::MyNode()", "Called");
    }
    MyNode(int n, int s) : Bob::BBIntMinNode(n), seed(s) {
        DBGAFF_USER("MyNode::MyNode(Eval)", "Called");
    }
    virtual ~MyNode() {
        DBGAFF_USER("MyNode::~MyNode", "Called");
    }
    unsigned int getseed() const {
        return seed;
    }
    virtual void copy(const MyNode &mn)  { 
         Bob::BBIntMinNode::copy(mn);
         seed = mn.getseed();
    }

};

class MyProblem : public Bob::Problem<MyTrait> {
public:
    MyProblem() : Bob::Problem<MyTrait>() {}
    virtual ~MyProblem() {}
};

class MyInstance : public Bob::BBInstance<MyTrait> {
protected:
    int lb, ub, step, nbs;
public:
    /// Constructor
    MyInstance() : Bob::BBInstance<MyTrait>("SM"), lb(0), ub(0), step(0), nbs(0)  {
        DBGAFF_USER("MyInstance::~MyInstance()", "Called");
    }
    /// Constructor
    MyInstance(int _lb, int _ub, int _step, int _nbs) : Bob::BBInstance<MyTrait>("SM"), lb(_lb), ub(_ub), step(_step), nbs(_nbs) {
        DBGAFF_USER("MyInstance::~MyInstance()", "Called");
    }
    virtual ~MyInstance() {/*std::cout << "Destructor MyInstance\n";*/
    }
    virtual void copy(const MyInstance &mn)  { 
         Bob::BBInstance<MyTrait>::copy(mn);
         lb= mn.lb;
         ub= mn.ub;
         step= mn.step;
         nbs= mn.nbs;
    }
    ///
    int getnbson() const {
        return nbs;
    }
    ///
    int getstep() const {
        return step;
    }
    /// The init method
    void Init() { }
    /// The init Algo method
    void InitAlgo(MyTrait::Algo *a) const {
        std::cout << "MyInstance::Init(Algo)" << std::endl;
        MyNode *nd = new MyNode(lb, 0);
        //a->log_space(nd);
        a->GPQIns(nd);
    }
    /// Get the initial best known solution
    MyNode *getSol() {
        std::cout << "Instance::getSol " << ub << "\n";
        return new MyNode(ub, 0);
    }
};

class MyGenChild: public Bob::BBGenChild<MyTrait> {
public:
    /// Constructor of MyGenChild
    MyGenChild(const MyTrait::Instance *_inst, MyTrait::Algo *_al) :
            Bob::BBGenChild<MyTrait>(_inst, _al) {
        DBGAFF_USER("MyGenChild::MyGenChild()", "Called");
    }
    virtual ~MyGenChild() {
        DBGAFF_USER("MyGenChild::~MyGenChild()", "Called");
    }
    /// Operator called by the algo to Generate child
    bool operator()(MyNode *n) {
        bool fl = false;
        int nbs = inst->getnbson();
        int step = inst->getstep();
        unsigned int seed = n->getseed();

        DBGAFF_USER("MyGenChild::operator()", "is beginning");
        int i;
        for (i = 0;i < nbs;i++) {
            int r = rand_r(&seed);
            int v = 1 + (int)((float)(step) * ((float)r / (RAND_MAX + 1.0)));

            printf("%d %d %u %d %d\n", n->getEval(), i, r, v, step);
            MyNode *ns = new MyNode(n->getEval() + v, seed);
            algo->start_eval(ns,n);
            //std::cout <<"Alloc Son "<<i<<" : "<<n->getEval()<<std::endl;
            algo->end_eval(ns);
            fl +=algo->Search(ns);
        }
        DBGAFF_USER("MyGenChild::operator()", "is ending");
        return fl;
    }
};


int main(int n, char ** v) {

    Bob::core::opt().add(std::string("--bb"), Bob::Property("-lb", "Initial Value for the root node", 0));
    Bob::core::opt().add(std::string("--bb"), Bob::Property("-ub", "Initial Value for the Solution", 20));
    Bob::core::opt().add(std::string("--bb"), Bob::Property("-st", "MAximum Step to increase the evaluation of a son node", 4));
    Bob::core::opt().add(std::string("--bb"), Bob::Property("-ns", "number of sons", 2));
    Bob::SeqBBAlgoEnvProg<MyTrait> saenv;
    Bob::core::Config(n, v);
    MyProblem mp();

    MyInstance *Instance = new MyInstance(Bob::core::opt().NVal("--bb", "-lb"),
                                          Bob::core::opt().NVal("--bb", "-ub"),
                                          Bob::core::opt().NVal("--bb", "-st"),
                                          Bob::core::opt().NVal("--bb", "-ns"));
    // Initialize the instance.

    // Solve the instance i.e. all the tree
    std::cout << "-------- Start to solve the entire tree" << std::endl;
    saenv(Instance);
    /* Alloc one algo.
     * lance le Init de Instance
     * lance le () de l'algo
     */

    MyNode *nd = new MyNode(Bob::core::opt().NVal("--bb", "-lb"), 0);
    // Initialize the node

    std::cout << "-------- Start to solve a subtree" << std::endl;
    // solve only the subtree rooted by the node nd.
    saenv(nd, Instance);
    /* Alloc one algo.
     * Recupere la meilleur solution connue
     * insere le nd
     * lance le () de l'algo
     */
    delete Instance;
    Bob::core::End();
}
