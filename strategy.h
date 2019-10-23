#pragma once

#include <iostream>
#include <string>
class IStrategy
{
 protected:
  virtual std::string step();
  virtual ~IStrategy() = default;

};
