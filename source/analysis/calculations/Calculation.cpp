#include "Calculation.hpp"
Calculation::Calculation(const InputPack& input){
  input.params().readString("name", ParameterPack::KeyType::Required, name_);
  input.params().readString("type", ParameterPack::KeyType::Required, type_);
  input.params().readString("base", ParameterPack::KeyType::Required, base_);
  input.params().readString("dataset", ParameterPack::KeyType::Required, dataset_name_);
  dataset_ = input.findDataset(dataset_name_);
  return;
}