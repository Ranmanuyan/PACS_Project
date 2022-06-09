#ifndef BOB_MESSMPXENVPROG
#define BOB_MESSMPXENVPROG


namespace Bob {
namespace mpx {

/**
@defgroup MPEnvProgMessGRP The message format 
@brief The Message classes for the Bobpp MPx programming environment
@ingroup MPEnvProgGRP
@{
*/

/** the Class MPPackBuf
 * this class implements a buffer for packed objects for MPI communication.
 */
class MPPackBuf {
public:
  /// the buffer that stores the data
  void * buff;
  /// the size of buffer that stores the data
  int buff_size;
  /// the position of the end of the buffer.
  int position;

  /** Constructor
    */
  MPPackBuf(): buff(0), buff_size(0), position(0) {}
  /** Constructor
    * @param size the size of the internal buffer
    */
  MPPackBuf(int size);
  /** Destructor
    */
  virtual ~MPPackBuf();
  /** method to perform the allocation of the internal buffer.
    */
  void alloc(int size);
  /** method to reset the position
    */
  void reset();
};



/** the Class MPOut
 * this class inherits from the Serialize class, this is a wrapper
 * to call the MP methods to pack datas but respecting the
 * Bob++ interface.
 * It also implements send operations
 */
class MPOut : public Serialize, public MPPackBuf {
public:
  /// the classical MPI communicator
  MPI_Comm comm;
  /// the MPI request to send the message.
  MPI_Request req;
  /// the MPI status to store the return of the last request.
  MPI_Status status;
  /// boolean to know if the stored message has been sent or not.
  bool sent;
  /** Constructor
    * @param _comm the MP communicator
    */
  MPOut(MPI_Comm _comm = MPI_COMM_WORLD): comm(_comm), sent(false) {}
  /** Constructor
    * @param size the size of the buffer.
    * @param _comm the MP communicator
    */
  MPOut(int size, MPI_Comm _comm = MPI_COMM_WORLD): MPPackBuf(size), comm(_comm) {}
  /** Destructor
    */
  virtual ~MPOut() {}
  /// Pack method for array of bool
  virtual int Pack(const bool *n, int nb = 1) {
    int i;
    char t[nb];
    for (i = 0; i < nb; i++)
      t[i] = n[i];
    return _Pack(t, nb, MPI_CHAR);
  }
  /// Pack method for array of int
  virtual int Pack(const int *n, int nb = 1) {
    return _Pack(n, nb, MPI_INT);
  }
  /// Pack method for array of unsigned int
  virtual int Pack(const unsigned int *n, int nb = 1) {
    return _Pack(n, nb, MPI_UNSIGNED);
  }
  /// Pack method for array of short
  virtual int Pack(const short *n, int nb = 1) {
    return _Pack(n, nb, MPI_SHORT);
  }
  /// Pack method for array of unsigned short
  virtual int Pack(const unsigned short *n, int nb = 1) {
    return _Pack(n, nb, MPI_UNSIGNED_SHORT);
  }
  /// Pack method for array of long long
  virtual int Pack(const long long *n, int nb = 1) {
    return _Pack(n, nb, MPI_LONG_LONG);
  }
  /// Pack method for array of unsigned long long
  virtual int Pack(const unsigned long long *n, int nb = 1) {
    return _Pack(n, nb, MPI_UNSIGNED_LONG_LONG);
  }
  /// Pack method for array of long
  virtual int Pack(const long *n, int nb = 1) {
    return _Pack(n, nb, MPI_LONG);
  }
  /// Pack method for array of unsigned long
  virtual int Pack(const unsigned long *n, int nb = 1) {
    return _Pack(n, nb, MPI_UNSIGNED_LONG);
  }
  /// Pack method for array of float
  virtual int Pack(const float *n, int nb = 1) {
    return _Pack(n, nb, MPI_FLOAT);
  }
  /// Pack method for array of double
  virtual int Pack(const double *n, int nb = 1) {
    return _Pack(n, nb, MPI_DOUBLE);
  }
  /// Pack method for array of char
  virtual int Pack(const char *s, int nb = 1) {
    return _Pack(s, nb, MPI_CHAR);
  }
  /// Blocking send (standard mode)
  void send(int dest, const char *lib, int tag=0) {
    int r;
    DBG_COMM("----------------------------------------send to " << dest << " for :"<<lib<< " size:"<<position);
    r = MPI_Send(buff, position, MPI_PACKED, dest, tag , comm);
    if ( r!=MPI_SUCCESS ) DBG_COMM("MPI_Send returns "<<r);
    sent = true;
  }
  /// Blocking send (standard mode)
  void send(int dest, int tag=0) {
    DBG_COMM("----------------------------------------send to " << dest );
    MPI_Send(buff, position, MPI_PACKED, dest, tag , comm);
    sent = true;
  }

