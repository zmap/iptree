#ifndef _IPTREE_H
#define _IPTREE_H

#include <stdint.h>

typedef struct _node_t {
   uint32_t prefix;		/* what ip prefix */
   uint32_t mask;    		/* the mask for the subnet */
   struct _node_t *l, *r;	/* left and right children */
   char *data;			/* pointer to data */
} node_t;

node_t *create_iptree ();
uint32_t findCommonMask (uint32_t ip1, uint32_t ip2);
node_t *prefix_lookup_exact (node_t *root, uint32_t ip, uint32_t mask);
node_t *prefix_lookup_best (node_t *root, uint32_t ip);
node_t *prefix_insert (node_t *root, uint32_t ip, uint32_t mask, char *data);
void prefix_remove (node_t *root, uint32_t ip, uint32_t mask);
void destroy_iptree (node_t *root);

#endif /* _IPTREE_H */
