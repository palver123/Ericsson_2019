#include "Scores.h"
#include "simulation.h"
#include <iostream>
#include <array>
#include <set>

using namespace std;

double Scores::distance_based_scoring_change_handling(const NetworkState& state, int playerId)
{
    const auto score_request_dist = [](double d) {
        return (7 - d) * 1.0;
    };
    const auto score_resp_dist = score_request_dist;
    const auto arrived_request = [](double count) {
        return count * 7;
    };
    const auto arrived_resp = [](double count) {
        return count * 7;
    };

    double res = .0;
    res += arrived_request(state.simuInfo.additionalArrivedReq);
    res += arrived_resp(state.simuInfo.additionalArrivedResp);
    for (const auto& d : state.dataPackets) {
        if (d.fromRouter != playerId || (GameContext::hasReceivedEmptyMessage() && d.messageId >= GameContext::_lowestEmptyAnswer))
            continue;
        if (d.is_request()) {
            res += score_request_dist(d.distance_from_target());
        }
        else {
            res += score_resp_dist(d.distance_from_target());
        }
    }
    return res;
}

double Scores::future_seeing(const NetworkState& state, int playerId)
{
    double bc_mul = /*GameContext::hasReceivedEmptyMessage() ? 1.0 :*/ 30.0;
    double basescore = Scores::distance_based_scoring_change_handling(state, playerId);
    double best_score = max(.0, Scores::distance_based_scoring_change_handling(simulate(state, {}, playerId),playerId));
    if (state.getNumberOfPlayerPackets(playerId, true) < MAX_PACKETS_IN_SYSTEM /*&& !GameContext::hasReceivedEmptyMessage()*/) {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : state.dataPackets)
        {
            if (data.currRouter == playerId && !data.will_disappear())
                slotTaken[data.currStoreId] = true;
        }

        vector<Command> ccmds;
        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && state.routerBits[playerId][slot])
                ccmds.push_back(Command::Create(playerId, slot, 33 ));
        for (const auto& c : ccmds) {
            best_score = max(best_score, Scores::distance_based_scoring_change_handling(simulate(state, { c }, playerId),playerId));
        }
    }


    set<int> possibleRouters;
    for (const auto& d : state.dataPackets)
        if (d.fromRouter == playerId)
            possibleRouters.insert(d.currRouter);

    vector<Command> cmds;

    for (int i : possibleRouters)
    {
        cmds.push_back(Command::Move(i, VerticalDirection::NEGATIVE ));
        cmds.push_back(Command::Move(i, VerticalDirection::POSITIVE ));
    }
    for (const auto& c : cmds) {
        double score = Scores::distance_based_scoring_change_handling(simulate(state, { c }, playerId), playerId);
        best_score = max(best_score, score);
    }
    return basescore + best_score * bc_mul;
}