  /// Non-blocking send (ready mode)
  void irsend(int dest) {
    DBG_COMM("init non-blocking send to " << dest);
    DBG_MEM("buffer size = " << position << " bytes");
    MPI_Irsend(buff, position, MPI_PACKED, dest, 0, comm, &req);
    sent = true;
  }
  /// Check if non-blocking send is complete
  bool test() {
    DBG_COMM("test send");
    int flag;
    MPI_Test(&req, &flag, &status);
    //DBG_ERR(status,"MPOut::test");
    return flag != 0;
  }
  /// Wait for non-blocking send to finish
  void wait() {
    DBG_COMM("wait send");
    MPI_Wait(&req, &status);
    //DBG_ERR(status,"MPOut::wait");
  }
  /// Free allocated send request
  void free() {
    DBG_COMM("free send request");
    MPI_Request_free(&req);
  }
private:
  int _Pack(const void * buf, int count, MPI_Datatype datatype) {
    int size;
    //std::cout<<"Pack ";
    //MPEnvProg::DisplayPackedBuff(buf,count,datatype);
    if (sent) {
      reset();
      sent = false;
    }
    MPI_Pack_size(count, datatype, comm, &size);
    alloc(size);
    MPI_Pack((void*)buf, count, datatype, buff, buff_size, &position, comm);
    return count;
  }
};


/** The Class MPIn
 * this class inherits from the DeSerialize class, this is a wrapper
 * to call the MP methods to unpack datas but respecting the
 * Bob++ interface.
 * It also implements receive operations
 */
class MPIn : public DeSerialize, public MPPackBuf {
public:
  /// the classical MPI communicator
  MPI_Comm comm;
  /// the MPI request to receive the message.
  MPI_Request req;
  /// the MPI status to store the return of the last request.
  MPI_Status status;
  /// a boolean
  bool _test;
  /** Constructor
    * @param _comm the MP communicator
    */
  MPIn(MPI_Comm _comm = MPI_COMM_WORLD): comm(_comm), _test(false) {}
  /** Constructor
    * @param size the size of the buffer.
    * @param _comm the MP communicator
    */
  MPIn(int size, MPI_Comm _comm = MPI_COMM_WORLD): MPPackBuf(size), comm(_comm),
      _test(false) {}
  /** Destructor
    */
  virtual ~MPIn() {}
  /// UnPack method for array of bool
  virtual void UnPack(bool *n, int nb = 1) {
    int i;
    char t[nb];
    _UnPack(t, nb, MPI_CHAR);
    for (i = 0;i < nb;i++)
      n[i] = t[i];
  }
  /// UnPack method for array of int
  virtual void UnPack(int *n, int nb = 1) {
    _UnPack(n, nb, MPI_INT);
  }
  /// UnPack method for array of unsigned int
  virtual void UnPack(unsigned int *n, int nb = 1) {
    _UnPack(n, nb, MPI_UNSIGNED);
  }
  /// UnPack method for array of short
  virtual void UnPack(short *n, int nb = 1) {
    _UnPack(n, nb, MPI_SHORT);
  }
  /// UnPack method for array of unsigned short
  virtual void UnPack(unsigned short *n, int nb = 1) {
    _UnPack(n, nb, MPI_UNSIGNED_SHORT);
  }
  /// UnPack method for array of long long
  virtual void UnPack(long long *n, int nb = 1) {
    _UnPack(n, nb, MPI_LONG_LONG);
  }
  /// UnPack method for array of unsigned long long
  virtual void UnPack(unsigned long long *n, int nb = 1) {
    _UnPack(n, nb, MPI_UNSIGNED_LONG_LONG);
  }
  /// UnPack method for array of long
  virtual void UnPack(long *n, int nb = 1) {
    _UnPack(n, nb, MPI_LONG);
  }
  /// UnPack method for array of unsigned long
  virtual void UnPack(unsigned long *n, int nb = 1) {
    _UnPack(n, nb, MPI_UNSIGNED_LONG);
  }
  /// UnPack method for array of float
  virtual void UnPack(float *n, int nb = 1) {
    _UnPack(n, nb, MPI_FLOAT);
  }
  /// UnPack method for array of double
  virtual void UnPack(double *n, int nb = 1) {
    _UnPack(n, nb, MPI_DOUBLE);
  }
  /// UnPack method for array of char
  virtual void UnPack(char *s, int nb = 1) {
    _UnPack(s, nb, MPI_CHAR);
  }
  /// Blocking receive
  void recv(int source, int tag=MPI_ANY_TAG, int count = 0) {
    DBG_COMM("----------------------------------------receive from " << source);
    MPI_Probe (source,tag,comm,&status);
    DBG_COMM(" probe from " << source);
    MPI_Get_count( &status, MPI_PACKED, &count );
    DBG_COMM(" get count:"<<count<<" from " << source);
    alloc(count);
    MPI_Recv(buff, count, MPI_PACKED, source, tag, comm, &status);
    DBG_ERR(status, "MPIn::recv");
    reset();
  }

