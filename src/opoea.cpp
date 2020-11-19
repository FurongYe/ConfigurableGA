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
#include "opoea.h"

void ea1p1(shared_ptr<IOHprofiler_problem<int> > problem,
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

// first we generate the random initial solution
  xcur.reserve(n);
  for (int i = n; (--i) >= 0;) {
    xcur.push_back((int) (2 * uniform_random()));
  }
// we evaluate the random initial solution
  ycur = problem->evaluate(xcur);
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

// if new solution is at least as good as current one, accept it
    if (ynew >= ycur) {
      ycur = ynew;
      xcur = xnew;
    }
  }
}

// run the ea
void run_ea1p1(const string folder_path,
    shared_ptr<IOHprofiler_suite<int> > suite, const int eval_budget,
    const int independent_runs, const unsigned rand_seed) {
  const string algorithm_name = "opoea";
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
      ea1p1(problem, logger, eval_budget);
    }
  }

  logger->clear_logger();
}
