#include "Nodes.h"
#include <vector>
#include <string>
using namespace std;

class Nary {

public:
    Node* root = new Node("/", 0, 0);
    Nary();
    vector<string> findAllPaths();
private:
    void findPathsHelper(Node* node, string &currentPath, vector<string>& allPaths);
};
