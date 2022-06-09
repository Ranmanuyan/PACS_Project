#ifndef BOB_MPXSTRATENVPROG
#define BOB_MPXSTRATENVPROG

namespace Bob {

namespace mpx {
/**
  * @defgroup MPStratEnvProgGRP The Strategy module
  * @brief The different classes to define the parallelization strategy
  * @ingroup MPEnvProgGRP
  * @{
  * The aim of this module is to define all the class usefull to define the parallelization strategy. 
  * We would like to define different topology of communication, hierachical, collegial. 
  * And also different load balancing strategies.
  * At this time we developp only the collegial topology, with a bi-threshold load balancing strategy
  */

// Tags
#define TAG_TOKEN 0 /* token message */
#define TAG_LOAD 1 /* load message */
#define TAG_ASKWORK 2 /* ask_work message */
#define TAG_WORK 3 /* work message */
#define TAG_NOWORK 4 /* no_work message */
#define TAG_GOAL 5 /* goal message */
#define TAG_END 6 /* end message */
#define TAG_OTHER 100 /* no meaning message */

// Headers
#define HEADER_NODE 1 /* indicates a node */
#define HEADER_FINISH 0 /* finish a message */

// token
#define TOKEN_NOBODY -1 /* nobody wants to end the algorithm */
#define TOKEN_MODE_LOAD 0 /* token shares load */
#define TOKEN_MODE_END 1 /* token tries to stop the algorithm */

// wait management
#define MIN_SLEEP 10 /* the minimum time to sleep */
#define MAX_SLEEP 50000 /* the maximal time to sleep. */
#define MULTIPLY_SLEEP 2 /* the multiplicator for the time */
#define REPEAT_SLEEP 4 /* the maximum number of time the sleeping time is not changed */

// load management
#define LOAD_MIN 0.2 /* percentage under the average load a processor is considered as underloaded */
#define LOAD_MAX 0.2 /* percentage over the average load a processor is considered as overloaded */

// state
#define RUNNING 0 /* process state is running */
#define IDLE 1 /* process state is idle */
#define STOPPING 2 /* process state is stopping */

/**
  * @defgroup MPtopoEnvProgGRP Handle the Communication topology 
  * @brief The different classes to define the topology of communication
  * @ingroup MPStratEnvProgGRP
  * @{
  */

/** @defgroup MPtopoTypeMessGRP The typed Messages 
  * @brief This group stores the Classes to manage the Bobpp Typed communications
  * @ingroup MPtopoEnvProgGRP
  * @{
  * Mainly MPI process needs to communicates Load, Nodes, and Solutions.
  * Then, we have create specific classes to handle the comunication of these specific bobpp datas.
  * A message is composed as follows :
  */

/** Token Struct
  * Token struct facilitates the termination of MPI environnement.
  */
struct Token {
public:
	int starter; /**< the starter of the token, usefull when end is required */
	int mode; /**< the mode of the token: LOAD or END */
	int nb_nodes_in; /**< number of nodes that have been received by message */
	int nb_nodes_out; /**< number of nodes that have been sent by message */
	pvector<long> load; /**< Load for each processor */

	/// Constructors
	Token(int size) : starter(TOKEN_NOBODY), mode(TOKEN_MODE_LOAD), nb_nodes_in(0), nb_nodes_out(0), load(size,0) {}
	/// Destructor
	virtual ~Token() {}

	/// test if the token has finished the turn of the ring
	virtual bool ring_completed() { return (starter == EnvProg::rank()); }
	/// modify the mode
	virtual void set_mode(int _mode) { mode = _mode; if (mode == TOKEN_MODE_END) reset_stats(); }
	/// get the mode
	virtual int get_mode() { return mode; }

	/// set the number of messages
	virtual void update_stats(int _in, int _out) { nb_nodes_in += _in; nb_nodes_out += _out; }
	/// reset the number of messages
	virtual void reset_stats() { nb_nodes_in = 0; nb_nodes_out = 0; starter = EnvProg::rank(); }
	/// test if the number of incoming messages is equal to the number of outgoing messages 
	virtual bool verify_stats() { return (nb_nodes_in == nb_nodes_out); }

