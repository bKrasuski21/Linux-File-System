#include "Wad.h"

Wad *Wad::loadWad(const std::string &path) {
    return new Wad(path);
}
void Wad::DFS(Node *node) {
    if (node == nullptr) return;
// Process the current node
    //cout << node->pathName << endl;

// Recursively visit each child
    for (Node* child : node->children) {
        DFS(child);
    }
}
int Wad::getDirectory(const std::string &path, vector<std::string> *directory) {
    string apple = path;
    if(apple.back() == '/' && apple.size() > 1){
        apple.pop_back();
    }
    if(!isDirectory(apple)){
        //cout << "happened " << path << endl;
        return -1;
    }else {
        string temps = path;
        //cout << "got here" << path << endl;
        if(temps.back() == '/' && temps.size() > 1){
           // //cout << "Went in here " << temps << endl;
            temps.pop_back();
        }

        int temp = allPathsMap[temps]->children.size();
        for(int i =0; i < temp; i++){
            //cout << "names: " << allPathsMap[temps]->children[i]->pathName << endl;

            //cout << "num of slash and count " << numOfSlash << " " << count << endl;
            directory->push_back(allPathsMap[temps]->children[i]->pathName);

        }
        return temp;
    }

}
string Wad::getParentDirectory(const std::string& path) {
    if(path.size() < 2){
        return "";
    }
    string temp = path;
    if(temp.back() == '/' && temp.size() > 1){
        temp.pop_back();
    }
    if(allPathsMap.find(temp) != allPathsMap.end()){
        return "";
    }

    size_t lastSlash = temp.find_last_of('/');
    if (temp.size() - (lastSlash+1) > 2){
        //cout << " happ" << endl;
        return "";
    }
    if(lastSlash == 0){
        return "/";
    }


    return temp.substr(0, lastSlash);
}
string Wad::getParentDir4File(const std::string &path) {

    string temp = path;
    if(temp.back() == '/' && temp.size() > 1){
            temp.pop_back();
    }
    if(allPathsMap.find(temp) != allPathsMap.end()){
            return "";
    }

    size_t lastSlash = temp.find_last_of('/');
    if (temp.size() - (lastSlash+1) > 8){
            //cout << " happ" << endl;
        return "";
    }
    if(lastSlash == 0){
        return "/";
    }
    return temp.substr(0, lastSlash);
}
string getNewFile(const string &path){

        string temp = path;
        if(temp.back() == '/' && temp.size() > 1){
            temp.pop_back();
        }
        size_t lastSlash = temp.find_last_of('/');
        int parentLength = temp.size() - (lastSlash+1);
        string parent = path.substr(lastSlash+1, parentLength);
        if(parent[0] == 'E' && parent[2] == 'M'){
            return "";
        }
        return parent;

}
string getNewDir(const string &path){
    string temp = path;
    if(temp.back() == '/' && temp.size() > 1){
        temp.pop_back();
    }
    size_t lastSlash = temp.find_last_of('/');
    int parentLength = temp.size() - (lastSlash+1);
    string parent = path.substr(lastSlash+1, parentLength);
    return parent;
}
int Wad::parentOffset(string parent) {
    //cout << "parent: " << parent << endl;
    string tempStart = parent;
    size_t lastSlash = tempStart.find_last_of('/');
    int parentLength = tempStart.size() - (lastSlash + 1);
    string currNode = tempStart.substr(lastSlash + 1, parentLength);
    string father = tempStart.substr(0,lastSlash);
    //cout << "father " << father << endl;
    tempStart = currNode;
    if(allPathsMap[parent] != nullptr){
       // cout << allPathsMap[parent]->father->pathName << endl;
    }

    string tempEnd = tempStart;
    tempEnd += "_END";
    tempStart += "_START";
    //cout << "temp end and start " << tempEnd << tempStart << endl;
    if(!file.is_open()){
        file.open(fileName);
    }

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    //cout << " file size " << fileSize << endl;
    file.seekg(descriptorLoc, ios::beg);
    int tempDescLoc = descriptorLoc;
    int count = 0;
    //cout << "temp end and temp start: "<<  tempEnd << tempStart << endl;
    for(int i =0; i < descriptorNum; i++){
        uint32_t eOffset;
        file.read(reinterpret_cast<char*>(&eOffset), 4);

        uint32_t eLength;
        file.read(reinterpret_cast<char*>(&eLength), 4);

        char name[8];
        file.read(name, 8);
        string tempComp;
        //cout << "temp end" << tempEnd << endl;
        for(int k =0 ; k< 8; k++){
            tempComp += name[k];

            if(tempComp == tempEnd){
      //          cout << "temp end " << tempComp <<" " << tempEnd << " " << tempDescLoc << endl;
                //cout << tempDescLoc << endl;
                count = tempDescLoc;
                //return tempDescLoc;
            }
        }
        tempDescLoc += 16;

    }
    return count;
}
map<std::string, Node*> Wad::rebuildMap(Node *root){
    stack<Node*> pathStack;
    if(!file.is_open()){
        file.open(fileName);
    }
    nary->root->children.clear();
    //cout << "fiulename: " << fileName << endl;
    if (file.is_open()) {
        if (file) {
            char buffer[4];
            file.read(buffer, 4);
            //std::cout << "Magic: ";

            for (int i = 0; i < 4; ++i) {
                Magic +=buffer[i];
            }
            uint32_t numDescriptors;
            file.read(reinterpret_cast<char*>(&numDescriptors), 4);
    //        std::cout << "num of descriptors: " << numDescriptors << endl;
            descriptorNum = numDescriptors;
            uint32_t startLoc;
            file.read(reinterpret_cast<char*>(&startLoc), 4);
      //      std::cout << "start loc of descriptor: " << startLoc << endl;
            descriptorLoc = startLoc;
            int count = descriptorLoc;
            file.seekg(descriptorLoc, ios::beg);
            int makerSpaceCount = 0;
            bool inMakerSpace = false;
            bool dir = false;
            Node* currentNode = nary->root;
            for(int i =0; i < descriptorNum; i++){
                //cout << "stack top: " << pathStack.top();
                if(!pathStack.empty()){
                    currentNode = pathStack.top();
                }else {
                    currentNode = nary->root;
                }
               // int currLoc = file.tellg();

                if(inMakerSpace){
                    makerSpaceCount++;
                }
                uint32_t eOffset;
                file.read(reinterpret_cast<char*>(&eOffset), 4);

                uint32_t eLength;
                file.read(reinterpret_cast<char*>(&eLength), 4);

                char name[8];
                file.read(name, 8);
                for(int k =0 ; k< 8; k++){
                    //cout << name[k];
                    if(name[k] == '_'){
                        if(strncmp(&name[k+1], "START", 5) == 0){
                            string temp = name;
                            string substring = temp.substr(0, 2);
                            Node* child = new Node(substring, eLength, eOffset);
                            child->descriptorOffset = count;
                            child->directory = true;
                            child->content = false;
                            child->father = currentNode;
                            currentNode->addNode(currentNode, child);
                            //cout << "added " << temp << " to: "<< currentNode->pathName << endl;
                            pathStack.push(child);

                            dir = true;
                        }
                    }
                    if(name[k] == 'E' && k == 0){
                        //cout << "e? " << endl;
                        if(name[k+2] == 'M'){
                            //cout << "happ" << endl;
                            string temp = name;
                            Node* child = new Node(temp, eLength, eOffset);
                            child->descriptorOffset = count;
                            child->directory = true;
                            child->content = false;
                            child->father = currentNode;
                            currentNode->addNode(currentNode, child);
                            //cout << "added " << temp << " to: "<< currentNode->pathName << endl;
                            pathStack.push(child);

                            inMakerSpace = true;
                            dir = true;
                        }
                    }
                    if(name[k] == '_'){
                        if(strncmp(&name[k+1], "END", 3) == 0){
                            pathStack.pop();
                            //cout << "dir close" << pathStack.size() << endl;
                            //cout << pathStack.size() << endl;
                            dir = true;
                        }
                    }
                }
                if(!dir){
                    //cout << " didnt" << endl;
                    string temp = name;
                    //cout << "added " << temp << " to: "<< currentNode->pathName << endl;
                    Node* child = new Node(temp, eLength, eOffset);
                    child->directory = false;
                    child->content = true;
                    child->father = currentNode;
                    child->descriptorOffset = count;
                    currentNode->addNode(currentNode, child);
                }
                if(makerSpaceCount == 10){
                    pathStack.pop();
                    if(pathStack.empty()){
                        currentNode = nary->root;
                    }else {
                        currentNode = pathStack.top();
                    }
                    makerSpaceCount = 0;
                    inMakerSpace = false;
                }
                //cout << endl;
                dir = false;
                count += 16;
            }
            //file.clear();

            buildFullPathMap(nary->root, allPathsMap);
            dirMapBuilder(nary->root, dirMap);
            setPathVals(dirMap);

        }
    }
    file.close();
    return allPathsMap;
}
void Wad::createFile(const std::string &path) {
    //cout << path << endl;
    string temp = getParentDir4File(path);
    //cout << "get parent directory of new file: " << temp << endl;
    if(temp == "/") {
        string newFile = getNewFile(path);
     //   cout << "new dir "<< newFile << endl;
        if(newFile == ""){
            return;
        }
        if(!file.is_open()){
            file.open(fileName);
        }
        file.seekg(0, std::ios::end);
        int fileSize = file.tellg();
        file.close();
        std::fstream outfile(fileName, std::ios::in | std::ios::out); // Opens for both reading and writing
        outfile.seekp(0, std::ios::end);
        //int descOffset = outfile.tellg();
        if(outfile.is_open()){
            //cout <<" uau" << endl;
        }
        char eOffset[4];
        eOffset[0] = 0;
        char eLength[4];
        eLength[0] = 0;
        for(int i = 1; i < 4; i++){
            eOffset[i] = '\0';
            eLength[i] = '\0';
        }

        outfile.write(eOffset, 4);
        outfile.write(eLength, 4);
        char newFileStart[8];
        for(int i =0 ;i < 8 ; i++){
            if(i < newFile.size()){
                newFileStart[i] = newFile[i];
            }else {
                newFileStart[i] = '\0';
            }
        }
        outfile.write(newFileStart, 8);
        outfile.seekg(0, std::ios::end);
        streampos sizeTemp = outfile.tellg();
        //cout << " file size 2: " << sizeTemp << endl;
        outfile.seekg(4, std::ios::beg);
        descriptorNum += 1;
        uint32_t numDescriptors = descriptorNum;
        outfile.write(reinterpret_cast<char*>(&numDescriptors), 4);
        outfile.close();
        dirMap.clear();
        allPathsMap.clear();
        map<std::string, Node*> apple;
        //loadWad(fileName);
        apple = rebuildMap(nary->root);

        for (const auto& pair : apple) {
             //std::cout << pair.first << " " << pair.second->directory << std::endl;

        }
    }
    else if (isDirectory(temp)) {
        for (const auto& pair : allPathsMap) {
            //std::cout << pair.first << " " << pair.second->directory << std::endl;

        }
        string newFile = getNewFile(path);
        //cout << "new file: " << newFile << endl;
        int fileStartLoc = parentOffset(temp);
        //cout << "file start loc: " << fileStartLoc << endl;
        if(fileStartLoc != 0){
            file.seekg(0, std::ios::end);
            int fileSize = file.tellg();
          //  cout << " file size " << fileSize << endl;
            file.seekg(fileStartLoc, ios::beg);
            int numOfBytes = fileSize - fileStartLoc;
            vector<char> buffer(numOfBytes);
            file.read(buffer.data(), numOfBytes);
            file.close();
            std::fstream outfile(fileName, std::ios::in | std::ios::out); // Opens for both reading and writing

            if(outfile.is_open()){
                //cout <<" uau" << endl;
            }
            outfile.seekp((fileStartLoc + 16), std::ios::beg);
            outfile.write(buffer.data(), numOfBytes);
            outfile.seekp(fileStartLoc, std::ios::beg);
            char eOffset[4];
            eOffset[0] = 0;
            char eLength[4];
            eLength[0] = 0;
            for(int i = 1; i < 4; i++){
                eOffset[i] = '\0';
                eLength[i] = '\0';
            }

            outfile.write(eOffset, 4);
            outfile.write(eLength, 4);
            char newFileStart[8];
            for(int i =0 ;i < 8 ; i++){
                if(i < newFile.size()){
                    newFileStart[i] = newFile[i];
                }else {
                    newFileStart[i] = '\0';
                }
            }
            outfile.write(newFileStart, 8);
            outfile.seekg(0, std::ios::end);
            streampos sizeTemp = outfile.tellg();
            //cout << " file size 2: " << sizeTemp << endl;
            outfile.seekg(4, std::ios::beg);
            descriptorNum += 1;
            uint32_t numDescriptors = descriptorNum;
            outfile.write(reinterpret_cast<char*>(&numDescriptors), 4);
            //file.read(reinterpret_cast<char*>(&numDescriptors), 4);
            outfile.close();
            dirMap.clear();
            allPathsMap.clear();
            map<std::string, Node*> apple;
            apple = rebuildMap(nary->root);
            this->finalApple = apple;
            //loadWad(fileName);
            //cout << "apple map: " << endl;
            for (const auto& pair : apple) {
              //  std::cout << pair.first << " " << pair.second->directory << std::endl;
                }
            }
        }
    }

