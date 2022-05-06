#include "API.h"
#include <fstream>

using std::ifstream;
using std::ofstream;

TopologyAPI::topologyNetlists::topologyNetlists(Topology *ptr) : top(ptr) {}

bool TopologyAPI::readJSON(const string &FileName) {}

bool TopologyAPI::writeJSON(const string &TopologyID) const {
  auto it = tops.find(TopologyID);
  if (it == tops.end())
    return false;
  ofstream outFile(TopologyID + ".json");
  it->second.top->print(outFile);
  return true;
}

vector<shared_ptr<Topology>> TopologyAPI::queryTopologies() const {}

bool TopologyAPI::deleteTopology(const string &TopologyID) {}

vector<shared_ptr<Device>>
TopologyAPI::queryDevices(const string &TopologyID) const {}

vector<shared_ptr<Device>>
TopologyAPI::queryDevicesWithNetlistNode(const string &TopologyID,
                                         const string &NetlistNodeID) const {}
