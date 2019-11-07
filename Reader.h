#pragma once

#include <string>
#include <vector>
#include "turnData.h"

struct Reader {
    // An optional field that contains a message from the server to our previous command. For example: 'PREVIOUS Wrong solution string.'
    std::string previous;

    bool readData(NetworkState&, GameContext&);
};
