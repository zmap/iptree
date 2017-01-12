/*
 * IPTree Copyright 2015 Regents of the University of Michigan
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

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

int iptree_parse_cidr(const char* cidr, uint32_t* ipAddr, uint32_t* mask);

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
