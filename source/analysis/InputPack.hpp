#pragma once
#include <map>
#include "../tools/InputParser.hpp"
#include "../tools/Stacktrace.hpp"
class Bias;
class Generator;
class Dataset;
class Calculation;

//input packs contain a parameterpack and pointer to the registries that different calculations might need to function
class InputPack{
public:
  InputPack(){
    return;
  }
  InputPack(const ParameterPack* params){
    params_ = params;
    initializeRegistries();
    return;
  }
  ~InputPack(){
    if(is_master_pack){ //only a pack that initialized the registries should delete them
      for(auto it = bias_registry_->begin(); it!=bias_registry_->end(); it++) {
        it = bias_registry_->erase(it);
      }
      for(auto it = calc_registry_->begin(); it!=calc_registry_->end(); it++) {
       it = calc_registry_->erase(it);
      }
      for(auto it = dataset_registry_->begin(); it!=dataset_registry_->end(); it++) {
       it = dataset_registry_->erase(it);
      }
      delete bias_registry_;
      delete calc_registry_;
      delete dataset_registry_;
    }
  }
  Bias* findBias(std::string name) const{
    auto it = bias_registry_->find(name);
    if(it != bias_registry_->end()) return it->second;
    return 0; //return nullptr if search fails
  }
  Generator* findGenerator(std::string name) const{
    auto it = generator_registry_->find(name);
    if(it != generator_registry_->end()) return it->second;
    return 0; //return nullptr if search fails
  }
  Calculation* findCalculation(std::string name) const{
    auto it = calc_registry_->find(name);
    if(it != calc_registry_->end()) return it->second;
    return 0; //return nullptr if search fails
  }
  Dataset* findDataset(std::string name) const{
    auto it = dataset_registry_->find(name);
    if(it != dataset_registry_->end()) return it->second;
    return 0; //return nullptr if search fails
  }
  void addBias(std::string name, Bias* bias){
    if(bias_registry_ != 0) bias_registry_->insert(std::pair<std::string, Bias*>{name, bias});
    return;
  }  
  void addGenerator(std::string name, Generator* generator){
    if(generator_registry_ != 0) generator_registry_->insert(std::pair<std::string, Generator*>{name, generator});
    return;
  }  
  void addCalculation(std::string name, Calculation* calc){
    if(calc_registry_ != 0) calc_registry_->insert(std::pair<std::string, Calculation*>{name, calc});
    return;
  }
  void addDataset(std::string name, Dataset* dataset){
    if(dataset_registry_ != 0) dataset_registry_->insert(std::pair<std::string, Dataset*>{name, dataset});
    return;
  }
  void initializeRegistries(){ //use this for master pack
    bias_registry_ = new std::map<std::string, Bias*>;
    generator_registry_ = new std::map<std::string, Generator*>;
    calc_registry_ = new std::map<std::string, Calculation*>;
    dataset_registry_ = new std::map<std::string, Dataset*>; 
  }

  void setCalculationRegistry(std::map<std::string, Calculation*>* calc_reg){
    calc_registry_ = calc_reg;
    return;
  }
  void setBiasRegistry(std::map<std::string, Bias*>* bias_reg){
    bias_registry_ = bias_reg;
    return;
  }
  void setDatasetRegistry(std::map<std::string, Dataset*>* dataset_reg){
    dataset_registry_ = dataset_reg;
    return;
  }
  void setGeneratorRegistry(std::map<std::string, Generator*>* generator_reg){
    generator_registry_ = generator_reg;
    return;
  }
  const ParameterPack& params() const {
    return *params_; 
  }
  void setParams(const ParameterPack* param){
    params_ = param;
  }
  const std::map<std::string, Bias*>& BiasMap() const {
    return *bias_registry_; 
  }
  const std::map<std::string, Generator*>& GeneratorMap() const {
    return *generator_registry_; 
  }
  const std::map<std::string, Calculation*>& CalculationMap() const {
    return *calc_registry_; 
  }  
  const std::map<std::string, Dataset*>& DatasetMap() const {
    return *dataset_registry_; 
  }   
  std::vector<InputPack> buildDerivedInputPacks(std::string key){
    std::vector<InputPack> inputpacks;
    auto parameterpacks = params().findParameterPacks(key, ParameterPack::KeyType::Optional);
    inputpacks.resize(parameterpacks.size());
    for(std::size_t i = 0; i < parameterpacks.size(); i++){
      inputpacks[i].setCalculationRegistry(calc_registry_);
      inputpacks[i].setGeneratorRegistry(generator_registry_);
      inputpacks[i].setBiasRegistry(bias_registry_);
      inputpacks[i].setDatasetRegistry(dataset_registry_);
      inputpacks[i].setParams(parameterpacks[i]);
    }
    return inputpacks;
  }


private:
  const ParameterPack* params_;
  std::map<std::string, Bias*>* bias_registry_ = 0; //contains pointer to the true map
  std::map<std::string, Calculation*>* calc_registry_ = 0; //contains pointer to the true map
  std::map<std::string, Dataset*>* dataset_registry_ = 0;
  std::map<std::string, Generator*>* generator_registry_ = 0;
  bool is_master_pack = 0;
};