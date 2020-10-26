//
//  main.cpp
//  ConfigurableGA
//
//  Created by Furong Ye on 26/10/2020.
//  Copyright © 2020 Furong Ye. All rights reserved.
//

#include "instance/ea.cpp"
#include "instance/RLS.cpp"
#include "instance/fastGA.cpp"
#include "instance/oneLLEA.cpp"
#include <vector>

using namespace std;

int main(int argc, const char * argv[]) {
    vector<int> problem_id = {1,2};
    vector<int> instance_id = {1};
   vector<int> dimension = {100};
   shared_ptr<IOHprofiler_suite<int> > pbo(new PBO_suite(problem_id,instance_id,dimension));
  
//  staticEA EA(1,1);
//  EA.run("/Users/fye/Codes/Git/ConfigurableGA/ConfigurableGA/", "EA", pbo, 10000, 10000, 10,1);
//
//  RLS rls_(1,1);
//  rls_.run("/Users/fye/Codes/Git/ConfigurableGA/ConfigurableGA/", "RLS", pbo, 10000, 10000, 10,1);
//
//  fastGA fga(1,1);
//  fga.run("/Users/fye/Codes/Git/ConfigurableGA/ConfigurableGA/", "fastGA", pbo, 10000, 10000, 100,1);
  oneLambdaLambdaEA ollEA(1);
  ollEA.run("/Users/fye/Codes/Git/ConfigurableGA/ConfigurableGA/", "(1+lambda,lambda) EA", pbo, 10000, 10000, 100,1);
}
