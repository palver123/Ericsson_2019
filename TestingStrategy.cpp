#include "TestingStrategy.h"


std::string TestingStrategy::step(Reader &turnData)
{
    if (turnData.gotEmptyMessagePiece)
    {
        size_t solutionLength = 0;
        for (const auto& receivedPiece : turnData.receivedPieces)
            solutionLength += receivedPiece.message.size();

        std::sort(
            turnData.receivedPieces.begin(),
            turnData.receivedPieces.end(),
            [] (const MessagePiece& m1, const MessagePiece& m2) { return m1.index < m2.index; });

        string solution{};
        solution.reserve(solutionLength);
        for (const auto& receivedPiece : turnData.receivedPieces)
            solution.append(receivedPiece.message);

        return fmt::format("SOLUTION {}", solution);
    }

    if (turnData.dataArray.size() < MAX_PACKETS_IN_SYSTEM)
    {
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

    return fmt::format("MOVE {} {}", getRandom(0, NROUTERS), getRandom(0,1) ? "^" : "v");
}
