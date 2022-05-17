#include "API.h"

using namespace topology;
using std::getline;
using std::ofstream;
using std::static_pointer_cast;

/* inserting every device with its corresponding net nodes */
void TopologyAPI::topologyNetlists::insertNetlists() {
  for (const shared_ptr<Topology> &devtop : top->components) {
    shared_ptr<Device> dev = static_pointer_cast<Device>(devtop);

    for (const pair<string, string> &it : dev->netlist)
      net2dev[it.second].insert(dev);
  }
}

/* reading "id" and "type" */
bool TopologyAPI::readAttribute(ifstream &inFile, const string &str,
                                shared_ptr<Topology> &top,
                                shared_ptr<Device> &dev, bool &isDevice) {
  char current;
  inFile >> current;
  string val;
  // attribute should be enclosed by ""
  if (!getline(inFile, val, '"'))
    return false;

  // every device should have exactly one "type", every topology or device
  // should have exactly one "id"
  bool isRepeated = str == "type" && dev != NULL;
  isRepeated = isRepeated || (str == "id" && top != NULL && !top->id.empty());
  if (isRepeated)
    return false;

  if (str == "type") {
    dev = shared_ptr<Device>(new Device(val));

    // copying previously defined id and components since it is now a device not
    // a topology
    if (top != NULL) {
      dev->id = top->id;
      dev->components.insert(dev->components.end(), top->components.begin(),
                             top->components.end());
    }

    top = dev;
    isDevice = true;
  } else {
    if (top != NULL)
      top->id = val;
    else
      top = shared_ptr<Topology>(new Topology(val));
  }
  return true;
}

/* reading devices inside a given topology/device */
bool TopologyAPI::readComponents(ifstream &inFile, shared_ptr<Topology> &top) {
  char current;
  inFile >> current;

  // components should be enclosed by []
  if (current != '[')
    return false;

  do {
    // recursively reading nested devices
    top->components.emplace_back(readData(inFile));

    // handling errors
    if (top->components.back() == NULL)
      return false;

    inFile >> current;
  } while (current == ',');

  return current == ']';
}

/* reading properties and netlists */
bool TopologyAPI::readPropertiesAndNetlists(ifstream &inFile, const string &str,
                                            shared_ptr<Device> &dev) {
  char current;
  inFile >> current;

  // properties and netlists should be enclosed by {}
  if (current != '{')
    return false;

  // everything other than netlists is a property
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

        // reading numbers as strings
        inFile >> current;
      } while (current != ',' && current != '}');

      dev->properties.back().second.emplace_back(prop, val);
    }

  } while (current == ',');

  return current == '}';
}

/* reading all data of a topology/device from a file */
shared_ptr<Topology> TopologyAPI::readData(ifstream &inFile) {
  char current;
  inFile >> current;

  // topologies or devices should be enclosed by {}
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
      failed = top != NULL && !readComponents(inFile, top);
    else if (isDevice)
      failed = !readPropertiesAndNetlists(inFile, str, dev);
    else
      failed = true;
    // catching all errors down here
    if (failed)
      return NULL;

    inFile >> current;
    if (current == '}')
      return top;
  }

  // if it doesn't end in }, it is syntactically wrong
  return NULL;
}

/* reading all data from a file */
bool TopologyAPI::readJSON(const string &FileName) {
  ifstream inFile(FileName);
  if (!inFile.good())
    return false;

  shared_ptr<Topology> top(readData(inFile));
  // if found, then it is a duplicate (don't add it)
  if (top == NULL || tops.find(top->id) != tops.end())
    return false;

  topologyNetlists &topnet = tops[top->id];
  topnet.top = top;
  topnet.insertNetlists();

  return true;
}

/* writing the data to a JSON file */
bool TopologyAPI::writeJSON(const string &TopologyID) const {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  // if found, continue
  if (it == tops.end())
    return false;

  ofstream outFile(TopologyID + ".json");
  if (!outFile.good())
    return false;

  it->second.top->print(outFile);
  return true;
}

/* gets all topologies stored in the object */
vector<shared_ptr<Topology>> TopologyAPI::queryTopologies() const {
  vector<shared_ptr<Topology>> ret;
  for (const pair<string, topologyNetlists> &it : tops)
    ret.emplace_back(it.second.top);
  return ret;
}

/* deletes a topology from the object */
bool TopologyAPI::deleteTopology(const string &TopologyID) {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  // if found, continue
  if (it == tops.end())
    return false;

  tops.erase(it);
  return true;
}

/* gets devices in a given topology */
vector<shared_ptr<Device>>
TopologyAPI::queryDevices(const string &TopologyID) const {
  vector<shared_ptr<Device>> ret;

  unordered_map<string, topologyNetlists>::const_iterator top =
      tops.find(TopologyID);
  // if found, continue
  if (top == tops.end())
    return ret;

  for (const shared_ptr<Topology> &it : top->second.top->components)
    ret.emplace_back(static_pointer_cast<Device>(it));
  return ret;
}

/* gets devices having at least one net with a given value */
vector<shared_ptr<Device>>
TopologyAPI::queryDevicesWithNetlistNode(const string &TopologyID,
                                         const string &NetlistNodeID) const {
  unordered_map<string, topologyNetlists>::const_iterator it =
      tops.find(TopologyID);
  // if found, continue
  if (it == tops.end())
    return vector<shared_ptr<Device>>();

  unordered_map<string, unordered_set<shared_ptr<Device>>>::const_iterator net =
      it->second.net2dev.find(NetlistNodeID);
  if (net == it->second.net2dev.end())
    return vector<shared_ptr<Device>>();

  return vector<shared_ptr<Device>>(net->second.begin(), net->second.end());
}
