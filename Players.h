#pragma once

#include <vector>
#include <string>
#include "commands.h"
#include "turnData.h"
#include <map>

class Player {
public:
    int id;
    Player(int id = -1) :id(id) {
        assert(id != -1);
    }
    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const {
        return {};
    };
    static std::vector<Command> getPossibleMoves(const NetworkState& turnData, int ourId, bool pass, bool move, int create_req = -1);

};

class RandomNetworkMovements : public Player {
public:
    RandomNetworkMovements() : Player(-2) {
    }

    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const override
    {
        std::vector<std::pair<double, Command> > res;
        for (int r = 0; r < NROUTERS; ++r)
        {
            res.push_back({ 0.5 / NROUTERS ,Command::Move(r, VerticalDirection::NEGATIVE) });
            res.push_back({ 0.5 / NROUTERS ,Command::Move(r, VerticalDirection::POSITIVE) });
        }
    }

};
