#pragma once

#include "Device.h"
#include "Topology.h"
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace topology {

using std::ifstream;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class TopologyAPI {
private:
  struct topologyNetlists {
    shared_ptr<Topology> top;
    unordered_map<string, vector<shared_ptr<Device>>> net2dev;
  };
  unordered_map<string, topologyNetlists> tops;

  bool readAttribute(ifstream &inFile, const string &str,
                     shared_ptr<Topology> &top, shared_ptr<Device> &dev,
                     bool &isDevice);
  bool readComponents(ifstream &inFile, shared_ptr<Topology> &top);
  bool readPropertiesAndNetlists(ifstream &inFile, const string &str,
                                 shared_ptr<Device> &dev);
  shared_ptr<Topology> readData(ifstream &inFile);

public:
  bool readJSON(const string &FileName);
  bool writeJSON(const string &TopologyID) const;
  vector<shared_ptr<Topology>> queryTopologies() const;
  bool deleteTopology(const string &TopologyID);
  vector<shared_ptr<Device>> queryDevices(const string &TopologyID) const;
  vector<shared_ptr<Device>>
  queryDevicesWithNetlistNode(const string &TopologyID,
                              const string &NetlistNodeID) const;
};

} // namespace topology
