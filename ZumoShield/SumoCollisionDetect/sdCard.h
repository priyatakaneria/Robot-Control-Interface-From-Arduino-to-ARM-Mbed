#include "mbed.h"

class sdCard
{
public:
    sdCard(); 
    void mountFileSystem();
    void writeToSDCard(const char *__restrict data);
    void closeAndUnmount();
};