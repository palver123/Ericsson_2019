#include "Scores.h"

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
        if (d.is_request()) {
            res += score_request_dist(d.distance_from_target());
        }
        else {
            res += score_resp_dist(d.distance_from_target());
        }
    }
    return res;
}
