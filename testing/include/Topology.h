#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

using std::ostream;
using std::shared_ptr;
using std::string;
using std::vector;

class Topology {
protected:
  string id;
  vector<shared_ptr<Topology>> components;

  void print_attribute(ostream &s, int spaces, const string &name,
                       const string &attr) const;
  void print_components(ostream &s, int spaces) const;
  void print_spaces(ostream &s, int spaces) const;

  friend class TopologyAPI;

public:
  Topology(const string &ID);
  virtual ~Topology();
  virtual void print(ostream &s, int spaces = 0) const;
};
