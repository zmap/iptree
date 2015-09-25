#include "iptree.h"
#include <iostream>
#include <string>
#include <fstream>

#include "json/json.h"
using namespace std;

void buildTree(std::string file, node_t * root) {
    std::string line;
    std::ifstream test_data(file, std::ifstream::in); 
    while (test_data) {
        std::getline(test_data, line);
        Json::Value res;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(line, res);
        if(!parsingSuccessful) {
            return;
        }
        string *prefix = new string(res["bgp_prefix"].asString());
        insert(root, prefix->c_str(), 
                     const_cast<char*>(prefix->c_str()));
    }
}

void testTree(std::string file, node_t * root) {
    std::string line;
    std::ifstream in_data(file, std::ifstream::in); 
    while (in_data) {
        std::getline(in_data, line);
        if(line.size() < 7) {
            continue;
        }
        std::cout << lookup_best(root, line.c_str()) << endl;
    }
}

int main() {
    node_t * root = create_iptree();
    buildTree("test_data.json", root);
    testTree("in.txt", root);
    destroy_iptree(root);
    return 0;
}
