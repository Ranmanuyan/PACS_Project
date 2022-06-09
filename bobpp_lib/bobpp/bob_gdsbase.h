

#ifndef BOBGDSBASE
#define BOBGDSBASE



namespace Bob {



/** @defgroup GDS_GRP Bob++ Data structures
    @brief This package encapsulates all the definition for the Data structures used to store Nodes.
    @ingroup CoreGRP

The data structure are used in Bob++ to store a set of Nodes.
According to the semantic, several data structures are proposed.
At this time only Priority Queues are proposed. These are to store the pending Nodes,
This group has been created, since several semantic of data structures will exist exist in the future.

Another data structure named Goal is also used in Bobpp algorithms.
These data structure are defined in each algorithm group (Brancha dn Bound, divide and conquer), since the
semantic of a Goal depends on the algorithm.

The design of the data structures in Bob++ has to handle different types of problem
 -# Only the application knows which data structure it needs.
 -# Only the Architecture side of Bob++ knows which is the implementation to handle parallel aspect of
   the data structure but in a sequential environment this level must be bypassed.
   For example, a Data structure could be shared by several algorithms. More precisely, several algorithms
   could access to one real data structure (in multi-threading environement for example).

To hide the real implementation of the data structure for a given algorithm,
we use <b>dynamic inheritance paradigm</b>.
As C++ does not permit to handle dynamic inheritance automatically, the implementation use generally
to level of data structure which respect exactly the same interface.

Thus the modelization is the following : a Data structure is composed by two levels of
objects.
 -# the first level, the lower level  is the local Data structure implementation named LDS
    (like in sequential) for exemple to implement a priority queue several
     algorithms exist D-Heap, Skew-Heap, and so one.
 -# the second level is the data structure used to handle parallel algorithms named ADS

For a given sementic of data structure said DSX all of them inherit from the DSXInterface
which is a pure abstract class defining the possible operations.

 The environment  initializes the data structures for a given algorithm.
 according to it, it allocated one (shared) or several (distributed) data structures.

The design is quite complicated, isn't it ? But that was the simplest way to
handle what we want to do.

@brief global and real implementation of Data structures

*/

};
#endif
