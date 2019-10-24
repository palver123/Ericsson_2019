#pragma once

#include "turnData.h"
#include "basics.h"
#include <iostream>
#include <string>
class IStrategy
{
 public:
  virtual std::string step(const Reader& turnData);
  virtual ~IStrategy() = default;


 protected:
    int getRandom(int from, int to_exclusive);
    ll getRandomLL(ll from, ll to_exclusive);

};
