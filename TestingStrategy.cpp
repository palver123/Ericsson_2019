#include "TestingStrategy.h"


std::string TestingStrategy::step(Reader &turnData)
{
    if (turnData.gotEmptyMessagePiece)
    {
        // Guess the solution
        size_t solutionLength = 0;
        std::cerr << "Before sorting: " << std::endl;
        for (const auto& receivedPiece : Reader::_allReceivedPieces)
        {
            std::cerr << '\t' <<  receivedPiece.index << " " << receivedPiece.message << std::endl;
            solutionLength += receivedPiece.message.size();
        }

        std::sort(
            Reader::_allReceivedPieces.begin(),
            Reader::_allReceivedPieces.end(),
            [] (const MessagePiece& m1, const MessagePiece& m2) { return m1.index < m2.index; });

        std::cerr << "After sorting: " << std::endl;
        string solution{};
        solution.reserve(solutionLength);
        for (const auto& receivedPiece : Reader::_allReceivedPieces)
        {
            std::cerr << '\t' << receivedPiece.index << " " << receivedPiece.message << std::endl;
            solution.append(receivedPiece.message);
        }
        std::cerr << "variable 'solution' contains: " << solution << std::endl;

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
