#include "filereader.hpp"
static inline bool isCommentLine(std::string line){
  if(line.at(0) == '#') return 1;
  if(StringTools::trimWhitespace(line).size() == 0) return 1;
  return 0;
}

//the file structure should be a series of comment lines followed by timeseries data
bool PLUMEDFileReader::load(std::string filename){
  std::ifstream ifile(filename);
  if(!ifile.is_open()){
    std::cout << "Failed to open input file " << filename << "." << std::endl;
    throw 0;
  }

  //output stuff
  std::vector<std::string> data_labels;
  std::vector<std::vector<double> > data;

  std::string line;
  auto lastline = ifile.tellg();
  auto linebeforelast = ifile.tellg();
  bool startflag = 0;
  int ncols = 0;
  int linecounter = -1;
  while(std::getline(ifile, line)){
    linecounter++;
    //when the first non-comment line is present, set the appropriate flags and get the number of columns to anticipate
    //before resetting the stream to the previous line to get the column labels
    if(isCommentLine(line)){
      linebeforelast = lastline;
      lastline = ifile.tellg();
      continue;
    }
    if(startflag == 0){
      startflag = 1;
      std::stringstream ss(line);
      double arg;
      while(ss >> arg){
        ncols++;
      }
      if(ncols == 0){
        std::cout << "No columns found..." << std::endl;
        throw 0;
      }
      ifile.seekg(linebeforelast);
      std::getline(ifile, line);
      line.erase(line.find("! FIELDS"), 8);
      std::string data_label;
      line = line.substr(1); //trim the initial #
      std::stringstream ss2(line);
      while(ss2 >> data_label){
        //std::cout << data_label << std::endl;
        data_labels.push_back(data_label);
      }
      if(data_labels.size() != ncols){
        std::cout << "Could not match column labels to columns." << std::endl;
        data_labels.resize(ncols, "");
      }
      continue;
    }
    data.resize(ncols);
    std::stringstream ss(line);
    double datapoint;
    std::vector<double> dataline(ncols, 0.0);
    int counter = 0;
    bool badColumn = 0;
    while(ss >> datapoint){
      if(counter > ncols){
        badColumn = 1;
        break;
      }
      dataline[counter] = datapoint;
      counter++;
    }
    if(counter != ncols) badColumn = 1;
    if(!badColumn){
      for(int i = 0; i < ncols; i++){
        data[i].push_back(dataline[i]);
      }
    }
    else{
      std::cout << "Bad column found on line " << linecounter << ".\n";
    }
  }
  ifile.close();
  //once this point has been reached, all of the data should be loaded into the appropriate vector and all columns should be labeled
  //now just have to construct the map and put it all in a timeseries object
  //separate time column
  timeseries_.setTime(data[0]);
  data.erase(data.begin());
  data_labels.erase(data_labels.begin());

  //build map
  std::map<std::string, int> label_map;
  for(int i = 0; i < data_labels.size(); i++){
    label_map.insert(std::pair<std::string, int>(data_labels[i], i));
  }

  //Vec<Vec<double> > data_; //precomputed bias also available
  //Vec<double> time_;
  //Vec<TimeseriesStatistics> stats_;
  //Map<std::string, int> label2column_; //if label data is available, make a mapping from labels to indexes
  //std::string name_;
  timeseries_.setData(data);
  timeseries_.setMap(label_map);
  timeseries_.setName(filename);
  //File successfully loaded
  return 0;
}