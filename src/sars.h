/**
 * Here we implement the Simulated Annealing algorithm with
 * exponential temperature schedule with iterative restarts.
 *
 * The problem with simulated annealing is that it has several
 * parameters that need to be configured well in order for it
 * to work.
 * The proper configuration of the parameters depends on the
 * scale of the objective function value as well as on the
 * available computation budget, and even stuff like, e.g.,
 * the expected differences in objective values of local optima.
 *
 * This makes it very complicated to properly use it for
 * black-box settings where absolutely nothing is known
 * about the objective function.
 *
 * The dependency on the budget is reduced here by performing
 * internal restarts. The first restart has a budget of 1024
 * FEs, the second one of 2048 FEs, and so on: the "inner"
 * runs always double in budget. Thus, the advantages of SA
 * can be obtained faster by wasting about half of the
 * computational budget.
 *
 * Still, to be able to do some experiments, we need to also
 * configure the temperatures. We propose the following
 * automatic setup based on the number 'n' of variables of
 * the problem and the budgets of the single internal runs:
 *
 * - the temperature schedule is exponential
 * - the start temperature is such that it time 1 the
 *   probability to accept a solution which is n/4 worse
 *   than the current solution would be 0.1
 * - the end temperature is such that the probability to
 *   accept a solution which is 1 worse than the current
 *   solution is 1/sqrt(n)
 * - the value of the epsilon parameter of the schedule
 *   is computed accordingly
 *
 * This will have several implications:
 *
 * - Problems that can be solved easily (OneMax,
 *   LeadingOnes) will be _still_ solved late in the runs.
 *   This is normal for simulated annealing, as the goal
 *   is to transcend from random walk to hill climbing
 *   behavior.
 * - Problems whose objective value range differs largely
 *   from n (e.g., LARS) will probably not be solved well.
 *
 * Author: Thomas Weise
 *         Institute of Applied Optimization
 *         Hefei University
 *         Hefei, Anhui, China
 * Email: tweise@hfuu.edu.cn, tweise@ustc.edu.cn
 */
#ifndef _SARS_H_
#define _SARS_H_
#include "common.h"

void run_simulated_annealing_exp_rs(const string folder_path,
    shared_ptr<IOHprofiler_suite<int>> suite,
    const unsigned long long eval_budget,
    const unsigned long long independent_runs,
    const unsigned long long rand_seed);

#endif
