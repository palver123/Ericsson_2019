#include "Players.h"
#include <array>
#include <set>
#include "simulation.h"

using namespace std;

std::vector<Command> Player::getPossibleMoves(const NetworkState& turnData, int ourId, bool pass,bool move, int create_req)
{
    std::vector<Command> cmds;
    if (pass)
        cmds.push_back(Command::Pass());
    if (create_req != -1 && turnData.getNumberOfPlayerPackets(ourId,true) < MAX_PACKETS_IN_SYSTEM && !GameContext::playerPackets[ourId].receivedEmptyPacket()) {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : turnData.dataPackets)
        {
            if (data.currRouter == ourId && !data.will_disappear())
                slotTaken[data.currStoreId] = true;
        }
        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[ourId][slot])
                cmds.push_back(Command::Create(ourId, slot, create_req));
    }
    if (move) {
        set<int> possibleRouters;
        for (const auto& d : turnData.dataPackets)
            if (d.fromRouter == ourId)
                possibleRouters.insert(d.currRouter);

        for (int i : possibleRouters)
        {
            cmds.push_back(Command::Move(i, VerticalDirection::NEGATIVE));
            cmds.push_back(Command::Move(i, VerticalDirection::POSITIVE));
        }
    }
    return cmds;
}

template<typename T>
void next_idx(const vector<vector<T> >& vec2d, vector<int>& idx) {
    assert(vec2d.size() == idx.size());
    for (int i = static_cast<int>(idx.size()) - 1; i >= 0; --i) {
        ++idx[i];
        if (idx[i] < vec2d[i].size())
            return;
        if (i == 0)
            return;
        idx[i] = 0;
    }
}

std::vector<std::pair<double, Command> > Player::getMovementScoresSimple(const NetworkState& state, const std::vector<Command>& moves, int ourId, scoringFuction scoring)
{
    if (moves.empty())
        return { { 0,Command::Pass() } };
    double origScore = scoring(state, ourId);
    vector<pair< double, Command> > res;
    for (const auto& ourC : moves) {
        double score = scoring(simulate(state, { ourC }, ourId), ourId);
        res.push_back({ score - origScore, ourC });
    }
    return res;
}

std::vector<std::pair<double, Command> > Player::getMovementScoresComplex(const NetworkState& state, const std::vector<Command>& moves, int ourId, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring)
{
    if (moves.empty())
        return { { 0,Command::Pass() } };
    double origScore = scoring(state, ourId);
    vector<pair< double, Command> > res;
    for (const auto& ourC : moves) {
        if (players.size()) {
            vector<vector<pair<double, Command> > > others;
            for (int i = 0; i < static_cast<int>(players.size()); ++i)
            {
                others.push_back(players[i]->getProbableMoves(state));
            }
            double scoreSum = 0;
            double scoreDiv = 0; // Will be 1 if all players gives back proper probabilities
            for (vector<int> idxs(others.size(), 0); idxs.front() < others.front().size(); next_idx(others, idxs)) {
                double pos = 1;
                static vector<Command> cmds;
                cmds.clear();
                cmds.push_back(ourC);
                for (int i = 0; i < idxs.size(); ++i) {
                    cmds.push_back(others[i][idxs[i]].second);
                    pos *= others[i][idxs[i]].first;
                }
                double score = scoring(simulate(state, cmds, ourId), ourId);
                scoreSum += (score - origScore) * pos;
                scoreDiv += pos;
            }
            if (scoreDiv > 0)
                scoreSum /= scoreDiv;
            else
                scoreSum = 0;
            res.push_back({ scoreSum, ourC });
        }
        else {
            double score = scoring(simulate(state, { ourC }, ourId), ourId);
            res.push_back({ score, ourC });
        }
    }
    return res;
}

std::vector<std::pair<double, Command> > RandomNetworkMovements::getProbableMoves(const NetworkState& turnData) const
{
    std::vector<std::pair<double, Command> > res;
    for (int r = 0; r < NROUTERS; ++r)
    {
        res.push_back({ 0.5 / NROUTERS ,Command::Move(r, VerticalDirection::NEGATIVE) });
        res.push_back({ 0.5 / NROUTERS ,Command::Move(r, VerticalDirection::POSITIVE) });
    }
    return res;
}

std::vector<std::pair<double, Command> > RandomPlayer::getProbableMoves(const NetworkState& turnData) const
{
    auto cmds = getPossibleMoves(turnData, id, true, true, 666);
    std::vector<std::pair<double, Command> > res;
    for (const auto& c : cmds) {
        res.push_back({ 1.0 / cmds.size(), c });
    }
    return res;
}
