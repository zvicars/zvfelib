#include "BWHAM.hpp"
#include <cmath>
#include "make_histogram.hpp"
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
static inline double LSE(std::vector<double> arr) 
{
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
static inline double LSNE(std::vector<double> arr) 
{
		int count = arr.size();
		if(count > 0 )
		{
			double maxVal = -arr[0];
			double sum = 0;

			for (int i = 1 ; i < count ; i++)
			{
				if (-arr[i] > maxVal)
				{
					maxVal = -arr[i];
				}
			}
			if(std::isinf(maxVal)) maxVal = 0;

			for (int i = 0; i < count ; i++)
			{
				if(std::isinf(arr[i])) continue; 
				sum += exp(-arr[i] - maxVal);
			}
		  return fmax(log(sum) + maxVal, -1000);
		}
		  return 0.0;
}


void BWHAM::makeHistogramsFromDataset(){
	std::vector<std::vector<double> > Ni;
	std::vector<std::vector<double> > bias;;
  for(int i = 0; i < dataset_->getNumTimeseries(); i++){
    const Timeseries* ts = dataset_->getTimeseriesViaIndex(i);
    auto ts_vec = ts->getDataViaLabel(column_label_);
    std::vector<double> histx, histyconv, biascol;
    std::vector<int> histy;
    makeHistogram(ts_vec, bin_bounds_[0], bin_bounds_[1], bin_size_, 1, 1, 1, histx, histy);
    histyconv.resize(histy.size());
    biascol.resize(histx.size(), 0.0);
    auto biases = dataset_->getBiasesViaIndex(i);
    for(int j = 0; j < histx.size(); j++){
      histyconv[j] = log(std::max((double)histy[j], 0.1));
      double bias_sum = 0.0;
      for(int k = 0; k < biases.size(); k++){
        bias_sum += biases[k]->calculate(std::vector<double>(1, histx[j] + (0.5*bin_size_)));
      }
      biascol[j] = bias_sum;
    }
    Ni.push_back(histyconv);
    bias.push_back(biascol);
  }
	lognij.resize(Ni.size(), Ni[0].size());
	logcij.resize(Ni.size(), Ni[0].size());
	ubmat.resize(Ni.size(), Ni[0].size());
	logm.resize(bias[0].size());
	logn.resize(Ni.size());
	for(int i = 0; i < Ni.size(); i++)
	for(int j = 0; j < Ni[0].size(); j++)
	{
		lognij(i,j) = Ni[i][j];
		logcij(i,j) = -bias[i][j];
		ubmat(i,j) = -Ni[i][j] - bias[i][j];
	}
	for(unsigned int i = 0; i < bias.size(); i++)
	{
		logn(i) = LSE(Ni[i]);
	}		
	for(unsigned int i = 0; i < bias[0].size(); i++)
	{
		std::vector<double> cvec;
		for(unsigned int j = 0; j < bias.size(); j++)
		{
			cvec.push_back(Ni[j][i]);
		}
		logm(i) = LSE(cvec);
	}
  return;
}

BWHAM::BWHAM(const InputPack& input) : Calculation{input}
{
  //need to build a histogram from the dataset
  bin_range_flag_ = input.params().readVector("bin_range", ParameterPack::KeyType::Required, bin_bounds_);
  FANCY_ASSERT(bin_bounds_.size() == 2, "Improper range specified for bins, need a min and max bin");
  FANCY_ASSERT(bin_bounds_[1] - bin_bounds_[0] > 0, "Either min and max bin are the same or you have them in the improper order."); 
  bin_size_flag_ = input.params().readNumber("bin_size", ParameterPack::KeyType::Optional, bin_size_);
  input.params().readNumber("epsilon", ParameterPack::KeyType::Optional, EPSILON);
  input.params().readNumber("max_iterations", ParameterPack::KeyType::Optional, MAX_ITER);
  input.params().readString("column", ParameterPack::KeyType::Optional, column_label_);
  //default to 50 bins, with a bin spacing of range / n-1
  if(!bin_size_flag_){
    nbins_ = 50;
    bin_size_ = bin_bounds_[1] - bin_bounds_[0] / (nbins_ - 1);
  }
  else{
    nbins_ = 1 + ceil((bin_bounds_[1] - bin_bounds_[0])/bin_size_);
    bin_bounds_[1] = bin_bounds_[0] + nbins_*bin_size_; //make sure the max bin corresponds to the actual maximum bin
  }
  makeHistogramsFromDataset();
  return;
}

double BWHAM::compute_denominator(const Eigen::MatrixXd& x, int j)
{
	double sum = 0;
	std::vector<double> vals;
	for(unsigned int i = 0; i < logn.size(); i++){
		vals.push_back(logn(i) + logcij(i,j) + x(i));
	}
	sum = LSE(vals);
	return sum;
}
double BWHAM::compute_msum_der(const Eigen::MatrixXd& x, int idx)
{
	double sum = 0;
	std::vector<double> sumvec;
	for(unsigned int i = 0; i < logm.size(); i++){
		sumvec.push_back( logm(i) + logcij(idx,i) - compute_denominator(x,i) );
	}
	return LSE(sumvec);
}	
double BWHAM::compute_nsum(const Eigen::MatrixXd& x)
{
	std::vector<double> cvec(logn.size(), 0);
	for(unsigned int i = 0; i < logn.size(); i++){
		cvec[i] = logn(i) + log(x(i));
	}
	return -LSE(cvec);
}
double BWHAM::compute_a_der(const Eigen::MatrixXd& x, int i)
{
	return  exp(x(i) + compute_msum_der(x, i) + logn(i)) - exp(logn(i));
}
double BWHAM::compute_a(const Eigen::MatrixXd& x)
{
	double sum1 = 0.0;
	double sum2 = 0.0;

	#pragma omp parallel 
	{
		#pragma omp for
		for(int i = 0; i < x.size(); i++){
			#pragma omp atomic update
			sum1 -= exp(logn(i))*x(i);
		}
		#pragma omp for
		for(int l = 0; l < logm.size(); l++){
			#pragma omp atomic update
			if(std::isinf(logm(l)) || std::isnan(logm(l)) ) continue;
			sum2 -= exp(logm(l))*(logm(l) - compute_denominator(x, l));
		}
	}
	return sum1 + sum2;	
}
Eigen::VectorXd BWHAM::p_of_bin(const Eigen::VectorXd& x)
{
	Eigen::VectorXd fe(logm.size());
	for(unsigned int i = 0; i < logm.size(); i++){
		fe(i) = logm(i) - compute_denominator(x, i);
	}
	return fe;
}
double BWHAM::compute_objective_function(const Eigen::VectorXd& x)
{
	return compute_a(x);
}
double BWHAM::get_gradient(const Eigen::VectorXd& x, Eigen::VectorXd& grad)
{
	double sum = 0.0;
	grad(0) = 0.0;
	#pragma omp parallel 
	{
		#pragma omp for
		for(unsigned int i = 1; i < grad.size(); i++)
		{
			grad(i) = compute_a_der(x, i);
			#pragma omp atomic update
			sum += fabs(grad(i));
		}
	}
	return sum;
}

double BWHAM::get_entropy(int idx, const Eigen::VectorXd& x, const Eigen::VectorXd& f_consensus) //f_consensus should already contain info
{
	float weight_sum = 0.0;
	Eigen::VectorXd fc2 = f_consensus;
	Eigen::VectorXd pc = f_consensus;
	Eigen::VectorXd pw = f_consensus;
	
	for(int i = 0; i < f_consensus.size(); i++){
		fc2(i) = f_consensus(i) - logcij(idx,i) - x(idx);
	}
	double sum = 0.0;
	for(int i = 0; i < f_consensus.size(); i++){
		if(std::isinf(fc2(i))) continue;
		sum += exp(-fc2(i)); 
	}
	for(int i = 0; i < f_consensus.size(); i++){
		if(std::isinf(fc2(i))) continue;
		pc(i) = -fc2(i) - log(sum);
		pw(i) = lognij(idx, i) - logn(idx);
	}	
	double eval = 0.0;
	for(int i = 0; i < f_consensus.size(); i++){
		if(std::isinf( pw(i) ) || std::isnan( pw(i) ) ) continue;
		eval += ( exp(pw(i)) ) * ( pw(i) - pc(i) );
	}
	return eval;
}
void BWHAM::calculate(){
  nwindows_ = dataset_->getNumTimeseries();
  param_.epsilon = EPSILON;
  param_.max_iterations = MAX_ITER;
  solver_ = new LBFGSpp::LBFGSSolver<double>(param_);
  Eigen::VectorXd x = Eigen::VectorXd::Ones(nwindows_);
  Eigen::VectorXd grad = Eigen::VectorXd::Zero(nwindows_);
  double fx;
	int niter = 0;
  try{niter = solver_->minimize(*this, x, fx);}
	catch(int e){};
    std::cout << niter << " iterations" << std::endl;
    std::cout << "x = \n" << x.transpose() << std::endl;
    std::cout << "f(x) = " << fx << std::endl;  

	x_ = x;
	return;
}
void BWHAM::output()
{
	Eigen::VectorXd fep = p_of_bin(x_);
  fep = fep*(-1);
	std::ofstream results(base_ + "_binnedwham.txt");
	
	results << "#WHAM RESULTS" << std::endl;
	results << "%g vector" << std::endl;
	results << "#";
	for(int i = 0; i < x_.size(); i++){
		results << x_(i) << "   ";
	}
	
	results << std::endl;
	results << "#KL Divergences" << std::endl;
	results << "#";
	for(int i = 0; i < x_.size(); i++){
		results << get_entropy(i, x_, fep) << "   ";
	}
	results << std::endl;
	
	for(int i = 0; i < fep.size(); i++)
	{
		results << bin_bounds_[0] + bin_size_*i << "   " << fep(i) << "   ";
		for(int j = 0; j < x_.size(); j++)
		{
			results << -lognij(j, i) + logcij(j,i) + x_(j) << "   ";
		}
		results << "\n";
	}
	results.close();
}