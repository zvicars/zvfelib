#pragma once
#include "BWHAM.hpp"
#include "UBWHAM.hpp"
inline Calculation* CalculationFactory(std::string key, const InputPack& input_pack){
  if(key == "BWHAM") return new BWHAM(input_pack);
  if(key == "UBWHAM") return new UBWHAM(input_pack);
  std::cout << "Invalid calculation specified..." << std::endl;
  throw 0;
  return 0;
}