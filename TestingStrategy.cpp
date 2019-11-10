#include "TestingStrategy.h"

std::string TestingStrategy::step(const NetworkState &turnData, const GameContext& ctx)
{
    stepPre(turnData,ctx);
    if (ctx.hasReceivedEmptyMessage())
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
    else if(actualData->getNumberOfPlayerPackets(GameContext::ourId) < MAX_PACKETS_IN_SYSTEM)
    {
        // Try to ask for a new packet
        std::array<bool, NSLOTS> slotTaken {};
        for (const auto& data : turnData.dataPackets)
        {
            if (data.currRouter == GameContext::ourId)
                slotTaken[data.currStoreId] = true;
        }

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[GameContext::ourId][slot])
                return fmt::format("CREATE {} {}", slot, _requestCounter++);
    }

    // Try to move
    for (int routerIdx = 0; routerIdx < NROUTERS; ++routerIdx)
        for (const auto& data : turnData.dataPackets)
            if (data.currRouter == routerIdx && data.fromRouter == GameContext::ourId)
                return fmt::format("MOVE {} {}", routerIdx, getRandom(0, 1) ? VerticalDirection::NEGATIVE : VerticalDirection::POSITIVE);

    // Do nothing
    return "PASS";
}
