#include "visor.h"

int main(int argc, char *argv[])
{
    auto pVisor = std::make_shared<TVisor>(argc, argv);

    return 0;
}
