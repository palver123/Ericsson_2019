#include "Scores.h"
#include "simulation.h"
#include <iostream>
#include <array>
#include <set>
#include "Players.h"
using namespace std;

double Scores::distance_based_scoring_change_handling(const NetworkState& state, int playerId)
{
    const auto score_request_dist = [](double d) {
        double res = (7 - d) * 1.0;
        return res;
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
    double best_score = 0;

    vector<Command> cmds = Player::getPossibleMoves(state, playerId, true, true, 666);
    for (const auto& c : cmds) {
        best_score = max(best_score, Scores::distance_based_scoring_change_handling(simulate(state, { c }, playerId), playerId));
    }
    return basescore + best_score * bc_mul;
}
