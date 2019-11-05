#include "TestingStrategy.h"


bool have_all_message_pieces()
{
    auto highestReceivedIdx = -1;
    for (const auto& receivedPiece : Reader::_allReceivedPieces)
    {
        if (receivedPiece.first - highestReceivedIdx > 1)
            return false; // A packet is missing
        highestReceivedIdx = receivedPiece.first;
    }

    return highestReceivedIdx = Reader::_lowestEmptyAnswer - 1;
}

std::string TestingStrategy::step(Reader &turnData)
{
    if (Reader::hasReceivedEmptyMessage())
    {
        // Guess the solution
        if (have_all_message_pieces())
        {
            // Determine solution length
            size_t solutionLength = 0;
            for (const auto& receivedPiece : Reader::_allReceivedPieces)
                solutionLength += receivedPiece.second.message.size();

            // Concat the message pieces
            string guess{};
            guess.reserve(solutionLength);
            for (const auto& receivedPiece : Reader::_allReceivedPieces)
                guess.append(receivedPiece.second.message);

            return "SOLUTION " + guess;
        }
    }
    // IMPORTANT: Assuming packets cannot get lost.
    // If we received at least 1 empty message then all the missing message pieces are somewhere in the network and will eventually get back to us
    else if(turnData.dataArray.size() < MAX_PACKETS_IN_SYSTEM)
    {
        // Try to ask for a new packet
        std::array<bool, NSLOTS> slotTaken {};
        for (const auto& data : turnData.dataArray)
        {
            if (data.currRouter == turnData.commandPrefix.routerId)
                slotTaken[data.currStoreId] = true;
        }

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[turnData.commandPrefix.routerId][slot])
                return fmt::format("CREATE {} {}", slot, _requestCounter++);
    }

    // Try to move
    for (size_t routerIdx = 0; routerIdx < NROUTERS; ++routerIdx)
        for (const auto& data : turnData.dataArray)
            if (data.currRouter == routerIdx && data.fromRouter == turnData.commandPrefix.routerId)
                return fmt::format("MOVE {} {}", routerIdx, getRandom(0, 1) ? "^" : "v");

    // Do nothing
    return "PASS";
}
