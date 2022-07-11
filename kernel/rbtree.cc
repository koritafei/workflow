#include "rbtree.h"

/**
 * @brief
 * 以当前节点node为轴左旋，将node的右孩子变为根节点，当前节点变为根节点的左子树
 *        同时node右孩子的左子树，变为node的右子树
 * @param  node  旋转轴
 * @param  root  根节点
 * */
static void __rb_lotate_left(struct rb_node *node, struct rb_root *root) {
  struct rb_node *right = node->rb_right;

  // 先赋值，在判断值是否为空
  // 右子树的左孩子变为旋转中心的右孩子
  if ((node->rb_right = right->rb_left)) {
    node->rb_left->rb_parent = node;
  }

  // 旋转中心变为其右孩子的左子树
  right->rb_left = node;

  // 修改右子树的父节点
  if ((right->rb_parent = node->rb_parent)) {
    // 修改父节点的左右子树
    if (node == node->rb_parent->rb_left) {
      // node 为父节点左子树
      node->rb_parent->rb_left = right;
    } else {
      // node为父节点右子树
      node->rb_parent->rb_right = right;
    }
  } else {
    // 当旋转节点的父指针为空时，直接赋值头结点
    root->rb_node = right;
  }

  // 修改旋转轴的父节点
  node->rb_parent = right;
}

/**
 * @brief
 * 将旋转轴node的左孩子变为node的父节点，node的左孩子的右子树变为node的左子树
 *        node变为左孩子的右子树
 * @param  node 旋转轴
 * @param  root 根节点
 * */
static void __rb_lotate_right(struct rb_node *node, struct rb_root *root) {
  // 旋转轴的左孩子
  struct rb_node *left = node->rb_left;

  // 左孩子的右子树变为旋转轴的左孩子
  if ((node->rb_left = left->rb_right)) {
    left->rb_right->rb_parent = node;
  }

  // 旋转轴变为左孩子的右子树
  left->rb_right = node;

  // 调整父节点指针
  if ((left->rb_parent = node->rb_parent)) {
    if (node == node->rb_parent->rb_left) {
      node->rb_parent->rb_left = left;
    } else {
      node->rb_parent->rb_right = left;
    }
  } else {
    root->rb_node = left;
  }

  node->rb_parent = left;
}

/**
 * @brief 新插入节点着色
 * @param  node   新插入节点
 * @param  root  根节点
 * */
void rb_insert_color(struct rb_node *node, struct rb_root *root) {
  // 父节点与祖父节点
  struct rb_node *parent, *gparent;

  // 当新插入节点的父节点存在，且父节点的颜色为红色时
  while ((parent = node->rb_parent) && parent->rb_color == RB_RED) {
    gparent = parent->rb_parent;

    // 父节点为左孩子
    if (parent == gparent->rb_left) {
      {
        // 获取父节点的兄弟节点
        register struct rb_node *uncle = gparent->rb_right;
        // 如果父节点的兄弟节点为红色，将父节点与其兄弟节点颜色置为黑色
        // 祖父节点颜色置为红色，向上递归
        // 即： 插入节点变为父节点
        if (uncle && uncle->rb_color == RB_RED) {
          uncle->rb_color   = RB_BLACK;
          parent->rb_color  = RB_BLACK;
          gparent->rb_color = RB_RED;
          node              = parent;
          continue;
        }
      }

      // 如果插入节点为父节点的右孩子，此时右子树破坏了红黑树规则，需要进行左旋。
      // 旋转之后node和parent节点互换
      if (parent->rb_right == node) {
        register struct rb_node *tmp;
        __rb_lotate_left(parent, root);
        tmp    = parent;
        parent = node;
        node   = tmp;
      }

      // 此时设置父节点颜色为黑色，祖父节点颜色为红色
      // 违反了红黑树性质，进行右旋
      parent->rb_color  = RB_BLACK;
      gparent->rb_color = RB_RED;
      __rb_lotate_right(gparent, root);
    } else {
      // 父节点为右孩子
      {
        //获取兄弟节点
        register struct rb_node *uncle = gparent->rb_left;
        // 兄弟节点颜色为红色
        // 父节点及其兄弟节点置为黑色，祖父节点置为红色
        // 向上递归
        if (uncle && uncle->rb_color == RB_RED) {
          uncle->rb_color   = RB_BLACK;
          parent->rb_color  = RB_BLACK;
          gparent->rb_color = RB_RED;
          node              = parent;
          continue;
        }
      }

      // 插入节点为父节点的左孩子
      if (parent->rb_left == node) {
        register struct rb_node *tmp;
        // 右旋
        __rb_lotate_right(parent, root);
        tmp    = parent;
        parent = node;
        node   = tmp;
      }

      // 以祖父节点为轴左旋
      parent->rb_color  = RB_BLACK;
      gparent->rb_color = RB_RED;
      __rb_lotate_left(gparent, root);
    }
  }

  // 根节点为黑色
  root->rb_node->rb_color = RB_BLACK;
}
