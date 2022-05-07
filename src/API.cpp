#include "API.h"
#include <stack>

using std::ofstream;
using std::stack;

TopologyAPI::topologyNetlists::topologyNetlists(Topology *ptr) : top(ptr) {}

bool TopologyAPI::readQuoted(ifstream &inFile, string &str) {
  char current;
  while (inFile.get(current)) {
    if (current == '"')
      return true;
    str += current;
  }
  return false;
}

shared_ptr<Topology> TopologyAPI::readData(ifstream &inFile) {
  // Each opening and closing of [] and {} pairs follow each other in the ASCII
  // table with a difference of 2
  char current;
  inFile >> current;
  if (current != '{')
    return NULL;
  stack<char> openedBrackets;
  openedBrackets.push(current);
  shared_ptr<Topology> top = NULL;
  string tmp;
  while (!openedBrackets.empty()) {
    inFile >> current;
    if (!inFile.good())
      return NULL;
    if (current == '"') {
      string str;
      if (!readQuoted(inFile, str))
        return NULL;
      if (str == "id" || str == "type") {

      } else if () {
      }
    } else if (current == ']' && current == openedBrackets.top())
  }
}

bool TopologyAPI::readJSON(const string &FileName) {
  ifstream inFile(FileName);
  if (!inFile.good())
    return false;
  shared_ptr<Topology> top = readData(inFile);
  if (top == NULL || tops.find(id) != tops.end())
    return false;
  tops.emplace(top->id, top);
  topologyNetlists &topnet = tops[top->id];
  for (const shared_ptr<const Device> &dev : top->components) {
    for (auto &it : dev->netlist)
      topnet.net2dev[it.second].emplace_back(dev);
  }
  return true;
}

bool TopologyAPI::writeJSON(const string &TopologyID) const {
  auto it = tops.find(TopologyID);
  if (it == tops.end())
    return false;
  ofstream outFile(TopologyID + ".json");
  if (!outFile.good())
    return false;
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
