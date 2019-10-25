#include "TestingStrategy.h"


std::string TestingStrategy::step(Reader &turnData)
{
    if (turnData.gotEmptyMessagePiece)
    {
        // Guess the solution
        size_t solutionLength = 0;
        for (const auto& receivedPiece : Reader::_allReceivedPieces)
            solutionLength += receivedPiece.message.size();

        std::sort(
            Reader::_allReceivedPieces.begin(),
            Reader::_allReceivedPieces.end(),
            [] (const MessagePiece& m1, const MessagePiece& m2) { return m1.index < m2.index; });

        string solution{};
        solution.reserve(solutionLength);
        for (size_t i = 1; i < Reader::_allReceivedPieces.size(); ++i)
            solution.append(Reader::_allReceivedPieces[i].message);

        return "SOLUTION " + solution;
    }

    if (turnData.dataArray.size() < MAX_PACKETS_IN_SYSTEM)
    {
        // Try to ask for a new packet
        std::array<bool, NSLOTS> slotTaken {};
        auto maxPckIdx = 0u;
        for (const auto& data : turnData.dataArray)
        {
            if (data.dataIndex > maxPckIdx)
                maxPckIdx = data.dataIndex;
            if (data.currRouter == MY_ROUTER)
                slotTaken[data.currStoreId] = true;
        }

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[MY_ROUTER][slot])
                return fmt::format("CREATE {} {}", slot, maxPckIdx);
    }

    // Try to move
    for (size_t routerIdx = 0; routerIdx < NROUTERS; ++routerIdx)
        for (const auto& data : turnData.dataArray)
            if (data.currRouter == routerIdx && data.fromRouter == MY_ROUTER)
                return fmt::format("MOVE {} {}", routerIdx, getRandom(0, 1) ? "^" : "v");

    // Do nothing
    return "PASS";
}
