#include "simulation.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
using namespace std;

namespace {

    void create_occupation_map(NetworkState& state)
    {
        auto& occuMap = state.simuInfo.routerBitsOccupied;
        for(int i = 0; i<NROUTERS; ++i) {
            for (int j = 0; j < NSLOTS; ++j)
                occuMap[i][j] = !state.routerBits[i][j];
        }
        for(const auto& d: state.dataPackets)
            occuMap[d.currRouter][d.currStoreId] = true;        
    }

    void handle_arrived_packets(NetworkState& state, int ourId)
    {
        static vector<Data> __additionalPackakes;
        vector<Data>& additionalPackakes = __additionalPackakes;
        additionalPackakes.clear();
        state.dataPackets.erase(std::remove_if(state.dataPackets.begin(), state.dataPackets.end(), [&](const Data& data) {
            if (data.currRouter != data.toRouter) return false;
            if (data.toRouter != data.fromRouter) {
                Data nd = data;
                nd.dir = Dir::opposite(data.dir);
                nd.toRouter = data.fromRouter;
                __additionalPackakes.push_back(nd);
                if (data.fromRouter == ourId)
                    ++state.simuInfo.additionalArrivedReq;
                else 
                    ++state.simuInfo.additionalEnemyArrivedReq;
            }
            else {
                if (data.fromRouter == ourId)
                    ++state.simuInfo.additionalArrivedResp;
                else
                    ++state.simuInfo.additionalEnemyArrivedResp;
                state.simuInfo.routerBitsOccupied[data.currRouter][data.currStoreId] = false;
            }
            return true;
            }), state.dataPackets.end());
        state.dataPackets.insert(state.dataPackets.end(), additionalPackakes.begin(), additionalPackakes.end());
    }

    void add_new_packets(NetworkState& state, const vector<Command>& cmds)
    {
        for (const auto& genc : cmds) {
            if (genc.t != Command::CREATE)
                continue;
            const auto& c = genc.i.create;
            if (c.routerId == NROUTERS) {
                std::cerr << "Invalid router id in CreateCommand during simulation";
                continue;
            }
            if (!state.simuInfo.routerBitsOccupied[c.routerId][c.storeId])
            {
                state.simuInfo.routerBitsOccupied[c.routerId][c.storeId] = true;
                Data data;
                data.currRouter = c.routerId;
                data.fromRouter = c.routerId;
                data.toRouter = Router::GetPairOfRouter(c.routerId);
                data.dataIndex = c.packageId;
                data.uperPrio = c.upPrio;
                // data.messageId No one cares
                data.currStoreId = c.storeId;
                data.dir = state.nextDir[c.routerId];
                state.nextDir[c.routerId] = Dir::opposite(state.nextDir[c.routerId]);
                state.dataPackets.push_back(data);
            }
        }
    }

    void move_router_pos(NetworkState& state, int idx, int offset) {
        std::array<bool, NSLOTS> newBits;
        for(int i = 0; i < NSLOTS; ++i) {
            newBits[(i + offset) % NSLOTS] = state.routerBits[idx][i];
        }
        state.routerBits[idx] = newBits;
        for (int i = 0; i < NSLOTS; ++i) {
            newBits[(i + offset) % NSLOTS] = state.simuInfo.routerBitsOccupied[idx][i];
        }
        state.simuInfo.routerBitsOccupied[idx] = newBits;
        for(auto& d : state.dataPackets)
        {
            if (d.currRouter != idx) continue;
            d.currStoreId = (d.currStoreId + offset) % NSLOTS;
        }
    }

    void move_router_neg(NetworkState& state, int idx, int offset_dd) {
        for (int dummy = 0; dummy < std::abs(offset_dd); ++dummy)
        {
            std::array<bool, NSLOTS> newBits;
            for (int i = 0; i < NSLOTS; ++i) {
                newBits[i] = state.routerBits[idx][(i +1) % NSLOTS];
            }
            state.routerBits[idx] = newBits;
            for (int i = 0; i < NSLOTS; ++i) {
                newBits[i] = state.simuInfo.routerBitsOccupied[idx][(i + 1) % NSLOTS];
            }
            state.simuInfo.routerBitsOccupied[idx] = newBits;

            for (auto& d : state.dataPackets)
            {
                if (d.currRouter != idx) continue;
                if (d.currStoreId == 0 && !state.simuInfo.routerBitsOccupied[idx][0]) {
                    state.simuInfo.routerBitsOccupied[idx][0] = true;
                    state.simuInfo.routerBitsOccupied[idx][NSLOTS - 1] = false;
                }
                else {
                    d.currStoreId = (d.currStoreId + NSLOTS - 1) % NSLOTS;
                }

            }
        }
    }

