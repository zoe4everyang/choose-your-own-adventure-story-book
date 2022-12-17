#include "cyoa.h"



///////////////////////// Page class methods /////////////////////////////

/*
* This function stores page name, isResultPage, choices and story content
* @param: a file stream to store the page content
* @param: a string of the name of this page
*/
void Page::storePageInfo(std::ifstream & pageRead, std::string pageName){
  // store pagename
  name = pageName;
  // handle first line of the file exceptionally: 
  // check if it's a result page (announce win/lose)
  std::string s;
  getline(pageRead, s);
  isResultPage = (s.compare("WIN") == 0) ? 1 : ((s.compare("LOSE") == 0) ? 2: 0);
  // isResultPage == 0 -> not result page current line has choice content to store 
  if (!isResultPage){
    storeChoice(s); 
  }
  // Keep reading and storing choice lines until "#"
  while(getline(pageRead, s)){
    if (s[0] == '#') {
      break;
    }
    storeChoice(s);
  }
  // read story content after the "#" and store each line into a story vector
  while(getline(pageRead, s)){
    story.push_back(s);
  }
}


/*
* This function stores the content as a choice pair into a 'choices' map
* First element of pair is a page number before the colon, and the second element is the choice description after the colon
* It also saves the page number into a choiceOrder vector to record the order for printing purpose
* @param: a string of one line in the navigation section
*/
void Page::storeChoice(std::string s) {
  size_t colPos = s.find(':');
  // get the page number before the ':'
  std::string num = s.substr(0, colPos);
  // check if page number is valid (>0)
  if (!isPosNum(num)){
    throw invalidChoiceNum();
  }
  // get the choice description after the ':'
  std::string choiceText = s.substr(colPos+1);
  // insert the current pair into choices map
  choices.insert(std::pair<std::string, std::string> (num, choiceText));
  // insert the page number into the choiceOrder vector
  choiceOrder.push_back(num);
}


/*
* A helper function to check if the a string is a number > 0
* @param: the string to check
* @return: a bool indicating if string is postive num or not
*/
bool Page::isPosNum(const string& s){
    for (char const &ch : s) {
      if (ch > '9' && ch <= '0')
        return false;
    }
    return true;
}


/*
* Print out current page by going through all we have stored
* First is the story content store in story vector
* Second is either WIN/LOSE message or a list of page number for the user to choose and next jump to
*/
void Page::printPage() {
    // iterate through story vector to print out whole content of current page
    std::vector<std::string>::iterator itStory = story.begin();
    while(itStory != story.end()){
        std::cout << *itStory << std::endl;
        ++itStory;
    }
    std::cout << std::endl;
    // check if page is with WIN or LOSE
    if(isResultPage == 1){
        std::cout << "Congratulations! You have won. Hooray!" << std::endl;
    }
    else if(isResultPage == 2){
        std::cout << "Sorry, you have lost. Better luck next time!" << std::endl; 
    }
    // is page with choices
    // print out all choices content
    else{
        std::cout << "What would you like to do?" << std::endl;
        std::cout << std::endl;
        std::vector<string> choiceNums = getChoiceOrder();
        int i = 0;
        while(i < choiceNums.size()) {
            std::cout << " " << i+1 << ". " << choices[choiceNums[i]] << std::endl;
            ++i;
        }
    }
}



///////////////////////// Story class methods /////////////////////////////

/*
* Before starting the game, we need to store all the page info from specified directory user chose. This function serves this purposes
* @param: specified directory name
*/
void Story::storeEachPage(char * dirName){

  int pageNum = 1;
  // stop game if there is no first page in the directory
  bool isStoryEnd = addPageList(pageNum, dirName);

  // store each page into the pageList vector
  while(!isStoryEnd){
      ++pageNum;
      isStoryEnd = addPageList(pageNum, dirName);
  }
  // check and handle exceptions
  checkStoryValid(pageNum - 1);

  // calculate the depth for each page in the pageList
  updateAllDepth(pageList[0], 0);
}


