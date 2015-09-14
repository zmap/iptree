extern "C" {
    #include "iptree.h"
}
#include <gtest/gtest.h>

TEST(CompilationTest, TestsCompile) {
	EXPECT_TRUE(true);
}

TEST(CreateTest,CreateIpTree) {
    node_t * root = create_iptree();
    EXPECT_EQ(root->prefix, 0);
    EXPECT_EQ(root->mask, 0);
    EXPECT_EQ(root->l, nullptr);
    EXPECT_EQ(root->r, nullptr);
    EXPECT_EQ(root->data, nullptr);
    destroy_iptree(root);
}

TEST(InsertTest, BasicInsert) {
    node_t * root = create_iptree();
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_EQ(root->r->r->r, third);
    EXPECT_EQ(root->r->r->r->r, fourth);
    EXPECT_EQ(root->r->r->r->r->r, fifth);
    EXPECT_STREQ(prefix_lookup_exact(root,0x80000000,0x80000000)->data, "first child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xC0000000,0xC0000000)->data, "second child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7000000,0xFF000000)->data, "third child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFF000,0xFFFFF000)->data, "fourth child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "fifth child");
    destroy_iptree(root);
}

TEST(InsertTest,InsertRoot) {
    node_t * root = create_iptree();
    prefix_insert(root, 0xFFFFFFFF, 0x00000000, "root value");
    EXPECT_EQ(root->data, "root value");
    destroy_iptree(root);
}

TEST(InsertTest,InsertExisting) {
    node_t * root = create_iptree();
    prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFE, "fourth child");
    node_t * right = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "right child");
    node_t * left = prefix_insert(root, 0xF7FFFFFE, 0xFFFFFFFF, "left child");
    EXPECT_EQ(root->r->r->r->r->r, right);
    EXPECT_EQ(root->r->r->r->r->l, left);
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFFFFE,0xFFFFFFFF)->data, "left child");
    right = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "new right child");
    left = prefix_insert(root, 0xF7FFFFFE, 0xFFFFFFFF, "new left child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFFFFF,0xFFFFFFFF)->data, "new right child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF7FFFFFE,0xFFFFFFFF)->data, "new left child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertSameIPLargerMask) {
    node_t * root = create_iptree();
    node_t * first = prefix_insert(root, 0xFFFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xFFFFFFFF, 0xFFFF0000, "second child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(prefix_lookup_exact(root,0xFFFFFFFF,0x80000000)->data, "first child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xFFFFFFFF,0xFFFF0000)->data, "second child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertSameIPSmallerMask) {
    node_t * root = create_iptree();
    node_t * second = prefix_insert(root, 0xFFFFFFFF, 0xFFFF0000, "second child");
    EXPECT_EQ(root->r, second);
    node_t * first = prefix_insert(root, 0xFFFFFFFF, 0x80000000, "first child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(prefix_lookup_exact(root,0xFFFFFFFF,0x80000000)->data, "first child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xFFFFFFFF,0xFFFF0000)->data, "second child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertPrefixOfExisting) {
    node_t * root = create_iptree();
    node_t * second = prefix_insert(root, 0x0000FFFF, 0xFFFFFFFF, "second child");
    EXPECT_EQ(root->r, second);
    node_t * first = prefix_insert(root, 0x00000000, 0xFFFF0000, "first child");
    EXPECT_EQ(root->r, first);
    EXPECT_EQ(root->r->r, second);
    EXPECT_STREQ(prefix_lookup_exact(root,0x0000FFFF,0xFFFF0000)->data, "first child");
    EXPECT_STREQ(prefix_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "second child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertCommonPrefix) {
    node_t * root = create_iptree();
    node_t * right = prefix_insert(root, 0x0000FFFF, 0xFFFFFFFF, "right child");
    EXPECT_EQ(root->r, right);
    node_t * left = prefix_insert(root, 0x00F0FFFF, 0xFFFFFFFF, "left child");
    EXPECT_EQ(root->r->prefix, 0x00000000);
    EXPECT_EQ(root->r->mask, 0xFF000000);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, left);
    EXPECT_EQ(prefix_lookup_exact(root,0x00000000,0xFF000000)->data, nullptr);
    EXPECT_STREQ(prefix_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(prefix_lookup_exact(root,0x00F0FFFF,0xFFFFFFFF)->data, "left child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertIntoIntermediary) {
    node_t * root = create_iptree();
    node_t * right = prefix_insert(root, 0x0000FFFF, 0xFFFFFFFF, "right child");
    node_t * left = prefix_insert(root, 0x00F0FFFF, 0xFFFFFFFF, "left child");
    node_t * intermediary = prefix_insert(root, 0x00000000, 0xFF000000, "intermediary");
    EXPECT_EQ(root->r, intermediary);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, left);
    EXPECT_EQ(prefix_lookup_exact(root,0x00000000,0xFF000000)->data, "intermediary");
    EXPECT_STREQ(prefix_lookup_exact(root,0x0000FFFF,0xFFFFFFFF)->data, "right child");
    EXPECT_STREQ(prefix_lookup_exact(root,0x00F0FFFF,0xFFFFFFFF)->data, "left child");
    destroy_iptree(root);
}