	/// neighbour: token is always used on a ring
	virtual int neighbour() { return ((EnvProg::rank() + 1) % EnvProg::nproc()); }
};

/** TypeOut Structure
  * TypeOut structure facilitates the send MPI operation.
  */
template<class Trait,class Load>
struct TypedOut {
	/// Type of the node
	typedef typename Trait::Node TheNode;
	/// Type of the stat
	typedef typename Trait::Stat TheStat;
	/// Type of the Goal
	typedef MPGoal<Trait> TheMPGoal;
	/// The out object allows the unpack of the Bobpp types
	MPOut out;
	/// Constructor
	TypedOut() : out() {}
	/// Destructor
	virtual ~TypedOut() {}
	/** Reset method
	* flush the out variable, and assign 0 to the type
	*/
	virtual void reset() { out.reset(); }
	/// pack the load
	virtual void pack_load(Load *l) { Pack(out,l); }
	/// pack a given node
	virtual void pack_node(TheNode *n) {
		int header = HEADER_NODE;
		out.Pack(&header,1);
		n->Pack(out);
	}
	/// pack the token 
	virtual void pack_token(Token token) {
		out.Pack(&token.starter,1);
		out.Pack(&token.mode,1);
		out.Pack(&token.nb_nodes_in,1);
		out.Pack(&token.nb_nodes_out,1);
		token.load.Pack(out);
	}
	/// pack the incubent
	virtual void pack_incubent(TheMPGoal *g) { g->Pack(out); }
	/// pack the end message
	virtual void pack_finish() { int header = HEADER_FINISH; out.Pack(&header,1); }
	/// send the message with a string to debug
	virtual void send(int d, const char *lib, int tag=TAG_OTHER) { out.send(d, lib, tag); }
	/// send the message
	virtual void send(int d, int tag=TAG_OTHER) { out.send(d, tag); }
};

/** TypeIn Structure
  * TypeIn structure facilitates the send MPI operation.
  */
template<class Trait,class Load>
struct TypedIn {
	/// Type of the node
	typedef typename Trait::Node TheNode;
	/// Type of the stat
	typedef typename Trait::Stat TheStat;
	/// Type of the Goal
	typedef MPGoal<Trait> TheMPGoal;
	/// The in object allows the pack of the Bobpp types
	MPIn in;
	/// Constructor
	TypedIn() : in() {}
	/// Destructor
	virtual ~TypedIn() {}
	/// unpack the load
	virtual void unpack_load(Load *l) { UnPack(in,l); }
	/// unpack a node
	virtual void unpack_node(TheNode *n) { n->UnPack(in); }
	/// unpack the header of a message containing nodes
	virtual int unpack_header() {
		int h;
		in.UnPack(&h,1);
		return h;
	}
	/// unpack the token 
	virtual void unpack_token(Token &token) {
		in.UnPack(&token.starter,1);
		in.UnPack(&token.mode,1);
		in.UnPack(&token.nb_nodes_in,1);
		in.UnPack(&token.nb_nodes_out,1);
		token.load.UnPack(in);
	}
	/// unpack the incubent
	virtual void unpack_incubent(TheMPGoal *g) { g->UnPack(in); }
	/// test if a message is arrived
	int probe() { return in.probe(); }
	/// receive a message
	virtual void recv(int d, int tag=TAG_OTHER) { in.recv(d, tag); }
};

/** @defgroup MPtopologyGRP The topology classes
  * @brief This group stores the Classes which represent the topology of communication
  * @ingroup MPtopoEnvProgGRP
  * @{
  * Mainly MPI process needs to communicate Load, Nodes, and Solutions.
  * to perform a load balancing, a processor could ask node to all other MPI processors,
  * the processors could be organized with a specific topology.
  * The unique and best topology, which outperforms all the other for all branch and bound
  * behaviour does not exist.
  * This module proposes different topologies to use with load blancing strategies.
  * At this time we have all2all, ring. We plan to implement a hierarchical topology 
  * like trees.
  */

/** Abstract topology class is the base to define the different topologies we want.
  */
class topology {
public:
	/// the Token
	Token token;
	/// Constructor
	topology() : token(EnvProg::nproc()) { }
	/// Destructor
	virtual ~topology() { }
	/// give the id of the process from its neighbour index
	virtual int rank2neighbour(int i)=0;
	/// give the id of the neighbour from its process id
	virtual int neighbour2rank(int i)=0;
	/// return the number of neighbours for this topology
	virtual int nb_neighbour()=0;
	/// return if the topology is collegial of if there exists a master
	virtual int is_collegial()=0;
	/// return the id of the master.
	virtual int master()=0;
	/// return true if the process is the master
	virtual int is_master() { return master() == EnvProg::rank(); }
	/// return the main rank
	virtual int main_rank()=0;
	/// return true if process is the main process
	virtual int is_main() { return main_rank() == EnvProg::rank(); }
	/// return the rank process
	virtual int my_rank() { return EnvProg::rank(); }
};

/** define a ring topology
  */
class topo_ring : public topology {
public:
	/// Constructor
	topo_ring() :topology() { }
	/// Destructor
	virtual ~topo_ring() { }
	/// in a ring topology all processes have the same level of decision
	virtual int is_collegial() { return true; }
	/// give the neighbour from the rank
	virtual int rank2neighbour(int i) { return ((EnvProg::rank()+EnvProg::nproc()-1+2*i)%EnvProg::nproc()); }
	/// give the rank from the neighbour index
	virtual int neighbour2rank(int i) { return ((EnvProg::rank()-1+2*i)); }
	/// return always 2
	virtual int nb_neighbour() { return 2; }
	/// return -1 there is no master
	virtual int master() { return -1; }
	/// return the main rank
	virtual int main_rank() { return 0; }
};

/** define a all2all topology
  */
class topo_a2a : public topology {
public:
	/// Constructor
	topo_a2a(): topology() { }
	/// Destructor
	virtual ~topo_a2a() { }
	/// return always true
	virtual int is_collegial() { return true; }
	/// simple translation
	virtual int rank2neighbour(int i) { return (i<EnvProg::rank()?i:i-1); }
	/// simple translation
	virtual int neighbour2rank(int i) { return (i<EnvProg::rank()?i:i+1); }
	/// return the number of process minus 1
	virtual int nb_neighbour() { return EnvProg::nproc()-1; }
	/// return -1 there is no master
	virtual int master() { return -1; }
	/// return the main rank
	virtual int main_rank() { return 0; }
};

/**
  * @defgroup MPLoadEnvProgGRP The load balancing strategy
  * @brief The different classes to define the load balancing strategy
  * @ingroup MPStratEnvProgGRP
  * @{
  */

/** Abstract load balancing strategy class
  * This class is the base class for all, Load balancing strategies.
  */
template<class Trait,class Topo, class PQ, class Goal,class Load=long>
class lb_strat {
protected:
	typedef typename Trait::Instance TheInstance; /**< the type of the instance */
	typedef typename Trait::Node TheNode; /**< the type of node */
	typedef typename Trait::Algo TheAlgo; /**< the type of algo */
	typedef typename Trait::Goal TheGoal; /**< the type of Goal */
	typedef typename Trait::Stat TheStat; /**< the type of statistics */
	typedef typename Trait::PriComp ThePriComp; /**< the type of priority */
	Topo topo; /**< the topology */
	std::vector<TypedIn<Trait,Load> > in; /**< the vector of message to receive */
	std::vector<TypedOut<Trait,Load> > out; /**< the vector of message to send */
	std::vector<Load> load; /**< vector of load of the other process */
	std::vector<bool> asking; /**< vector to indicate if nodes are asked to this neighbour */
	PQ *pq; /**< the priority queue used by the algorithms */
	Goal *goal; /**< the Goal used by the algorithms */

public:
	/// Constructor
	lb_strat() : topo(), in(topo.nb_neighbour()), out(topo.nb_neighbour()), load(topo.nb_neighbour()+1), asking(topo.nb_neighbour(),false), pq(0), goal(0) { }
	/** Constructor
	* @param _pq the priority queue used
	* @param _goal the Goal used 
	*/
	lb_strat(PQ *_pq, Goal *_goal) : topo(), in(topo.nb_neighbour()), out(topo.nb_neighbour()), load(topo.nb_neighbour()+1), asking(topo.nb_neighbour(),false), pq(_pq), goal(_goal) { }
	/// Destructor
	virtual ~lb_strat() { }
	/// operator launched by the main thread to perform the load balancing strategy
	void operator()() { // right now, all strats are collegial
		if ( topo.is_collegial() ) { run_collegial(); }
		else if ( topo.is_master() ) { run_master(); }
		else { run_slave(); }
	}
	/// Algorithm to run if the topology is collegial
	virtual void run_collegial()=0;
	/// Algorithm to run by the master if it exists
	virtual void run_master()=0;
	/// Algorithm to run by the slaves if it exists
	virtual void run_slave()=0;

