#pragma once

#include "API.h"
#include "Topology.h"
#include <memory>
#include <random>
#include <string>

namespace topology {

using std::mt19937;
using std::shared_ptr;
using std::string;

class TopologyTest {
private:
  typedef TopologyAPI::topologyNetlists topologyNetlists;

  mt19937 rnd;

  string getRandomString(const char &start, const char &end);
  shared_ptr<TopologyAPI> getRandomTopologyAPI();
  void validate(const string &firstFileName, const string &secondFileName);
  void validate(shared_ptr<Topology> firstTop, shared_ptr<Topology> secondTop);

public:
  TopologyTest();
  void testReadJSON();
  void testWriteJSON();
  void testQueryTopologies();
  void testDeleteTopology();
  void testQueryDevices();
  void testQueryDevicesWithNetlistNode();
  void testAll();
};
} // namespace topology
