#pragma once

#include <iostream>
#include <string>
class IStrategy
{
 public:
  virtual std::string step();
  virtual ~IStrategy() = default;

};
