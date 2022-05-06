#pragma once

#include "Device.h"
#include "Topology.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class TopologyAPI {
private:
  struct topologyNetlists {
    shared_ptr<Topology> top;
    unordered_map<string, vector<shared_ptr<Device>>> net2dev;
    topologyNetlists(Topology *ptr);
  };
  unordered_map<string, topologyNetlists> tops;

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
