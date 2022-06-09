
#include<bobpp/bobpp>


class MyMinNode : public Bob::BBIntMinNode {
        public:
                MyMinNode() : Bob::BBIntMinNode() {}
                MyMinNode(int n) : Bob::BBIntMinNode(n) {}

};
class MyMaxNode : public Bob::BBIntMaxNode {
        public:
                MyMaxNode() : Bob::BBIntMaxNode() {}
                MyMaxNode(int n) : Bob::BBIntMaxNode(n) {}
};

template<class Node>
class DummyGoal {
	public:
		bool is4Search(const Node &n) const { return true; }
};

typedef Bob::PQSkew<MyMinNode,Bob::BestDPri<MyMinNode>,DummyGoal<MyMinNode> > pqminbd;
typedef Bob::PQSkew<MyMinNode,Bob::BestEPri<MyMinNode>,DummyGoal<MyMinNode> > pqminb;
typedef Bob::PQSkew<MyMinNode,Bob::DepthPri<MyMinNode>,DummyGoal<MyMinNode> > pqmind;

typedef Bob::PQSkew<MyMaxNode,Bob::BestDPri<MyMaxNode>,DummyGoal<MyMaxNode> > pqmaxbd;
typedef Bob::PQSkew<MyMaxNode,Bob::BestEPri<MyMaxNode>,DummyGoal<MyMaxNode> > pqmaxb;
typedef Bob::PQSkew<MyMaxNode,Bob::DepthPri<MyMaxNode>,DummyGoal<MyMaxNode> > pqmaxd;

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--node"),Bob::Property("-min"," minimum or maximum (default maximum)"));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-i","inferior limit of evaluation",0));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-s","superior limit of evaluation",20));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-di","distance of n1 ",0));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-ds","distance of n2 ",10));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-nb","nb nodes ",20));
	Bob::core::Config(n,v);
	
	std::cout << "Node Sens of optimization "<<Bob::core::opt().BVal("--node","-min")<<std::endl;

	long ei,es,di,ds,nb,eval,dist;
	ei = Bob::core::opt().NVal("--node","-i");
	es = Bob::core::opt().NVal("--node","-s");
	di = Bob::core::opt().NVal("--node","-di");
	ds = Bob::core::opt().NVal("--node","-ds");
	nb = Bob::core::opt().NVal("--node","-nb");
	if ( Bob::core::opt().BVal("--node","-min") ) {
		MyMinNode *n;int i;
		pqmind pqmd;
		pqminb pqmb;
		pqminbd pqmbd;
		for (i=0;i<nb;i++ ) {
			eval= (rand()%(es-ei))+ei;
			dist= (rand()%(ds-di))+di;
			n = new MyMinNode(eval);
			n->dist() = dist;
			std::cout<<"Insert Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<std::endl;;
			pqmd.Ins(n);
			pqmb.Ins(n);
			pqmbd.Ins(n);
		}
		std::cout << "Depth first ordering"<<std::endl;
		while(( n=pqmd.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;
		}
		std::cout << "Best first ordering"<<std::endl;
		while(( n=pqmb.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;
		}
		std::cout << "Best (Depth) first ordering"<<std::endl;
		while(( n=pqmbd.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;
		}
      std::cout<< *(pqmd.getStat());
      std::cout<< *(pqmb.getStat());
      std::cout<< *(pqmbd.getStat());
	} else {
		MyMaxNode *n;int i;
		pqmaxd pqmxd;
		pqmaxb pqmxb;
		pqmaxbd pqmxbd;
		for (i=0;i<nb;i++ ) {
			eval= (rand()%(es-ei))+ei;
			dist= (rand()%(ds-di))+di;
			n = new MyMaxNode(eval);
			std::cout<<"Insert Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<std::endl;;
			n->dist() = dist;
			pqmxd.Ins(n);
			pqmxb.Ins(n);
			pqmxbd.Ins(n);
		}
		std::cout << "Depth first ordering"<<std::endl;
		while(( n=pqmxd.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;;
		}
		std::cout << "Best first ordering"<<std::endl;
		while(( n=pqmxb.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;;
		}
		std::cout << "Best (Depth) first ordering"<<std::endl;
		while(( n=pqmxbd.Del())!=0 ) {
			std::cout<<"Delete Eval:"<<n->getEval()<<" Dist:"<<n->dist()<<" r:"<<n->ref()<<std::endl;;
		}
      std::cout<< *(pqmxd.getStat());
      std::cout<< *(pqmxb.getStat());
      std::cout<< *(pqmxbd.getStat());
	}

	Bob::core::End();
}
