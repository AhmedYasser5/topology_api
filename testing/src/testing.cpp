#include "testing.h"
#include "Device.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>

#define RANDOM_STRENGTH 10

using std::cout;
using std::endl;
using std::ifstream;
using std::make_pair;
using std::mt19937;
using std::ofstream;
using std::pair;
using std::static_pointer_cast;
using std::string;
using std::uniform_int_distribution;
using std::unordered_set;
using namespace topology;

TopologyTest::TopologyTest()
    : rnd(std::chrono::steady_clock::now().time_since_epoch().count()) {}

string TopologyTest::getRandomString(const char &start, const char &end) {
  string s;
  int len = uniform_int_distribution<int>(1, RANDOM_STRENGTH * 1.5)(rnd);
  bool first = true;
  while (len--) {
    s += '"';
    while (s.back() == '"' || (first && s.back() == '0'))
      s.back() = char(uniform_int_distribution<int>(start, end)(rnd));
    first = false;
  }
  return s;
}

shared_ptr<TopologyAPI> TopologyTest::getRandomTopologyAPI() {
  shared_ptr<TopologyAPI> api(new TopologyAPI);

  unordered_set<string> topName;
  int numberOfTopologies =
      uniform_int_distribution<int>(1, RANDOM_STRENGTH)(rnd);
  for (int i = 0; i < numberOfTopologies; i++) {
    string id;
    while (true) {
      id = getRandomString('A', 'Z');
      if (topName.find(id) == topName.end()) {
        topName.insert(id);
        break;
      }
    }
    id = "./JSONs/" + id;
    topologyNetlists &topnet = api->tops[id];
    shared_ptr<Topology> &top = topnet.top;
    top = shared_ptr<Topology>(new Topology(id));

    int numberOfComponents =
        uniform_int_distribution<int>(1, RANDOM_STRENGTH * 0.5)(rnd);
    for (int j = 0; j < numberOfComponents; j++) {
      shared_ptr<Device> dev(new Device(getRandomString('A', 'Z')));
      top->components.emplace_back(dev);
      dev->id = getRandomString('A', 'Z');

      int numberOfNetlists = uniform_int_distribution<int>(2, 4)(rnd);
      for (int k = 0; k < numberOfNetlists; k++) {
        if (!k || uniform_int_distribution<int>(0, 2)(rnd))
          dev->netlist.emplace_back(getRandomString('A', 'Z'),
                                    getRandomString('A', 'Z'));
        else {
          int index = uniform_int_distribution<int>(
              0, (int)top->components.size() - 1)(rnd);
          shared_ptr<Device> otherDev =
              static_pointer_cast<Device>(top->components[index]);
          int net = uniform_int_distribution<int>(
              0, (int)otherDev->netlist.size() - 1)(rnd);
          dev->netlist.emplace_back(getRandomString('A', 'Z'),
                                    otherDev->netlist[net].second);
        }
      }

      int numberOfProperties =
          uniform_int_distribution<int>(1, RANDOM_STRENGTH * 0.5)(rnd);
      for (int k = 0; k < numberOfProperties; k++) {
        dev->properties.emplace_back(getRandomString('A', 'Z'),
                                     vector<pair<string, string>>());
        const static string values[] = {"default", "min", "max"};
        for (int p = 0; p < 3; p++)
          dev->properties.back().second.emplace_back(
              values[p],
              getRandomString('0', '9') + '.' + getRandomString('0', '9'));
      }
    }
    topnet.insertNetlists();
  }
  return api;
}

void TopologyTest::validate(const string &firstFileName,
                            const string &secondFileName) {
  ifstream functionIn(firstFileName), correctIn(secondFileName);
  while (true) {
    char functionChar, correctChar;
    functionIn >> functionChar;
    correctIn >> correctChar;
    if (functionIn.eof() || correctIn.eof())
      break;
    assert(functionChar == correctChar);
  }
  assert(functionIn.eof() && correctIn.eof());
}

void TopologyTest::validate(shared_ptr<Topology> firstTop,
                            shared_ptr<Topology> secondTop) {
  assert(firstTop->id == secondTop->id);
  int n = (int)firstTop->components.size();
  assert(n == (int)secondTop->components.size());
  for (int i = 0; i < n; i++) {
    shared_ptr<Device> firstDev =
        static_pointer_cast<Device>(firstTop->components[i]);
    shared_ptr<Device> secondDev =
        static_pointer_cast<Device>(secondTop->components[i]);
    assert(firstDev->type == secondDev->type);
    assert(firstDev->netlist == secondDev->netlist);
    assert(firstDev->properties == secondDev->properties);
  }
}