	/** TOKEN **/
	/// Send the token
	virtual void send_token() {
		int rank = topo.token.neighbour();
		int neighbour = topo.rank2neighbour(rank);
		DBG_COMM("sends TOKEN to " << rank);
		out[neighbour].reset();
		out[neighbour].pack_token(topo.token);
		out[neighbour].send(rank, "token", TAG_TOKEN);
	}
	/// Receive the token
	virtual void receive_token(int rank) {
		int neighbour = topo.rank2neighbour(rank);
		DBG_COMM("receives TOKEN from " << rank);
		in[neighbour].recv(rank, TAG_TOKEN);
		in[neighbour].unpack_token(topo.token);
	}

	/** LOAD **/
	/// Update the load array
	virtual bool update_load () {
		long end_detection = 0;
		for (int i=0 ; i < (int) topo.token.load.size() ; i++ ) {
			if (i != EnvProg::rank()) {
				load[topo.rank2neighbour(i)] = topo.token.load[i];
				end_detection += topo.token.load[i];
			}
		}
		return (end_detection == 0);
	}
	/// Update my load in structures
	virtual void update_myload() {
		Load current_load = pq->Load();
		load[topo.nb_neighbour()] = current_load;
		topo.token.load[EnvProg::rank()] = current_load;
	}
	/// Pack the load
	virtual void pack_load(int n) { update_myload(); out[n].pack_load(&(load[topo.nb_neighbour()])); }
	/// Unpack the load
	virtual void unpack_load(int n) { in[n].unpack_load(&(load[n])); }
	/// Send the load
	virtual void send_load(int neighbour) {
		int rank = topo.neighbour2rank(neighbour);
		DBG_STRAT("sends LOAD to " << rank);
		out[neighbour].reset();
		pack_load(neighbour);
		out[neighbour].send(rank, "load", TAG_LOAD);
	}
	/// Receive the load
	virtual void receive_load(int rank) {
		DBG_STRAT("receives LOAD from " << rank);
		int neighbour = topo.rank2neighbour(rank);
		in[neighbour].recv(rank, TAG_LOAD);
		unpack_load(neighbour);
	}

