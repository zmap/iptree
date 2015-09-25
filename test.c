#include "iptree.h"
#include <stdio.h>

int main(void) {
    uint32_t ip;
    uint32_t mask;
    parseCIDR("192.122.2.2", &ip, &mask);
    node_t * root = create_iptree();
    insert(root, "115.114.251.0/0", "h");
    insert(root, "115.114.251.0/24", "heeee");
    insert(root, "213.5.200.0/21", "yeeee");
    insert(root, "8.26.230.0/23", "hee");
    insert(root, "121.244.20.0/23", "hee");
    insert(root, "204.127.151.0/24", "hee");
    insert(root, "91.229.238.0/24", "heeeeeeeeeeeeeeeee");
    insert(root, "69.197.91.0/24", "hee");
    insert(root, "206.117.199.0/24", "haaaaaaaaa");
    insert(root, "200.45.40.0/21", "hee");
    printf("%s\n", lookup_best(root, "91.229.0.0"));
    printf("%s\n", lookup_best(root, "91.229.238.0"));
    destroy_iptree(root);
    return 0;
}
