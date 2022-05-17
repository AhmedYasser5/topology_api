#include <API.h>
#include <iostream>
#include <memory>

using namespace std;
using topology::TopologyAPI;

int main() {
  TopologyAPI api;
  cout << "\"topology.json\" file read status = " << boolalpha
       << api.readJSON("topology.json") << endl;
  cout << "\"toptmp.json\" file read status = " << api.readJSON("toptmp.json")
       << endl;

  cout << "All topologies stored in memory now are:" << endl;
  auto vec = api.queryTopologies();
  for (auto &it : vec) {
    it->print(cout);
    cout << endl;
  }

  cout << "Topology \"top1\" remove from memory state = "
       << api.deleteTopology("top1") << endl;
  cout << "Either states would not change the first vector returned from the "
          "first queryTopologies():"
       << endl;
  for (auto &it : vec) {
    it->print(cout);
    cout << endl;
  }
  cout << endl;

  cout << "These are all devices stored in topology \"top2\":" << endl;
  auto vecd = api.queryDevices("top2");
  for (auto &it : vecd) {
    it->print(cout);
    cout << endl;
  }
  cout << endl;

  cout << "However, only those devices in topology \"top2\" have a netlist "
          "\"vss\":"
       << endl;
  auto vecn = api.queryDevicesWithNetlistNode("top2", "vss");
  for (auto &it : vecn) {
    it->print(cout);
    cout << endl;
  }
  return 0;
}
