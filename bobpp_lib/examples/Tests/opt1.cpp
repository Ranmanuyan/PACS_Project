
#include<bobpp/bobpp>

int main(int n, char ** v) {
	Bob::core::opt().add(std::string("--Truc"),Bob::Property("-l","test l"));
	Bob::core::opt().add(std::string("--Truc"),Bob::Property("-longoption","test longoption"));
	Bob::core::opt().add(std::string("--Truc"),Bob::Property("-file","test string ","thefile"));
	Bob::core::opt().add(std::string("--machin"),Bob::Property("-n","test numerical option ",4));
	Bob::core::opt().add(std::string("--machin"),Bob::Property("-f","test floating option ",4.0));
	Bob::core::Config(n,v);
	
   
	std::cout << "--Truc -l:"<<Bob::core::opt().BVal("--Truc","-l")<<std::endl;
	std::cout << "--Truc -longoption:"<<Bob::core::opt().BVal("--Truc","-longoption")<<std::endl;
	std::cout << "--machin -n:"<<Bob::core::opt().NVal("--machin","-n")<<std::endl;
	std::cout << "--machin -f:"<<Bob::core::opt().DVal("--machin","-f")<<std::endl;


	Bob::core::End();
}
