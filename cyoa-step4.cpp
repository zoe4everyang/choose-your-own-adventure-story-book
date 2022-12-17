#include "cyoa.h"



int main(int argc, char ** argv) {
    if(argc != 2) {
        std::cerr << "wrong number of arguments!\n" << std::endl;
        return EXIT_FAILURE;
    }
    // read all story in the directory starting from page1
    try{
        char* dirName = argv[1];
        Story newStory;
        newStory.storeEachPage(dirName);
        std::vector<std::pair<std::string, int> > newPath;
        newStory.findAllPath("1", newPath);
        newStory.printAllPath();
    }catch(exception &e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;  
}