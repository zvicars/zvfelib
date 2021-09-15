#include "timeseries.hpp"
#include <cmath>
#include <iostream>
bool TimeseriesStatistics::computeStats(const std::vector<double>& data){
  mean_ = 0.0;
  var_ = 0.0;
  for(int i = 0; i < data.size(); i++){
    mean_ += data[i];
  }
  mean_ = mean_ / (double)data.size();
  for(int i = 0; i < data.size(); i++){
    var_ += (data[i]-mean_)*(data[i]-mean_);
  }
  var_ = var_ / (double)data.size();
  return 0;
}

bool TimeseriesStatistics::computeTcorr(const std::vector<double>& time, const std::vector<double>& data, double lagtime){
  //consistency check to make sure dt stays the same throughout the sim before attempting to compute tcorr stuff
  if(time.size() > 1){
    dt_ = time[2] - time[1];
  }
  int nlags = lagtime / dt_;
  for(int i = 2; i < time.size(); i++){
    double dtstep = time[i] - time[i-1];
    //some numerical error is allowed in dt, assuming ps or other reasonable unit of time
    if(fabs(dtstep - dt_) > 1e-5){
      std::cout << "dt is not constant, cannot compute correlation times for this timeseries" << std::endl;
    }
  }
  //naive calculation of acf
  std::vector<double> acf(nlags, 0.0);
  for(int dl = 0; dl < nlags; dl++){
    int ncounts = 0;
    for(int i = 0; i < data.size() - dl; i++){
      acf[dl] += data[i]*data[i+dl];
      ncounts++;
    }
    acf[dl] *= 1.0/(double)ncounts;
  }
  //normalize
  for(int i = 0; i < acf.size(); i++){
    acf[i] = acf[i]/acf[0];
  }
  //integrate
  double sum = 0.0;
  for(int i = 0; i < acf.size(); i++){
    sum += acf[i];
  }
  sum *= 2;
  sum += 1.0;
  itcorr_ = sum;
  return 0;
}


Timeseries::Timeseries(){
  return;
}
const std::vector<double>& Timeseries::getDataViaIndex(int i) const{
  return data_[i];
}
double Timeseries::getDatapointViaIndex(int i, int j) const{
  return data_[i][j];
}
const std::vector<double>& Timeseries::getDataViaLabel(std::string label) const{
  auto iter = label2column_.find(label);
  if(iter == label2column_.end()){
    std::cout << "Could not find column with specified label." << std::endl;
    throw 0;
  }
  return data_[iter->second];
}

int Timeseries::getIndexViaLabel(std::string label) const{
  auto iter = label2column_.find(label);
  if(iter == label2column_.end()){
    std::cout << "Could not find column with specified label." << std::endl;
    throw 0;
  }
  return iter->second;
}

const std::vector<double>& Timeseries::getTime() const{
  return time_;
};

bool Timeseries::computeStats(){
  if(stats_.size() != data_.size()) stats_.resize(data_.size());
  for(int i = 0; i < data_.size(); i++){
    stats_[i].computeStats(data_[i]);
  }
  return 1;
}