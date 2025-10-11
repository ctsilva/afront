#include <gtb/gtb.hpp>
#include "kdtree.h"

GTB_BEGIN_NAMESPACE

kdtree_exception_no_more_values _exception_no_more_values;

CHighResTimer _timer_push;
CHighResTimer _timer_pop;
CHighResTimer _timer_insert;
CHighResTimer _timer_makeheap;
CHighResTimer _timer_ordered_init_pq;
CHighResTimer _timer_ordered_init_pq2;
CHighResTimer _timer_ordered_getnext;

GTB_END_NAMESPACE
