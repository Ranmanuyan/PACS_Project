/** Communication types between PPE and SPE */

/*typedef unsigned int uint32_t;
typedef unsigned long long uint64_t; */

#include <cstdlib>

struct cbdata {
  union {
    uint32_t i[4];
    uint64_t l[2];
  };
};

/* callback function identifiers */
enum cb_id {
  CB_NEWNODE,
  CB_GPQGET,
  CB_SEARCH,
  CB_SETEVAL,
  CB_GETEVAL,
  __CB_MAX
};



