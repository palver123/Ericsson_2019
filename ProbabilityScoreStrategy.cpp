#include "ProbabilityScoreStrategy.h"
#include "commands.h"
#include "Scores.h"
#include "simulation.h"

using namespace std;

namespace {
    array<bool, NSLOTS> slotCannotTakeNewPacketBot{};
    array<bool, NROUTERS> canMoveRouterMe{};
    array<bool, NROUTERS> canMoveRouterBot{};
}

string ProbabilityScoreStrategy::step(NetworkState& turnData, const GameContext& ctx)
{
    stepPre(turnData,ctx);
    for (const auto& p : GameContext::playerPackets)
        turnData.nextDir[p.first] = ((p.second.active.size() + p.second.received.size()) % 2) ? HorizontalDirection::RIGHT : HorizontalDirection::LEFT;


    if (GameContext::receivedEmptyPacket(ourId))
    {
        // Guess the solution
        if (ctx.have_all_message_pieces())
        {
            // Determine solution length
            size_t solutionLength = 0;
            for (const auto& receivedPiece : ctx._allReceivedPieces)
                solutionLength += receivedPiece.second.message.size();

            // Concat the message pieces
            string guess{};
            guess.reserve(solutionLength);
            for (const auto& receivedPiece : ctx._allReceivedPieces)
                guess.append(receivedPiece.second.message);

            return "SOLUTION " + guess;
        }
    }
    // IMPORTANT: Assuming packets cannot get lost.
    // If we received at least 1 empty message then all the missing message pieces are somewhere in the network and will eventually get back to us
    else if (actualData->getNumberOfPlayerPackets(ourId,true) < MAX_PACKETS_IN_SYSTEM)
    {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : turnData.dataPackets)
        {
            if (data.currRouter == ourId)
                slotTaken[data.currStoreId] = true;
        }

        vector<Command> ccmds;

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[ourId][slot])
                ccmds.push_back(Command::Create(ourId, slot, _requestCounter ) );

        if (ccmds.size())
        {
            Command bcmd = Command::Pass();
            double best = -1e22;
            for (auto& c : ccmds) {
                double score = Scores::future_seeing(simulate(turnData, { c }, ourId), ourId);
                if (best < score) {
                    bcmd = c;
                    best = score;
                }
            }
            ++_requestCounter;
            return bcmd.to_exec_string();
        }
        else
        {
            std::cerr << "!!! Failed creation !!!" << std::endl;
        }
    }
    vector<std::shared_ptr<Player>> players;
    players.emplace_back(new RandomNetworkMovements{});
    for(const auto& p : GameContext::playerPackets)
        if (p.first != ourId)
            players.emplace_back(new RandomPlayerCreatePref(p.first));
    return command_execute(getBestMove(turnData, players, Scores::future_seeing));
}

template<typename T>
void next_idx(const vector<vector<T> >& vec2d, vector<int>& idx) {
    assert(vec2d.size() == idx.size());
    for(int i = static_cast<int>(idx.size()) -1; i>=0; --i) {
        ++idx[i];
        if (idx[i] < vec2d[i].size())
            return;
        if (i == 0)
            return;
        idx[i] = 0;
    }
}

std::vector<std::pair<double, Command> > ProbabilityScoreStrategy::getMovementScores(const NetworkState& state, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring)
{
    auto moves = getPossibleMoves(state,true,false,true);
    if (moves.empty())
        return { { 0,Command::Pass() } };
    vector<pair< double, Command> > res;
    for (const auto& ourC : moves) {
        if (players.size()) {
            vector<vector<pair<double, Command> > > others;
            for(int i = 0; i< static_cast<int>(players.size()); ++i) 
            {
                others.push_back(players[i]->getProbableMoves(state));
            }
            double scoreSum = 0;
            double scoreDiv = 0; // Will be 1 if all players gives back proper probabilities
            for(vector<int> idxs(others.size(), 0); idxs.front() < others.front().size(); next_idx(others,idxs) ) {
                double pos = 1;
                static vector<Command> cmds;
                cmds.clear();
                cmds.push_back(ourC);
                for(int i =0; i<idxs.size(); ++i) {
                    cmds.push_back(others[i][idxs[i]].second);
                    pos *= others[i][idxs[i]].first;
                }
                double score = scoring(simulate(state, cmds, ourId), ourId);
                scoreSum += score;
                scoreDiv += pos;
            }
            if (scoreDiv > 0)
                scoreSum /= scoreDiv;
            else
                scoreSum = 0;
            res.push_back({ scoreSum, ourC });
        }
        else {
            double score = scoring(simulate(state, {ourC}, ourId), ourId);
            res.push_back({ score, ourC});
        }
    }
    return res;
}

Command ProbabilityScoreStrategy::getBestMove(const NetworkState& state, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring)
{
    auto res = getMovementScores(state, players, scoring);
    Command best = Command::Pass();
    double best_score = -1e22;
    for(const auto& it : res)
    {
        if (it.first > best_score)
        {
            best_score = it.first;
            best = it.second;
        }
    }
    return best;
}