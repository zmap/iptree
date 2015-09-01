#include "subnettree.h"

#include <stdlib.h>

node_t *create_subnettree () {
    node_t *root = calloc(1, sizeof(node_t));
    root->prefix = 0;
    root->mask = 0;
    root->l = NULL;
    root->r = NULL;
    root->data = NULL;
    return root;
}

node_t *create_prefix (uint32_t ip, uint32_t mask, char* data) {
    node_t *node = calloc(1, sizeof(node_t));
    node->prefix = ip & mask;
    node->mask = mask;
    node->l = NULL;
    node->r = NULL;
    node->data = data;
    return node;
}

uint32_t findCommonMask (uint32_t ip1, uint32_t ip2){
    uint32_t ipXOR = ip1 ^ ip2;
    uint32_t suffix = 32 - __builtin_clz(ipXOR);
    uint32_t mask = (0xFFFFFFFF >> suffix) << suffix;
    return mask;
}

node_t *insert_helper (node_t *root, uint32_t ip, uint32_t mask, char *data) {
    uint32_t commonMask;
    if (root->r == NULL) {
        root->r = create_prefix(ip, mask, data);
        return root->r;
    } else if (root->r->prefix == (ip & root->r->mask)) {
        if (root->r->mask == mask) {
            root->r->data = data;
            return root->r;
        } else if (root->r->mask > mask) {
            node_t *node = create_prefix(ip, mask, data);
            node->r = root->r;
            root->r = node;
            return root->r;
        } else {       	
            return insert_helper(root->r, ip, mask, data);
        }
    } else if (ip == (root->r->prefix & mask)) {
        node_t *node = create_prefix(ip, mask, data);
        node->r = root->r;
        root->r = node;
        return root->r;
    } else if ((commonMask = findCommonMask(root->r->prefix, ip)) != root->prefix) {
        node_t *node = create_prefix(ip, commonMask, NULL);
        node->r = root->r;
        node->l = create_prefix(ip, mask, data);
        root->r = node;
        return root->r->l;
    }
    if (root->l == NULL) {
        root->l = create_prefix(ip, mask, data);
        return root->l;
    } else if (root->l->prefix == (ip & root->l->mask)) {
        if (root->l->mask == mask) {
            root->l->data = data;
            return root->l;
        } else if (root->l->mask > mask) {
            node_t *node = create_prefix(ip, mask, data);
            node->r = root->l;
            root->l = node;
            return root->l;
        } else {       	
            return insert_helper(root->l, ip, mask, data);
        }
    } else if (ip == (root->l->prefix & mask)) {
        node_t *node = create_prefix(ip, mask, data);
        node->r = root->l;
        root->l = node;
        return root->l;
    } else if ((commonMask = findCommonMask(root->l->prefix, ip)) != root->prefix) {
        node_t *node = create_prefix(ip, commonMask, NULL);
        node->r = root->l;
        node->l = create_prefix(ip, mask, data);
        root->l = node;
        return root->l->l;
    } else {
        return NULL;
    }
}

node_t *prefix_insert (node_t *root, uint32_t ip, uint32_t mask, char *data) {
    if (0 == mask) {
        root->data = data;
        return root;
    }
    return insert_helper(root, ip, mask, data);
}

node_t *lookup_helper (node_t *root, uint32_t ip, uint32_t mask) {
    if (root->r == NULL) {

    } else if (root->r->prefix == (ip & root->r->mask)) {
        if (root->r->mask == mask) {
            return root->r;
        } else if (root->r->mask > mask) {

        } else {       	
            return lookup_helper(root->r, ip, mask);
        }
    }
    if (root->l == NULL) {
        return NULL;
    } else if (root->l->prefix == (ip & root->l->mask)) {
        if (root->l->mask == mask) {
            return root->l;
        } else if (root->l->mask > mask) {
            return NULL;
        } else {       	
            return lookup_helper(root->l, ip, mask);
        }
    } else {
        return NULL;
    }
}

node_t *prefix_lookup_exact (node_t *root, uint32_t ip, uint32_t mask) {
    if (0 == mask) {
        return root;
    }
    return lookup_helper(root, ip, mask);
}

node_t *lookup_best_helper (node_t *root, uint32_t ip, node_t* prefix) {
    if (root->r == NULL) {

    } else if (root->r->prefix == (ip & root->r->mask)) {
        if (root->r->data != NULL) {
            prefix = root->r;
        }
        return lookup_best_helper(root->r, ip, prefix);
    }
    if (root->l == NULL) {
        return root;
    } else if (root->l->prefix == (ip & root->l->mask)) {
        if (root->l->data != NULL) {
            prefix = root->l;
        }
        return lookup_best_helper(root->l, ip);
    } else {
        return root;
    }
}

node_t *prefix_lookup_best (node_t *root, uint32_t ip) {
    node_t *prefix = )(root->data != NULL) ? root : NULL);
    return lookup_best_helper(root, ip, prefix); 
}

void prefix_remove (node_t *root, uint32_t ip, uint32_t mask) {
    prefix_lookup_exact(root, ip, mask)->data = NULL;
}

void destroy_subnettree (node_t *root) {
    if (root == NULL) {
        return;
    }
    destroy_subnettree(root->r);
    destroy_subnettree(root->l);
    free(root);
}
