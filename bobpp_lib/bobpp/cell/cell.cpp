
#include <pthread.h>
#include "cell.hpp"


namespace  Bob {

pthread_once_t base_Callback::cb_key_once = PTHREAD_ONCE_INIT;
pthread_key_t base_Callback::cb_key;


} /* namespace Bob */

