
#include<bobpp/mpx/mpx.hpp>
#include<mpi.h>
#include<cstdlib>

namespace  Bob {
namespace  mpx {
  int EnvProg::_nproc;
  int EnvProg::_rank;

  void EnvProg::init(int &argc, char **&argv) {
    ThrEnvProg::init(1);
    //MPI_Init_thread(argc, argv, MPI_THREAD_FUNNELED);
    MPI_Comm_size(MPI_COMM_WORLD, &_nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
    std::cout << "Init of node #" << _rank << " of " << _nproc << std::endl;
    core::log().setEId(new MPId());
  }
  void EnvProg::start() {
    ThrEnvProg::start();
  }

  void EnvProg::end() {
    DBG_COMM("Call end ThrEnvProg\n");
    ThrEnvProg::end();
    DBG_COMM("Call MPI_Finalize\n");
    MPI_Finalize();
    DBG_COMM("End of process");
  }


#if 0
  void EnvProg::AllocBuff(int size) {
    //std::cout << "EnvProg::AllocBuff(" << size << ")" << std::endl;
    if (position + size <= buff_size)
      return;
    if (buff == 0)
      buff_size = 4096;
    while (position + size > buff_size)
      buff_size *= 2;
    buff = realloc(buff, buff_size);
    if (buff == 0) {
      perror("EnvProg::AllocBuff");
      exit(1);
    }
  }

  void EnvProg::ResetBuff() {
    position = 0;
  }

  void EnvProg::Prt() {
		std::cout << "EnvProg::buff_size = " << buff_size << std::endl;
		std::cout << "EnvProg::position = " << position << std::endl;
	}
#endif
#if 0
  void EnvProg::DisplayPackedBuff(const void * buf, int count,
      MPI_Datatype datatype) {
    int i;
    const char *s = "unknown";
    std::cout<<"node #"<<_rank<<" ";
    if (datatype==MPI_CHAR) s = "MPI_CHAR";
    else if (datatype==MPI_INT) s = "MPI_INT";
    else if (datatype==MPI_LONG) s = "MPI_LONG";
    else if (datatype==MPI_FLOAT) s = "MPI_FLOAT";
    else if (datatype==MPI_DOUBLE) s = "MPI_DOUBLE";
    std::cout<<s<<"*"<<count<<" =";
    for(i=0; i<count; i++) {
      std::cout<<" ";
      if (datatype==MPI_CHAR) std::cout<<((int)((char*)buf)[i]);
      else if (datatype==MPI_INT) std::cout<<((int*)buf)[i];
      else if (datatype==MPI_LONG) std::cout<<((long*)buf)[i];
      else if (datatype==MPI_FLOAT) std::cout<<((float*)buf)[i];
      else if (datatype==MPI_DOUBLE) std::cout<<((double*)buf)[i];
    }
    if (datatype==MPI_CHAR) {
      std::string s;
      s.resize(count);
      for(i=0;i<count;i++)
        s[i] = ((char*)buf)[i];
      std::cout << " -> \"" << s << "\"";
    }
    std::cout<<std::endl;
  }
#endif

/*const char *MPId::id() {
   return rk.str();
}*/

void MPPackBuf::alloc(int size) {
  //std::cout << "MPPackBuf::alloc(" << size << ")" << std::endl;
  if (position + size <= buff_size)
    return;
  if (buff == 0)
    buff_size = 4096;
  while (position + size > buff_size)
    buff_size *= 2;
  buff = realloc(buff, buff_size);
  if (buff == 0) {
    perror("MPPackBuf::alloc");
    exit(1);
  }
}

void MPPackBuf::reset() {
  position = 0;
}

MPPackBuf::MPPackBuf(int size): buff_size(size), position(0) {
  buff = malloc(size);
  if (buff == 0) {
    perror("MPPackBuf::alloc");
    exit(1);
  }
}

MPPackBuf::~MPPackBuf() {
  if(buff)
    free(buff);
}

};
};
