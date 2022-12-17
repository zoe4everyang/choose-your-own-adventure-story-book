#include "cyoa.h"



int main(int argc, char ** argv){
  if(argc != 2) {
    std::cerr << "wrong number of arguments!\n" << std::endl;
    return EXIT_FAILURE;
  }
  const char * pageName = argv[1];
  std::ifstream pageRead(pageName);
  if(!pageRead.is_open()){
    std::cerr << "Error opening file" << std::endl;
    return EXIT_FAILURE;
  }

  // create a Page instant to store current page info
  Page newPage;
  try{
    newPage.storePageInfo(pageRead, pageName);
    newPage.printPage();
  }catch(exception &e) {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  pageRead.close();
  
  return EXIT_SUCCESS;
}


