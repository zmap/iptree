#include "iptree.h"
#include <gtest/gtest.h>

TEST(CompilationTest, TestsCompile) {
	EXPECT_TRUE(true);
}

TEST(CreateTest,CreateIpTree) {
    iptree_node_t * root = iptree_create();
    EXPECT_EQ(root->prefix, 0);
    EXPECT_EQ(root->mask, 0);
    EXPECT_EQ(root->l, nullptr);
    EXPECT_EQ(root->r, nullptr);
    EXPECT_EQ(root->data, nullptr);
    iptree_destroy(root);
}

TEST(InsertTest, BasicInsert) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * first = iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    iptree_node_t * second = iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    iptree_node_t * third = iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    iptree_node_t * fourth = iptree_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    iptree_node_t * fifth = iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_EQ(root->r->r->r, third);
    EXPECT_EQ(root->r->r->r->r, fourth);
    EXPECT_EQ(root->r->r->r->r->r, fifth);
    EXPECT_STREQ(iptree_lookup_exact(root,0x80000000,0x80000000)->data, "first child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xC0000000,0xC0000000)->data, "second child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7000000,0xFF000000)->data, "third child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFF000,0xFFFFF000)->data, "fourth child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "fifth child");
    iptree_destroy(root);
}

TEST(InsertTest,InsertRoot) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0xFFFFFFFF, 0x00000000, "root value");
    EXPECT_EQ(root->data, "root value");
    iptree_destroy(root);
}

TEST(InsertTest,InsertExisting) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFE, "fourth child");
    iptree_node_t * right = iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "right child");
    iptree_node_t * left = iptree_insert(root, 0xF7FFFFFE, 0xFFFFFFFF, "left child");
    EXPECT_EQ(root->r->r->r->r->r, right);
    EXPECT_EQ(root->r->r->r->r->l, left);
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFFFFE,0xFFFFFFFF)->data, "left child");
    right = iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "new right child");
    left = iptree_insert(root, 0xF7FFFFFE, 0xFFFFFFFF, "new left child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "new right child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF7FFFFFE,0xFFFFFFFF)->data, "new left child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertSameIPLargerMask) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * first = iptree_insert(root, 0xFFFFFFFF, 0x80000000, "first child");
    iptree_node_t * second = iptree_insert(root, 0xFFFFFFFF, 0xFFFF0000, "second child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(iptree_lookup_exact(root,0xFFFFFFFF,0x80000000)->data, "first child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xFFFFFFFF,0xFFFF0000)->data, "second child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertSameIPSmallerMask) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * second = iptree_insert(root, 0xFFFFFFFF, 0xFFFF0000, "second child");
    EXPECT_EQ(root->r, second);
    iptree_node_t * first = iptree_insert(root, 0xFFFFFFFF, 0x80000000, "first child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(iptree_lookup_exact(root,0xFFFFFFFF,0x80000000)->data, "first child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xFFFFFFFF,0xFFFF0000)->data, "second child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertPrefixOfExisting) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * second = iptree_insert(root, 0x0000FFFF, 0xFFFFFFFF, "second child");
    EXPECT_EQ(root->r, second);
    iptree_node_t * first = iptree_insert(root, 0x00000000, 0xFFFF0000, "first child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(iptree_lookup_exact(root,0x0000FFFF,0xFFFF0000)->data, "first child");
    EXPECT_STREQ(iptree_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "second child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertCommonPrefix) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * right = iptree_insert(root, 0x0000FFFF, 0xFFFFFFFF, "right child");
    EXPECT_EQ(root->r, right);
    iptree_node_t * left = iptree_insert(root, 0x00F0FFFF, 0xFFFFFFFF, "left child");
    EXPECT_EQ(root->r->prefix, 0x00000000);
    EXPECT_EQ(root->r->mask, 0xFF000000);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, left);
    EXPECT_EQ(iptree_lookup_exact(root,0x00000000,0xFF000000)->data, nullptr);
    EXPECT_STREQ(iptree_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(iptree_lookup_exact(root,0x00F0FFFF,0xFFFFFFFF)->data, "left child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertIntoIntermediary) {
    iptree_node_t * root = iptree_create();
    iptree_node_t * right = iptree_insert(root, 0x0000FFFF, 0xFFFFFFFF, "right child");
    iptree_node_t * left = iptree_insert(root, 0x00F0FFFF, 0xFFFFFFFF, "left child");
    iptree_node_t * intermediary = iptree_insert(root, 0x00000000, 0xFF000000, "intermediary");
    EXPECT_EQ(root->r, intermediary);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, left);
    EXPECT_EQ(iptree_lookup_exact(root,0x00000000,0xFF000000)->data, "intermediary");
    EXPECT_STREQ(iptree_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(iptree_lookup_exact(root,0x00F0FFFF,0xFFFFFFFF)->data, "left child");
    iptree_destroy(root);
}

TEST(InsertTest, InsertCommonPrefixFullChildren) {
    // Similar to above children but both left and right children exist when
    // the ip with a common prefix is inserted
    iptree_node_t * root = iptree_create();
    iptree_node_t * right = iptree_insert(root, 0x0000FFFF, 0xFFFF0000, "right child");
    EXPECT_EQ(root->r, right);
    iptree_node_t * left = iptree_insert(root, 0xF0F0FFFF, 0xFFFF0000, "left child");
    EXPECT_EQ(root->l, left);
    iptree_node_t * common_with_left = iptree_insert(root, 0xF0FFFFFF, 0xFFFF0000, "left left child");
    iptree_node_t * common_with_right = iptree_insert(root, 0x000FFFFF, 0xFFFF0000, "right left child");
    EXPECT_EQ(root->l->prefix, 0xF0F00000);
    EXPECT_EQ(root->l->mask, 0xFFF00000);
    EXPECT_EQ(root->r->prefix, 0x00000000);
    EXPECT_EQ(root->r->mask, 0xFFF00000);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, common_with_right);
    EXPECT_EQ(root->l->r, left);
    EXPECT_EQ(root->l->l, common_with_left);
    EXPECT_EQ(iptree_lookup_exact(root,0x00000000,0xFFF00000)->data, nullptr);
    EXPECT_EQ(iptree_lookup_exact(root,0xF0F00000,0xFFF00000)->data, nullptr);
    EXPECT_STREQ(iptree_lookup_exact(root,0x0000FFFF,0xFFFF0000)->data, "right child");
    EXPECT_STREQ(iptree_lookup_exact(root,0x000FFFFF,0xFFFF0000)->data, "right left child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF0F0FFFF,0xFFFF0000)->data, "left child");
    EXPECT_STREQ(iptree_lookup_exact(root,0xF0FFFFFF,0xFFFF0000)->data, "left left child");
    iptree_destroy(root);
}

TEST(LookupTest, BasicLookup) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "root");
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "third child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    iptree_destroy(root);

}

