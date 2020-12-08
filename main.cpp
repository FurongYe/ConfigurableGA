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
#include "instance/twoRateEA.h"
#include <vector>
#include "src/opoea.h"
#include "src/opofea.h"
#include "src/sa.h"
#include "src/sars.h"
#include "src/ghc.h"
#include "src/estimationOfDistribution.h"

#include "run/w_model_OneMax_suite.hpp"
#include "run/w_model_LeadingOnes_suite.hpp"

using namespace std;

/**
 * Alrogrithm names:
 *    ea : (1+1)-EA>0 p = 1/n
 *    ea2 : (1+1)-EA>0 p = 2/n
 *    ea23 : (1+1)-EA>0 p = 2/3n
 *    llea  : (1+(lambda,lambda))-EA>_0, initial lambda = 10, p = 1/n
 *    rls : randomized local search
 *    rs : random search
 *    ghc : greedy hill climber
 *    fga : (1+1)-fast GA
 *    2ratega : (1+10)-EA>0 with 2rate self adaptation of mutation rate
 *    umda : univarate marginal distribution algorithm, population size = 50
 *    sa : simulated annealing
 *    sars : the simulated annealing algorithm with exponential temperature schedule with iterative restarts
 *    fea : (1+1)-EA>0 with frequency fitness assignment
 */
void runAlgorithm(shared_ptr< IOHprofiler_suite<int> > suite, const string algorithm_name, const string dir, const int budget, const int runs, const unsigned seed)
{
  if (algorithm_name == "ea") {
    staticEA ea(1, 1, 1.0);
    ea.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "ea2") {
    staticEA ea(1, 1, 2.0);
    ea.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "ea23") {
    staticEA ea(1, 1, 1.5);
    ea.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "llea") {
    oneLambdaLambdaEA llEA(10);
    llEA.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "rls") {
    RLS rls;
    rls.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  // } else if (algorithm_name == "rs") {
  //   staticEA EA(1, 1, 1.5);
  //   EA.run(dir, "EA", pbo_EA, budget, budget, runs, seed);
  // } 
  } else if (algorithm_name == "ghc") {
    GreedyHillClimber ghc;
    ghc.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "fga") {
    FastGA fGA(1, 1);
    fGA.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "2ratega") {
    TwoRateEA twoRateGA(10);
    twoRateGA.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "umda") {
    EstimationOfDistribution umda(25,50);
    umda.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "sa") {
    run_simulated_annealing_exp(dir, suite, budget, runs, seed);
  } else if (algorithm_name == "sars") {
    run_simulated_annealing_exp_rs(dir, suite, budget, runs, seed);
  } else if (algorithm_name == "fea") {
    run_fea1p1(dir, suite, budget, runs, seed);
  } else {
    cout << "Unkownn algorithm" << endl;
  }
}

int main(int argc, const char *argv[])
{
  const vector<double> dummy = {0.0, 0.9};
  const vector<int> epistasis = {0, 2, 5, 7};
  const vector<int> neutrality = {1, 5};
  const vector<double> ruggedness = {0, 0.8, 1};

  int number_of_problems = dummy.size() * epistasis.size() * neutrality.size() * ruggedness.size();
  vector<int> problem_id;
  problem_id.reserve(number_of_problems);
  for (int i = 1; i <= number_of_problems; ++i)
  {
    problem_id.push_back(i);
  }
  const vector<int> instance_id = {1};
  const vector<int> dimension = {20};
  const string dir = "./";
  const int budget = 1000;
  const int runs = 10;
  const unsigned seed = 1;

  staticEA EA(1, 1); // for comparison with opoea
  shared_ptr<W_Model_LeadingOnes_suite> EAsuite(
      new W_Model_LeadingOnes_suite(problem_id, instance_id, dimension, dummy, epistasis, neutrality, ruggedness));
  runAlgorithm(EAsuite, "oea", dir, budget, runs, seed);
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