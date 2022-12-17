#include "cyoa.h"



int main(int argc, char ** argv) {
    if(argc != 2) {
        std::cerr << "wrong number of arguments!\n" << std::endl;
        return EXIT_FAILURE;
    }
    // read all story in the directory starting from page1
    char* dirName = argv[1];
    Story newStory;
    try{
        newStory.storeEachPage(dirName);
        newStory.startGame();
    }catch(exception &e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;  
}