#include "iptree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const uint32_t masks[33] = {0x00000000, 0x80000000, 0xC0000000, 0xE0000000,
                            0xF0000000, 0xF8000000, 0xFC000000, 0xFE000000, 
                            0xFF000000, 0xFF800000, 0xFFC00000, 0xFFE00000, 
                            0xFFF00000, 0xFFF80000, 0xFFFC0000, 0xFFFE0000, 
                            0xFFFF0000, 0xFFFF8000, 0xFFFFC000, 0xFFFFE000,
                            0xFFFFF000, 0xFFFFF800, 0xFFFFFC00, 0xFFFFFE00, 
                            0xFFFFFF00, 0xFFFFFF80, 0xFFFFFFC0, 0xFFFFFFE0, 
                            0xFFFFFFF0, 0xFFFFFFF8, 0xFFFFFFFC, 0xFFFFFFFE,
                            0xFFFFFFFF};

iptree_node_t *iptree_create () {
    iptree_node_t *root = calloc(1, sizeof(iptree_node_t));
    root->prefix = 0;
    root->mask = 0;
    root->l = NULL;
    root->r = NULL;
    root->data = NULL;
    return root;
}

iptree_node_t *create_prefix (uint32_t ip, uint32_t mask, char* data) {
    iptree_node_t *node = calloc(1, sizeof(iptree_node_t));
    node->prefix = ip & mask;
    node->mask = mask;
    node->l = NULL;
    node->r = NULL;
    node->data = data;
    return node;
}

uint32_t findCommonMask (uint32_t ip1, uint32_t ip2){
    uint32_t ipXOR = ip1 ^ ip2;
    uint32_t mask = masks[__builtin_clz(ipXOR)];
    return mask;
}

int iptree_parse_cidr(const char* cidr, uint32_t* ipAddr, uint32_t* mask) {
    int ipbytes[4];
    char *slash = strchr(cidr, '/');
    if (slash) {
        int count = sscanf(cidr, "%d.%d.%d.%d/%d", &ipbytes[3], &ipbytes[2],
			&ipbytes[1], &ipbytes[0], mask);
	if (count != 5) {
		return EXIT_FAILURE;
	}
    } else {
        int count = sscanf(cidr, "%d.%d.%d.%d", &ipbytes[3], &ipbytes[2],
			&ipbytes[1], &ipbytes[0]);
	if (count != 4) {
		return EXIT_FAILURE;
	}
        *mask = 0;
    }
    if (*mask > 32) {
		return EXIT_FAILURE;
    }
    *mask = masks[*mask];
    *ipAddr = ipbytes[0] | ipbytes[1] << 8 | ipbytes[2] << 16 | ipbytes[3] << 24;
    return EXIT_SUCCESS;
}

iptree_node_t *insert_helper (iptree_node_t *root, uint32_t ip, uint32_t mask, 
                              char *data) {
    uint32_t commonMask;
    if (root->r == NULL) {
        root->r = create_prefix(ip, mask, data);
        return root->r;
    } else if (root->r->prefix == (ip & root->r->mask)) {
        if (root->r->mask == mask) {
            root->r->data = data;
            return root->r;
        } else if (root->r->mask > mask) {
            iptree_node_t *node = create_prefix(ip, mask, data);
            node->r = root->r;
            root->r = node;
            return root->r;
        } else {       	
            return insert_helper(root->r, ip, mask, data);
        }
    } else if (ip == (root->r->prefix & mask)) {
        iptree_node_t *node = create_prefix(ip, mask, data);
        node->r = root->r;
        root->r = node;
        return root->r;
    } else if (((commonMask = findCommonMask(root->r->prefix, ip)) & ip) 
                  != root->prefix || commonMask > root->mask) {
        iptree_node_t *node = create_prefix(ip, commonMask, NULL);
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
            iptree_node_t *node = create_prefix(ip, mask, data);
            node->r = root->l;
            root->l = node;
            return root->l;
        } else {       	
            return insert_helper(root->l, ip, mask, data);
        }
    } else if (ip == (root->l->prefix & mask)) {
        iptree_node_t *node = create_prefix(ip, mask, data);
        node->r = root->l;
        root->l = node;
        return root->l;
    } else if (((commonMask = findCommonMask(root->l->prefix, ip)) & ip) 
                  != root->prefix || commonMask > root->mask) {
        iptree_node_t *node = create_prefix(ip, commonMask, NULL);
        node->r = root->l;
        node->l = create_prefix(ip, mask, data);
        root->l = node;
        return root->l->l;
    } else {
        return NULL;
    }
}

iptree_node_t *iptree_insert (iptree_node_t *root, uint32_t ip, uint32_t mask, 
                              char *data) {
    if (0 == mask) {
        root->data = data;
        return root;
    }
    return insert_helper(root, ip, mask, data);
}

void iptree_insert_str (iptree_node_t *root, const char * cidr, char *data) {
    uint32_t ipAddr;
    uint32_t mask;
    iptree_parse_cidr(cidr, &ipAddr, &mask);
    iptree_insert(root, ipAddr, mask, data);
}

iptree_node_t *lookup_helper (iptree_node_t *root, uint32_t ip, uint32_t mask) {
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

iptree_node_t *iptree_lookup_exact (iptree_node_t *root, uint32_t ip, 
                                    uint32_t mask) {
    if (0 == mask) {
        return root;
    }
    return lookup_helper(root, ip, mask);
}

iptree_node_t *lookup_best_helper (iptree_node_t *root, uint32_t ip, 
                                   iptree_node_t* prefix) {
    if (root->r == NULL) {

    } else if (root->r->prefix == (ip & root->r->mask)) {
        if (root->r->data != NULL) {
            prefix = root->r;
        }
        return lookup_best_helper(root->r, ip, prefix);
    }
    if (root->l == NULL) {
        return prefix;
    } else if (root->l->prefix == (ip & root->l->mask)) {
        if (root->l->data != NULL) {
            prefix = root->l;
        }
        return lookup_best_helper(root->l, ip, prefix);
    } else {
        return prefix;
    }
}

iptree_node_t *iptree_lookup_best (iptree_node_t *root, uint32_t ip) {
    iptree_node_t *prefix = ((root->data != NULL) ? root : NULL);
    return lookup_best_helper(root, ip, prefix); 
}

char * iptree_lookup_best_str (iptree_node_t *root, const char * ip) {
    uint32_t ipAddr;
    uint32_t mask;
    iptree_parse_cidr(ip, &ipAddr, &mask);
    iptree_node_t * ret = iptree_lookup_best(root, ipAddr);
    return ((ret == NULL) ? ret : ret->data);
}

void iptree_remove (iptree_node_t *root, uint32_t ip, uint32_t mask) {
    iptree_lookup_exact(root, ip, mask)->data = NULL;
}

void iptree_remove_str (iptree_node_t *root, const char * cidr) {
    uint32_t ipAddr;
    uint32_t mask;
    iptree_parse_cidr(cidr, &ipAddr, &mask);
    iptree_remove(root, ipAddr, mask);
}

void iptree_destroy (iptree_node_t *root) {
    if (root == NULL) {
        return;
    }
    iptree_destroy(root->r);
    iptree_destroy(root->l);
    free(root);
}