  /// Non-blocking receive
  void irecv(int source, int count = 0) {
    DBG_COMM("init NB receive from " << source);
    if (count == 0)
      count = buff_size;
    MPI_Irecv(buff, count, MPI_PACKED, source, MPI_ANY_TAG, comm, &req);
    reset();
  }
  /// Check if non-blocking receive is complete
  bool test() {
    if (!_test) {
      _test = true;
      DBG_COMM("test for receive");
    }
    int flag;
    MPI_Test(&req, &flag, &status);
    //DBG_ERR(status,"MPIn::test");
    return flag != 0;
  }
  /// Wait for non-blocking receive to finish
  void wait() {
    _test = false;
    DBG_COMM("wait for receive");
    MPI_Wait(&req, &status);
    DBG_COMM("Out of wait for receive");
    //DBG_ERR(status,"MPIn::wait");
#ifdef DEBUG_MEMORY
    int count;
    MPI_Get_count(&status, MPI_PACKED, &count);
    DBG_MEM("size of received message = " << count << " bytes");
#endif
    reset();
  }
  /// Check for incoming message
  int probe(int source = MPI_ANY_SOURCE) {
    DBG_COMM("probe");
    MPI_Probe(source, MPI_ANY_TAG, comm, &status);
    DBG_ERR(status, "MPIn::probe");
    return status.MPI_SOURCE;
  }
  /// Get number of received elements
  int count() {
    int count;
    MPI_Get_count(&status, MPI_PACKED, &count);
    return count;
  }
  /// Cancel pending non-blocking receive
  void cancel() {
    DBG_COMM("cancel NB receive");
    MPI_Cancel(&req);
  }
private:
  int _UnPack(void * buf, int count, MPI_Datatype datatype) {
    int size;
    MPI_Pack_size(count, datatype, comm, &size);
    alloc(size);
    MPI_Unpack(buff, buff_size, &position, buf, count, datatype, comm);
    //std::cout<<"UnPack ";
    //MPEnvProg::DisplayPackedBuff(buf,count,datatype);
    return count;
  }
};


/** @}
 */

};
};

#endif
