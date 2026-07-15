#include "DirectoryLister.hpp"

int main(void)
{
    DirectoryLister dl(".", true);

    for (std::string file = dl.get (); !file.empty(); file = dl.get())
        std::cout << file << std::endl;
    dl.open("invalid path", true);
    if (dl.open (".", false) == true)
        for (std::string file = dl.get(); !file.empty(); file = dl.get())
            std::cout << file << std::endl ;
    return 0;
}