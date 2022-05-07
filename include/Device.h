#pragma once

#include "Topology.h"
#include <ostream>
#include <string>
#include <vector>

using std::ostream;
using std::pair;
using std::string;
using std::vector;

class Device : public Topology {
protected:
  string type;
  vector<pair<string, string>> netlist;
  vector<pair<string, vector<pair<string, string>>>> properties;

  void print_properties(ostream &s, int spaces) const;
  void print_netlist(ostream &s, int spaces) const;

  friend class TopologyAPI;

public:
  Device(const string &TYPE);
  virtual ~Device();
  virtual void print(ostream &s, int spaces = 0) const;
};
