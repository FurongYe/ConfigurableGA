#include "geneticAlgorithm.h"

bool GeneticAlgorithm::Termination() {
  if (!this->optimum_found_flag_ && this->evaluation_ < this->evluation_budget_ && this->generation_ <= this->generation_budget_) {
    return false;
  } else {
    return true;
  }
}

void GeneticAlgorithm::SetAllParameters(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para) {
  assert(integer_para.size() == 4);
  this->set_mu(integer_para[0]);
  this->set_lambda(integer_para[1]);
  this->set_l(integer_para[2]);
  this->set_tournament_k(integer_para[3]);
  
  assert(continuous_para.size() == 6);
  this->set_crossover_probability(continuous_para[0]);
  this->set_p_u(continuous_para[1]);
  this->set_mutation_rate(continuous_para[2]);
  this->set_r_n(continuous_para[3]);
  this->set_sigma_n(continuous_para[4]);
  this->set_beta_f(continuous_para[5]);
  
  assert(category_para.size() == 4);
  this->set_crossover_mutation_r(category_para[0]);
  this->set_crossover_operator(category_para[1]);
  this->set_mutation_operator(category_para[2]);
  this->set_selection_operator(category_para[3]);
}

void GeneticAlgorithm::run(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para) {
  this->SetAllParameters(integer_para, continuous_para, category_para);
  this->DoGeneticAlgorithm();
}

void GeneticAlgorithm::run(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para,  shared_ptr<IOHprofiler_suite<int> > suite) {
  this->SetAllParameters(integer_para, continuous_para, category_para);
  
  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
    this->DoGeneticAlgorithm();
  }
}

void GeneticAlgorithm::run_N(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para) {
  this->SetAllParameters(integer_para, continuous_para, category_para);
  
  size_t r = 0;
  this->ecdf_sum_ = 0;
  while (r < this->independent_runs_) {
    this->DoGeneticAlgorithm();
//    if (this->ecdf_logger_ != nullptr) {
//      IOHprofiler_ecdf_sum sum;
//      this->ecdf_sum_ += sum(this->ecdf_logger_->data());
//    }
    r++;
  }
  
  this->ecdf_sum_ = this->ecdf_sum_ / this->independent_runs_;
  this->ecdf_ratio_ = (double)this->ecdf_sum_ / (double)this->ecdf_budget_width_ / (double)this->ecdf_target_width_ / this->independent_runs_;
}

void GeneticAlgorithm::run_N(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para, shared_ptr<IOHprofiler_suite<int> > suite) {
  this->SetAllParameters(integer_para, continuous_para, category_para);
  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
    this->run_N(integer_para,continuous_para,category_para);
  }
}

void GeneticAlgorithm::run_N(shared_ptr<IOHprofiler_suite<int> > suite) {
  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
    size_t r = 0;
    this->ecdf_sum_ = 0;
    while (r < this->independent_runs_) {
      this->DoGeneticAlgorithm();
  //    if (this->ecdf_logger_ != nullptr) {
  //      IOHprofiler_ecdf_sum sum;
  //      this->ecdf_sum_ += sum(this->ecdf_logger_->data());
  //    }
      r++;
    }
    
    this->ecdf_sum_ = this->ecdf_sum_ / this->independent_runs_;
    this->ecdf_ratio_ = (double)this->ecdf_sum_ / (double)this->ecdf_budget_width_ / (double)this->ecdf_target_width_ / this->independent_runs_;
  }
}

void GeneticAlgorithm::DoGeneticAlgorithm() {
  double rand;
  this->Preparation();
  
  this->Initialization();
  while (!this->Termination()) {
    ++this->generation_;
    
    this->offspring_population_.clear();
    this->offspring_fitness_.clear();
    for (size_t i = 0; i < this->lambda_; ++i) {
      this->SelectTwoParents();
      this->offspring_population_.push_back(this->parents_population_[this->selected_parents_[0]]);
      
      rand = uniform_random();
      this->c_flipped_index.clear();
      this->m_flipped_index.clear();
      if (rand < this->crossover_probability_) {
        this->DoCrossover(this->offspring_population_[i], this->parents_population_[this->selected_parents_[0]], this->parents_population_[this->selected_parents_[1]]);
      }
      
      if (this->crossover_mutation_r_) {
        this->DoMutation(this->offspring_population_[i]);
      } else if (rand >= this->crossover_probability_) {
        this->DoMutation(this->offspring_population_[i]);
      }
      
      if (this->c_flipped_index == this->m_flipped_index) { /// If the flipping indexes of crossover and mutation are identical, the individual remains the same.
        this->offspring_fitness_.push_back(this->parents_fitness_[this->selected_parents_[0]]);
      } else if (this->offspring_population_[i] == this->parents_population_[this->selected_parents_[1]]) { /// If the offspring is identical with the second parent.
        /// TODO: Do something to save time for this comparison.
        this->offspring_fitness_.push_back(this->parents_fitness_[this->selected_parents_[1]]);
      } else { /// otherwise evaluate.
        this->offspring_fitness_.push_back(this->Evaluate(this->offspring_population_[i]));
      }
      
      if (this->Termination()) break;
    }
    
    if (this->Termination()) break;
    
    this->DoSelection(this->parents_population_, this->parents_fitness_, this->offspring_population_, this->offspring_fitness_);
    this->AdaptiveStrategy();
  }
}

