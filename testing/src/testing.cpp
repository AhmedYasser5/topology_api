#include "testing.h"
#include "Device.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>

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

/* initializing random generator */
TopologyTest::TopologyTest()
    : rnd(std::chrono::steady_clock::now().time_since_epoch().count()) {}

/* get a random string with a length proportional to RANDOM_STRENGTH */
string TopologyTest::getRandomString(const char &start, const char &end,
                                     const int &RANDOM_STRENGTH) {
  string s;
  int len = uniform_int_distribution<int>(1, RANDOM_STRENGTH * 0.5)(rnd);
  bool first = true;
  while (len--) {
    s += '"';
    while (s.back() == '"' || (first && s.back() == '0'))
      s.back() = char(uniform_int_distribution<int>(start, end)(rnd));
    first = false;
  }
  return s;
}

/* get a random api with strings that have lengths proportional to
 * RANDOM_STRENGTH */
shared_ptr<TopologyAPI>
TopologyTest::getRandomTopologyAPI(const char &start, const char &end,
                                   const int &RANDOM_STRENGTH) {
  shared_ptr<TopologyAPI> api(new TopologyAPI);
  unordered_set<string> topName;

  int numberOfTopologies =
      uniform_int_distribution<int>(1, RANDOM_STRENGTH)(rnd);
  for (int i = 0; i < numberOfTopologies; i++) {
    string id;
    // getting a unique id among all previously generated ids
    while (true) {
      id = getRandomString(start, end, RANDOM_STRENGTH);
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
      // create a device using a random "type"
      shared_ptr<Device> dev(
          new Device(getRandomString(start, end, RANDOM_STRENGTH)));
      top->components.emplace_back(dev);
      dev->id = getRandomString(start, end, RANDOM_STRENGTH);

      int numberOfNetlists =
          uniform_int_distribution<int>(2, RANDOM_STRENGTH * 0.5)(rnd);
      for (int k = 0; k < numberOfNetlists; k++) {
        // don't choose a net from previously generated nets if it is the first
        // net in the device
        if (!k || uniform_int_distribution<int>(0, 2)(rnd)) {
          // choose a random net
          dev->netlist.emplace_back(
              getRandomString(start, end, RANDOM_STRENGTH),
              getRandomString(start, end, RANDOM_STRENGTH));
        } else {
          // or choose one of the previously generated nets
          int index = uniform_int_distribution<int>(
              0, (int)top->components.size() - 1)(rnd);

          shared_ptr<Device> otherDev =
              static_pointer_cast<Device>(top->components[index]);

          int net = uniform_int_distribution<int>(
              0, (int)otherDev->netlist.size() - 1)(rnd);

          dev->netlist.emplace_back(
              getRandomString(start, end, RANDOM_STRENGTH),
              otherDev->netlist[net].second);
        }
      }

      int numberOfProperties =
          uniform_int_distribution<int>(1, RANDOM_STRENGTH * 0.5)(rnd);
      for (int k = 0; k < numberOfProperties; k++) {
        // create a random property name
        dev->properties.emplace_back(
            getRandomString(start, end, RANDOM_STRENGTH),
            vector<pair<string, string>>());

        // put 3 values for each property, but the numbers are of no meaning
        const static string values[] = {"default", "min", "max"};
        for (int p = 0; p < 3; p++)
          dev->properties.back().second.emplace_back(
              values[p], getRandomString('0', '9', RANDOM_STRENGTH) + '.' +
                             getRandomString('0', '9', RANDOM_STRENGTH));
      }
    }

    topnet.insertNetlists();
  }
  return api;
}

/* make sure that two files have the same topology */
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

