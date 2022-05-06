#pragma once

#include "Topology.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::ostream;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

class Device : public Topology {
public:
  string type;
  unordered_map<string, string> netlist;
  vector<pair<string, vector<pair<string, string>>>> properties;

  void print_properties(ostream &s, int spaces) const;
  void print_netlist(ostream &s, int spaces) const;

public:
  Device(const string &ID, const string &TYPE);
  virtual ~Device();
  virtual void print(ostream &s, int spaces = 0) const;
};