TEST(LookupTest, LookupWithIntermediary) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "root");
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "third child");
    iptree_insert(root, 0xF60FFFFF, 0xFF000000, "wrong child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "third child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    iptree_destroy(root);

}

TEST(LookupTest, LookupRoot) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(iptree_lookup_best(root,0x07FFFFFF)->data, "root");
    iptree_destroy(root);

}

TEST(LookupTest, LookupAffectedByInsert) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "third child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "new third child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "new third child");
    iptree_destroy(root);

}

TEST(LookupTest, LookupWithNullRoot) {
    iptree_node_t * root = iptree_create();
    EXPECT_EQ(iptree_lookup_best(root,0xF7FFFFFF), nullptr);
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "first child");
    iptree_destroy(root);

}

TEST(RemoveTest, RemoveTest) {
    iptree_node_t * root = iptree_create();
    iptree_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    iptree_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    iptree_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    iptree_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    iptree_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    iptree_remove(root, 0xF7FFFFFF, 0xFFFFFFFF);
    EXPECT_STREQ(iptree_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    iptree_destroy(root);

}

TEST(WrapperTest, WrapperTest) {
    iptree_node_t * root = iptree_create();
    iptree_insert_str(root, "143.255.255.255/0", "root");
    EXPECT_STREQ(iptree_lookup_best_str(root,"247.255.255.255"), "root");
    iptree_insert_str(root, "143.255.255.255/1", "first child");
    EXPECT_STREQ(iptree_lookup_best_str(root, "247.255.255.255"), "first child");
    iptree_insert_str(root, "199.255.255.255/2", "second child");
    EXPECT_STREQ(iptree_lookup_best_str(root, "247.255.255.255"), "second child");
    iptree_insert_str(root, "247.15.255.255/8", "third child");
    EXPECT_STREQ(iptree_lookup_best_str(root, "247.255.255.255"), "third child");
    iptree_insert_str(root, "247.255.255.255/20", "fourth child");
    EXPECT_STREQ(iptree_lookup_best_str(root, "247.255.255.255"), "fourth child");
    iptree_insert_str(root, "247.255.255.255/32", "fifth child");
    EXPECT_STREQ(iptree_lookup_best_str(root, "247.255.255.255"), "fifth child");
    iptree_destroy(root);
}

TEST(IPTreeTest, ParseCidr) {
    uint32_t ip, mask;
    EXPECT_EQ(EXIT_SUCCESS, iptree_parse_cidr("1.2.3.4/32", &ip, &mask));
    EXPECT_EQ(0x01020304, ip);
    EXPECT_EQ(mask, 0xFFFFFFFF);

    EXPECT_EQ(EXIT_SUCCESS, iptree_parse_cidr("228.32.127.4/17", &ip, &mask));
    EXPECT_EQ(0xE4207F04, ip);
    EXPECT_EQ(mask, 0xFFFF8000);

    EXPECT_EQ(EXIT_FAILURE, iptree_parse_cidr("1.2.3.4/33", &ip, &mask));
    EXPECT_EQ(EXIT_FAILURE, iptree_parse_cidr("1.2.3.4/-2", &ip, &mask));

    EXPECT_EQ(EXIT_SUCCESS, iptree_parse_cidr("13.1.99.1/1", &ip, &mask));
    EXPECT_EQ(0x0D016301, ip);
    EXPECT_EQ(mask, 0x80000000);
}
