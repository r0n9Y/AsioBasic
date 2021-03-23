#include  "shutendouji_server.hpp"

int main()
{
    ShutenDoujiServer shutendouji(60000);
    shutendouji.Start();
    while (true)
    {
        shutendouji.Update();
    }

    return 0;
}