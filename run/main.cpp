//
//  main.cpp
//  ConfigurableGA
//
//  Created by Furong Ye on 26/10/2020.
//  Copyright © 2020 Furong Ye. All rights reserved.
//

#include "../algorithm/ga/instance/ea.h"
#include "../algorithm/ga/instance/RLS.h"
#include "../algorithm/ga/instance/fastGA.h"
#include "../algorithm/ga/instance/oneLLEA.h"
#include "../algorithm/ga/instance/twoRateEA.h"
#include "../algorithm/fea/opoea.h"
#include "../algorithm/fea/opofea.h"
#include "../algorithm/sa/sa.h"
#include "../algorithm/sa/sars.h"
#include "../algorithm/ghc/ghc.h"
#include "../algorithm/eda/estimationOfDistribution.h"
#include "../algorithm/rs/randomsearch.h"

#include "IOHprofiler_string.hpp"
#include "wmodelsuite/w_model_OneMax_suite.hpp"
#include "wmodelsuite/w_model_LeadingOnes_suite.hpp"
#include "IOHprofiler_PBO_suite.hpp"

#include <vector>

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
    oneLambdaLambdaEA llEA(1);
    llEA.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "rls") {
    RLS rls;
    rls.run(dir, algorithm_name, suite, budget, std::numeric_limits<int>::max(), runs, seed);
  } else if (algorithm_name == "rs") {
    RandomSearch rs;
    rs.run(dir, algorithm_name, suite, budget,runs, seed);
  } else if (algorithm_name == "ghc") {
    GreedyHillClimber ghc;
    ghc.run(dir, algorithm_name, suite, budget, runs, seed);
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
    cout << "Unkownn algorithm : " << algorithm_name << endl;
  }
}

/**
 * Arguments: suite_name problem_id instance_id dimension
 *  algorithm_name : 'name'
 *  suite_name : 'name', name \in {'PBO','WmodelOneMax', 'WmodelLeadingOnes'}
 *  problem_id : 'start-end' , id \in [1..25] for PBO, [1..product.size()] for Wmodel extensions.
 *  instance_id: 'start-end' , id \in [1..100]
 *  dimension :  'dimension1, dimension2, ...'
 *  dir : 'dir', the directory where the output folder locates.
 *  runs : 'number_of_indepedent_runs', runs > 1
 *  budget : 'budget', the maximum function evaluations. budget > 1
 *  seed : 'seed', a random seed
 * 
 * An instance: ./main ea pbo 1-3 1-5 10,100 ./ 10 100 1
 **/