void GeneticAlgorithm::Initialization() {
  int n = this->problem_->IOHprofiler_get_number_of_variables();
  for (int i = 0; i != this->mu_; ++i) {
    vector<int> tmp(n,0);
    for (int j = 0; j != n; ++j) {
      if (uniform_random() < 0.5) {
        tmp[j] = 1;
      }
    }
    this->parents_population_.push_back(tmp);
    this->parents_fitness_.push_back(this->Evaluate(tmp));
  }
}

void GeneticAlgorithm::Preparation() {
  this->problem_->reset_problem();
  if (this->csv_logger_ != nullptr) {
    this->csv_logger_->track_problem(*this->problem_);
  }
//  if (this->ecdf_logger_ != nullptr) {
//    this->ecdf_logger_->track_problem(*this->problem_);
//  }
  this->selected_parents_ = vector<size_t>(2);
  this->optimum_found_flag_ = false;
  this->optimum_ = this->problem_->IOHprofiler_get_optimal()[0];
  this->dimension_ = this->problem_->IOHprofiler_get_number_of_variables();
  this->PowerLawDistribution(this->problem_->IOHprofiler_get_number_of_variables());
  this->parents_fitness_.clear();
  this->parents_population_.clear();
  this->offspring_fitness_.clear();
  this->offspring_population_.clear();
  this->evaluation_ = 0;
  this->generation_ = 0;
  this->best_individual_ = vector<int>(this->problem_->IOHprofiler_get_number_of_variables());
  if (Opt == optimizationType::MAXIMIZATION) {
    this->best_found_fitness_ = numeric_limits<double>::lowest();
  } else {
    this->best_found_fitness_ = numeric_limits<double>::max();
  }
  this->hitting_flag_ = false;
}

void GeneticAlgorithm::SelectTwoParents() {
  this->selected_parents_[0] = static_cast<size_t>( floor(uniform_random() * this->mu_) );
  if (this->mu_ >= 2) {
    do {
      this->selected_parents_[1] = static_cast<size_t>( floor(uniform_random() * this->mu_) );
    } while(this->selected_parents_[0] == this->selected_parents_[1]);
  }
}

void GeneticAlgorithm::AssignProblem(shared_ptr<IOHprofiler_problem<int> > problem_ptr) {
  this->problem_ = problem_ptr;
}

void GeneticAlgorithm::AssignLogger(shared_ptr<IOHprofiler_csv_logger<int> > logger_ptr) {
  this->csv_logger_ = logger_ptr;
}

double GeneticAlgorithm::Evaluate(vector<int> &x) {
  double result;
  result = this->problem_->evaluate(x);
  if (this->csv_logger_ != nullptr) {
    this->csv_logger_->do_log(this->problem_->loggerInfo()); /// TODO: we assume only using PBO suite now.
  }
//  if (this->ecdf_logger_ != nullptr) {
//    this->ecdf_logger_->do_log(this->problem_->loggerInfo()); /// TODO: we assume only using PBO suite now.
//  }
  ++this->evaluation_;
  this->optimum_found_flag_ = this->problem_->IOHprofiler_hit_optimal();
  
  if (Opt == optimizationType::MAXIMIZATION) {
    if (result > this->best_found_fitness_) {
      this->best_found_fitness_ = result;
      this->best_individual_ = x;
    }
  } else {
    if (result < this->best_found_fitness_) {
      this->best_found_fitness_ = result;
      this->best_individual_ = x;
    }
  }
  
  // This is only used to calculate ERT
  if (this->ERT_flag_) {
    if (Opt == optimizationType::MAXIMIZATION) {
      if (!this->hitting_flag_ && this->best_found_fitness_ >= this->hiting_target_) {
        this->hitting_time_.push_back(this->evaluation_);
        this->hitting_flag_ = true;
      }
    } else {
      if (!this->hitting_flag_ && this->best_found_fitness_ <= this->hiting_target_) {
        this->hitting_time_.push_back(this->evaluation_);
        this->hitting_flag_ = true;
      }
    }
  }
  return result;
}

