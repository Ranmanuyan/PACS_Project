
#include<bobpp/bobpp>


class MyMinNode : public Bob::BBIntMinNode {
        public:
                MyMinNode(int n) : Bob::BBIntMinNode(n) {}

};
class MyMaxNode : public Bob::BBIntMaxNode {
        public:
                MyMaxNode(int n) : Bob::BBIntMaxNode(n) {}

};

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--node"),Bob::Property("-min"," minimum or maximum (default maximum)"));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-n1","evaluation of n1 ",4));
	Bob::core::opt().add(std::string("--node"),Bob::Property("-n2","evaluation of n2 ",3));
	Bob::core::Config(n,v);
	
	std::cout << "Node Sens of optomization "<<Bob::core::opt().BVal("--node","-min")<<std::endl;

	long e1,e2;
	e1 = Bob::core::opt().NVal("--node","-n1");
	e2 = Bob::core::opt().NVal("--node","-n2");
	if ( Bob::core::opt().BVal("--node","-min") ) {
		MyMinNode ni1(e1),ni2(e2);
		std::cout << ni1 <<std::endl;
		std::cout << ni2 <<std::endl;
		if ( ni1>ni2 ) std::cout << "N1 better than N2"<<std::endl;
		if ( ni1<=ni2 ) std::cout << "N1 worst or equal to N2"<<std::endl;
		if ( ni1==ni2 ) std::cout << "N1 equal to N2"<<std::endl;
	} else {
		MyMaxNode na1(e1),na2(e2);
		std::cout << na1 <<std::endl;
		std::cout << na2 <<std::endl;
		if ( na1>na2 ) std::cout << "N1 better than N2"<<std::endl;
		if ( na1<=na2 ) std::cout << "N1 worst or equal to N2"<<std::endl;
		if ( na1==na2 ) std::cout << "N1 equal to N2"<<std::endl;
	}

	Bob::core::End();
}
