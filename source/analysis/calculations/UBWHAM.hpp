#include "Calculation.hpp"
#include "../../../extern/LBFGS/LBFGS.h"
#include <vector>
class UBWHAM : public Calculation{
public:
  UBWHAM(const InputPack& input);
  virtual void calculate();
  virtual void output();
  double operator()(const Eigen::VectorXd& x, Eigen::VectorXd& grad){
    get_gradient(x, grad);
    return compute_kappa(x);
  }
protected:
  double calc_ubwham_term();
  double calc_log_term(const Eigen::VectorXd& x, int sim, int point, int k);
  double calc_log_term2(const Eigen::VectorXd& x, int sim, int pt, int r, int k);
  double compute_kappa(const Eigen::VectorXd& x);
  double compute_dkappa(const Eigen::VectorXd& x, int l);
  double calc_theta_uij(int sim, int point, int bias_sim);
  double calc_zk(const Eigen::VectorXd& x, int k_in);
  double calc_z0(const Eigen::VectorXd& x);
  double calc_log_obs_weight(const Eigen::VectorXd& x, int sim, int point);
  double get_gradient(const Eigen::VectorXd& x, Eigen::VectorXd& grad);
  void makeOutputHistogram(const std::vector<double>& obs, const std::vector<double>& weights);
  //x_ is the vector of log(z) values
  //lognvec_ contains the number of datapoints per simulation
  //logn contains total number of datapoints
  Eigen::VectorXd x_, grad_;
  std::vector<int> nvec_, lognvec_;
  double logn_;
  int npoints_, nsims_;
  //lbfgs params
  double eps = 1e-2;
	double iter = 1000; 
  //output control
  std::string column_label_; //can only do up to 2d histogram
  bool doHistogram_;
  std::vector<double> bin_bounds_;
  double bin_size_;
  int nbins_;  

};