void TopologyTest::testReadJSON() {
  cout << "\ntestReadJSON() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI();
  for (const pair<string, topologyNetlists> &it : api->tops) {
    bool ok = api->writeJSON(it.first);
    if (!ok) {
      cout << "failed to write" << it.first + ".json" << endl;
      exit(-1);
    }
    TopologyAPI testAPI;
    ok = testAPI.readJSON(it.first + ".json");
    if (!ok) {
      cout << "failed to read " << it.first + ".json" << endl;
      exit(-2);
    }
    validate(testAPI.tops.begin()->second.top, it.second.top);
  }
  cout << "testReadJSON() has passed\n";
}

void TopologyTest::testWriteJSON() {
  cout << "\ntestWriteJSON() has started\n";
  shared_ptr<Topology> top(new Topology("./JSONs/top1"));
  top->components.emplace_back(new Device("resistor"));
  shared_ptr<Device> dev = static_pointer_cast<Device>(top->components.back());
  dev->id = "res1";
  dev->properties.emplace_back(
      "resistance", vector<pair<string, string>>({make_pair("default", "100"),
                                                  make_pair("min", "10"),
                                                  make_pair("max", "1000")}));
  dev->netlist = vector<pair<string, string>>(
      {make_pair("t1", "vdd"), make_pair("t2", "n1")});
  top->components.emplace_back(new Device("nmos"));
  dev = static_pointer_cast<Device>(top->components.back());
  dev->id = "m1";
  dev->properties.emplace_back(
      "m(l)", vector<pair<string, string>>({make_pair("default", "1.5"),
                                            make_pair("min", "1"),
                                            make_pair("max", "2")}));
  dev->netlist = vector<pair<string, string>>({make_pair("drain", "n1"),
                                               make_pair("gate", "vin"),
                                               make_pair("source", "vss")});
  TopologyAPI api;
  topologyNetlists &topnet = api.tops[top->id];
  topnet.top = top;
  topnet.insertNetlists();
  bool ok = api.writeJSON(top->id);
  if (!ok) {
    cout << "Failed to write ./JSONs/topology.json" << endl;
    exit(-1);
  }
  validate(top->id + ".json", "./JSONs/topology.json");
  cout << "testWriteJSON() has passed\n";
}

void TopologyTest::testQueryTopologies() {
  cout << "\ntestQueryTopologies() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI();
  vector<shared_ptr<Topology>> tops = api->queryTopologies();
  assert(api->tops.size() == tops.size());
  for (shared_ptr<Topology> &top : tops) {
    unordered_map<string, topologyNetlists>::const_iterator it =
        api->tops.find(top->id);
    assert(it != api->tops.end() && it->second.top == top);
  }
  cout << "testQueryTopologies() has passed\n";
}

void TopologyTest::testDeleteTopology() {
  cout << "\ntestDeleteTopology() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI();
  vector<string> ids;
  for (const pair<string, topologyNetlists> &it : api->tops)
    ids.emplace_back(it.first);
  std::shuffle(ids.begin(), ids.end(), rnd);

  for (const string &it : ids) {
    api->deleteTopology(it);
    assert(api->tops.find(it) == api->tops.end());
  }
  cout << "testDeleteTopology() has passed\n";
}

void TopologyTest::testQueryDevices() {
  cout << "\ntestQueryDevices() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI();
  for (const pair<string, topologyNetlists> &topnet : api->tops) {
    vector<shared_ptr<Device>> devs = api->queryDevices(topnet.second.top->id);
    int i = 0;
    for (const shared_ptr<Device> &dev : devs) {
      assert(dev ==
             static_pointer_cast<Device>(topnet.second.top->components[i]));
      i++;
    }
  }
  cout << "testQueryDevices() has passed\n";
}

void TopologyTest::testQueryDevicesWithNetlistNode() {
  cout << "\ntestQueryDevicesWithNetlistNode() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI();
  for (const pair<string, topologyNetlists> &topnet : api->tops) {
    unordered_set<string> nets;
    for (shared_ptr<Topology> top : topnet.second.top->components) {
      shared_ptr<Device> dev = static_pointer_cast<Device>(top);
      for (const pair<string, string> &net : dev->netlist)
        nets.insert(net.second);
    }
    for (const string &net : nets) {
      vector<shared_ptr<Device>> devs =
          api->queryDevicesWithNetlistNode(topnet.second.top->id, net);
      for (shared_ptr<Device> dev : devs) {
        bool hasNetlist = false;
        for (const pair<string, string> &devnet : dev->netlist)
          if (devnet.second == net) {
            hasNetlist = true;
            break;
          }
        assert(hasNetlist);
      }
    }
  }
  cout << "testQueryDevicesWithNetlistNode() has passed\n";
}

void TopologyTest::testAll() {
  cout << "Testing has started\n";
  testWriteJSON();
  testReadJSON();
  testQueryTopologies();
  testDeleteTopology();
  testQueryDevices();
  testQueryDevicesWithNetlistNode();
  cout << "\nAll tests have passed\n";
}
