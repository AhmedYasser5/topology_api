#include "API.h"

using namespace topology;
using std::getline;
using std::ofstream;
using std::static_pointer_cast;

void TopologyAPI::topologyNetlists::insertNetlists() {
  for (const shared_ptr<Topology> &devtop : top->components) {
    shared_ptr<Device> dev = static_pointer_cast<Device>(devtop);

    for (const pair<string, string> &it : dev->netlist)
      net2dev[it.second].insert(dev);
  }
}

bool TopologyAPI::readAttribute(ifstream &inFile, const string &str,
                                shared_ptr<Topology> &top,
                                shared_ptr<Device> &dev, bool &isDevice) {
  char current;
  inFile >> current;
  string val;
  if (!getline(inFile, val, '"'))
    return false;
  if (isDevice)
    dev->id = val;
  else if (str == "type") {
    dev = shared_ptr<Device>(new Device(val));
    top = dev;
    isDevice = true;
  } else
    top = shared_ptr<Topology>(new Topology(val));
  return true;
}

bool TopologyAPI::readComponents(ifstream &inFile, shared_ptr<Topology> &top) {
  char current;
  inFile >> current;
  if (current != '[')
    return false;
  do {
    top->components.emplace_back(readData(inFile));
    if (top->components.back() == NULL)
      return false;
    inFile >> current;
  } while (current == ',');

  return current == ']';
}

bool TopologyAPI::readPropertiesAndNetlists(ifstream &inFile, const string &str,
                                            shared_ptr<Device> &dev) {
  char current;
  inFile >> current;
  if (current != '{')
    return false;

  if (str != "netlist")
    dev->properties.emplace_back(str, vector<pair<string, string>>());

  do {
    inFile >> current;

    string prop;
    if (!getline(inFile, prop, '"'))
      return false;

    inFile >> current >> current;
    string val;
    if (str == "netlist") {
      if (!getline(inFile, val, '"'))
        return false;
      dev->netlist.emplace_back(prop, val);

      inFile >> current;
    } else {
      do {
        val += current;
        inFile >> current;
      } while (current != ',' && current != '}');

      dev->properties.back().second.emplace_back(prop, val);
    }

  } while (current == ',');

  return current == '}';
}

shared_ptr<Topology> TopologyAPI::readData(ifstream &inFile) {
  char current;
  inFile >> current;
  if (current != '{')
    return NULL;
  shared_ptr<Topology> top = NULL;
  shared_ptr<Device> dev = NULL;
  bool isDevice = false;

  while (inFile.good()) {
    inFile >> current;
    string str;
    if (!getline(inFile, str, '"'))
      return NULL;
    inFile >> current;

    bool failed;
    if (str == "id" || str == "type")
      failed = !readAttribute(inFile, str, top, dev, isDevice);
    else if (str == "components")
      failed = !readComponents(inFile, top);
    else if (isDevice)
      failed = !readPropertiesAndNetlists(inFile, str, dev);
    else
      failed = true;
    if (failed)
      return NULL;

    inFile >> current;
    if (current == '}')
      return top;
  }

  return NULL;
}

bool TopologyAPI::readJSON(const string &FileName) {
  ifstream inFile(FileName);
  if (!inFile.good())
    return false;

  shared_ptr<Topology> top(readData(inFile));
  if (top == NULL || tops.find(top->id) != tops.end())
    return false;

  topologyNetlists &topnet = tops[top->id];
  topnet.top = top;
  topnet.insertNetlists();

  return true;
}

bool TopologyAPI::writeJSON(const string &TopologyID) const {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  if (it == tops.end())
    return false;

  ofstream outFile(TopologyID + ".json");
  if (!outFile.good())
    return false;

  it->second.top->print(outFile);
  return true;
}

vector<shared_ptr<Topology>> TopologyAPI::queryTopologies() const {
  vector<shared_ptr<Topology>> ret;
  for (const pair<string, topologyNetlists> &it : tops)
    ret.emplace_back(it.second.top);
  return ret;
}

bool TopologyAPI::deleteTopology(const string &TopologyID) {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  if (it == tops.end())
    return false;

  tops.erase(it);
  return true;
}

vector<shared_ptr<Device>>
TopologyAPI::queryDevices(const string &TopologyID) const {
  vector<shared_ptr<Device>> ret;

  unordered_map<string, topologyNetlists>::const_iterator top =
      tops.find(TopologyID);
  if (top == tops.end())
    return ret;

  for (const shared_ptr<Topology> &it : top->second.top->components)
    ret.emplace_back(static_pointer_cast<Device>(it));
  return ret;
}

vector<shared_ptr<Device>>
TopologyAPI::queryDevicesWithNetlistNode(const string &TopologyID,
                                         const string &NetlistNodeID) const {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  if (it == tops.end())
    return vector<shared_ptr<Device>>();

  unordered_map<string, unordered_set<shared_ptr<Device>>>::const_iterator net =
      it->second.net2dev.find(NetlistNodeID);
  if (net == it->second.net2dev.end())
    return vector<shared_ptr<Device>>();

  return vector<shared_ptr<Device>>(net->second.begin(), net->second.end());
}
