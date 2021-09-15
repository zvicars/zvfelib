#pragma once
#include <vector>
#include <string>
#include <map>
class Bias;
//a timeseries contains a single time axis and one or more data axes
//this is the raw container for data from input files

//bias objects will be dynamically created for biasedtimeseries objects when relevant
//it will define the biasing function and tell the timeseries what data to send
struct TimeseriesStatistics{
  double mean_, var_, tcorr_, nsamples_, dt_, itcorr_;
  bool computeStats(const std::vector<double>& data);
  bool computeTcorr(const std::vector<double>& time, const std::vector<double>& data, double lagtime);
  bool isValid = 0;
};

class Timeseries{
public:
  template <class T>
  using Vec = std::vector<T>;
  template <class T, class V>
  using Map = std::map<T, V>;
  Timeseries();
  const Vec<double>& getDataViaIndex(int i) const;
  double getDatapointViaIndex(int i, int j) const;
  int getNumCols()const {
    return data_.size();
  }
  const Vec<double>& getDataViaLabel(std::string label) const;
  int getIndexViaLabel(std::string label) const;
  const Vec<double>& getTime() const;
  bool computeStats();
  const Vec<TimeseriesStatistics>& getStats(){
    return stats_;
  }
  int getNumCols(){
    return data_.size();
  }
  void setData(Vec<Vec<double> > data){
    data_ = data;
  }
  void setTime(Vec<double> time){
    time_ = time;
  }
  void setMap(Map<std::string, int> map){
    label2column_ = map;
  }
  void setName(std::string name){
    name_ = name;
  } 

private:
  //indexing in this case is column-row [c][r] to make it easy to extract columns for processing
  Vec<Vec<double> > data_; //precomputed bias also available
  Vec<double> time_;
  Vec<TimeseriesStatistics> stats_;
  Map<std::string, int> label2column_; //if label data is available, make a mapping from labels to indexes
  std::string name_;
};