    void apply_move_commands(NetworkState& state, const vector<Command>& cmds)
    {
        std::array<int, NROUTERS> dirs{};
        for (const auto& genc : cmds) {
            if (genc.t != Command::MOVE)
                continue;
            const auto& c = genc.i.move;
            dirs[c.routerId] += Dir::dirToint(c.dir);
        }
        for (int idx = 0; idx < NROUTERS; ++idx) {
            int offset = dirs[idx];
            if (offset == 0) continue;
            if (offset > 0) {
                move_router_pos(state, idx, offset);
            } else {
                move_router_neg(state, idx, offset);
            }
        }
    }

    struct Step
    {
        char msgIdx;
        char vdir = 0;
        char hdir = 0;
    };

    bool step_packets(NetworkState& state)
    {
        auto& occuMap = state.simuInfo.routerBitsOccupied;
        vector<Step> steps;
        bool movingHappend = false;
        auto& datas = state.dataPackets;
        steps.clear();
        for(int i = 0; i<static_cast<int>(datas.size()); ++i) {
            const auto& d = datas[i];
            if (d.toRouter == d.currRouter)
                continue; // Frozen

            const auto& occupied = occuMap[d.currRouter];
            int dir = Dir::dirToint(d.dir);
            if (d.uperPrio) {
                if (d.currStoreId < (NSLOTS -1) && !occupied[d.currStoreId + 1]) {// Can move inside router
                    steps.push_back({ static_cast<char>(i),static_cast<char>(+1),static_cast<char>(0) });
                    continue;
                }
            } else {
                if (d.currStoreId > 0 && !occupied[d.currStoreId - 1]) {// Can move inside router
                    steps.push_back({ static_cast<char>(i),static_cast<char>(-1),static_cast<char>(0) });
                    continue;
                }
            }
            bool occupiedTarget = occuMap[(d.currRouter + NROUTERS + dir) % NROUTERS][d.currStoreId];
            if (!occupiedTarget) {
                steps.push_back({ static_cast<char>(i),static_cast<char>(0),static_cast<char>(dir)});
            }
        }

        std::sort(steps.begin(), steps.end(), [&](const Step& a, const Step& b)->bool {
                if (a.vdir && !b.vdir) return true;
                if (!a.vdir && b.vdir) return false;
                if (a.vdir) {
                    return a.vdir > b.vdir; // Doesnt really matter;
                } else {
                    return datas[a.msgIdx].currRouter < datas[b.msgIdx].currRouter; // New Rule applied (no exception for n-1 -> 0
                }
            });

        for(const auto& s : steps) {
            auto& data = datas[s.msgIdx];
            int targetRouter = (data.currRouter + s.hdir + NROUTERS) % NROUTERS;
            int targetSlot = (data.currStoreId + s.vdir + NSLOTS) % NSLOTS; // Unnecessary modulo because, but better safe than sorry
            if (state.simuInfo.routerBitsOccupied[targetRouter][targetSlot])
                continue; // Probably someone moved in out way
            occuMap[data.currRouter][data.currStoreId] = false;
            occuMap[targetRouter][targetSlot] = true;
            data.currRouter = targetRouter;
            data.currStoreId = targetSlot;
            movingHappend = true;
        }
        return movingHappend;
    }


}
NetworkState simulate(const NetworkState& initialState, const vector<Command>& cmds, int ourId)
{
    auto retVal{ initialState };

    create_occupation_map(retVal);

    handle_arrived_packets(retVal, ourId);
    add_new_packets(retVal, cmds);
    apply_move_commands(retVal, cmds);

    while (step_packets(retVal)) {}

    return retVal;
}


namespace {
    // 63245. game
    // 0 seed map
    // tick 12
    NetworkState inititalStateBase1()
    {
        NetworkState state;
        vector<string> rmp = {
            {".x.x.xx..x"},
            {"xxxx....x."},
            {"xx.xx...x."},
            {"x.x...x.xx"},
            {"xxx.xx...."},
            {"x.x..xx.x."},
            {"xx...x.xx."},
                         
            {"x.xx...xx."},
            {"xx.x..x.x."},
            {"x.xx.x..x."},
            {".xxx..x.x."},
            {"xx..x..x.x"},
            {".x.x.xx..x"},
            {"xx.x...xx."}
        };
        for (int i = 0; i < NROUTERS; ++i) state.nextDir[i] = HorizontalDirection::LEFT;
        for(int i = 0; i< NROUTERS; ++i)
            for(int j = 0; j < NSLOTS; ++j)
                state.routerBits[i][j] = rmp[i][j] == '.';

        return state;

    }

}

