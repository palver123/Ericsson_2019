#pragma once

#include <iostream>
#include <string>
class IStrategy
{
 public:
  virtual std::string step() {
    static int tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
  }

  virtual ~IStrategy() = default;

};
