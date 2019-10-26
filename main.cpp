#include "turnData.h"
#include "TestingStrategy.h"

using namespace std;

int main()
{
    char teamToken[] = "fQFUQhDn4LTMqWgA59Sz";
    // int seed = 0;
    
    cout << "START " << teamToken 
        // << " " << seed 
        << endl;

    Reader turnData = {};
    TestingStrategy strategy;
    
    while(readData(turnData)) 
    {
        // TODO logika jobb mint a semmitteves
        auto command = strategy.step(turnData);
        
        // Ha szeretnetek debug uzenetet kuldeni, akkor megtehetitek.
        // Vigyazzatok, mert maximalisan csak 1024 * 1024 bajtot kaptok vissza
        cerr << "Send " << command << endl;
       
        // standard out-ra meg mehet ki a megoldas! Mas ne irodjon ide ki ;)
        cout << turnData.commandPrefix.gameId << " " << turnData.commandPrefix.tickId << " " << turnData.commandPrefix.routerId << " " << command << endl;
    }

    cerr << "END (latest message): " << turnData.previous << endl;
}