int Wad::writeToFile(const std::string &path, const char *buffer, int length, int offset) {
    //cout << "apple" << endl;

    if(!file.is_open()){
        file.open(fileName);
    }
    string temp = path;
    if(temp.back() == '/' && temp.size() != 1){
        temp.pop_back();
    }
    //cout <<" temp " << temp << endl;
    if(isDirectory(temp)){
        return -1;
    }
    if(isContent(temp)){
        if(allPathsMap[temp]->elementLength != 0){
            return 0;
        } else {
            file.seekg(descriptorLoc, ios::beg);
            int OGDescLoc = descriptorLoc;
            int sizeOfDescriptor = descriptorNum * 16;
            //cout << "OG and Size " << descriptorLoc << " " << sizeOfDescriptor << endl;
            vector<char> buff(sizeOfDescriptor);
            file.read(buff.data(), sizeOfDescriptor);
            file.close();
            std::fstream outfile(fileName, std::ios::in | std::ios::out); // Opens for both reading and writing
            if(outfile.is_open()){
                //cout <<" uau" << endl;
            }
            int newDescLoc = descriptorLoc + length;
            //cout << "OG, NEw, LEngtH: " << OGDescLoc << " " << newDescLoc << " " << length << endl;
            outfile.seekp(newDescLoc, std::ios::beg);
            outfile.write(buff.data(), sizeOfDescriptor);
            outfile.seekp(0, std::ios::end);
            //cout << "size of file now: " << outfile.tellg() << endl;
            outfile.seekp(OGDescLoc, std::ios::beg);
            outfile.write(buffer, length);
            descriptorLoc = newDescLoc;
            uint32_t descLoc = descriptorLoc;
            outfile.seekp(8, std::ios::beg);
            outfile.write(reinterpret_cast<char*>(&descLoc), 4);
            outfile.seekp((allPathsMap[temp]->descriptorOffset+length), std::ios::beg);
            uint32_t elOffset = OGDescLoc;
            outfile.write(reinterpret_cast<char*>(&elOffset), 4);
            uint32_t elLength = length;
            outfile.write(reinterpret_cast<char*>(&elLength), 4);
            //cout << "Descriptor loc: " << descLoc << endl;
            //cout << "current file descriptor offset: " << allPathsMap[temp]->descriptorOffset << endl;
            string parentTempDir = getParentDir4File(path);
            //cout << parentTempDir << "parent dir" << endl;
            if(parentTempDir == "/"){

            }else {

            }
            allPathsMap[temp]->elementLength = length;
            allPathsMap[temp]->elementOffset = OGDescLoc;
            outfile.close();
            //cout << "get size : " << getSize(temp) << endl;
            for (const auto& pair : allPathsMap) {
                //std::cout << pair.first << " " << pair.second->elementLength <<" "<< pair.second->descriptorOffset << std::endl;
                //getDirectory(pair.first, &apple);
                for(int i =0; i < 25; i++){
                    // nugget[i] = '\0';
                }
            }
            //cout << "rebuild" << endl;
            rebuildMap(nary->root);
            for (const auto& pair : allPathsMap) {
                //std::cout << pair.first << " " << pair.second->elementLength <<" " <<  pair.second->descriptorOffset << std::endl;
                //getDirectory(pair.first, &apple);
                for(int i =0; i < 25; i++){
                    // nugget[i] = '\0';
                }
            }
        }
    }
    return length;
}
void Wad::createDirectory(const std::string &path) {
    string temp = getParentDirectory(path);
    //cout <<"create dir path: " <<  path << endl;
    //cout <<  "parent dir path: " << temp << endl;
    if(temp == ""){
        //cout << "Invalid Dir" << endl;
        return;
    }
    if(temp == "/"){
        string newDir = getNewDir(path);
       // cout << "new dir "<< newDir << endl;
        string startDir = newDir;
        string endDir = newDir;
        endDir += "_END";
        startDir += "_START";
       // cout << "start and end " << endDir << " " << startDir << endl;
        if(!file.is_open()){
            file.open(fileName);
        }
        file.seekg(0, std::ios::end);
        int fileSize = file.tellg();
        file.close();
        std::fstream outfile(fileName, std::ios::in | std::ios::out); // Opens for both reading and writing
        outfile.seekp(0, std::ios::end);
        char eOffset[4];
        eOffset[0] = 0;
        char eLength[4];
        eLength[0] = 0;
        for(int i = 1; i < 4; i++){
            eOffset[i] = '\0';
            eLength[i] = '\0';
        }
        outfile.write(eOffset, 4);
        outfile.write(eLength, 4);
        char newDirStart[8];
        for(int i =0 ;i < 8 ; i++){
            newDirStart[i] = startDir[i];
        }
        outfile.write(newDirStart, 8);

        outfile.write(eOffset, 4);
        outfile.write(eLength, 4);
        char newDirEnd[8];
        for(int i =0 ;i < 8 ; i++){
            if(i < 6){
                newDirEnd[i] = endDir[i];
            }else {
                newDirEnd[i] = '\0';
            }
        }
        outfile.write(newDirEnd, 8);
        outfile.seekg(0, std::ios::end);
        streampos sizeTemp = outfile.tellg();
      //  cout << " file size 2: " << sizeTemp << endl;
        outfile.seekg(4, std::ios::beg);
        descriptorNum += 2;
        uint32_t numDescriptors = descriptorNum;
        outfile.write(reinterpret_cast<char*>(&numDescriptors), 4);
        outfile.close();
       // cout << "all paths map" << endl;
        for (const auto& pair : allPathsMap) {
           // std::cout << pair.first << " " << pair.second->directory << std::endl;
            //getDirectory(pair.first, &apple);
            for(int i =0; i < 25; i++){
                // nugget[i] = '\0';
            }
        }
        dirMap.clear();
        allPathsMap.clear();
        map<std::string, Node*> apple;
        //loadWad(fileName);
        apple = rebuildMap(nary->root);
        //this->finalApple = apple;
        //this->allPathsMap = apple;
        //cout << "apple map " << endl;
        //string testPath = "ex";
        //cout << "is directory: " << isDirectory(testPath) << endl;
        for (const auto& pair : apple) {
           // std::cout << pair.first << " " << pair.second->directory << std::endl;

        }
    } else if(isDirectory(temp)){
        string newDir = getNewDir(path);
        //cout << "new dir "<< newDir << endl;
        int dirStartLoc = parentOffset(temp);
        //cout << "dirstarT: " << dirStartLoc << endl;
        if(dirStartLoc != 0){
            string startDir = newDir;
            string endDir = newDir;
            endDir += "_END";
            startDir += "_START";
            file.seekg(0, std::ios::end);
            int fileSize = file.tellg();
            //cout << " file size " << fileSize << endl;
            file.seekg(dirStartLoc, ios::beg);
            int numOfBytes = fileSize - dirStartLoc;
            vector<char> buffer(numOfBytes);
            file.read(buffer.data(), numOfBytes);
            file.close();
            std::fstream outfile(fileName, std::ios::in | std::ios::out); // Opens for both reading and writing
            //cout << "fule name" << fileName << endl;
            //cout << "dir startt" << dirStartLoc << endl;
            if(outfile.is_open()){
                //cout <<" uau" << endl;
            }
            outfile.seekp((dirStartLoc + 32), std::ios::beg);
            outfile.write(buffer.data(), numOfBytes);
            outfile.seekp(dirStartLoc, std::ios::beg);
            char eOffset[4];
            eOffset[0] = 0;
            char eLength[4];
            eLength[0] = 0;
            for(int i = 1; i < 4; i++){
                eOffset[i] = '\0';
                eLength[i] = '\0';
            }

            outfile.write(eOffset, 4);
            outfile.write(eLength, 4);
            char newDirStart[8];
            for(int i =0 ;i < 8 ; i++){
                newDirStart[i] = startDir[i];
            }
            outfile.write(newDirStart, 8);

            outfile.write(eOffset, 4);
            outfile.write(eLength, 4);
            char newDirEnd[8];
            for(int i =0 ;i < 8 ; i++){
                if(i < 6){
                    newDirEnd[i] = endDir[i];
                }else {
                    newDirEnd[i] = '\0';
                }
            }
            outfile.write(newDirEnd, 8);
            outfile.seekg(0, std::ios::end);
            streampos sizeTemp = outfile.tellg();
            //cout << " file size 2: " << sizeTemp << endl;
            outfile.seekg(4, std::ios::beg);
            descriptorNum += 2;
            uint32_t numDescriptors = descriptorNum;
            outfile.write(reinterpret_cast<char*>(&numDescriptors), 4);
            //file.read(reinterpret_cast<char*>(&numDescriptors), 4);
            outfile.close();
            //cout << "all paths map: " << endl;
            for (const auto& pair : allPathsMap) {
                //std::cout << pair.first << " " << pair.second->directory << std::endl;
                //getDirectory(pair.first, &apple);
                for(int i =0; i < 25; i++){
                    // nugget[i] = '\0';
                }
            }
            dirMap.clear();
            allPathsMap.clear();
            map<std::string, Node*> apple;
            apple = rebuildMap(nary->root);
           // cout << "apple map: " << endl;

            for (const auto& pair : apple) {
               // std::cout << pair.first << " " << pair.second->directory << std::endl;

            }
            /*
             Node* child = new Node(newDir, 0, 0);
             child->directory = true;
             child->content = false;
             allPathsMap[temp]->addNode(allPathsMap[temp], child);

             buildFullPathMap(nary->root, allPathsMap);
             setPathVals(dirMap);
            cout << "we are at the end of ex2" << endl;
            for (const auto& pair : allPathsMap) {
                std::cout << pair.first << " " << pair.second->directory << std::endl;
                //getDirectory(pair.first, &apple);
                for(int i =0; i < 25; i++){
                    // nugget[i] = '\0';
                }
            }*/
        }
    }
}
int Wad::getContents(const std::string &path, char *buffer, int length, int offset) {
    //cout << "path: " << path << " is content: " << isContent(path) <<  endl;
    if(!isContent(path)) {
        //cout << "this" << endl;
        return -1;
    }

    if(allPathsMap[path] == nullptr){
        //cout << "that" << endl;
        return -1;
    }
    //cout << " hi " << endl;
    int sizeOfData = allPathsMap[path]->elementLength;
    int dataOffset = allPathsMap[path]->elementOffset;
    if(!file.is_open()){
        file.open(fileName);
    }
    if (file.is_open()) {
        //cout <<" yay "<< endl;
        if (file) {
            //cout << " ya" << endl;
            file.seekg(dataOffset, ios::beg);
            if(offset == 0){
                if(length <= sizeOfData){
                    file.read(buffer, length);
                    buffer[length] = '\0';
                    //cout << "copied data = " << buffer << '\0' << endl;
                    //file.close();
                    return length;
                }else if(length > sizeOfData){
                    file.read(buffer, sizeOfData);
                    buffer[sizeOfData] = '\0';
                    //cout << "copied data = " << buffer << '\0' << endl;
                    //file.close();
                    return sizeOfData;
                }
            }else if(offset > 0){
                if(offset + length <= sizeOfData){
                    file.seekg(dataOffset + offset, ios::beg);
                    file.read(buffer, length);
                    buffer[length] = '\0';
                    //cout << "copied data = " << buffer << '\0' << endl;

                    //file.close();
                    return length;
                }else if(offset + length > sizeOfData){
                    if(offset >= sizeOfData){
                        //file.close();
                        return 0;
                    }
                    int temp = sizeOfData - offset;
                    file.seekg(dataOffset + offset, ios::beg);
                    file.read(buffer, temp);
                    buffer[temp] = '\0';
                    //cout << "copied data = " << buffer << '\0' << endl;
                    //file.close();
                    return temp;
                }
            }
            //file.close();
        }
    }
    //cout << " apple " << endl;
    return -1;
}
bool Wad::isContent(const std::string &path) {
    //cout << " feafafd" << endl;

    string temp = path;
    if(temp.back() == '/' && temp.size() > 1){
        temp.pop_back();
    }
    if(allPathsMap.find(temp) == allPathsMap.end()){
        return false;
    }else {
        if(allPathsMap[temp] != nullptr){
            if(allPathsMap[temp]->content == 1){
                return true;
            }else if(allPathsMap[temp]->content == 0) {
                return false;
            }
        }
    }
    return false;
}
bool Wad::isDirectory(const std::string &path) {
    string temp = path;
    if(temp.back() == '/' && temp.size() > 1){
        //cout << "org" << endl;
        string substring = temp.substr(0, (temp.size()-1));
        //temp.pop_back();
       // cout << "temp then Sub: " <<temp <<  " "  << substring << endl;
        temp = substring;
    }


    if(allPathsMap.find(temp) == allPathsMap.end()){
        //cout << "Ap" << endl;
        return false;
    }else if(allPathsMap[temp] != nullptr) {
        //cout << " app[l;e " << endl;
        if(allPathsMap[temp]->directory){
            //cout << "in here? " << allPathsMap[path]->directory << endl;
            return true;
        }
    }

    return false;
}
void Wad::BFS(Node *root)  {
    if (root == nullptr) return;

    queue<Node*> nodeQueue;
    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        Node* currentNode = nodeQueue.front();
        nodeQueue.pop();

        // Process the current node
       // cout << currentNode->pathName << endl;

        // Enqueue all children of the current node
        for (Node* child : currentNode->children) {
            nodeQueue.push(child);
        }
    }
}
int Wad::getSize(const std::string &path) {
    //cout << " path " << path << endl;
    if(isContent(path)){
        for (const auto& pair : allPathsMap) {
             //std::cout << pair.first << " " << pair.second->elementLength << std::endl;
        }
        return allPathsMap[path]->elementLength;
    }else {
        return -1;
    }

}
void Wad::setPathVals(map<std::string, int> &pathMap) {
    for (auto& pair : pathMap) {
         string key = pair.first;
        if (key.length() >= 8) {
            string lastEight = key.substr(key.length() - 8);
            string lastFour = key.substr((key.length() -4));
            size_t found = lastEight.find("_START");
            if(found != string::npos){
                pair.second = 0;
            }
            if(lastEight[1] == 'E' && lastEight[3] == 'M' || lastFour[0] == 'E' && lastFour[2] == 'M'){
                pair.second = 0;
            }
        }else if(key == "/" || key[1] == 'E' && key[3] == 'M'){
            pair.second = 0;
        }
    }
}
void Wad::dirMapBuilder(Node* root, std::map<std::string, int>& pathMap) {
    if (root == nullptr) return;

    std::queue<std::pair<Node*, std::string>> nodeQueue; // Queue holds pairs of Node* and their full path
    nodeQueue.push({root, ""}); // Root has an empty path initially

    while (!nodeQueue.empty()) {
        auto [currentNode, path] = nodeQueue.front();
        nodeQueue.pop();

        // Create the full path for the current node
        std::string fullPath;
        if (path.empty()) {
            // For the root node, the path is just its name
            fullPath = currentNode->pathName;
        } else if (path == "/"){
            // For other nodes, append the current node's name to the parent's path
            fullPath = path + currentNode->pathName;
        }else {
            fullPath = path + "/" + currentNode->pathName;

        }
        // Add the full path to the map
        pathMap[fullPath] = 1;
        // 0 = directory
        // 1 = file

        // Enqueue all children of the current node with the updated path
        for (Node* child : currentNode->children) {
            nodeQueue.push({child, fullPath});
        }
    }
}
void Wad::buildFullPathMap(Node* root, std::map<std::string, Node*>& pathMap) {
    if (root == nullptr) return;

    std::queue<std::pair<Node*, std::string>> nodeQueue; // Queue holds pairs of Node* and their full path
    nodeQueue.push({root, ""}); // Root has an empty path initially

    while (!nodeQueue.empty()) {
        auto [currentNode, path] = nodeQueue.front();
        nodeQueue.pop();

        std::string fullPath;
        if (path.empty()) {
            fullPath = currentNode->pathName; // Root node path
        } else if (path == "/") {
            fullPath = path + currentNode->pathName; // Direct child of root
        } else {
            fullPath = path + "/" + currentNode->pathName; // Other nodes
        }

        // Map the full path to the node pointer
        pathMap[fullPath] = currentNode;

        // Enqueue all children of the current node with the updated path
        for (Node* child : currentNode->children) {
            nodeQueue.push({child, fullPath});
        }
    }
}



string Wad::getMagic() {return Magic;}

Wad::Wad(const std::string &path) : file(path) {
    fileName = path;
    allPathsMap = rebuildMap(nary->root);
    //finalApple = rebuildMap(nary->root);
   // cout << "path: " << path << endl;
}


/*rule:  Wad.cpp Nodes.cpp  Nary.cpp
        g++ -c Wad.cpp -o Wad.o
        g++ -c Nary.cpp -o Nary.o
        g++ -c Nodes.cpp -o Nodes.o
        ar rcs libWad.a Wad.o Nary.o Nodes.o
        ar -t libWad.a
*/




