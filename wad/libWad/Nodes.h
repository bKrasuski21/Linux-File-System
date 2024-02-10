#include <vector>
#include <string>

using namespace std;


class Node {
public:
    string pathName;
    bool directory;
    bool content;
    vector<Node*> children;
    uint32_t elementLength;
    uint32_t elementOffset;
    uint32_t descriptorOffset;
    Node* father;
    void createChild();
    void addFather(Node* parent);
    void addNode(Node* parent, Node* child);
    Node(string name, uint32_t length, uint32_t offset);
private:

};
