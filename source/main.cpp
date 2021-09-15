#include "tools/Assert.hpp"
#include "tools/InputParser.hpp"
#include "analysis/Dataset.cpp"
#include "analysis/biases/BiasFactory.hpp"
#include "analysis/calculations/CalculationFactory.hpp"
#include "analysis/generators/GeneratorFactory.hpp"
int main(int argc, char **argv)
{
  FANCY_ASSERT(argc == 2, "Analysis code only accepts a single input that specifies the op input file.");
  std::string op_input_file_ = argv[1];
  InputParser input_parser;
  ParameterPack master_pack = input_parser.parseFile(op_input_file_);
  using KeyType = ParameterPack::KeyType;
  InputPack master_input_pack = InputPack(&master_pack);
  std::vector<InputPack> bias_packs = master_input_pack.buildDerivedInputPacks("Bias");
  for(std::size_t i = 0; i < bias_packs.size(); i++){
    std::string type, name;
    bias_packs[i].params().readString("type", ParameterPack::KeyType::Required, type);
    bias_packs[i].params().readString("name", ParameterPack::KeyType::Required, name);
    auto bias_ptr = BiasFactory(type, bias_packs[i]);
    master_input_pack.addBias(name, bias_ptr);
  }
  std::vector<InputPack> generator_packs = master_input_pack.buildDerivedInputPacks("Generator");
  for(std::size_t i = 0; i < generator_packs.size(); i++){
    std::string type, name;
    generator_packs[i].params().readString("type", ParameterPack::KeyType::Required, type);
    generator_packs[i].params().readString("name", ParameterPack::KeyType::Required, name);
    auto generator_ptr = GeneratorFactory(type, generator_packs[i]);
    master_input_pack.addGenerator(name, generator_ptr);
  }
  std::vector<InputPack> dataset_packs = master_input_pack.buildDerivedInputPacks("Dataset");
  for(std::size_t i = 0; i < dataset_packs.size(); i++){
    std::string name;
    dataset_packs[i].params().readString("name", ParameterPack::KeyType::Required, name);
    auto dataset_ptr = new Dataset(dataset_packs[i]);
    master_input_pack.addDataset(name, dataset_ptr);
  } 
  std::vector<Calculation*> calculation_vector;
  std::vector<InputPack> calc_packs = master_input_pack.buildDerivedInputPacks("Calculation");
  for(std::size_t i = 0; i < calc_packs.size(); i++){
    std::string type, name;
    calc_packs[i].params().readString("type", ParameterPack::KeyType::Required, type);
    calc_packs[i].params().readString("name", ParameterPack::KeyType::Required, name);
    auto calc_ptr = CalculationFactory(type, calc_packs[i]);
    master_input_pack.addCalculation(name, calc_ptr);

    calculation_vector.push_back(calc_ptr);
  }
  for(int i = 0; i < calculation_vector.size(); i++){
    calculation_vector[i]->calculate();
    calculation_vector[i]->output();
  }

  return 0;
}