	/** ASKWORK **/
	/// Ask for work
	virtual void send_askwork(int neighbour) {
		int rank = topo.neighbour2rank(neighbour);
		DBG_STRAT("sends ASKWORK to " << rank);
		out[neighbour].reset();
		pack_load(neighbour);
		out[neighbour].send(rank, "ask_work", TAG_ASKWORK);
		asking[neighbour] = true;
	}
	/// Receive a request for work
	virtual void receive_askwork(int rank) {
		DBG_STRAT("receives ASKWORK from " << rank);
		int neighbour = topo.rank2neighbour(rank);
		in[neighbour].recv(rank, TAG_ASKWORK);
		unpack_load(neighbour);
	}

	/** WORK **/
	/// Send work or not
	// TODO Careful: DelLB get nodes from a set of real PQ
	virtual int send_work(int neighbour, long lm) {
		int rank = topo.neighbour2rank(neighbour);
		TheNode *n;
		long nb = 0;

		out[neighbour].reset();

		n = 0;
		while( nb < lm ) {
			while ( n==0 && !pq->are_all_wait() ) {
				pq->set_need_node4LB(1);
				n = pq->DelLB();
			}
			if ( n!=0 ) {
				out[neighbour].pack_node(n); 
				pq->set_need_node4LB(0);
				n = 0;
				nb++;
			}
			else { break; }
		}

		if (nb > 0) {
			out[neighbour].pack_finish();
			pack_load(neighbour);
			out[neighbour].send(rank, "work",TAG_WORK);
			load[neighbour] += nb;
			DBG_STRAT("sends WORK to " << rank << " (" << nb << " nodes)");
		}
		else {
			pack_load(neighbour);
			out[neighbour].send(rank, "nowork", TAG_NOWORK);
			DBG_STRAT("sends NOWORK to " << rank);
		}
		return nb;
	}
	/// Receive work
	virtual int receive_work(int rank) {
		int neighbour = topo.rank2neighbour(rank);
		int header = HEADER_FINISH, nb = 0;

		in[neighbour].recv(rank, TAG_WORK);

		header = in[neighbour].unpack_header();
		while ( header != HEADER_FINISH ) {
			TheNode *n = new TheNode();
			in[neighbour].unpack_node(n);
			pq->Ins(n); // computation threads are automatically awaken by "Ins"
			nb++;
			header = in[neighbour].unpack_header();
		}
		unpack_load(neighbour);
		asking[neighbour] = false;
		DBG_STRAT("receives WORK from " << rank << " (" << nb << " nodes)");
		return nb;
	}
	/// Receive no work
	virtual void receive_nowork(int rank) {
		DBG_STRAT("receives NOWORK from " << rank);
		int neighbour = topo.rank2neighbour(rank);
		in[neighbour].recv(rank, TAG_NOWORK);
		unpack_load(neighbour);
		asking[neighbour] = false;
	}