NetworkState inititalState1() {
    // 63245. game
    // 0 seed map
    // tick 12
    NetworkState st = inititalStateBase1();
    {
        Data d;
        d.currRouter = 4;
        d.currStoreId = 6;
        d.fromRouter = 12;
        d.toRouter = 5;
        d.dataIndex = 3;
        d.dir = HorizontalDirection::RIGHT;
        st.dataPackets.push_back(d);
    }
    return st;
}


bool test_1() {
    NetworkState st = inititalState1();
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::NEGATIVE ));
    st = simulate(st,cmds,12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 4 || d1.currStoreId != 5) return false;
    return true;
}


bool test_2() {
    NetworkState st = inititalState1();
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::POSITIVE ));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 5 || d1.currStoreId != 7) return false;
    return true;
}

bool test_3() {
    NetworkState st = inititalState1();
    vector<Command> cmds;
    cmds.push_back(Command::Move( 4, VerticalDirection::POSITIVE ));
    cmds.push_back(Command::Move(6, VerticalDirection::POSITIVE ));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 5 || d1.currStoreId != 7) return false;
    return true;
}

bool test_4() {
    NetworkState st = inititalState1();
    vector<Command> cmds;
    cmds.push_back(Command::Move( 4, VerticalDirection::POSITIVE ));
    cmds.push_back(Command::Move(5, VerticalDirection::POSITIVE ));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 4 || d1.currStoreId != 7) return false;
    return true;
}


bool test_5() {
    NetworkState st = inititalState1();
    auto& d = st.dataPackets.front();
    d.toRouter = d.fromRouter;
    d.dir = HorizontalDirection::LEFT;
    vector<Command> cmds;
    cmds.push_back(Command::Move( 4, VerticalDirection::NEGATIVE ));
    st = simulate(st,cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 3 || d1.currStoreId != 3) return false;
    return true;
}

bool test_6() {
    NetworkState st = inititalState1();
    auto& d = st.dataPackets.front();
    d.toRouter = d.fromRouter;
    d.dir = HorizontalDirection::LEFT;
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(0, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(1, VerticalDirection::POSITIVE));

    cmds.push_back(Command::Move( 12, VerticalDirection::POSITIVE ));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 13 || d1.currStoreId != 4) return false;
    return true;
}

bool test_7() {
    NetworkState st = inititalState1();
    auto& d = st.dataPackets.front();
    d.toRouter = d.fromRouter;
    d.dir = HorizontalDirection::LEFT;
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(0, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(1, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(13, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 12 || d1.currStoreId != 5) return false;
    return true;
}


bool test_8() {
    NetworkState st = inititalState1();
    auto& d = st.dataPackets.front();
    d.toRouter = d.fromRouter;
    d.dir = HorizontalDirection::LEFT;
    vector<MoveCommand> mvcs;
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(0, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(1, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(13, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 1) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 12 || d1.currStoreId != 5) return false;
    return true;
}


bool test_9() {
    NetworkState st = inititalState1();
    auto& d = st.dataPackets.front();
    d.toRouter = d.fromRouter;
    d.dir = HorizontalDirection::LEFT;
    st.nextDir[12] = HorizontalDirection::RIGHT;
    vector<MoveCommand> mvcs;
    vector<CreateCommand> crcs;
    vector<Command> cmds;
    cmds.push_back(Command::Move(4, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(0, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(3, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(1, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(13, VerticalDirection::POSITIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Move(12, VerticalDirection::NEGATIVE));
    cmds.push_back(Command::Create(12, 4, 12 , false));

    st = simulate(st, cmds, 12);
    const auto& datas = st.dataPackets;
    if (datas.size() != 2) return false;
    const auto& d1 = datas[0];
    if (d1.currRouter != 2 || d1.currStoreId != 5) return false;
    const auto& d2 = datas[1];
    if (d2.currRouter != 1 || d2.currStoreId != 5) return false;
    return true;
}

void run_sim_tests()
{
    std::cout << "Simulation_Test1 " << test_1() << std::endl;
    std::cout << "Simulation_Test2 " << test_2() << std::endl;
    std::cout << "Simulation_Test3 " << test_3() << std::endl;
    std::cout << "Simulation_Test4 " << test_4() << std::endl;
    std::cout << "Simulation_Test5 " << test_5() << std::endl;
    std::cout << "Simulation_Test6 " << test_6() << std::endl;
    std::cout << "Simulation_Test7 " << test_7() << std::endl;
    std::cout << "Simulation_Test8 " << test_8() << std::endl;
}
