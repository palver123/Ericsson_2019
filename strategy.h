#pragma once

#include "turnData.h"
#include <iostream>
#include <string>
class IStrategy
{
 public:
  virtual std::string step(const Reader& turnData);
  virtual ~IStrategy() = default;

};
