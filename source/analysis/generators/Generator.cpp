#include "Generator.hpp"
Generator::Generator(const InputPack& input){
  input.params().readString("name", ParameterPack::KeyType::Required, name_);
  input.params().readString("type", ParameterPack::KeyType::Required, type_);

  input.params().readString("filereader", ParameterPack::KeyType::Required, filereader_type_);
  //need to set up the specified file reader so I know how to parse the timeseries
  fr_ = createFileReader(filereader_type_);

  //biases will be constructed from substituting into a bias template
  bias_template_ = 0;
  bias_template_name_ = "none";
  input.params().readString("bias_template", ParameterPack::KeyType::Optional, bias_template_name_);
  if(bias_template_name_ != "none"){
    bias_template_ = input.findBias(bias_template_name_);
    isBiased = 1;
  }
  else{
    isBiased = 0;
  }
  input.params().readVector("subcols", ParameterPack::KeyType::Optional, subcols_);
  nsubs_ = subcols_.size();
  subvals_.resize(nsubs_);
  for(int i = 0; i < nsubs_; i++){
    std::string loop_param = "param" + std::to_string(i); 
    input.params().readVector(loop_param, ParameterPack::KeyType::Required, subvals_[i]);
    FANCY_ASSERT(bias_template_->hasParameter(subcols_[i]), "Bias template does not have the specified parameter " + subcols_[i] + " available to change");
  }
  int nbiases_ = 0;
  if(nsubs_ != 0) nbiases_ = subvals_[0].size();
  for(int i = 0; i < nsubs_; i++){
    FANCY_ASSERT(subvals_[i].size() == nbiases_, "Too many/few subsitutions specified for parameter " + std::to_string(i) + ".");
  }
  //each simulation will be given a single bias of the template type that will be copied and have a particular parameter changed
  biases_.resize(nbiases_, std::vector<Bias*>(1, 0));
  for(int i = 0; i < biases_.size(); i++){
    Bias* b2 = bias_template_->clone();
    for(int j = 0; j < subcols_.size(); j++){
      b2->setParameter(subcols_[j], subvals_[j][i]);
    }
    biases_[i][0] = b2;
  }
  return;
}
//deletes bias and timeseries objects it generates
Generator::~Generator(){
  for(int i = 0; i < biases_.size(); i++){
    for(int j = 0; j < biases_[i].size(); j++){
      delete biases_[i][j];
    }
  }
  for(int i = 0; i < timeseries_.size(); i++){
    delete timeseries_[i];
  }
  return;
}

bool Generator::load(){
  timeseries_.resize(filepaths_.size());
  for(int i = 0; i < filepaths_.size(); i++){
    fr_->load(filepaths_[i]);
    Timeseries* t1 = new Timeseries(fr_->getTimeSeries());
    timeseries_[i] = t1;
  }
  return 0;
}