#include "TestingStrategy.h"


std::string TestingStrategy::step(Reader &turnData)
{
    if (turnData.gotEmptyMessagePiece)
    {
        // Guess the solution
        size_t solutionLength = 0;
        for (const auto& receivedPiece : turnData.receivedPieces)
            solutionLength += receivedPiece.message.size();

        std::sort(
            turnData.receivedPieces.begin(),
            turnData.receivedPieces.end(),
            [] (const MessagePiece& m1, const MessagePiece& m2) { return m1.index < m2.index; });

        //string solution{};
        //solution.reserve(solutionLength);
        stringstream ss{"SOLUTION "};
        for (const auto& receivedPiece : turnData.receivedPieces)
            ss << receivedPiece.message;
            //solution.append(receivedPiece.message);

        return ss.str();
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
