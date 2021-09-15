#include "../filereader.hpp"
#include <cmath>

inline bool isNear(double val1, double val2){
  double tol = 1e-5;
  if(fabs(val1 - val2) < tol) return 1;
  return 0;
}

int main(){
  std::string filename = "./test_files/sample_op.input";
  Timeseries t1;
  OPFileReader opf;
  opf.load(filename);
  t1 = opf.getTimeSeries();
  if(t1.getNumCols() != 3){
    std::cout << "Failed to get proper number of columns.\nExpected 3, Observed " << t1.getNumCols() << "\n";
    return 1;
  }
  //# Time[ps]	avg(q(i))_v	N_v	Ntilde_v
  bool failure_flag = 0;
  bool datalabel1 = t1.getIndexViaLabel("avg(q(i))_v") == 0;
  bool datalabel2 = t1.getIndexViaLabel("N_v") == 1;
  bool datalabel3 = t1.getIndexViaLabel("Ntilde_v") == 2;
  if(!(datalabel1 && datalabel2 && datalabel3)){
    std::cout << "Something is amiss regarding the column labels or index mapping scheme." << std::endl;
    failure_flag = 1;
  }
  t1.computeStats();
  if(!isNear(t1.getStats()[0].mean_, -0.34172)){
    std::cout << "Mean calculation does not agree for column 0." << std::endl;
    failure_flag = 1;
  }
  if(!isNear(t1.getStats()[1].mean_, 81)){
    std::cout << "Mean calculation does not agree for column 1." << std::endl;
    failure_flag = 1;
  }  
  if(!isNear(t1.getStats()[2].mean_, 81)){
    std::cout << "Mean calculation does not agree for column 2." << std::endl;
    failure_flag = 1;
  }  
  if(!isNear(t1.getStats()[0].var_, 0.001147336)){
    std::cout << "Variance calculation does not agree for column 0." << std::endl;
    failure_flag = 1;
  }
  if(!isNear(t1.getStats()[1].var_, 8.545454545)){
    std::cout << "Variance calculation does not agree for column 1." << std::endl;
    failure_flag = 1;
  }  
  if(!isNear(t1.getStats()[2].var_, 8.545454545)){
    std::cout << "Variance calculation does not agree for column 2." << std::endl;
    failure_flag = 1;
  }  
  if(failure_flag) return 1;

  std::cout << "Test successfully passed." << std::endl;
  return 0;
}