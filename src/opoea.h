/**
 * Here we implement the (1+1)-EA>0, is a slight extension of
 * the off-the-shelf evolutionary algorithm with one parent
 * (mu=1) that generates one single offspring solution in each
 * step (lambda=1) by flipping each bit of the parent solution
 * with the same probability of 1/n.
 * The difference of the (1+1)-EA<0 and the (1+1)-EA is that
 * in the (1+1)-EA<0 [implemented here], it is ensured that
 * always at least one bit is flipped.
 *
 * Author: Thomas Weise
 *         Institute of Applied Optimization
 *         Hefei University
 *         Hefei, Anhui, China
 * Email: tweise@hfuu.edu.cn, tweise@ustc.edu.cn
 */

#ifndef _OPOEA_H_
#define _OPOEA_H_
#include "common.h"

void run_ea1p1(const string folder_path,
    shared_ptr<IOHprofiler_suite<int> > suite, const int eval_budget,
    const int independent_runs, const unsigned rand_seed);

#endif
