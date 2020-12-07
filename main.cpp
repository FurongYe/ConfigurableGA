//
//  main.cpp
//  ConfigurableGA
//
//  Created by Furong Ye on 26/10/2020.
//  Copyright © 2020 Furong Ye. All rights reserved.
//

#include "instance/ea.h"
#include "instance/RLS.h"
#include "instance/fastGA.h"
#include "instance/oneLLEA.h"
#include <vector>
#include "src/opoea.h"
#include "src/opofea.h"
#include "src/sa.h"
#include "src/sars.h"


#include "run/w_model_OneMax_suite.hpp"

using namespace std;

int main(int argc, const char *argv[]) {
  const vector<int> problem_id = { 1, 2, 10, 18, 20, 21, 22, 23 };
  const vector<int> instance_id = { 1 };
  const vector<int> dimension = { 100 };
  const string dir = "/tmp/experiment";
  const int budget = 100000;
  const int runs = 10;
  const unsigned seed = 1;

  staticEA EA(1, 1); // for comparison with opoea
  shared_ptr<W_Model_OneMax_suite > EAsuite(
      new W_Model_OneMax_suite());
  EA.run(dir, "EA", EAsuite, budget, budget, runs, seed);
  
  // staticEA EA(1, 1); // for comparison with opoea
  // shared_ptr<IOHprofiler_suite<int> > pbo_EA(
  //     new PBO_suite(problem_id, instance_id, dimension));
  // EA.run(dir, "EA", pbo_EA, budget, budget, runs, seed);

  // shared_ptr<IOHprofiler_suite<int> > pbo_ea1p1(
  //     new PBO_suite(problem_id, instance_id, dimension));
  // run_ea1p1(dir, pbo_ea1p1, budget, runs, seed);

  // shared_ptr<IOHprofiler_suite<int> > pbo_fea1p1(
  //     new PBO_suite(problem_id, instance_id, dimension));
  // run_fea1p1(dir, pbo_fea1p1, budget, runs, seed);

  // shared_ptr<IOHprofiler_suite<int>> pbo_sa(
  //     new PBO_suite(problem_id, instance_id, dimension));
  // run_simulated_annealing_exp(dir, pbo_sa, budget, runs, seed);

  // shared_ptr<IOHprofiler_suite<int>> pbo_sars(
  //     new PBO_suite(problem_id, instance_id, dimension));
  // run_simulated_annealing_exp_rs(dir, pbo_sars, budget, runs, seed);
}