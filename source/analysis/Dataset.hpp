//the dataset object contains the collective timeseries information for a set of simulations
//each simulation can have one or more biases, timeseries, etc.
#pragma once
#include "../filereaders/timeseries.hpp"
#include "biases/Bias.hpp"
#include "generators/Generator.hpp"
#include "InputPack.hpp"

class Dataset{
public:
  template <class T>
  using Vec = std::vector<T>;
  template <class T, class V>
  using Map = std::map<T, V>;
  Dataset(const InputPack& inputpack);
  const Timeseries* getTimeseriesViaIndex(int i) const{
    return ts_[i];
  }
  int getNumTimeseries() const{
    return ts_.size();
  }
  const std::vector<Bias*> getBiasesViaIndex(int i) const{
    return biases_[i];
  }
private:
  //a dataset contains pointers to all timeseries data
  std::vector<Timeseries*> ts_;
  std::vector<std::vector<Bias*> > biases_; //each simulation may have some biases associated with it that the dataset would be aware of 
  Generator* generator_;
  std::string name_, generator_name_;
  
};
