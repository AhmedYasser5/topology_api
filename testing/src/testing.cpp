#include "testing.h"
#include <chrono>
#include <iostream>
#include <random>
#include <string>

using std::cout;
using std::mt19937;
using std::string;
using std::uniform_int_distribution;
using namespace topology;

string TopologyTest::getRandomString(mt19937 &rnd, const char &start,
                                     const char &end) const {
  string s;
  int len = uniform_int_distribution<int>(1, 15)(rnd);
  while (len--)
    s += char(uniform_int_distribution<int>(start, end)(rnd));
  return s;
}

shared_ptr<TopologyAPI> TopologyTest::getRandomTopologyAPIs() const {
  const static string values[] = {"default", "min", "max"};
  mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
  shared_ptr<TopologyAPI> api(new TopologyAPI);
  int numberOfTopologies = uniform_int_distribution<int>(1, 10)(rnd);
  for (int i = 0; i < numberOfTopologies; i++) {
    string id = getRandomString(rnd);
    shared_ptr<Topology> &top = api->tops[id];
    top = shared_ptr<Topology>(new Topology(id));
    int numberOfComponents = uniform_int_distribution<int>(1, 5)(rnd);
    for (int j = 0; j < numberOfComponents; j++) {
      shared_ptr<Device> dev(new Device(getRandomString(rnd)));
      top->components.emplace_back(dev);
      dev->type = getRandomString(rnd);
      int numberOfNetlists = uniform_int_distribution<int>(1, 4)(rnd);
      for (int k = 0; k < numberOfNetlists; k++)
        dev->netlist.emplace_back(getRandomString(rnd), getRandomString(rnd));
      int numberOfProperties = uniform_int_distribution<int>(1, 3)(rnd);
      for (int k = 0; k < numberOfProperties; k++) {
        dev->properties.emplace_back(getRandomString(rnd),
                                     vector<pair<string, string>>());
        for (int p = 0; p < 3; p++)
          dev->properties.back().emplace_back(
              values[p], getRandomString(rnd, '0', '9') + '.' +
                             getRandomString(rnd, '0', '9'));
      }
    }
  }
  return api;
}

void TopologyTest::testReadJSON() const {}

void TopologyTest::testWriteJSON() const {}

void TopologyTest::testQueryTopologies() const {}

void TopologyTest::testDeleteTopology() const {}

void TopologyTest::testQueryDevices() const {}

void TopologyTest::testQueryDevicesWithNetlistNode() const {}

void TopologyTest::testAll() const {
  cout << "Testing has started\n";
  testWriteJSON();
  testReadJSON();
  testQueryTopologies();
  testDeleteTopology();
  testQueryDevices();
  testQueryDevicesWithNetlistNode();
  cout << "\nAll tests have passed";
}
