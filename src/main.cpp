#include "Device.h"
#include "Topology.h"
#include <iostream>

using namespace std;

int main() {
  Topology *top = new Topology("top1");
  Device *d1 = new Device("res1", "resistor"), *d2 = new Device("m1", "nmos");
  top->components["res1"] = d1;
  top->components["m1"] = d2;
  d1->netlist["vdd"] = "t1";
  d1->netlist["n1"] = "t2";
  vector<pair<string, string>> vec;
  vec.emplace_back("default", "100");
  vec.emplace_back("min", "10");
  vec.emplace_back("max", "1000");
  d1->properties.emplace_back("resistance", vec);
  d2->netlist["n1"] = "drain";
  d2->netlist["vin"] = "gate";
  d2->netlist["vss"] = "source";
  vec.clear();
  vec.emplace_back("default", "1.5");
  vec.emplace_back("min", "1");
  vec.emplace_back("max", "2");
  d2->properties.emplace_back("m(1)", vec);
  top->print(cout);
  delete top;
  return 0;
}
