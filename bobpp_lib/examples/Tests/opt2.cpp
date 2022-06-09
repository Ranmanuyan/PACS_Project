
#include<bobpp/bobpp>

class MyOpt {
   public:
   static bool t;
   static bool longopt;
   static int mac_n;
   static double mac_f;
};

bool MyOpt::t;
bool MyOpt::longopt;
int MyOpt::mac_n;
double MyOpt::mac_f;

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--Truc"),Bob::Property("-l","test l",&MyOpt::t));
	Bob::core::opt().add(std::string("--Truc"),Bob::Property("-longoption","test longoption",&MyOpt::longopt));
	Bob::core::opt().add(std::string("--machin"),Bob::Property("-n","test numerical option ",4,&MyOpt::mac_n));
	Bob::core::opt().add(std::string("--machin"),Bob::Property("-f","test floating option ",4.0,&MyOpt::mac_f));
	Bob::core::Config(n,v);
	
	std::cout << "--Truc -l:"<<MyOpt::t<<" or " <<Bob::core::opt().BVal("--Truc","-l")<<std::endl;

	std::cout << "--Truc -longoption:"<<MyOpt::longopt<<std::endl;
	std::cout << "--machin -n:"<<MyOpt::mac_n<<std::endl;
	std::cout << "--machin -f:"<<MyOpt::mac_f<<std::endl;


	Bob::core::End();
}
