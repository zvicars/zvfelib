#pragma once
#include "Bias.hpp"
class HarmonicBias : public Bias{
public:
  HarmonicBias(const InputPack& input);
  virtual double calculate(const Vec<double>& input);
  virtual std::string output();
  virtual bool hasParameter(std::string name) const;
  virtual double getParameter(std::string name) const;
  virtual bool setParameter(std::string name, double value);
  virtual Bias* clone() const;
private:
  double k_, x_star_, mu_; 
};