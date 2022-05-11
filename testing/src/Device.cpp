#include "Device.h"

using namespace topology;

Device::Device(const string &TYPE) : Topology(""), type(TYPE) {}

Device::~Device() {}

void Device::print_properties(ostream &s, int spaces) const {
  int size = properties.size();
  for (const pair<string, vector<pair<string, string>>> &it : properties) {
    print_spaces(s, spaces);
    s << "\"" << it.first << "\": ";
    spaces += 2;
    bool pfirst = true;
    for (const pair<string, string> &p : it.second) {
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
  s << "{\n";
  for (const pair<string, string> &it : netlist) {
    if (nfirst)
      nfirst = false;
    else
      s << ",\n";
    print_spaces(s, spaces);
    s << "\"" << it.first << "\": \"" << it.second << "\"";
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
