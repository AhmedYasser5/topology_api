#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::ostream;
using std::string;
using std::unordered_map;
using std::vector;

class Topology {
public:
  string id;
  unordered_map<string, Topology *> components;

  void print_attribute(ostream &s, int spaces, const string &name,
                       const string &attr) const;
  void print_components(ostream &s, int spaces) const;
  void print_spaces(ostream &s, int spaces) const;

public:
  Topology(const string &ID);
  virtual ~Topology();
  virtual void print(ostream &s, int spaces = 0) const;
};
