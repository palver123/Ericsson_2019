#include "Scores.h"
#include "simulation.h"
#include <iostream>
#include <array>
#include <set>

using namespace std;

double Scores::distance_based_scoring(const NetworkState& state)
{
    const auto score_request_dist = [](double d) {
        return (7 - d) * 1.0;
    };
    const auto score_resp_dist = score_request_dist;
    const auto arrived_request = [](double count) {
        return count * 10;
    };
    const auto arrived_resp = [](double count) {
        return count * 10;
    };

    double res = .0;
    res += arrived_request(state.simuInfo.additionalArrivedReq);
    res += arrived_resp(state.simuInfo.additionalArrivedResp);
    for(const auto& d : state.dataPackets) {
        if (d.fromRouter != GameContext::ourId)
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

double Scores::distance_based_scoring_change_handling(const NetworkState& state)
{
    double creation_bonus = 0; // Dont change, possitive 2 value causes minimum to drop to 424 from 590 :(
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
        if (d.fromRouter != GameContext::ourId || (GameContext::hasReceivedEmptyMessage() && d.messageId >= GameContext::_lowestEmptyAnswer))
            continue;
        res += creation_bonus;
        if (d.is_request()) {
            res += score_request_dist(d.distance_from_target());
        }
        else {
            res += score_resp_dist(d.distance_from_target());
        }
    }
    return res;
}

double Scores::future_seeing(const NetworkState& state)
{
    double bc_mul = /*GameContext::hasReceivedEmptyMessage() ? 1.0 :*/ 30.0;
    double basescore = Scores::distance_based_scoring_change_handling(state);
    double best_score = max(.0, Scores::distance_based_scoring_change_handling(simulate(state, {}, {})));
    if (state.getNumberOfPlayerPackets(GameContext::ourId, true) < MAX_PACKETS_IN_SYSTEM /*&& !GameContext::hasReceivedEmptyMessage()*/) {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : state.dataPackets)
        {
            if (data.currRouter == GameContext::ourId && !data.will_disappear())
                slotTaken[data.currStoreId] = true;
        }

        vector<CreateCommand> ccmds;
        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && state.routerBits[GameContext::ourId][slot])
                ccmds.push_back({ GameContext::ourId, slot, 33 });
        for (const auto& c : ccmds) {
            best_score = max(best_score, Scores::distance_based_scoring_change_handling(simulate(state, { c }, {})));
        }
    }


    set<int> possibleRouters;
    for (const auto& d : state.dataPackets)
        if (d.fromRouter == GameContext::ourId)
            possibleRouters.insert(d.currRouter);

    vector<MoveCommand> cmds;

    for (int i : possibleRouters)
    {
        cmds.push_back(MoveCommand{ i, VerticalDirection::NEGATIVE });
        cmds.push_back(MoveCommand{ i, VerticalDirection::POSITIVE });
    }
    for (const auto& c : cmds) {
        double score = Scores::distance_based_scoring_change_handling(simulate(state, {}, { c }));
        best_score = max(best_score, score);
    }
    return basescore + best_score * bc_mul;
}
