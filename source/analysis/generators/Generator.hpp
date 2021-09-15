//A generator's goal is to take in a set of minimal input information and to generate a set of timeseries data paired with the appropriate bias
//A dataset can use a generator to generate and gather its contents
//The generator is also responsible for the destruction of the timeseries data once the code has finished running
#pragma once
#include "../InputPack.hpp"
#include "../../filereaders/filereader.hpp"
#include "../biases/BiasFactory.hpp"
#include <vector>
#include <string>
#include <map>
class Generator{
public:
  //this just loads in basic information regarding the generator
  Generator(const InputPack& input);
  ~Generator(); //needs to delete any timeseries and biases it makes
  virtual bool load(); //loads all of the timeseries and biases and stores them internally
  std::vector<std::string> getFilepaths(){
    return filepaths_;
  }
  std::vector<std::vector<Bias*> > getBiases(){
    return biases_;
  }
  std::vector<Timeseries*> getTimeseries(){
    return timeseries_;
  }
  std::string getName(){
    return name_;
  }
  std::string getType(){
    return type_;
  } 
protected:
  //all timeseries data just requires a filepath and a filereader type, initially going to code this for just one file reader
  std::vector<std::string> filepaths_;
  std::string bias_template_name_;
  bool isBiased;
  int nsubs_; //number of bias parameters that will be substituted
  std::vector<std::string> subcols_; //a list of parameter names to be substituted
  std::vector< std::vector<double> > subvals_; //a list of parameters for each column to be substituted
  //biases will need their own input packs, which are most conveniently constructed dynamically
  std::vector<InputPack> bias_input_packs_;
  Bias* bias_template_;
  std::vector<std::vector<Bias*> > biases_;
  std::vector<Timeseries*> timeseries_;
  std::string filereader_type_;
  FileReader* fr_;
  std::string type_;
  std::string name_;
};