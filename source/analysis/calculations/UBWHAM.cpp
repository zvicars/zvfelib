#include "UBWHAM.hpp"
#include "make_histogram.hpp"
UBWHAM::UBWHAM(const InputPack& input) : Calculation{input} {
  input.params().readNumber("epsilon", ParameterPack::KeyType::Optional, eps);
  input.params().readNumber("max_iterations", ParameterPack::KeyType::Optional, iter);
  doHistogram_ = 0; //will otherwise just output per-bin weights
  input.params().readFlag("histogram", ParameterPack::KeyType::Optional, doHistogram_);
  if(doHistogram_){
    input.params().readString("column", ParameterPack::KeyType::Optional, column_label_);
    input.params().readVector("bin_range", ParameterPack::KeyType::Required, bin_bounds_);
    FANCY_ASSERT(bin_bounds_.size() == 2, "Improper range specified for bins, need a min and max bin");
    FANCY_ASSERT(bin_bounds_[1] - bin_bounds_[0] > 0, "Either min and max bin are the same or you have them in the improper order."); 
    bool bin_size_flag = input.params().readNumber("bin_size", ParameterPack::KeyType::Optional, bin_size_);
    if(!bin_size_flag){
      nbins_ = 50;
      bin_size_ = bin_bounds_[1] - bin_bounds_[0] / (nbins_ - 1);
    }
    else{
      nbins_ = 1 + ceil((bin_bounds_[1] - bin_bounds_[0])/bin_size_);
      bin_bounds_[1] = bin_bounds_[0] + nbins_*bin_size_; //make sure the max bin corresponds to the actual maximum bin
    }
  }
  nsims_ = dataset_->getNumTimeseries();
  lognvec_.resize(nsims_);
  nvec_.resize(nsims_);
  npoints_ = 0;
  for(int i = 0; i < nsims_; i++){
    auto ts = dataset_->getTimeseriesViaIndex(i);
    auto time_vec = ts[i].getTime();
    npoints_ += time_vec.size();
    nvec_[i] = time_vec.size();
    lognvec_[i] = log(time_vec.size());
  }
  logn_ = log(npoints_);

  x_ = Eigen::VectorXd::Ones(nsims_);
  grad_ = Eigen::VectorXd::Zero(nsims_);
  return;
}
double LSE(std::vector<double> arr) {
		int count = arr.size();
		if(count > 0 )
		{
			double maxVal = arr[0];
			double sum = 0;
			for (int i = 1 ; i < count ; i++)
			{
				if (arr[i] > maxVal)
				{
					maxVal = arr[i];
				}
			}
			if(std::isinf(maxVal)) maxVal = 0;
			for (int i = 0; i < count ; i++)
			{
				if(std::isinf(arr[i])) continue; 
				sum += exp(arr[i] - maxVal);
			}
		  return log(sum) + maxVal;
		}
		return 0.0;
}

double UBWHAM::calc_theta_uij(int sim, int pt, int bias_sim){
  double thetauij = 0.0;
  auto timeseries = dataset_->getTimeseriesViaIndex(sim);
  auto bias = dataset_->getBiasesViaIndex(sim);
  for(int i = 0; i < bias.size(); i++){
    auto input_columns = bias[i]->getInputColumns();
    std::vector<double> params;
    for(int j = 0; j < params.size(); j++){
      params.push_back(timeseries->getDatapointViaIndex(input_columns[j], pt));
    }
    thetauij += bias[i]->calculate(params);
  }
  return thetauij;
}

double UBWHAM::calc_log_term(const Eigen::VectorXd& x, int sim, int pt, int r){
  return lognvec_[r] - logn_ - x[r] - calc_theta_uij(sim, pt, r);
}

double UBWHAM::compute_kappa(const Eigen::VectorXd& x){
  std::vector<double> log_terms(nsims_);
  double sum = 0.0;
  for(int i = 0; i < nsims_; i++){
    for(int j = 0; j < nvec_[i]; j++){
      for(int k = 0; k < nsims_; k++){
        log_terms[k] = calc_log_term(x, i, j, k);
      }
      sum += LSE(log_terms);
    }
  }
  double sum2 = 0.0;
  for(int i = 0; i < nsims_; i++){
    sum2 += x[i]*(double)lognvec_[i]/(double)logn_;
  }
  return (sum/(double)npoints_) + sum2;
}

