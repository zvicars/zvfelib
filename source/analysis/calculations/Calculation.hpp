//base class for calculations, the premise is that it accepts a dataset object and computes/outputs information from it
//binned wham, unbinned wham, sparse sampling, etc. each calculation produces its own set of output files
#pragma once
#include "../InputPack.hpp"
#include "../Dataset.hpp"
class Calculation{
public:
  Calculation(const InputPack& input);
  virtual void calculate() = 0;
  virtual void output() = 0;
protected:
  const Dataset* dataset_;
  //base is the root word for the output files
  std::string name_, type_, base_, dataset_name_;
};