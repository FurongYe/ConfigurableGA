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

void ea1p1(shared_ptr<IOHprofiler_problem<int>> problem,
    shared_ptr<IOHprofiler_csv_logger<int>> logger,
    const unsigned long long eval_budget) {
// check input variables
  if (eval_budget <= 1) throw "eval_budget must be > 1";
  if (!problem) throw "problem must not be null";
  if (!logger) throw "logger must not be null";

// n be the number of variables
  const int n = problem->IOHprofiler_get_number_of_variables();
  if (n <= 0) throw "number of variables must be positive";
// the bit flip probability
  const double p = 1.0 / ((double) n);
  if ((!isfinite(p)) || (p <= 0.0) || (p >= 1.0)) throw "p must be from (0,1)";

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
  unsigned long long int step = 1;
  while (((++step) <= eval_budget) && (!problem->IOHprofiler_hit_optimal())) {

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

// run the (1+1) ea
void run_ea1p1(const string folder_path,
    shared_ptr<IOHprofiler_suite<int>> suite,
    const unsigned long long eval_budget,
    const unsigned long long independent_runs,
    const unsigned long long rand_seed) {
  if (folder_path.empty()) throw "folder path cannot be empty";
  if (!suite) throw "suite cannot be null";
  if (eval_budget <= 1) throw "eval_budget must be > 1";
  if (independent_runs < 1) throw "independent_runs must be > 0";

  const string algorithm_name = "opoea";
  std::shared_ptr<IOHprofiler_csv_logger<int>> logger(
      new IOHprofiler_csv_logger<int>(folder_path, algorithm_name,
          algorithm_name, algorithm_name));
  logger->activate_logger();

  random_gen.seed(rand_seed);
  shared_ptr<IOHprofiler_problem<int> > problem;
  while ((problem = suite->get_next_problem()) != nullptr) {
    for (unsigned long long i = 0; i < independent_runs; i++) {
      problem->reset_problem();
      logger->track_problem(*problem);
      ea1p1(problem, logger, eval_budget);
    }
  }

  logger->clear_logger();
}
