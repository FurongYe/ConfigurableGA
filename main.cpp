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

using namespace std;

int main(int argc, const char * argv[]) {
    vector<int> problem_id = {1,2};
    vector<int> instance_id = {1};
   vector<int> dimension = {100};
   
  
  staticEA EA(2,2);
  shared_ptr<IOHprofiler_suite<int> > pbo_EA(new PBO_suite(problem_id,instance_id,dimension));
  EA.run("/tmp/", "EA", pbo_EA, 10000, 10000, 10,1);
//
//  RLS rls_;
//  shared_ptr<IOHprofiler_suite<int> > pbo_RLS(new PBO_suite(problem_id,instance_id,dimension));
//  rls_.run("./", "RLS", pbo_RLS, 10000, 10000, 10,1);

//
//  fastGA fga(1,1);
//  shared_ptr<IOHprofiler_suite<int> > pbo_fga(new PBO_suite(problem_id,instance_id,dimension));
//  fga.run("./", "fastGA", pbo_fga, 10000, 10000, 100,1);

//  oneLambdaLambdaEA ollEA(1);
//  shared_ptr<IOHprofiler_suite<int> > pbo_ollEA(new PBO_suite(problem_id,instance_id,dimension));
//  ollEA.run("./", "ollEA", pbo_ollEA, 10000, 10000, 100,1);
}
