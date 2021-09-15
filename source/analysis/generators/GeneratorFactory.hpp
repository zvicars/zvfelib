#pragma once
#include "Generator.hpp"
#include "ListGenerator.hpp"
#include "SubstitutionGenerator.hpp"
#include "UnionGenerator.hpp"

inline Generator* GeneratorFactory(std::string key, const InputPack& input_pack){
  if(key == "list"){
    return new ListGenerator(input_pack);
  }
  return 0;
}