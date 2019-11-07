#pragma once

#include "turnData.h"
#include "basics.h"
#include <string>

class IStrategy
{
 public:
  virtual std::string step(const NetworkState&, const GameContext&);
  virtual ~IStrategy() = default;


 protected:
  static int getRandom(int from, int to_exclusive);
  static ll getRandomLL(ll from, ll to_exclusive);

    int getNumberOfPlayerPackets(unsigned routerIdx) const;


    const NetworkState* actualData = nullptr;
    virtual void stepPre(const NetworkState& turnData);
};
