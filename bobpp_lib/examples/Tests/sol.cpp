
#include<bobpp/bobpp>

/** To update because LSol are no more used. Now Goal is used.*/

class MyMinNode : public Bob::BBIntMinNode {
        public:
                MyMinNode(int n) : Bob::BBIntMinNode(n) {}

};
class MyMaxNode : public Bob::BBIntMaxNode {
        public:
                MyMaxNode(int n) : Bob::BBIntMaxNode(n) {}

};

typedef Bob::LSol<MyMinNode> minsol;
typedef Bob::LSol<MyMaxNode> maxsol;

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--node"),Bob::Property("-min"," minimum or maximum (default maximum)"));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-i","inferior limit of cost",10L));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-s","superior limit of cost",20L));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-size","nb nodes ",3L));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-nb","nb nodes ",10L));
	
	Bob::core::Config(n,v);
	
	std::cout << "Node Sens of optomization "<<Bob::core::opt().BVal("--node","-min")<<std::endl;

	long ei,es,size,nb,i,eval;
	ei = Bob::core::opt().NVal("--node","-i");
	es = Bob::core::opt().NVal("--node","-s");
	size = Bob::core::opt().NVal("--node","-size");
	nb= Bob::core::opt().NVal("--node","-nb");
	if ( Bob::core::opt().BVal("--node","-min") ) {
		MyMinNode *ni;
		minsol Sol(size);
		ni = new MyMinNode((es+ei)/2);
		std::cout<<"Init with :"<<((es+ei)/2)<<std::endl;
		Sol.Init(ni);
		for (i=0;i<nb;i++ ) {
			eval= (rand()%(es-ei))+ei+3;
			std::cout<<"Try Eval for a solution :"<<eval <<" ";
			ni = new MyMinNode(eval);
			ni->SetSol();
			if ( Sol.Upd(ni) ) std::cout<<"Update "<<std::endl;
			else { std::cout<<"Not update"<<std::endl; delete ni; };
			eval= (rand()%(es-ei))+ei;
			std::cout<<"Try Eval for a node :"<<eval <<" ";
			ni = new MyMinNode(eval);
			if ( Sol.Better(ni) ) std::cout<<"Node could be explorated "<<std::endl;
			else { std::cout<<"Node discarded"<<std::endl; delete ni; };

		}
      std::cout << *(Sol.getStat());
	} else {
		MyMaxNode *nx;
		maxsol solx(size);
		nx = new MyMaxNode((es+ei)/2);
		std::cout<<"Init with :"<<((es+ei)/2)<<std::endl;
		solx.Init(nx);
		for (i=0;i<nb;i++ ) {
			eval= (rand()%(es-ei))+ei;
			std::cout<<"Try Eval for a solution:"<<eval <<" ";
			nx = new MyMaxNode(eval);
			nx->SetSol();
			if ( solx.Upd(nx) ) std::cout<<"Update "<<std::endl;
			else { std::cout<<"Not update"<<std::endl; delete nx; };
			std::cout<<"Try Eval for a node :"<<eval <<" ";
			nx = new MyMaxNode(eval);
			nx->SetSol();
			if ( solx.Upd(nx) ) std::cout<<"Node could be explorated"<<std::endl;
			else { std::cout<<"Node discarded"<<std::endl; delete nx; };

		}
      std::cout << *(solx.getStat());
	}
	Bob::core::End();
}
