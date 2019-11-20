#pragma once

#include <vector>
#include <string>
#include "commands.h"
#include "turnData.h"
#include <map>
#include <memory>
#include "Scores.h"

using namespace std;


template<typename T> 
pair<double,T> bestInVector(const vector<pair<double, T>>& v, const T& def) {
    auto best = def;
    double best_score = -1e22;
    for (const auto& it : v)
    {
        if (it.first > best_score)
        {
            best_score = it.first;
            best = it.second;
        }
    }
    return { best_score,best };
}

template<typename T>
void normalizeVector(vector<pair<double, T>>& v) {
    double ss = 0;
    for(const auto& [x,y] : v)
    {
        ss += x;
    }
    if (ss <= 0.0) return;
    for (auto& [x, y] : v)
    {
        x /= ss;
    }

}


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
    static std::vector<std::pair<double, Command> > getMovementScoresComplex(const NetworkState& state, const std::vector<Command>& moves, int ourId, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring);
    static std::vector<std::pair<double, Command> > getMovementScoresSimple(const NetworkState& state, const std::vector<Command>& moves, int ourId, scoringFuction scoring);

};

class RandomNetworkMovements : public Player {
public:
    RandomNetworkMovements() : Player(-2) {
    }

    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const override;

};

class RandomPlayer : public Player {
public:
    RandomPlayer(int id) :Player(id) {

    }
    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const override;
};


class RandomPlayerCreatePref : public Player {
public:
    RandomPlayerCreatePref(int id) :Player(id) {

    }
    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const override
    {
        auto cmds = getPossibleMoves(turnData, id, false, false, 666);
        if (cmds.empty())
            cmds = getPossibleMoves(turnData, id, true, true);
        std::vector<std::pair<double, Command> > res;
        for (const auto& c : cmds) {
            res.push_back({ 1.0 / cmds.size(), c });
        }
        return res;
    }
};


class ThinkingPlayer : public Player {
public:
    ThinkingPlayer(int id) :Player(id) {

    }

    virtual std::vector<std::pair<double, Command> > getScoredMoves(const NetworkState& turnData) const {
        auto cmds = getPossibleMoves(turnData, id, false, false, 666);
        if (cmds.empty())
            cmds = getPossibleMoves(turnData, id, true, true);

        auto moves = Player::getMovementScoresSimple(turnData, cmds, id, Scores::distance_based_scoring_change_handling);
        return moves;
    }

    virtual std::vector<std::pair<double, Command> > transformScoresToProb(std::vector<std::pair<double, Command> >& moves) const
    {
        // 50% is distributed evenly and 50% is distibuted normally;
        std::vector<std::pair<double, Command> > res;
        for (const auto& [score, cmd] : moves) {
            res.push_back({ score, cmd });
        }
        normalizeVector(res);
        for (auto& [p, c] : res) {
            p += 1.0 / res.size();
        }
        normalizeVector(res);
        return res;

    }

    virtual std::vector<std::pair<double, Command> > getProbableMoves(const NetworkState& turnData) const override
    {
        auto moves = getScoredMoves(turnData);
        if (moves.empty()) {
            moves.push_back({ 1.,Command::Pass() });
        }
        auto res = transformScoresToProb(moves);
        normalizeVector(res);
        return res;
    }
};