double UBWHAM::compute_dkappa(const Eigen::VectorXd& x, int l){
  if(l = 0) return 0.0; //first point fixed
  std::vector<double> big_lse(npoints_);
  std::vector<double> little_lse(nsims_);
  int counter = 0;
  for(int i = 0; i < nsims_; i++){
    for(int j = 0; j < nvec_[i]; j++){
      double term1 = calc_log_term(x, i, j, l);
      for(int r = 0; r < nsims_; r++){
        little_lse[r] = calc_log_term(x,i,j,r);
      }
      term1 -= LSE(little_lse);
      big_lse[counter] = term1;
      counter++; //keeps track of biglse index
    }
  }
  double eval = LSE(big_lse);
  eval = (-1.0/(double)npoints_) * exp(eval) + ( x[l] * (double)nvec_[l]/(double)npoints_ );
  return eval;
}
double UBWHAM::get_gradient(const Eigen::VectorXd& x, Eigen::VectorXd& grad)
{
	double sum = 0.0;
	grad(0) = 0.0;
	#pragma omp parallel 
	{
		#pragma omp for
		for(unsigned int i = 1; i < grad.size(); i++)
		{
			grad(i) = compute_dkappa(x, i);
			#pragma omp atomic update
			sum += fabs(grad(i));
		}
	}
	return sum;
}

void UBWHAM::calculate(){
  LBFGSpp::LBFGSParam<double> param_;
  LBFGSpp::LBFGSSolver<double>* solver_;  
  param_.epsilon = eps;
  param_.max_iterations = iter;
  solver_ = new LBFGSpp::LBFGSSolver<double>(param_);
  Eigen::VectorXd x = Eigen::VectorXd::Ones(nsims_);
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(nsims_);
  double fx;
	int niter = 0;
  try{niter = solver_->minimize(*this, x, fx);}
	catch(int e){};
    std::cout << niter << " iterations" << std::endl;
    std::cout << "x = \n" << x.transpose() << std::endl;
    std::cout << "f(x) = " << fx << std::endl;  

	x_ = x;
  delete solver_;
	return;
}

double UBWHAM::calc_log_obs_weight(const Eigen::VectorXd& x, int sim, int point){
  std::vector<double> small_lse(nsims_);
  for(int r = 0; r < nsims_; r++){
    small_lse[r] = lognvec_[r] - x[r] - calc_theta_uij(sim, point, r);
  }
  return -LSE(small_lse);
}
//only true for 1d histograms
void UBWHAM::makeOutputHistogram(const std::vector<double>& obs, const std::vector<double>& weights){
  std::vector<double> x_data;
  std::vector<double> y_data;
  makeHistogramWeighted(obs, weights, bin_bounds_[0], bin_bounds_[1], bin_size_, 1, 1, 1, x_data, y_data);
	std::ofstream results(base_ + "_ubwhamhist.txt");
	results << "#WHAM RESULTS" << std::endl;
	results << "#g vector" << std::endl;
	results << "#";
	for(int i = 0; i < x_.size(); i++){
		results << x_(i) << "   ";
  }
  for(int i = 0; i < x_data.size(); i++){
    results << x_data[i] << "     " << y_data[i] << "\n";
  }
	results.close();
}

void UBWHAM::output(){
  std::vector<double> all_observations(npoints_);
  std::vector<double> weight(npoints_);
  std::vector<double> logweight(npoints_);
  int counter = 0;
  for(int i = 0; i < nsims_; i++){
    auto ts = dataset_->getTimeseriesViaIndex(i);
    auto data = ts->getDataViaLabel(column_label_);
    for(int j = 0; j < data.size(); j++){
      all_observations[counter] = data[j];
      logweight[counter] = calc_log_obs_weight(x_, i, j);
      weight[counter] = exp(logweight[counter]);
      counter++;
    }
  }
  if(doHistogram_){
    makeOutputHistogram(all_observations, weight);
  }
  std::ofstream output_(base_ + "_ubwhamts.txt");
  for(int i = 0; i < all_observations.size(); i++){
    output_ << all_observations[i] << "     " << weight[i] << "\n";
  }
  output_.close();
	return;
}