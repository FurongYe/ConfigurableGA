/**
 * Here we implement the (1+1)-FEA, i.e., the (1+1)-EA>0 with frequency
 * fitness assignment
 *
 * The (1+1)-FEA is based on the (1+1)-EA:
 * The (1+1)-EA>0, in turn, is a slight extension of
 * the off-the-shelf evolutionary algorithm with one parent
 * (mu=1) that generates one single offspring solution in each
 * step (lambda=1) by flipping each bit of the parent solution
 * with the same probability of 1/n.
 * The difference of the (1+1)-EA<0 and the (1+1)-EA is that
 * in the (1+1)-EA<0, it is ensured that  always at least one
 * bit is flipped.
 * In the (1+1)-FEA, we use the encounter frequency of objective
 * values as fitness. This fitness is subject to minimization.
 *
 * The (1+1)-FEA is introduced in:
 * 1. Thomas Weise, Zhize Wu, Xinlu Li, and Yan Chen. Frequency Fitness
 *    Assignment: Making Optimization Algorithms Invariant under Bijective
 *    Transformations of the Objective Function Value. IEEE Transactions
 *    on Evolutionary Computation. doi:10.1109/TEVC.2020.3032090
 * 2. The preprint of the above paper at arXiv:2001.01416v5 [cs.NE] 15 Oct
 *    2020. http://arxiv.org/abs/2001.01416
 *
 * Author: Thomas Weise
 *         Institute of Applied Optimization
 *         Hefei University
 *         Hefei, Anhui, China
 * Email: tweise@hfuu.edu.cn, tweise@ustc.edu.cn
 */

#ifndef _OPOFEA_H_
#define _OPOFEA_H_
#include "common.h"

void run_fea1p1(const string folder_path,
    shared_ptr<IOHprofiler_suite<int>> suite,
    const unsigned long long eval_budget,
    const unsigned long long independent_runs,
    const unsigned long long rand_seed);

#endif
