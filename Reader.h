#pragma once

#include <string>
#include <vector>
#include "turnData.h"

#define LOG_FOR_PYTHON_GUI

struct Reader {
    // An optional field that contains a message from the server to our previous command. For example: 'PREVIOUS Wrong solution string.'
    std::string previous;

#ifdef LOG_FOR_PYTHON_GUI    
    void openLogFile(int mapSeed);
#endif // LOG_FOR_PYTHON_GUI

    bool readData(NetworkState&, GameContext&);
};
