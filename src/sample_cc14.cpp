#include <iostream>
#include <map>

int main(int argc, char **argv)
{
    std::cout << "Hello world !" << std::endl;
    auto x = 2;

    std::map<std::string, int> test;
    test["all"] = 42;
    test["bad"] = 666;

    for(auto & [key, val] : test) {
        std::cout << key << val << std::endl;
    }

    return 0;
}