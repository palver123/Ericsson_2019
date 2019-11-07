#include "turnData.h"
#include "TestingStrategy.h"

using namespace std;

// run 1
int main(int argc, char *argv[])
{
    char teamToken[] = "fQFUQhDn4LTMqWgA59Sz";
    int seed = 0;
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
    cerr << "Using seed: " << seed << endl;

    cout << "START " << teamToken 
        << " " << seed
        << " " << "0.0.0.11"
        << endl;

    Reader reader = {};
    GameState turnData;
    Context context;
    TestingStrategy strategy;
    
    while(reader.readData(turnData, context))
    {
        // TODO logika jobb mint a semmitteves
        auto command = strategy.step(turnData, context);
        
        // Ha szeretnetek debug uzenetet kuldeni, akkor megtehetitek.
        // Vigyazzatok, mert maximalisan csak 1024 * 1024 bajtot kaptok vissza
        cerr << "Send " << command << endl;
       
        // standard out-ra meg mehet ki a megoldas! Mas ne irodjon ide ki ;)
        cout << context.commandPrefix.gameId << " " << context.commandPrefix.tickId << " " << context.commandPrefix.routerId << " " << command << endl;
    }

    cerr << "END (latest message): " << reader.previous << endl;
}
