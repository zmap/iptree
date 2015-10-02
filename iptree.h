#ifndef _IPTREE_H
#define _IPTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern const uint32_t masks[33];

typedef struct _iptree_node_t {
   uint32_t prefix;		/* what ip prefix */
   uint32_t mask;    		/* the mask for the subnet */
   struct _iptree_node_t *l, *r;	/* left and right children */
   char *data;			/* pointer to data */
} iptree_node_t;

iptree_node_t *iptree_create ();
iptree_node_t *iptree_lookup_exact (iptree_node_t *root, uint32_t ip,
                                    uint32_t mask);
iptree_node_t *iptree_lookup_best (iptree_node_t *root, uint32_t ip);
char *iptree_lookup_best_str (iptree_node_t *root, const char * ip);
iptree_node_t *iptree_insert (iptree_node_t *root, uint32_t ip, uint32_t mask, 
                              char *data);
void iptree_insert_str (iptree_node_t *root, const char * cidr, char *data);
void iptree_remove (iptree_node_t *root, uint32_t ip, uint32_t mask);
void iptree_remove_str (iptree_node_t *root, const char * cidr);
void iptree_destroy (iptree_node_t *root);
#ifdef __cplusplus
}
#endif

#endif /* _IPTREE_H */
