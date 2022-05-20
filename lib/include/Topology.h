#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace topology {

using std::ostream;
using std::shared_ptr;
using std::string;
using std::vector;

class Topology {
public:
  string id;
  vector<shared_ptr<Topology>> components;

protected:
  void print_attribute(ostream &s, int spaces, const string &name,
                       const string &attr) const;
  void print_components(ostream &s, int spaces) const;
  void print_spaces(ostream &s, int spaces) const;

  friend class TopologyTest;

public:
  Topology(const string &ID);
  virtual ~Topology();
  virtual void print(ostream &s, int spaces = 0) const;
};

} // namespace topology
