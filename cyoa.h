#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <iterator>
#include <string>
#include <sstream>
#include <stack>
#include <exception>

using namespace std;

class Page{
private:
    std::string name;
    std::vector<std::string> story;
    std::map<std::string, std::string> choices;
    std::vector<std::string> choiceOrder;
    int isResultPage; // 0: false 1: win 2:lose
    int depth;
    friend class Story;

public:
    //rules of three?
    Page() : depth(-1) {};
    void printPage();
    void storeChoice(std::string s);
    void storePageInfo(std::ifstream & pageRead, std::string pageName);
    std::vector<string> getChoiceOrder(){return choiceOrder;};
    int getIsResultPage(){return isResultPage;};
    void setDepth(int d){depth = d;};
    bool isPosNum(const string& s);
};



class Story{
private:
    //friend class Page;
    std::vector<Page> pageList;
    std::set<std::string> pageReferred;
    bool hasResultPages;
    std::vector<std::vector<std::pair<std::string, int> > > allPath;

public:
    void storeEachPage(char * dirName);
    bool addPageList(int pageNum, char * dirName);
    void startGame();
    Page locatePage(std::vector<Page> pageList, std::string pageName);
    std::string getPageName(std::string usrChoice);
    void checkStoryValid(int pageNum);
    bool checkUserInput(std::string pageNum, Page currentPage);
    void updateAllDepth(Page &currPage, int depth);
    void printAllDepth();
    void findAllPath(std::string currPageNum, std::vector<std::pair<std::string, int> > & prevPath);
    void printAllPath();
};

void printExcep(const char * errmsg);

class invalidChoiceNum : public std::exception {
public:
    virtual const char * what() const throw() {
        return "Current page contains invalid choice number";
    }
};

class noPageFound : public std::exception {
public:
    virtual const char * what() const throw() {
        return "Cannot locate page";
    }
};

class noStartPage :  public std::exception {
public:
    virtual const char * what() const throw() {
        return "There is no page1.txt in specified directory!";
    }
};

class pageNotExist : public std::exception {
public:
    virtual const char * what() const throw() {
        return "Page being referenced does not exist!";
    }
};

class pageUnreferenced : public std::exception {
public:
    virtual const char * what() const throw() {
        return "There is no page in the directory unreferenced!";
    }
};

class noWinPage : public std::exception {
public:
    virtual const char * what() const throw() {
        return "There is no WIN page in the directory!";
    }
};

class noLosePage : public std::exception {
public:
    virtual const char * what() const throw() {
        return "There is no LOSE page in the directory!";
    }
};


