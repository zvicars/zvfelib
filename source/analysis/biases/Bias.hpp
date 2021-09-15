#pragma once
#include <string>
#include "../InputPack.hpp"
#include "../../tools/Assert.hpp"
//a simulation can have one or multiple biases
//each bias can depend on one or multiple parameters
//these parameters must be appropriately fed each time to the bias, each parameter
//should be available in the respective timeseries and the bias can remind the probe
//volume which columns to send
class Bias{
public:
  template <class T>
  using Vec = std::vector<T>;
  Bias(const InputPack& input){
    input.params().readString("name", ParameterPack::KeyType::Required, name_);
    input.params().readString("type", ParameterPack::KeyType::Required, type_);
    input.params().readVector("columns", ParameterPack::KeyType::Required, columns_);
    return;
  }
  virtual double calculate(const Vec<double>& input){
    return 0;
  }
  virtual std::string output(){
    return "";
  }
  virtual bool hasParameter(std::string name) const{
    return 0;
  }
  virtual double getParameter(std::string name) const{
    return 0.0;
  };
  virtual bool setParameter(std::string name, double value){
    return 0;
  }
  const Vec<int>& getInputColumns() const {
    return columns_;
  }
  int getNumCols() const {
    return columns_.size();
  }
  virtual Bias* clone() const = 0;
protected:
  std::string name_, type_;
  Vec<int> columns_;
};