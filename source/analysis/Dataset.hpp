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
    return &ts_[i];
  }
  int getNumTimeseries() const{
    return ts_.size();
  }
  const std::vector<Bias*> getBiasesViaIndex(int i) const{
    return biases_[i];
  }
private:
  //a dataset contains copies of all ts data, as time-range restrictions are applied here
  std::vector<Timeseries> ts_;
  std::vector<std::vector<Bias*> > biases_; //each simulation may have some biases associated with it that the dataset would be aware of 
  Generator* generator_;
  std::string name_, generator_name_;
  std::array<double, 2> trange_;
};

Dataset createBlockDataset(const Dataset& ds, int nblocks){
  int nts = ds.getNumTimeseries();
  std::vector<int> nsamples(nts, 0);
  std::vector<int> approx_samples_per_block(nts, 0);
  for(int i = 0; i < nts; i++){
    auto ts_temp = ds.getTimeseriesViaIndex(i);
    int approx_samples_per_block = ts_temp->getTime().size();

  }
  std::vector<Timeseries*> ts_data(nts, 0);

} 