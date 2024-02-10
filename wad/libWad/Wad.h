#include <string>
#include "Nary.h"
#include <fstream>
#include <iostream>
#include <cstdint>
#include <stack>
#include <queue>
#include <vector>
#include <map>
#include <cstring>



using namespace std;

class Wad {
public:
    map<std::string, Node*> allPathsMap;
    map<std::string, Node*> finalApple;
    map<string, int> dirMap;
    string Magic;
    string fileName;
    Nary* nary = new Nary();
    char nugget[1024];
    uint32_t descriptorNum;
    uint32_t descriptorLoc;
    void DFS(Node* node);
    void BFS(Node* root);
    ifstream file;
    static Wad* loadWad(const string &path);
    string getMagic();
    string getParentDirectory(const std::string& path);
    string getParentDir4File(const string& path);
    int parentOffset(string parent);
    map<std::string, Node*> rebuildMap(Node* root);
    void buildFullPathMap(Node* root, map<std::string, Node*>& pathMap);
    void dirMapBuilder(Node* root, map<std::string, int>& pathMap);
    void setPathVals(map<std::string, int>& pathMap);
    bool isContent(const string &path);
    bool isDirectory(const string &path);
    int getSize(const string &path);
    int getContents(const string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const string &path, vector<string> *directory);
    void createDirectory(const string &path);
    void createFile(const string &path);
    int writeToFile(const string &path,const char *buffer, int length, int offset = 0);
    void createTreeAndMap(const string &path);
private:
    Wad(const string &path);

};


