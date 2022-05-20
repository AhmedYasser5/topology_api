#pragma once

#include "Topology.h"
#include <ostream>
#include <string>
#include <vector>

namespace topology {

using std::ostream;
using std::pair;
using std::string;
using std::vector;

class Device : public Topology {
public:
  string type;
  vector<pair<string, string>> netlist;
  vector<pair<string, vector<pair<string, string>>>> properties;

protected:
  void print_properties(ostream &s, int spaces) const;
  void print_netlist(ostream &s, int spaces) const;

  friend class TopologyTest;

public:
  Device(const string &TYPE);
  virtual ~Device();
  virtual void print(ostream &s, int spaces = 0) const;
};

} // namespace topology
