#include "iptree.h"
#include <iostream>
#include <string>
#include <fstream>

#include "json/json.h"
using namespace std;

void buildTree(std::string file, iptree_node_t * root) {
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
        iptree_insert_str(root, prefix->c_str(), 
                     const_cast<char*>(prefix->c_str()));
    }
}

void testTree(std::string file, iptree_node_t * root) {
    std::string line;
    std::ifstream in_data(file, std::ifstream::in); 
    while (in_data) {
        std::getline(in_data, line);
        if(line.size() < 7) {
            continue;
        }
        std::cout << iptree_lookup_best_str(root, line.c_str()) << endl;
    }
}

int main() {
    iptree_node_t * root = iptree_create();
    buildTree("test_data.json", root);
    testTree("in.txt", root);
    iptree_destroy(root);
    return 0;
}
