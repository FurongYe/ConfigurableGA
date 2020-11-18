#include "localSearches.h"
#include <iostream>

inline static void simple_ea1p1(shared_ptr< IOHprofiler_problem<int> > problem, shared_ptr< IOHprofiler_csv_logger<int> > logger,
		int eval_budget) {
  int n = problem->IOHprofiler_get_number_of_variables();
  std::vector<int> xcur;
  std::vector<int> xnew;
  double ycur;
  double ynew;

  xcur.reserve(n);
   for (int i = n; (--i) >= 0; ) {
     xcur.push_back( (int) ( 2 * uniform_random() ) );
   }

  ycur = problem->evaluate(xcur);
  logger->do_log(problem->loggerInfo());

  int count = eval_budget;
  while(((--count)>0) && (!problem->IOHprofiler_hit_optimal())) {
    xnew = xcur;

    bool unchanged = true;
    do {
    	for(int i = n; (--i)>=0;){
    		if(uniform_random()>=0.5) {
    			unchanged = false;
    			xnew[i] ^= true;
    		}
    	}
    } while(unchanged);

    ynew = problem->evaluate(xnew);
    logger->do_log(problem->loggerInfo());

    if (ynew >= ycur) {
      ycur = ynew;
      xcur = xnew;
    }
  }
}

void run_simple_ea1p1(string folder_path,shared_ptr<IOHprofiler_suite<int> >suite, int eval_budget, int independent_runs, unsigned rand_seed){
  string algorithm_name = "opoea";
  std::shared_ptr<IOHprofiler_csv_logger<int>> logger(new IOHprofiler_csv_logger<int>(folder_path,algorithm_name,algorithm_name,algorithm_name) );
  logger->activate_logger();

  random_gen.seed(rand_seed);
  shared_ptr< IOHprofiler_problem<int> > problem;
  while ((problem = suite->get_next_problem()) != nullptr) {
	  for(int i = independent_runs; (--i)>=0;) {
		  problem->reset_problem();
		  logger->track_problem(*problem);
		  simple_ea1p1(problem, logger, eval_budget);
	  }
  }

  logger->clear_logger();
}