int main(int argc, const char *argv[])
{
  const vector<double> dummy = {0.0, 0.9};
  const vector<int> epistasis = {0, 2, 5, 7};
  const vector<int> neutrality = {1, 5};
  const vector<double> ruggedness = {0, 0.8, 1};
  int number_of_w_problems = dummy.size() * epistasis.size() * neutrality.size() * ruggedness.size();

  string algorithm_name = argv[1];
  string suite_name = argv[2];
  string problem_str = argv[3];
  string instance_str = argv[4];
  string dimension_str = argv[5];
  string dir = argv[6];
  int runs = stoi(argv[7]);
  int budget = stoi(argv[8]);
  unsigned seed =  static_cast<unsigned> (stoi(argv[9]));
  
  transform(suite_name.begin(),suite_name.end(),suite_name.begin(),::tolower);
  transform(algorithm_name.begin(),algorithm_name.end(),algorithm_name.begin(),::tolower);
  if (suite_name == "pbo") {
    vector<int> problem_id = get_int_vector_parse_string(problem_str,1,25);
    vector<int> instance_id = get_int_vector_parse_string(instance_str,1,100);
    vector<int> dimension = get_int_vector_parse_string(dimension_str,2,20000);
    shared_ptr<PBO_suite> suite(new PBO_suite(problem_id, instance_id, dimension));
    runAlgorithm(suite, algorithm_name, dir, budget, runs, seed);
  } else if (suite_name == "wmodelonemax") {
    vector<int> problem_id = get_int_vector_parse_string(problem_str,1,number_of_w_problems);
    vector<int> instance_id = get_int_vector_parse_string(instance_str,1,100);
    vector<int> dimension = get_int_vector_parse_string(dimension_str,2,20000);
    shared_ptr<W_Model_OneMax_suite> suite(
      new W_Model_OneMax_suite(problem_id, instance_id, dimension, dummy, epistasis, neutrality, ruggedness));
    runAlgorithm(suite, algorithm_name, dir, budget, runs, seed);
  } else if (suite_name == "wmodelleadingones") {
    vector<int> problem_id = get_int_vector_parse_string(problem_str,1,number_of_w_problems);
    vector<int> instance_id = get_int_vector_parse_string(instance_str,1,100);
    vector<int> dimension = get_int_vector_parse_string(dimension_str,2,20000);
    shared_ptr<W_Model_LeadingOnes_suite> suite(
      new W_Model_LeadingOnes_suite(problem_id, instance_id, dimension, dummy, epistasis, neutrality, ruggedness));
    runAlgorithm(suite, algorithm_name, dir, budget, runs, seed);
  } else {
    cout << "Unknown suite : " << suite_name << ", avaliable options are \"PBO\", \"WModelOneMax\", and \"WModelLeadingOnes\"." << endl;
  }
  return 0;
}


// int main(int argc, const char *argv[])
// {
//   const vector<double> dummy = {0.0, 0.9};
//   const vector<int> epistasis = {0, 2, 5, 7};
//   const vector<int> neutrality = {1, 5};
//   const vector<double> ruggedness = {0, 0.8, 1};

//   int number_of_problems = dummy.size() * epistasis.size() * neutrality.size() * ruggedness.size();
//   vector<int> problem_id;
//   problem_id.reserve(number_of_problems);
//   for (int i = 1; i <= number_of_problems; ++i)
//   {
//     problem_id.push_back(i);
//   }
//   const vector<int> instance_id = {1};
//   const vector<int> dimension = {20};
//   const string dir = "./";
//   const int budget = 1000;
//   const int runs = 10;
//   const unsigned seed = 1;

//   staticEA EA(1, 1); // for comparison with opoea
//   shared_ptr<W_Model_LeadingOnes_suite> EAsuite(
//       new W_Model_LeadingOnes_suite(problem_id, instance_id, dimension, dummy, epistasis, neutrality, ruggedness));
//   runAlgorithm(EAsuite, "oea", dir, budget, runs, seed);
//   // staticEA EA(1, 1); // for comparison with opoea
//   // shared_ptr<IOHprofiler_suite<int> > pbo_EA(
//   //     new PBO_suite(problem_id, instance_id, dimension));
//   // EA.run(dir, "EA", pbo_EA, budget, budget, runs, seed);

//   // shared_ptr<IOHprofiler_suite<int> > pbo_ea1p1(
//   //     new PBO_suite(problem_id, instance_id, dimension));
//   // run_ea1p1(dir, pbo_ea1p1, budget, runs, seed);

//   // shared_ptr<IOHprofiler_suite<int> > pbo_fea1p1(
//   //     new PBO_suite(problem_id, instance_id, dimension));
//   // run_fea1p1(dir, pbo_fea1p1, budget, runs, seed);

//   // shared_ptr<IOHprofiler_suite<int>> pbo_sa(
//   //     new PBO_suite(problem_id, instance_id, dimension));
//   // run_simulated_annealing_exp(dir, pbo_sa, budget, runs, seed);

//   // shared_ptr<IOHprofiler_suite<int>> pbo_sars(
//   //     new PBO_suite(problem_id, instance_id, dimension));
//   // run_simulated_annealing_exp_rs(dir, pbo_sars, budget, runs, seed);
// }
