//A filereader will take in an input file name and return a single timeseries object
//A timeseries object has the minimal information required to capture time-dependent data
//It will try to grab and store column labels, if available, and put it into a map
#pragma once
#include "timeseries.hpp"
#include "../tools/StringTools.hpp"

class FileReader{
public:
  Timeseries getTimeSeries(){
    return timeseries_;
  }
  virtual bool load(std::string filename) = 0;
protected:
  std::string filename_;
  Timeseries timeseries_;
};


class IndusFileReader : public FileReader{
public:
  virtual bool load(std::string filename){
    return 0;
  }
protected:
private:
};

class OPFileReader : public FileReader{
public:
  virtual bool load(std::string filename);
protected:
private:
};

class PLUMEDFileReader : public FileReader{
public:
  virtual bool load(std::string filename);
protected:
private:
};

class MCFileReader : public FileReader{
public:
  virtual bool load(std::string filename){
    return 0;
  }
protected:
private:
};

class ZVTLFileReader : public FileReader{
public:
  virtual bool load(std::string filename){
    return 0;
  }
protected:
private:
};

inline FileReader* createFileReader(std::string key){
  if(key == "op") return new OPFileReader();
  if(key == "plumed") return new PLUMEDFileReader();
  if(key == "mc") return new MCFileReader();
  if(key == "zvtl") return new ZVTLFileReader();
  if(key == "indus") return new IndusFileReader();
  std::cout << "Failed to find valid file reader." << std::endl;
  throw 0;
  return 0;
}