TEST(InsertTest, InsertCommonPrefixFullChildren) {
    // Similar to above children but both left and right children exist when
    // the ip with a common prefix is inserted
    node_t * root = create_iptree();
    node_t * right = prefix_insert(root, 0x0000FFFF, 0xFFFF0000, "right child");
    EXPECT_EQ(root->r, right);
    node_t * left = prefix_insert(root, 0xF0F0FFFF, 0xFFFF0000, "left child");
    EXPECT_EQ(root->l, left);
    node_t * common_with_left = prefix_insert(root, 0xF0FFFFFF, 0xFFFF0000, "left left child");
    node_t * common_with_right = prefix_insert(root, 0x000FFFFF, 0xFFFF0000, "right left child");
    EXPECT_EQ(root->l->prefix, 0xF0F00000);
    EXPECT_EQ(root->l->mask, 0xFFF00000);
    EXPECT_EQ(root->r->prefix, 0x00000000);
    EXPECT_EQ(root->r->mask, 0xFFF00000);
    EXPECT_EQ(root->r->r, right);
    EXPECT_EQ(root->r->l, common_with_right);
    EXPECT_EQ(root->l->r, left);
    EXPECT_EQ(root->l->l, common_with_left);
    EXPECT_EQ(prefix_lookup_exact(root,0x00000000,0xFFF00000)->data, nullptr);
    EXPECT_EQ(prefix_lookup_exact(root,0xF0F00000,0xFFF00000)->data, nullptr);
    EXPECT_STREQ(prefix_lookup_exact(root,0x0000FFFF,0xFFFF0000)->data, "right child");
    EXPECT_STREQ(prefix_lookup_exact(root,0x000FFFFF,0xFFFF0000)->data, "right left child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF0F0FFFF,0xFFFF0000)->data, "left child");
    EXPECT_STREQ(prefix_lookup_exact(root,0xF0FFFFFF,0xFFFF0000)->data, "left left child");
    destroy_iptree(root);
}

TEST(LookupTest, BasicLookup) {
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    destroy_iptree(root);

}

TEST(LookupTest, LookupWithIntermediary) {
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "third child");
    node_t * wrong = prefix_insert(root, 0xF60FFFFF, 0xFF000000, "wrong child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    destroy_iptree(root);

}

TEST(LookupTest, LookupRoot) {
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(prefix_lookup_best(root,0x07FFFFFF)->data, "root");
    destroy_iptree(root);

}

TEST(LookupTest, LookupAffectedByInsert) {
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "third child");
    third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "new third child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "new third child");
    destroy_iptree(root);

}

TEST(LookupTest, LookupWithNullRoot) {
    node_t * root = create_iptree();
    EXPECT_EQ(prefix_lookup_best(root,0xF7FFFFFF), nullptr);
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "first child");
    destroy_iptree(root);

}

TEST(RemoveTest, RemoveTest) {
    node_t * root = create_iptree();
    node_t * top = prefix_insert(root, 0x8FFFFFFF, 0x00000000, "root");
    node_t * first = prefix_insert(root, 0x8FFFFFFF, 0x80000000, "first child");
    node_t * second = prefix_insert(root, 0xC7FFFFFF, 0xC0000000, "second child");
    node_t * third = prefix_insert(root, 0xF70FFFFF, 0xFF000000, "third child");
    node_t * fourth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFF000, "fourth child");
    node_t * fifth = prefix_insert(root, 0xF7FFFFFF, 0xFFFFFFFF, "fifth child");
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fifth child");
    prefix_remove(root, 0xF7FFFFFF, 0xFFFFFFFF);
    EXPECT_STREQ(prefix_lookup_best(root,0xF7FFFFFF)->data, "fourth child");
    destroy_iptree(root);

}
