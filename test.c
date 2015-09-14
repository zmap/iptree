#include "iptree.h"
#include <stdio.h>

int main(void) {
    //node_t * root = create_iptree();
    //prefix_insert(root, 22, 0, "hello");
    //prefix_insert(root, 2147483648, 2147483648, "he");
    //prefix_insert(root, 2147483648, 4026531840, "heeee");
    //prefix_insert(root, 2147483648, 3221225472, "hee");
    //printf("%s \n",prefix_lookup_best(root,434234324)->data); 
    //printf("%s \n",prefix_lookup_best(root,4294967294)->data); 
    //printf("%s \n",prefix_lookup_best(root,434324)->data); 
    //printf("%s \n",prefix_lookup_best(root,998)->data); 
    //printf("%s \n",prefix_lookup_best(root,34)->data); 
    //destroy_iptree(root);
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    prefix_remove(root, 0xF7FFFFFF, 0xFFFFFFFF);
    destroy_iptree(root);
    return 0;
}
