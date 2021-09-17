#include "Dataset.hpp"
Dataset::Dataset(const InputPack& input){
  input.params().readString("name", ParameterPack::KeyType::Required, name_);
  input.params().readString("generator", ParameterPack::KeyType::Required, generator_name_);
  std::vector<double> trange;
  input.params().readVector("trange", ParameterPack::KeyType::Optional, trange);
  FANCY_ASSERT(trange.size() == 2, "Invalid time range specified");
  trange_[0] = trange[0];
  trange_[1] = trange[1];
  generator_ = input.findGenerator(generator_name_);
  generator_->load();
  ts_ = generator_->getTimeseriesCopy();
  for(int i = 0; i < ts_.size(); i++){
    auto time = ts_[i].getTime();
    for(int j = 0; j < ts_[i].getTime().size(); j++){
      if(ts_[i].getTime()[j] < trange_[0] || ts_[i].getTime()[j] > trange_[1]){
        ts_[i].deleteIndex(j);
        j--;
      }
    }
  }
  biases_ = generator_->getBiases();
  return;
}

Dataset::Dataset(const Dataset& d_in, int num_blocks){ //construct a block-bootstrap dataset from an existing dataset
  
}