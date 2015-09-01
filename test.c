#include "subnettree.h"
#include <stdio.h>

int main(void) {
    node_t * root = create_subnettree();
    prefix_insert(root, 22, 0, "hello");
    prefix_insert(root, 2147483648, 2147483648, "he");
    prefix_insert(root, 2147483648, 4026531840, "heeee");
    prefix_insert(root, 2147483648, 3221225472, "hee");
    printf("%s \n",prefix_lookup_best(root,434234324)->data); 
    printf("%s \n",prefix_lookup_best(root,4294967294)->data); 
    printf("%s \n",prefix_lookup_best(root,434324)->data); 
    printf("%s \n",prefix_lookup_best(root,998)->data); 
    printf("%s \n",prefix_lookup_best(root,34)->data); 
    destroy_subnettree(root);
    return 0;
}
