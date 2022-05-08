#include "API.h"
#include <iostream>
#include <memory>

using namespace std;
using topology::TopologyAPI;

int main() {
  TopologyAPI api;
  cout << boolalpha << api.readJSON("topology.json") << endl;
  cout << api.readJSON("toptmp.json") << endl;
  auto vec = api.queryTopologies();
  for (auto &it : vec) {
    it->print(cout);
    cout << endl;
  }
  cout << api.deleteTopology("top1") << endl;
  for (auto &it : vec) {
    it->print(cout);
    cout << endl;
  }
  cout << endl;
  auto vecd = api.queryDevices("top2");
  for (auto &it : vecd) {
    it->print(cout);
    cout << endl;
  }
  cout << endl;
  auto vecn = api.queryDevicesWithNetlistNode("top2", "n1");
  for (auto &it : vecn) {
    it->print(cout);
    cout << endl;
  }
  return 0;
}