/*
* This function add the page content from specified "./dirName/pageNum" to a pageList vector
* @param: a chosen directory
* @param: a chosen page's number
* @return: 
*/
bool Story::addPageList(int pageNum, char * dirName) {
  ostringstream convert;   // stream used for the conversion  
  convert << pageNum; 
  std::string pageName = getPageName(convert.str());
  std::string path = dirName;
  path.append("/");
  path.append(pageName);
  
  std::ifstream pageRead;
  pageRead.open(path.c_str());
  if(!pageRead){
    // invalid story, no page1.txt
    if(pageNum == 1){
      throw noStartPage();
    }
    // get to the end of the story
    return true;
  }

  Page newPage;
  newPage.storePageInfo(pageRead, pageName);
  pageList.push_back(newPage);
  pageRead.close();
  std::vector<string> pageChoices = newPage.getChoiceOrder();
  std::vector<string>::iterator it = pageChoices.begin();
  while(it != pageChoices.end()){
      pageReferred.insert(*it);
      ++it;
  }
  return false;
}


/*
* This function starts by printing the first page of the current story.
* Then, get a user input to indicate which next page to jump to
* As long as we get valid input from user, we continue the game until the end page of the story
*/
void Story::startGame(){
    Page currPage = locatePage(pageList, "page1.txt");
    currPage.printPage();
    std::string usrChoice;
    std::cin >> usrChoice;
    // as long as user doesn't quit the program, we store next page info and print them out
    while(!cin.eof()){
      bool isInputValid = checkUserInput(usrChoice, currPage);
      if(isInputValid){
        std::vector<std::string> choiceOrder = currPage.getChoiceOrder();
        std::string pageNum = choiceOrder[atoi(usrChoice.c_str()) - 1];
        std::string pageName = getPageName(pageNum);
        currPage = locatePage(pageList, pageName);
        currPage.printPage();
        // if the page is the result page, we will end the game
        if(currPage.isResultPage) {
            break;
        }
      }else{
        // print a message to inform the user of wrong input
        std::cout << "That is not a valid choice, please try again" << std::endl;
      }
      std::cin >> usrChoice;
    }
}


/*
* Calculate each page's depth (for step 3)
* @param: a reference to the current page object
* @param: the depth number to set for current page
*/
void Story::updateAllDepth(Page &currPage, int depth) {
  if(currPage.depth == -1 || depth < currPage.depth){
    currPage.setDepth(depth);
    std::vector<std::string> choiceOrder = currPage.getChoiceOrder();
    for(int i = 0; i < choiceOrder.size(); i++){
      int pageIndex = atoi(choiceOrder[i].c_str());
      Page & nextPage = pageList[pageIndex - 1];
      updateAllDepth(nextPage, depth + 1);
    }
  }
}

/*
* Print each page's depth (for step 3)
* If depth is -1, print unreachable
*/
void Story::printAllDepth(){
  for(int i = 0; i < pageList.size(); i++){
    if(pageList[i].depth == -1){
      std::cout << "Page " << i + 1 << " is not reachable" << std::endl;
    }else{
      std::cout << "Page " << i + 1 << ":" << pageList[i].depth << std::endl;
    }
  }
}


/*
* This function checks if user's input is valid (is a positive interger) 
* and within the number of choices the current page provides
* @param: a string for user input (should be a number)
* @param: the current page object where user's choice is based on
*/
bool Story::checkUserInput(std::string usrChoice, Page currentPage){
  std::vector<string> choiceNums = currentPage.getChoiceOrder();
  // check if user input is a positive number
  if(!currentPage.isPosNum(usrChoice)){
    return false;
  }
  // check if user choice can correctly locate the next page
  int usrChoiceIndex = atoi(usrChoice.c_str());
  if (usrChoiceIndex <= choiceNums.size() && usrChoiceIndex > 0){
    return true;
  }
  return false;
}


/*
* This function turns the page number into a complete file name for this page
* @param: a string for the page number (e.g. pageName = "3")
* @return: the complete file name (e.g. "page3.txt")
*/
std::string Story::getPageName(std::string pageNum){
  std::string pageName = "page";
  pageName.append(pageNum);
  pageName.append(".txt");
  return pageName;
  
}

