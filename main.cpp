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
#include "src/localSearches.h"
#include <vector>

using namespace std;

int main(int argc, const char *argv[]) {
	vector<int> problem_id = { 1, 2 };
	vector<int> instance_id = { 1 };
	vector<int> dimension = { 100 };

	string dir = "/tmp/experiment";

	staticEA EA(2, 2);
	shared_ptr<IOHprofiler_suite<int> > pbo_EA(new PBO_suite(problem_id, instance_id, dimension));
	EA.run(dir, "EA", pbo_EA, 10000, 10000, 10, 1);

	shared_ptr<IOHprofiler_suite<int> > pbo_simple_ea1p1(new PBO_suite(problem_id, instance_id, dimension));
	run_simple_ea1p1(dir, pbo_simple_ea1p1, 10000, 10, 1);
}
