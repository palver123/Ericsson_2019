#include "turnData.h"
#include "Reader.h"
#include "simulation.h"
#include "ProbabilityScoreStrategy.h"
#include <chrono>

#define  TIME_LOGGING
using namespace std;

int get_map_seed(const int argc, char *argv[])
{
    auto seed = 12;
    if (argc > 1)
    {
        const string arg{ argv[1] };
        try
        {
            seed = std::stoi(arg);
        }
        catch (...)
        {
            // ignored
        }
    }
    return seed;
}

void run_tests() {
    run_sim_tests();
}

int main(int argc, char *argv[])
{
    if (argc == 2 && std::string("--test") == argv[1]) {
        run_tests();
        return 0;
    }
    const auto seed = get_map_seed(argc, argv);
    cerr << "Using seed: " << seed << endl;

    char teamToken[] = "fQFUQhDn4LTMqWgA59Sz";
    cout << "START " << teamToken
        << " " << seed
        << " " << "0.0.0.11"
        << endl;

    Reader reader = {};
    NetworkState turnData;
    GameContext context;

    ProbabilityScoreStrategy strategy;

    while(reader.readData(turnData, context))
    {

#ifdef TIME_LOGGING
        std::cerr << "Data received" << std::endl;
        const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();
#endif
        auto command = strategy.step(turnData, context);
        
        // Ha szeretnetek debug uzenetet kuldeni, akkor megtehetitek.
        // Vigyazzatok, mert maximalisan csak 1024 * 1024 bajtot kaptok vissza
        cerr << "Send " << command << endl;

#ifdef TIME_LOGGING
        const auto end = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();
        std::cerr << end - start << " ms" << std::endl;
#endif

        // standard out-ra meg mehet ki a megoldas! Mas ne irodjon ide ki ;)
        cout << context.commandPrefix.gameId << " " << context.commandPrefix.tickId << " " << GameContext::ourId << " " << command << endl;
    }

    cerr << "END (latest message): " << reader.previous << endl;
}
