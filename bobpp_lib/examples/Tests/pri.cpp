
#include<bobpp/bobpp>


class MyMinNode : public Bob::BBIntMinNode {
        public:
                MyMinNode(int n) : Bob::BBIntMinNode(n) {}

};
class MyMaxNode : public Bob::BBIntMaxNode {
        public:
                MyMaxNode(int n) : Bob::BBIntMaxNode(n) {}
};

typedef Bob::BestDPri<MyMinNode> bestdprimin;
typedef Bob::BestEPri<MyMinNode> besteprimin;
typedef Bob::DepthPri<MyMinNode> depthprimin;

typedef Bob::BestDPri<MyMaxNode> bestdprimax;
typedef Bob::BestEPri<MyMaxNode> besteprimax;
typedef Bob::DepthPri<MyMaxNode> depthprimax;

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--node"),Bob::Property("-min"," minimum or maximum (default maximum)"));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-n1","evaluation of n1 ",4));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-n2","evaluation of n2 ",3));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-d1","distance of n1 ",4));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-d2","distance of n2 ",3));
	Bob::core::Config(n,v);
	
	std::cout << "Node Sens of optomization "<<Bob::core::opt().NVal("--node","-min")<<std::endl;

	long e1,e2,d1,d2;
	e1 = Bob::core::opt().NVal("--node","-n1");
	e2 = Bob::core::opt().NVal("--node","-n2");
	d1 = Bob::core::opt().NVal("--node","-d1");
	d2 = Bob::core::opt().NVal("--node","-d2");
	if ( Bob::core::opt().NVal("--node","-min") ) {
		MyMinNode ni1(e1),ni2(e2);
		ni1.dist() = d1;
		ni2.dist() = d2;
		std::cout << ni1 <<std::endl;
		std::cout << ni2 <<std::endl;
		if ( bestdprimin::isBetter(ni1,ni2) ) std::cout << "Best Depth N1 better than N2"<<std::endl;
		if ( besteprimin::isBetter(ni1,ni2) ) std::cout << "Best Evaluation N1 better than N2"<<std::endl;
		if ( depthprimin::isBetter(ni1,ni2) ) std::cout << "Depth N1 better than N2"<<std::endl;
	} else {
		MyMaxNode na1(e1),na2(e2);
		na1.dist() = d1;
		na2.dist() = d2;
		std::cout << na1 <<std::endl;
		std::cout << na2 <<std::endl;
		if ( bestdprimax::isBetter(na1,na2) ) std::cout << "Best Depth N1 better than N2"<<std::endl;
		if ( besteprimax::isBetter(na1,na2) ) std::cout << "Best Evaluation N1 better than N2"<<std::endl;
		if ( depthprimax::isBetter(na1,na2) ) std::cout << "Depth N1 better than N2"<<std::endl;
	}

	Bob::core::End();
}
