#include "A5.hpp"

#include <iostream>
using namespace std;

int main( int argc, char **argv ) 
{
    //std::string luaSceneFile(argv[1]);
    //std::string title("Assignment 5 - [");
    //title += luaSceneFile;
    //title += "]";

    CS488Window::launch(argc, argv, new A5(), 1024, 768, "Soccer");

	return 0;
}
