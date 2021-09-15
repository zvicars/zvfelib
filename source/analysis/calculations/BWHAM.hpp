#pragma once
#include "Calculation.hpp"
#include "../../../extern/LBFGS/LBFGS.h"
#include <vector>
class BWHAM: public Calculation
{
protected:
  bool bin_range_flag_, bin_size_flag_;
  std::vector<double> bin_bounds_;
  double bin_size_;
  int nbins_;
  int nwindows_;
  std::string column_label_;
	double EPSILON = 1e-5;
	double MAX_ITER = 1000;
	Eigen::MatrixXd lognij;
	Eigen::MatrixXd logcij;
	Eigen::MatrixXd ubmat;
	Eigen::VectorXd logm;
	Eigen::VectorXd logn;
  Eigen::VectorXd x_; 
  LBFGSpp::LBFGSParam<double> param_;
  LBFGSpp::LBFGSSolver<double>* solver_;

	double compute_denominator(const Eigen::MatrixXd& x, int j);
	double compute_msum_der(const Eigen::MatrixXd& x, int idx);
	double compute_nsum(const Eigen::MatrixXd& x);
	double compute_a_der(const Eigen::MatrixXd& x, int i);
	double compute_a(const Eigen::MatrixXd& x);
	Eigen::VectorXd p_of_bin(const Eigen::VectorXd& x);
	double get_entropy(int idx, const Eigen::VectorXd& x, const Eigen::VectorXd& f_consensus);
	double compute_objective_function(const Eigen::VectorXd& x);
	double get_gradient(const Eigen::VectorXd& x, Eigen::VectorXd& grad);
  void makeHistogramsFromDataset();
public:
  double operator()(const Eigen::VectorXd& x, Eigen::VectorXd& grad){
    get_gradient(x, grad);
    return compute_objective_function(x);
  }
  virtual void calculate();
  virtual void output();
	BWHAM(const InputPack& input);
  ~BWHAM(){
    delete solver_;
    return;
  }
};