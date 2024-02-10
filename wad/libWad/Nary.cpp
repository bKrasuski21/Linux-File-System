#include "Nary.h"


vector<string> Nary::findAllPaths() {
    vector<string> allPaths;
    string currentPath;
    findPathsHelper(root, currentPath, allPaths);
    for(int i =0; i < allPaths.size(); i++){
        allPaths[i].erase(0, 1);
    }
    return allPaths;
}
void Nary::findPathsHelper(Node *node, string &currentPath, vector<string> &allPaths) {
    if (node == nullptr) return;

    // Store the original path length for backtracking
    int originalPathLength = currentPath.length();

    // Check if the current node is the root node
    if (currentPath.empty()) {
        // For the root node, simply set the currentPath to its name
        currentPath = node->pathName; // Root node name (e.g., "/")
    } else {
        // For non-root nodes, append the separator and then the node's name
        currentPath += "/" + node->pathName;
    }

    // If it's a leaf node, add the current path to allPaths
    if (node->children.empty()) {
        //currentPath.erase(0, 1);
        allPaths.push_back(currentPath);
    } else {
        // Else, go deeper into each child
        for (Node* child : node->children) {
            findPathsHelper(child, currentPath, allPaths);
        }
    }

    // Backtrack - revert the current path to its original state
    currentPath = currentPath.substr(0, originalPathLength);
}


Nary::Nary() {
    root = new Node("/", 0, 0);
    root->directory = true;
    root->content = false;
}
