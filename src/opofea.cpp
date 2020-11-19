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

#include <tr1/unordered_map>
#include "opofea.h"

void fea1p1(shared_ptr<IOHprofiler_problem<int> > problem,
    shared_ptr<IOHprofiler_csv_logger<int> > logger, const int eval_budget) {
// n be the number of variables
  const int n = problem->IOHprofiler_get_number_of_variables();
  if(n <= 0) throw "number of variables must be positive";
// the bit flip probability
  const double p = 1.0 / ((double)n);

// xcur is the current best candidate solution (based on frequency fitness)
  std::vector<int> xcur;
// xnew is the new candidate solution generated in each step
  std::vector<int> xnew;
// ycur is the objective value of the current solution
  double ycur = std::numeric_limits<double>::infinity();
// ynew is the objective value of the new solution
  double ynew = std::numeric_limits<double>::infinity();

// H is the frequency table, storing the encounter frequency of objective values
  std::tr1::unordered_map<double, unsigned long long> H;

// first we generate the random initial solution
  xcur.reserve(n);
  for (int i = n; (--i) >= 0;) {
    xcur.push_back((int) (2 * uniform_random()));
  }

// we evaluate the random initial solution
  ycur = problem->evaluate(xcur);
  H[ycur] = 0; //and initialize its frequency
  logger->do_log(problem->loggerInfo());

// we perform iterations until either the optimum is discovered or the budget has been exhausted
  int count = eval_budget;
  while (((--count) > 0) && (!problem->IOHprofiler_hit_optimal())) {

// copy the current solution to the new solution
    xnew = xcur;
    bool unchanged = true;
// until the solution changes, repeat
    do {
// flip each bit with the independent probability of 1/n
      for (int i = n; (--i) >= 0;) {
        if (uniform_random() < p) {
          unchanged = false; // there was a change
          xnew[i] ^= 1; // flip the bit
        }
      }
    } while (unchanged); // repeat until at least one change

// evaluate the new candidate solution
    ynew = problem->evaluate(xnew);
    logger->do_log(problem->loggerInfo());

// is it completely new?
    if (H.find(ynew) == H.end()) {
      H[ynew] = 0; // store frequency 0
    }
// update the frequencies of both solutions
    ++H[ycur];
    ++H[ynew];
// if the new solution has a lower or equal frequency, take it
    if (H[ynew] <= H[ycur]) {
      ycur = ynew;
      xcur = xnew;
    }
  }
}

// run the fea
void run_fea1p1(const string folder_path,
    shared_ptr<IOHprofiler_suite<int> > suite, const int eval_budget,
    const int independent_runs, const unsigned rand_seed) {
  const string algorithm_name = "opofea";
  std::shared_ptr<IOHprofiler_csv_logger<int>> logger(
      new IOHprofiler_csv_logger<int>(folder_path, algorithm_name,
          algorithm_name, algorithm_name));
  logger->activate_logger();

  random_gen.seed(rand_seed);
  shared_ptr<IOHprofiler_problem<int> > problem;
  while ((problem = suite->get_next_problem()) != nullptr) {
    for (int i = independent_runs; (--i) >= 0;) {
      problem->reset_problem();
      logger->track_problem(*problem);
      fea1p1(problem, logger, eval_budget);
    }
  }

  logger->clear_logger();
}