/* make sure that two apis hold the same data */
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
  for (int i = 0; i < 4; i++) {
    shared_ptr<TopologyAPI> api;
    if (i == 1) // create a random api with more than 1 id
      api = getRandomTopologyAPI('d', 'i', 20);
    else if (i == 2) // create a random api with more than 1 type
      api = getRandomTopologyAPI('e', 'y', 20);
    else if (i == 3) // create a random api
      api = getRandomTopologyAPI('a', 'z', 20);
    else // create a valid random api
      api = getRandomTopologyAPI('A', 'Z');

    for (const pair<string, topologyNetlists> &it : api->tops) {

      bool notOK = false, notAPI = false;
      for (shared_ptr<Topology> comp : it.second.top->components) {
        shared_ptr<Device> dev = static_pointer_cast<Device>(comp);

        for (const pair<string, vector<pair<string, string>>> &prop :
             dev->properties) {
          if (prop.first == "id" || prop.first == "type" ||
              prop.first == "components") {
            notOK = true;
            break;
          }
          // if it contains a property named netlist, then this api can't be
          // checked for validity, and hence, it is not an api
          if (prop.first == "netlist") {
            notAPI = true;
            break;
          }
        }

        if (notAPI || notOK)
          break;
      }
      // in this case, create a new random api
      if (notAPI) {
        i--;
        break;
      }

      bool isOK = api->writeJSON(it.first);
      assert(isOK == true);

      TopologyAPI testAPI;
      isOK = testAPI.readJSON(it.first + ".json");
      // if this is a valid api (notOK == 0), then isOK should be true, and vice
      // versa
      assert(isOK == !notOK);
      if (isOK)
        validate(testAPI.tops.begin()->second.top, it.second.top);
    }
  }
  cout << "testReadJSON() has passed\n";
}

/* this function tests the api writeJSON function by writing to a file the same
 * information found in topology.json */
void TopologyTest::testWriteJSON() {
  cout << "\ntestWriteJSON() has started\n";

  // the id is changed because of relative location of the file
  shared_ptr<Topology> top(new Topology("./JSONs/top1"));

  // the first component: resistor
  top->components.emplace_back(new Device("resistor"));
  shared_ptr<Device> dev = static_pointer_cast<Device>(top->components.back());
  dev->id = "res1";
  dev->properties.emplace_back(
      "resistance", vector<pair<string, string>>({make_pair("default", "100"),
                                                  make_pair("min", "10"),
                                                  make_pair("max", "1000")}));
  dev->netlist = vector<pair<string, string>>(
      {make_pair("t1", "vdd"), make_pair("t2", "n1")});

  // the second component: transistor
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
  assert(ok == true);
  validate(top->id + ".json", "./JSONs/topology.json");

  cout << "testWriteJSON() has passed\n";
}

void TopologyTest::testQueryTopologies() {
  cout << "\ntestQueryTopologies() has started\n";
  shared_ptr<TopologyAPI> api = getRandomTopologyAPI('A', 'Z');
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

  shared_ptr<TopologyAPI> api = getRandomTopologyAPI('A', 'Z');
  vector<string> ids;
  for (const pair<string, topologyNetlists> &it : api->tops)
    ids.emplace_back(it.first);
  // delete topologies at random
  std::shuffle(ids.begin(), ids.end(), rnd);

  for (const string &it : ids) {
    api->deleteTopology(it);
    assert(api->tops.find(it) == api->tops.end());
  }

  cout << "testDeleteTopology() has passed\n";
}

void TopologyTest::testQueryDevices() {
  cout << "\ntestQueryDevices() has started\n";

  shared_ptr<TopologyAPI> api = getRandomTopologyAPI('A', 'Z');
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

  shared_ptr<TopologyAPI> api = getRandomTopologyAPI('A', 'Z');
  for (const pair<string, topologyNetlists> &topnet : api->tops) {
    // inserting all nets of a topology in a hash set
    unordered_set<string> nets;
    for (shared_ptr<Topology> top : topnet.second.top->components) {
      shared_ptr<Device> dev = static_pointer_cast<Device>(top);
      for (const pair<string, string> &net : dev->netlist)
        nets.insert(net.second);
    }

    // check every net, check every device and make sure it contains this net
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
        assert(hasNetlist == true);
      }
    }
  }

  cout << "testQueryDevicesWithNetlistNode() has passed\n";
}

void TopologyTest::testAll() {
  cout << "Testing has started...\n";

  testWriteJSON();
  // readJSON depends on writeJSON, so readJSON comes second in order
  testReadJSON();
  testQueryTopologies();
  testDeleteTopology();
  testQueryDevices();
  testQueryDevicesWithNetlistNode();

  cout << "\nAll tests have passed\n";
}
