
#include "turnData.h"
#include "strategy.h"


int main() {
    char teamToken[] = "fQFUQhDn4LTMqWgA59Sz";
    // int seed = 0;
    
    std::cout << "START " << teamToken 
        // << " " << seed 
        << std::endl;

    Reader turnData = {};

    IStrategy my_stat;
    
    while(true) {
        readData(turnData);
        
        if (turnData.hasEnd)
            break;
            
        // TODO logika jobb mint a semmitteves
        std::string command = my_stat.step(turnData);
        
        // Ha szeretnetek debug uzenetet kuldeni, akkor megtehetitek.
        // Vigyazzatok, mert maximalisan csak 1024 * 1024 bajtot kaptok vissza
        std::cerr << "Send " << command << std::endl;
       
        // standard out-ra meg mehet ki a megoldas! Mas ne irodjon ide ki ;)
        std::cout << turnData.data[0] << " " << turnData.data[1] << " " << turnData.data[2] << " " << command << std::endl;
    }
    std::cerr << "END (latest message): " << turnData.previous << std::endl;
}
