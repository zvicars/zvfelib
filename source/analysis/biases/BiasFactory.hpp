#pragma once
#include "Bias.hpp"
#include "HarmonicBias.hpp"

inline Bias* BiasFactory(std::string key, const InputPack& input_pack){
  if(key == "harmonic"){
    return new HarmonicBias(input_pack);
  }
  return 0;
}