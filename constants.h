#pragma once

//#define FFA

#ifdef FFA
constexpr int MAX_PACKETS_IN_SYSTEM = 3;
#define GAME_TYPE "ffa"
#else 
constexpr int MAX_PACKETS_IN_SYSTEM = 4;
#define GAME_TYPE "1v1"
#endif
constexpr int NSLOTS = 10;
constexpr int NROUTERS = 14;
