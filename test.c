#include "iptree.h"
#include <stdio.h>

int main(void) {
    iptree_node_t * root = iptree_create();
    iptree_insert_str(root, "115.114.251.0/0", "h");
    iptree_insert_str(root, "115.114.251.0/24", "heeee");
    iptree_insert_str(root, "213.5.200.0/21", "yeeee");
    iptree_insert_str(root, "8.26.230.0/23", "hee");
    iptree_insert_str(root, "121.244.20.0/23", "hee");
    iptree_insert_str(root, "204.127.151.0/24", "hee");
    iptree_insert_str(root, "91.229.238.0/24", "heeeeeeeeeeeeeeeee");
    iptree_insert_str(root, "69.197.91.0/24", "hee");
    iptree_insert_str(root, "206.117.199.0/24", "haaaaaaaaa");
    iptree_insert_str(root, "200.45.40.0/21", "hee");
    printf("%s\n", iptree_lookup_best_str(root, "91.229.0.0"));
    printf("%s\n", iptree_lookup_best_str(root, "91.229.238.0"));
    iptree_destroy(root);
    return 0;
}
