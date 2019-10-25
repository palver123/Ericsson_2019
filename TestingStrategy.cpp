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

    return fmt::format("MOVE {} {}", getRandom(0, NROUTERS), getRandom(0,1) ? "^" : "v");
}
