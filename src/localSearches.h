#ifndef LOCAL_SEARCHES_INC
#define LOCAL_SEARCHES_INC
#include "common.h"

void run_simple_ea1p1(string folder_path, shared_ptr<IOHprofiler_suite<int> > suite, int eval_budget, int independent_runs, unsigned rand_seed);

#endif