/*
* @param: pageList, a vector of pages
* @param: pageName, a string for the page name
* @return: a page object found in the pageList by the the pageName
* This function will throw an exception if page cannot be found by the pageName
*/
Page Story::locatePage(std::vector<Page> pageList, std::string pageName) {
  std::vector<Page>::iterator it = pageList.begin();
  while(it != pageList.end()){
    if(it->name.compare(pageName) == 0) {
      return *it;
    }
    ++it;
  }
  throw noPageFound();
}


/*
* This function checks if the current story(directory) meets all requirements
* @param: total page number in current directory
*/
void Story::checkStoryValid(int pageTotal){
  // 1. check if all referenced page choices exist
  std::set<std::string>::iterator itChoice = pageReferred.begin();
  while(itChoice != pageReferred.end()){
    int numChoice = atoi((*itChoice).c_str());
    if(numChoice > pageTotal || numChoice <= 0){
        throw pageNotExist();
    }
    ++itChoice;
  }

  // 2. check if all pages are referenced
  int numPageReferred = pageReferred.size() + 1;
  if(pageTotal != numPageReferred){
      throw pageUnreferenced();
  }

  // 3. check if there's win & lose page
  bool hasWinPage = false;
  bool hasLosePage = false;
  hasResultPages = false;
  std::vector<Page>::iterator itPage = pageList.begin();
  while(itPage != pageList.end()){
      if(itPage->getIsResultPage() == 1){
        hasWinPage = true;
      }else if(itPage->getIsResultPage() == 2) {
        hasLosePage = true;
      }
      if(hasWinPage && hasLosePage){
        hasResultPages = true;
        break;
      }
      itPage++;
  }
  // throw error if not exist
  if(!hasResultPages){
    if (!hasWinPage){
      throw noWinPage();
    }else if(!hasLosePage){
      throw noLosePage();
    }
  }
}


/*
* This recursive function finds all the story path and store in the allPath vector (for step 4)
* @param: currPageNum, used to locate the page during recursion
* @param: prevPath, the previous path from previous recursive steps, current step will append on this vector and pass to next recursion
*/
void Story::findAllPath(std::string currPageNum, std::vector<std::pair<std::string, int> > & prevPath){
  Page & currPage = pageList[atoi(currPageNum.c_str()) - 1];
  // the path ends with "win"
  if(currPage.getIsResultPage() == 1){
    std::pair<std::string, int> finalPair(currPageNum, 0);
    prevPath.push_back(finalPair);
    allPath.push_back(prevPath);
  }
  // won't do anything for path ends with "lose", 
  // discarding current path by NOT storing it into the allPath vector
  else if (currPage.getIsResultPage() == 2){ }
  // find the next page choices provided in the current page and procceed
  else{
    std::vector<std::string> nextChoices = currPage.getChoiceOrder();
    for(int i = 0; i < nextChoices.size(); i++){
      std::string nextChoice = nextChoices[i];
      // find if there's circle in the path 
      bool cycle = false;
      for(int j = 0; j < prevPath.size(); j++){
        if(prevPath[j].first == nextChoice){
          cycle = true;
          break;
        }
      }
      if(cycle){
        break;
      }
      // duplicate a new path for every other choice on current step
      std::vector<std::pair<std::string, int> > currPath = prevPath;
      std::pair<std::string, int> nextPair(currPageNum, i + 1);
      currPath.push_back(nextPair);
      findAllPath(nextChoice, currPath);
    }
  }
}

/*
* The function prints out all the path stored in allPath vector
*/
void Story::printAllPath(){
  int winnable = 0;
  for(int i = 0; i < allPath.size(); i++){
    for(int j = 0; j < allPath[i].size(); j++){
      std::pair<std::string, int> currPair= allPath[i][j];
      if(currPair.second == 0){
        winnable = 1;
        std::cout << currPair.first << "(win)" << std::endl;
        break;
      }
      else{
        std::cout << currPair.first << "(" << currPair.second << "),"; 
      }
    }
  }
  if(winnable == 0){
    std::cout << "This story is unwinnable!" << std::endl;
  }
}
