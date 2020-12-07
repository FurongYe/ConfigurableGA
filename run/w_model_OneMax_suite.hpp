/// \file IOHprofiler_PBO_suite.h
/// \brief Hpp file for class IOHprofiler_PBO_suite.
///
/// A suite of Pseudo Boolean problems (23 problems, 1-100 instances, and dimension <= 20000).
///
/// \author Furong Ye
/// \date 2019-06-27
#ifndef _W_MODEL_ONEMAX_SUITE_HPP
#define _W_MODEL_ONEMAX_SUITE_HPP

#include <sstream>
#include <iomanip>
#include "f_one_max.hpp"
#include "f_w_model_one_max.hpp"
#include "IOHprofiler_suite.h"

using namespace std;
vector< shared_ptr<W_Model_OneMax> > w_model_onemax_list;

vector<double> default_dummy_para = {0.0, 0.1, 0.25, 0.9};
vector<int> default_epistasis_para = {0, 2, 3, 4, 5, 7};
vector<int> default_neturality_para = {1, 3, 5};
vector<double> default_ruggedness_para = {0, 0.2, 0.4, 0.6, 0.8, 1};

size_t FE_budget = 100000;
size_t independent_runs  = 11;

class W_Model_OneMax_suite : public IOHprofiler_suite<int> {
public:
  using InputType = int;

  W_Model_OneMax_suite(vector<double>dummy_para = default_dummy_para,
                        vector <int> epistasis_para = default_epistasis_para, 
                        vector <int> neturality_para = default_neturality_para, 
                        vector<double> ruggedness_para = default_ruggedness_para)
                        : dummy_para(dummy_para),
                          epistasis_para(epistasis_para),
                          neturality_para(neturality_para),
                          ruggedness_para(ruggedness_para) {
    
        for (size_t du = 0; du != this->dummy_para.size(); ++du) {
      for (size_t ep = 0; ep != this->epistasis_para.size(); ++ ep) {
        for (size_t ne = 0; ne != this->neturality_para.size(); ++ne) {
          for (size_t ru = 0; ru != this->ruggedness_para.size(); ++ru) {
            this->para_product.push_back({du,ep,ne,ru});
          }
        }
      }
    }

    std::vector<int> problem_id;
    std::vector<int> instance_id;
    std::vector<int> dimension;
    for (int i = 0; i < para_product.size(); ++i) {
      problem_id.push_back(i+1);
    }
    for (int i = 0; i < 1; ++i) {
      instance_id.push_back(i+1);
    }
    dimension.push_back(20);
    dimension.push_back(30);

    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("W_Model_OneMax_suite");
    // Load problem, so that the user don't have to think about it.
    this->loadProblem();
  }

  W_Model_OneMax_suite(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension, 
                        vector<double>dummy_para = default_dummy_para,
                        vector <int> epistasis_para = default_epistasis_para, 
                        vector <int> neturality_para = default_neturality_para, 
                        vector<double> ruggedness_para = default_ruggedness_para)
                        : dummy_para(dummy_para),
                          epistasis_para(epistasis_para),
                          neturality_para(neturality_para),
                          ruggedness_para(ruggedness_para) {
    
    for (size_t du = 0; du != this->dummy_para.size(); ++du) {
      for (size_t ep = 0; ep != this->epistasis_para.size(); ++ ep) {
        for (size_t ne = 0; ne != this->neturality_para.size(); ++ne) {
          for (size_t ru = 0; ru != this->ruggedness_para.size(); ++ru) {
            this->para_product.push_back({du,ep,ne,ru});
          }
        }
      }
    }
    
    for (size_t i = 0; i < problem_id.size(); ++i) {
      if (problem_id[i] < 0 || problem_id[i] > para_product.size()) {
        IOH_error("problem_id " + std::to_string(problem_id[i]) + " is not in W_Model_OneMax_suite");
      }
    }

    for (size_t i = 0; i < instance_id.size(); ++i) {
      if (instance_id[i] < 0 || instance_id[i] > 100) {
        IOH_error("instance_id " + std::to_string(instance_id[i]) + " is not in W_Model_OneMax_suite");
      }
    }

    for (size_t i = 0; i < dimension.size(); ++i) {
      if (dimension[i] < 0 || dimension[i] > 20000) {
        IOH_error("dimension " + std::to_string(dimension[i]) + " is not in W_Model_OneMax_suite");
      }
    }

    IOHprofiler_set_suite_problem_id(problem_id);
    IOHprofiler_set_suite_instance_id(instance_id);
    IOHprofiler_set_suite_dimension(dimension);
    IOHprofiler_set_suite_name("W_Model_OneMax_suite");
    this->loadProblem();
  }

  void loadProblem() {

    if (this->size() != 0) {
      this->clear();
    }
    this->IOHprofiler_set_size_of_problem_list(this->para_product.size() * this->IOHprofiler_suite_get_dimension().size());
    this->IOHprofiler_set_problem_list_index(0);

    for (int i = 0; i != this->para_product.size(); ++i) {
      for (int j = 0; j != this->IOHprofiler_suite_get_dimension().size(); ++j) {
        shared_ptr<W_Model_OneMax> p(new W_Model_OneMax());
        p->set_w_setting(this->dummy_para[ this->para_product[i][0] ], this->epistasis_para[ this->para_product[i][1]], 
                          this->neturality_para[ this->para_product[i][2] ],
                          static_cast<int>( floor( this->IOHprofiler_suite_get_dimension()[j] * this->ruggedness_para[ this->para_product[i][3]] ) ) );
        
        string problem_name = "Onemax";
        std::stringstream dss; 
        dss << std::setprecision(3) << this->dummy_para[ this->para_product[i][0] ];
        problem_name += "_D" + dss.str();
        problem_name += "_E" + std::to_string(this->epistasis_para[ this->para_product[i][1] ]);
        problem_name += "_N" + std::to_string(this->neturality_para[ this->para_product[i][2] ]);
        std::stringstream rss;  
        rss << std::setprecision(3) << this->ruggedness_para[ this->para_product[i][3] ];
        problem_name += "_R" + rss.str();
        p->IOHprofiler_set_problem_name(problem_name);
        p->IOHprofiler_set_problem_id(i+1);
        p->IOHprofiler_set_number_of_variables(this->IOHprofiler_suite_get_dimension()[j]);
        this->push_back(p);
        mapIDTOName(i+1, problem_name);
      }
    }
    assert(this->IOHprofiler_get_size_of_problem_list() == this->size());
    this->IOHprofiler_set_get_problem_flag(false);
    this->IOHprofiler_set_load_problem_flag(true);
  }

  static W_Model_OneMax_suite * createInstance() {
    return new W_Model_OneMax_suite();
  }

  static W_Model_OneMax_suite * createInstance(std::vector<int> problem_id, std::vector<int> instance_id, std::vector<int> dimension) {
    return new W_Model_OneMax_suite(problem_id, instance_id, dimension);
  }

private:
  vector<double> dummy_para;
  vector<int> epistasis_para;
  vector<int> neturality_para;
  vector<double> ruggedness_para;
  vector < vector <size_t> > para_product;
};

#endif //_W_MODEL_ONEMAX_SUITE_HPP