void GeneticAlgorithm::SetSeed(unsigned seed) {
  random_gen.seed(seed);
}

void GeneticAlgorithm::set_mu(const int mu) {
  this->mu_ = mu;
}

void GeneticAlgorithm::set_lambda(const int lambda) {
  this->lambda_ = lambda;
}

void GeneticAlgorithm::set_crossover_probability(const double crossover_probability) {
  this->crossover_probability_ = crossover_probability;
}

void GeneticAlgorithm::set_crossover_mutation_r(const int crossover_mutation_r) {
  this->crossover_mutation_r_ = crossover_mutation_r;
}

void GeneticAlgorithm::set_crossover_mutation_r(string crossover_mutation_r) {
  transform(crossover_mutation_r.begin(), crossover_mutation_r.end(), crossover_mutation_r.begin(), ::toupper);
  if (crossover_mutation_r == "IND") {
    this->set_crossover_mutation_r(IND);
  } else if (crossover_mutation_r == "OR") {
    this->set_crossover_mutation_r(OR);
  } else {
    cerr << "invalid value for set_crossover_mutation_r";
    assert(false);
  }
}

void GeneticAlgorithm::set_evaluation_budget(const size_t evaluation_budget) {
  this->evluation_budget_ = evaluation_budget;
}

void GeneticAlgorithm::set_generation_budget(const size_t generation_budget) {
  this->generation_budget_ = generation_budget;
}

void GeneticAlgorithm::set_independent_runs(const size_t independent_runs) {
  this->independent_runs_ = independent_runs;
}

void GeneticAlgorithm::set_parents_population(const vector< vector<int> > parents_population) {
  this->parents_population_ = parents_population;
}

void GeneticAlgorithm::set_parents_fitness(const vector<double> &parents_fitness) {
  this->parents_fitness_ = parents_fitness;
}

void GeneticAlgorithm::set_parents_population(const vector<int> &parent, const size_t index) {
  assert(this->parents_population_.size() > index);
  this->parents_population_[index] = parent;
}

void GeneticAlgorithm::set_parents_fitness(const double fitness, const size_t index) {
  assert(this->parents_fitness_.size() > index);
  this->parents_fitness_[index] = fitness;
}

void GeneticAlgorithm::add_parents_population(const vector<int> & parent) {
  this->parents_population_.push_back(parent);
  
}
void GeneticAlgorithm::clear_parents_population() {
  this->parents_population_.clear();
}

void GeneticAlgorithm::add_parents_fitness(const double fitness) {
  this->parents_fitness_.push_back(fitness);
}

void GeneticAlgorithm::clear_parents_fitness() {
  this->parents_fitness_.clear();
}

void GeneticAlgorithm::set_offspring_population(const vector< vector<int> > &offspring_population) {
  this->offspring_population_ = offspring_population;
}

void GeneticAlgorithm::set_offspring_fitness(const vector<double> &offspring_fitness) {
  this->offspring_fitness_ = offspring_fitness;
}

void GeneticAlgorithm::set_offspring_population(const vector<int> &offspring, const size_t index) {
  assert(this->offspring_population_.size() > index);
  this->offspring_population_[index] = offspring;
}

void GeneticAlgorithm::set_offspring_fitness(const double fitness, const size_t index) {
  assert(this->offspring_fitness_.size() > index);
  this->offspring_fitness_[index] = fitness;
}

void GeneticAlgorithm::add_offspring_population(const vector<int> & parent) {
  this->offspring_population_.push_back(parent);
  
}
void GeneticAlgorithm::clear_offspring_population() {
  this->offspring_population_.clear();
}

void GeneticAlgorithm::add_offspring_fitness(const double fitness) {
  this->offspring_fitness_.push_back(fitness);
}

void GeneticAlgorithm::clear_offspring_fitness() {
  this->offspring_fitness_.clear();
}

void GeneticAlgorithm::set_best_found_fitness(const double best_found_fitness) {
  this->best_found_fitness_ = best_found_fitness;
}

void GeneticAlgorithm::set_best_individual(const vector <int> best_individual) {
  this->best_individual_ = best_individual;
}

void GeneticAlgorithm::set_generation(const size_t generation) {
  this->generation_ = generation;
}

void GeneticAlgorithm::update_generation() {
  this->generation_++;
}

int GeneticAlgorithm::get_mu() const {
  return this->mu_;
}

int GeneticAlgorithm::get_lambda() const {
  return this->lambda_;
}

