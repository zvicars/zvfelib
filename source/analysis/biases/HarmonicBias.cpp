#include "HarmonicBias.hpp"
#include <sstream>
HarmonicBias::HarmonicBias(const InputPack& input):Bias{input}{
  input.params().readNumber("k", ParameterPack::KeyType::Required, k_);
  input.params().readNumber("xstar", ParameterPack::KeyType::Required, x_star_);
  mu_ = 0.0;
  input.params().readNumber("mu", ParameterPack::KeyType::Optional, mu_);
  FANCY_ASSERT(columns_.size() == 1, "Invalid number of input columns selected for harmonic bias, expected 1.");
  return;
}
double HarmonicBias::calculate(const Vec<double>& input){
  return 0.5*k_*(input[0] - x_star_)*(input[0] - x_star_) + mu_*input[0];
}  
std::string HarmonicBias::output(){
  std::stringstream ss;
  ss << ";k= " << k_ << "\nxstar= " << x_star_ << "\nmu= " << mu_ << "\n";
  return ss.str();
}
bool HarmonicBias::hasParameter(std::string name) const{
  if(name == "xstar") return 1; 
  if(name == "k") return 1;
  if(name == "mu") return 1;
  return 0;
}
double HarmonicBias::getParameter(std::string name) const{
  if(name == "xstar") return x_star_; 
  if(name == "k") return k_;
  if(name == "mu") return mu_;
  return 0;
}
bool HarmonicBias::setParameter(std::string name, double value){
  if(name == "xstar"){
    x_star_ = value;
    return 0;
  }
  if(name == "k"){
    k_ = value;
    return 0;
  }
  if(name == "mu"){
    mu_ = value;
    return 0;
  }
  return 1;
}
Bias* HarmonicBias::clone() const{
  return new HarmonicBias(*this);
}