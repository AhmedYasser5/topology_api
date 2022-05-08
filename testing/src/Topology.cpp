#include "Topology.h"

using namespace topology;

Topology::Topology(const string &ID) : id(ID) {}

Topology::~Topology() {}

void Topology::print_attribute(ostream &s, int spaces, const string &name,
                               const string &attr) const {
  print_spaces(s, spaces);
  s << "\"" << name << "\": \"" << attr << "\"";
}

void Topology::print_components(ostream &s, int spaces) const {
  print_spaces(s, spaces);
  s << "\"components\": [\n";
  int size = components.size();
  spaces += 2;
  for (const shared_ptr<Topology> &it : components) {
    it->print(s, spaces);
    if (--size)
      s << ',';
    s << '\n';
  }
  spaces -= 2;
  print_spaces(s, spaces);
  s << "]";
}

void Topology::print_spaces(ostream &s, int spaces) const {
  while (spaces--)
    s << ' ';
}

void Topology::print(ostream &s, int spaces) const {
  print_spaces(s, spaces);
  s << "{\n";
  spaces += 2;
  print_attribute(s, spaces, "id", id);
  if (!components.empty()) {
    s << ",\n";
    print_components(s, spaces);
  }
  s << "\n";
  spaces -= 2;
  print_spaces(s, spaces);
  s << "}";
}