int GeneticAlgorithm::get_dimension() const {
  return this->dimension_;
}

double GeneticAlgorithm::get_crossover_probability() const {
  return this->crossover_probability_;
}

int GeneticAlgorithm::get_crossover_mutation_r() const {
  return this->crossover_mutation_r_;
}

vector< vector<int> > GeneticAlgorithm::get_parents_population() {
  return this->parents_population_;
}

vector<double> GeneticAlgorithm::get_parents_fitness() {
  return this->parents_fitness_;
}

vector< vector<int> >  GeneticAlgorithm::get_offspring_population() {
  return this->offspring_population_;
}

vector<double> GeneticAlgorithm::get_offspring_fitness() {
  return this->offspring_fitness_;
}

double GeneticAlgorithm::get_best_found_fitness() {
  return this->best_found_fitness_;
}

vector <int> GeneticAlgorithm::get_best_individual() {
  return this->best_individual_;
}

size_t GeneticAlgorithm::get_generation() {
  return this->generation_;
}

//double GeneticAlgorithm::EstimateLinearECDF(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para, shared_ptr<IOHprofiler_suite<int> > suite, vector<double> targets) {
//  assert(suite->IOHprofiler_suite_get_number_of_problems() == targets.size());
//  
//  if (integer_para[0] > integer_para[1] && category_para[3] == "BESTCOMMA") {
//    clog << "mu > lambda with comma selection." << endl;
//    return 0;
//  }
//  
//  if (integer_para[3] >integer_para[1] && category_para[3] == "TOURNAMENTCOMMA") {
//    clog << "tournament size is larger than lambda with comma selection." << endl;
//    return 0;
//  }
//  
//  if (integer_para[3] > integer_para[1] + integer_para[0] && category_para[3] == "TOURNAMENTPLUS" ) {
//    clog << "tournament size is larger than mu + lambda with plus selection." << endl;
//    return 0;
//  }
//  
//  if (continuous_para[0] > 0 && integer_para[0] < 2) {
//    clog << "doing crossover with population size less than 2" << endl;
//    return 0;
//  }
//  
//  this->ecdf_target_width_ = DEFAULT_ECDF_TARGET_LENGTH;
//  this->ecdf_budget_width_ = this->evluation_budget_; /// TODO: Deciding to count every evaluation, or to count evaluations of default length
//  
//  using Logger = IOHprofiler_ecdf_logger<int>;
//  
//  int ti = 0;
//  double result = 0;
//  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
//    /// TODO: UPDATING this in general.
//    IOHprofiler_RangeLinear<double> error(0,targets[ti],this->ecdf_target_width_);
//    if(this->problem_->IOHprofiler_get_problem_id() == 22) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLinear<double> (d - d * (d/2-1) * 4,targets[ti],this->ecdf_target_width_);
//    } else if (this->problem_->IOHprofiler_get_problem_id() == 23) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLinear<double> (5 * d * d - d * d* d * 4,targets[ti],this->ecdf_target_width_);
//    } else if (this->problem_->IOHprofiler_get_problem_id() == 25) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLinear<double> (-1,targets[ti],this->ecdf_target_width_);
//    }
//    IOHprofiler_RangeLinear<size_t> evals(0,this->evluation_budget_,this->ecdf_budget_width_);
//    shared_ptr<Logger> logger(new Logger(error, evals) );
//    this->ecdf_logger_ = logger;
//    this->ecdf_logger_->activate_logger();
//    this->ecdf_logger_->track_suite(*suite);
//    this->run_N(integer_para,continuous_para,category_para);
//    result += this->ecdf_ratio_;
//    ++ti;
//  }
//  
//  return result / (double) ti;
//}
//
//double GeneticAlgorithm::EstimateLogECDF(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para, shared_ptr<IOHprofiler_suite<int> > suite, vector<double> targets) {
//  assert(suite->IOHprofiler_suite_get_number_of_problems() == targets.size());
//  
//  if (integer_para[0] > integer_para[1] && category_para[3] == "BESTCOMMA") {
//    clog << "mu > lambda with comma selection." << endl;
//    return 0;
//  }
//  
//  if (integer_para[3] >integer_para[1] && category_para[3] == "TOURNAMENTCOMMA") {
//    clog << "tournament size is larger than lambda with comma selection." << endl;
//    return 0;
//  }
//  
//  if (integer_para[3] > integer_para[1] + integer_para[0] && category_para[3] == "TOURNAMENTPLUS" ) {
//    clog << "tournament size is larger than mu + lambda with plus selection." << endl;
//    return 0;
//  }
//  
//  if (continuous_para[0] > 0 && integer_para[0] < 2) {
//    clog << "doing crossover with population size less than 2" << endl;
//    return 0;
//  }
//  
//  this->ecdf_target_width_ = DEFAULT_ECDF_TARGET_LENGTH;
//  this->ecdf_budget_width_ = this->evluation_budget_; /// TODO: Deciding to count every evaluation, or to count evaluations of default length
//  using Logger = IOHprofiler_ecdf_logger<int>;
//  
//  int ti = 0;
//  double result = 0;
//  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
//    IOHprofiler_RangeLog<double> error(0,targets[ti],this->ecdf_target_width_);
//    IOHprofiler_RangeLog<size_t> evals(0,this->evluation_budget_,this->ecdf_budget_width_);
//    if(this->problem_->IOHprofiler_get_problem_id() == 22) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLog<double> (d - d * (d/2-1) * 4,targets[ti],this->ecdf_target_width_);
//    } else if (this->problem_->IOHprofiler_get_problem_id() == 23) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLog<double> (5 * d * d - d * d* d * 4,targets[ti],this->ecdf_target_width_);
//    } else if (this->problem_->IOHprofiler_get_problem_id() == 25) {
//      int d = this->problem_->IOHprofiler_get_number_of_variables();
//      error = IOHprofiler_RangeLog<double> (-1,targets[ti],this->ecdf_target_width_);
//    }
//    shared_ptr<Logger> logger(new Logger(error, evals) );
//    this->ecdf_logger_ = logger;
//    this->ecdf_logger_->activate_logger();
//    this->ecdf_logger_->track_suite(*suite);
//    this->run_N(integer_para,continuous_para,category_para);
//    result += this->ecdf_ratio_;
//    ++ti;
//  }
//  return result / (double) ti;
//}
//
//vector<double> GeneticAlgorithm::EstimateERT(const vector<int> &integer_para, const vector<double> &continuous_para, const vector<string> &category_para, shared_ptr<IOHprofiler_suite<int> > suite, const vector<double> &targets, const vector<size_t> &budgets) {
//  assert(suite->IOHprofiler_suite_get_number_of_problems() == targets.size());
//  assert(suite->IOHprofiler_suite_get_number_of_problems() == budgets.size());
//  
//  if (this->hitting_time_.size() != 0) {
//    this->hitting_time_.clear();
//  }
//  
//  this->ERT_flag_ = true;
//  vector <double> ERTs;
//  
//  int ti = 0;
//  while ((this->problem_ = suite->get_next_problem()) != nullptr) {
//    if (integer_para[0] > integer_para[1] && category_para[3] == "BESTCOMMA") {
//      ERTs.push_back(this->budgets[ti] * this->independent_runs_ * (100 + normal_random())); /// TODO: Deciding to return inf for infeasible cases, or to return penaltt values.
//      clog << "mu > lambda with comma selection." << endl;
//      continue;
//    }
//    
//    if (integer_para[3] >integer_para[1] && category_para[3] == "TOURNAMENTCOMMA") {
//      ERTs.push_back(this->budgets[ti] * this->independent_runs_ * (100 + normal_random())); /// TODO: Deciding to return inf for infeasible cases, or to return penaltt values.
//      clog << "tournament size is larger than lambda with comma selection." << endl;
//      continue;
//    }
//    
//    if (integer_para[3] > integer_para[1] + integer_para[0] && category_para[3] == "TOURNAMENTPLUS" ) {
//      ERTs.push_back(this->budgets[ti] * this->independent_runs_ * (100 + normal_random())); /// TODO: Deciding to return inf for infeasible cases, or to return penaltt values.
//      clog << "tournament size is larger than mu + lambda with plus selection." << endl;
//      continue;
//    }
//    
//    if (continuous_para[0] > 0 && integer_para[0] < 2) {
//      clog << "doing crossover with population size less than 2" << endl;
//      continue;
//    }
//    
//    this->hiting_target_ = targets[ti];
//    this->set_evaluation_budget(budgets[ti]);
//    this->run_N(integer_para,continuous_para,category_para);
//    
//    // Calculating ERT
//    double AHT = 0;
//    int s, r;
//    double ERT;
//    r = this->independent_runs_;
//    s = this->hitting_time_.size();
//    for (int i = 0; i != s; ++i) {
//      AHT += this->hitting_time_[i];
//    }
//    
//    if (s != 0) {
//      AHT = AHT / s;
//      ERT = (double)(r - s) / (double) s * (double) budgets[ti] + AHT;
//    } else {
//      ERT = (double) budgets[ti] * r * (100 + normal_random());
//    }
//    
//    ERTs.push_back(ERT);
//    ti++;
//  }
//  return ERTs;
//}

