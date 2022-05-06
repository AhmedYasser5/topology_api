#include "Device.h"

Device::Device(const string &ID, const string &TYPE)
    : Topology(ID), type(TYPE) {}

Device::~Device() {}

void Device::print_properties(ostream &s, int spaces) const {
  int size = properties.size();
  for (auto &it : properties) {
    print_spaces(s, spaces);
    s << "\"" << it.first << "\": ";
    spaces += 2;
    bool pfirst = true;
    for (auto &p : it.second) {
      if (pfirst) {
        pfirst = false;
        s << "{\n";
      } else
        s << ",\n";
      print_spaces(s, spaces);
      s << "\"" << p.first << "\": " << p.second;
    }
    spaces -= 2;
    s << "\n";
    print_spaces(s, spaces);
    s << "}";
    if (--size)
      s << ",\n";
  }
}

void Device::print_netlist(ostream &s, int spaces) const {
  print_spaces(s, spaces);
  s << "\"netlist\": ";
  spaces += 2;
  bool nfirst = true;
  for (auto &it : netlist) {
    if (nfirst) {
      nfirst = false;
      s << "{\n";
    } else
      s << ",\n";
    print_spaces(s, spaces);
    s << "\"" << it.second << "\": \"" << it.first << "\"";
  }
  spaces -= 2;
  s << "\n";
  print_spaces(s, spaces);
  s << "}";
}

void Device::print(ostream &s, int spaces) const {
  print_spaces(s, spaces);
  spaces += 2;
  s << "{\n";
  print_attribute(s, spaces, "type", type);
  s << ",\n";
  print_attribute(s, spaces, "id", id);
  s << ",\n";
  if (!properties.empty()) {
    print_properties(s, spaces);
    s << ",\n";
  }
  print_netlist(s, spaces);
  if (!components.empty()) {
    s << ",\n";
    print_components(s, spaces);
  }
  spaces -= 2;
  s << "\n";
  print_spaces(s, spaces);
  s << "}";
}