	/** GOAL **/
	/// Send the goal
	virtual void send_incubent(int neighbour) {
		int rank = topo.neighbour2rank(neighbour);
		DBG_STRAT("sends GOAL to " << rank);
		out[neighbour].reset();
		out[neighbour].pack_incubent(goal);
		out[neighbour].send(rank, "incubent", TAG_GOAL);
	}
	/// Broadcast the goal
	virtual void broadcast_incubent() {
printf("Broadcast de la solution...\n");
		for (int i=0 ; i<topo.nb_neighbour() ; i++) send_incubent(i); }
	/// Receive the goal
	virtual void receive_incubent(int rank) {
		DBG_STRAT("receives GOAL from " << rank);
		int neighbour = topo.rank2neighbour(rank);
		in[neighbour].recv(rank, TAG_GOAL);
		Goal tempg;
		goal->current_merge(&tempg);
	}

	/** END **/
	/// Send the end
	virtual void send_kill(int neighbour) {
		int rank = topo.neighbour2rank(neighbour);
		DBG_STRAT("sends END to " << rank);
		out[neighbour].reset();
		out[neighbour].send(rank, "end", TAG_END);
	}
	/// Broadcast the end
	virtual void broadcast_kill() { for (int i=0 ; i<topo.nb_neighbour() ; i++) send_kill(i); }
	/// Receive the end
	virtual void receive_end(int rank) {
		DBG_STRAT("receives END from " << rank);
		int neighbour = topo.rank2neighbour(rank);
		in[neighbour].recv(rank, TAG_END);
	}
};

/** load balancing strategy using two thresholds
  */
template<class Trait,class Topo, class PQ, class Goal,class Load=long>
	class lb_strat_2t : public lb_strat<Trait,Topo,PQ,Goal,Load> {
	typedef lb_strat<Trait,Topo,PQ,Goal,Load> strat_base; /**< type of the base strategy */
	typedef typename Trait::Node TheNode; /**< type of a node */
	std::vector<Load> diff_load; /**< vector of load differences with the average load */
	Load min; /**< the min value of the node */
	Load max; /**< the max value of the load */
	Load average; /**< the average value of the load */

public:
	/// Constructor
	lb_strat_2t(PQ *_pq, Goal *_g) : lb_strat<Trait,Topo,PQ,Goal,Load>(_pq,_g), diff_load(strat_base::topo.nb_neighbour()+1), min(0), max(0), average(0) { }
	/// Destructor
	virtual ~lb_strat_2t() { }
	/// Compute the thresholds (locally)
	virtual void compute_thresholds() {
		Load tmp(0);
		strat_base::update_myload();
		for (int i=0;i<=strat_base::topo.nb_neighbour();i++) { tmp += strat_base::load[i]; }
		average = tmp / (strat_base::topo.nb_neighbour()+1);
		min = average * (1-LOAD_MIN);
		max = average * (1+LOAD_MAX);
	}
	/// Ask for work
	virtual void send_askwork() {
		for (int i=0 ; i <= strat_base::topo.nb_neighbour() ; i++) { diff_load[i] = strat_base::load[i] - average; }
		if (diff_load[strat_base::topo.nb_neighbour()] < 0) {
			for (int i=0 ; i < strat_base::topo.nb_neighbour() ; i++) {
				if (diff_load[i] > 0 && strat_base::load[i] > max && !strat_base::asking[i]) { strat_base::send_askwork(i); }
			}
		}
	}
	/// Send load
	virtual void send_load() {
		for (int i=0 ; i <= strat_base::topo.nb_neighbour() ; i++) { diff_load[i] = strat_base::load[i] - average; }
		if (diff_load[strat_base::topo.nb_neighbour()] > 0) {
			for (int i=0 ; i < strat_base::topo.nb_neighbour() ; i++) {
				if (diff_load[i] < 0  && strat_base::load[i] < min) { strat_base::send_load(i); }
			}
		}
	}
	/// Send work
	virtual int send_work(int rank) {
		int neighbour = strat_base::topo.rank2neighbour(rank);
		Load share(0), sum_over(0), sum_under(0), my_diff_load(0);

		for (int i=0 ; i <= strat_base::topo.nb_neighbour() ; i++) {
			diff_load[i] = strat_base::load[i] - average;
			if (diff_load[i] > 0) sum_over += diff_load[i];
			else sum_under += -diff_load[i];
		}

		share = (sum_over + sum_under) / 2;
		my_diff_load = diff_load[strat_base::topo.nb_neighbour()];
		// the test with 'asking' here avoids a bug when 2 MPI_Send with big data are launched in the same time => deadlock
		if (my_diff_load > 0 && diff_load[neighbour] < 0 && !strat_base::asking[neighbour]) {
			share = round(-diff_load[neighbour]*my_diff_load/share);
		} else { share = 0; }
		return strat_base::send_work(neighbour, (long) share);
	}
	virtual bool overloaded() { return strat_base::pq->Load() > max; }
	virtual bool underloaded() { return strat_base::pq->Load() < min || strat_base::pq->Load() == 0; }
	virtual bool goodloaded() { return !overloaded() && !underloaded(); }

