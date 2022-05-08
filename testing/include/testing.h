#pragma once

#include "API.h"
#include "Device.h"
#include "Topology.h"
#include <memory>
#include <random>

namespace topology {

using std::mt19937;
using std::shared_ptr;

class TopologyTest : public Topology, public Device, public TopologyAPI {
private:
  string getRandomString(mt19937 &rnd, const char &start = '!',
                         const char &end = '~') const;
  shared_ptr<TopologyAPI> getRandomTopologyAPIs() const;

public:
  bool testReadJSON() const;
  bool testWriteJSON() const;
  bool testQueryTopologies() const;
  bool testDeleteTopology() const;
  bool testQueryDevices() const;
  bool testQueryDevicesWithNetlistNode() const;
  bool testAll() const;
};
} // namespace topology
