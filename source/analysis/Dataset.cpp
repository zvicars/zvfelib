#include "Dataset.hpp"
Dataset::Dataset(const InputPack& input){
  input.params().readString("name", ParameterPack::KeyType::Required, name_);
  input.params().readString("generator", ParameterPack::KeyType::Required, generator_name_);
  generator_ = input.findGenerator(generator_name_);
  generator_->load();
  ts_ = generator_->getTimeseries();
  biases_ = generator_->getBiases();
  return;
}