	/// Run the load balancing with a collegial strategy
	virtual void run_collegial() {
		// behaviour
		int state (RUNNING);
		bool token (false), end_detection (false);
		// management of the wait
		int sleeptime (MIN_SLEEP), useless(0);
		bool action (false);
		// in & out stats
		int nb_nodes_in (0), nb_nodes_out (0);
		// management of the message
		int flag = 0;
		MPI_Status message_status;

// TODO : ne pas afficher la racine + "start to solve the entire tree", etc. sauf pour le process 0
// (sûrement des choses à faire dans thr.h, ligne 1092, on peut jouer avec la verbosity ?? (j'ai essayé sans succès)
// TODO : empêcher de lancer réellement la stratégie si nb process = 1 => repasser en mode sans MPI

		// main processor has the token and wait for the beginning of the search
		if (strat_base::topo.is_main()) {
			token = true;
			strat_base::pq->wait_for_start();
		}

		DBG_STRAT("begins the strat loop with load = " << strat_base::pq->Load());
		// main loop for messages management
		while (state != STOPPING) {
			// update my load + compute: average & min & max loads + prepare misc arrays
			compute_thresholds();
			// Must I send load?
			if (token) {
				strat_base::send_token();
				token = false;
				if (overloaded()) { send_load(); }
			}
			// Must I ask for work?
			if (underloaded()) { send_askwork(); }
			// Must I send the goal?
			if (strat_base::goal->is_updated()) {
				strat_base::broadcast_incubent();
				strat_base::goal->reset_state();
			}
			// Incoming message?
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &message_status);
			if (flag) {
				switch (message_status.MPI_TAG) {
					case TAG_TOKEN:
						strat_base::receive_token(message_status.MPI_SOURCE);
						switch (strat_base::topo.token.mode) {
							case TOKEN_MODE_LOAD:
								DBG_COMM("TOKEN LOAD received");
								end_detection = strat_base::update_load();
								if (end_detection && strat_base::pq->are_all_wait() && strat_base::pq->Load() <= 0) {
									DBG_STRAT("begins TOKEN END");
									strat_base::update_myload();
									strat_base::topo.token.set_mode(TOKEN_MODE_END);
									strat_base::send_token();
								}
								else { token = true; }
								break;
							case TOKEN_MODE_END:
								DBG_STRAT("TOKEN END received");
								strat_base::topo.token.update_stats(nb_nodes_in, nb_nodes_out);
								if (strat_base::pq->are_all_wait() && strat_base::pq->Load() <= 0) {
									if (strat_base::topo.token.ring_completed()) {
										if (strat_base::topo.token.verify_stats())  {
											state = STOPPING;
											strat_base::broadcast_kill();
										}
										else {
											DBG_STRAT("begins TOKEN END again, stats were not good");
											strat_base::topo.token.reset_stats();
											strat_base::send_token();
										}
									}
									else strat_base::send_token();
								}
								else {
									DBG_STRAT("TOKEN END canceled, load OK");
									strat_base::topo.token.set_mode(TOKEN_MODE_LOAD);
									token = true;
									action = true;
								}
								break;
							default:
								std::cout << EnvProg::rank() << " Token Mode: " << strat_base::topo.token.mode << " not known" << std::endl;
								exit(0);
						}
						break;
					case TAG_LOAD:
						strat_base::receive_load(message_status.MPI_SOURCE);
						action = true;
						break;
					case TAG_ASKWORK:
						strat_base::receive_askwork(message_status.MPI_SOURCE);
						compute_thresholds();
						nb_nodes_out += send_work(message_status.MPI_SOURCE);
						action = true;
						break;
					case TAG_NOWORK:
						strat_base::receive_nowork(message_status.MPI_SOURCE);
						action = true;
						break;
					case TAG_WORK:
						nb_nodes_in += strat_base::receive_work(message_status.MPI_SOURCE);
						action = true;
						break;
					case TAG_GOAL:
						strat_base::receive_incubent(message_status.MPI_SOURCE);
						action = true;
						break;
					case TAG_END:
						strat_base::receive_end(message_status.MPI_SOURCE);
						state = STOPPING;
						action = true;
						break;
					default:
						std::cout << EnvProg::rank() << ": Tag=" << message_status.MPI_TAG << " not known" << " from " << message_status.MPI_SOURCE << std::endl;
						exit(0);
				}
			}
			if (action) {
				action = false;
				useless = 0;
				sleeptime = MIN_SLEEP;
			}
			else {
				useless++;
				if (useless >= REPEAT_SLEEP && sleeptime < MAX_SLEEP) sleeptime *= MULTIPLY_SLEEP;
			}
			// adaptive wait
			usleep(sleeptime);
		}
		strat_base::pq->wake_up_for_end();
		DBG_STRAT("finishes the strat loop");
	}

	/// run the master algorithm (not implemented yet)
	virtual void run_master() {
		bool end=false;
		while(!end) {
			std::cout << EnvProg::rank() << ": master not yet implemented" << std::endl;
			end = true;
		}
	}
	/// run the slave algorithm (not implemented yet)
	virtual void run_slave() {
		bool end=false;
		while(!end) {
			std::cout << EnvProg::rank() << ": slave not yet implemented" << std::endl;
			end = true;
		}
	}
}; // class lb_strat_2t

}; // end of namespace mpx
}; // end of namespace Bob
#endif

