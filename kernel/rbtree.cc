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

/**
 * @brief  改变指定节点的颜色
 * @param  node   指定节点
 * @param  parent 该指定节点的父节点
 * @param  root
 * */
static void __rb_erase_color(struct rb_node *node,
                             struct rb_node *parent,
                             struct rb_root *root) {
  struct rb_node *other;
  // 递归node 节点存在，或node的颜色为黑色，不为root节点，进行递归
  while ((!node || node->rb_color == RB_BLACK) && (node != root->rb_node)) {
    // node 节点为左孩子
    if (parent->rb_left == node) {
      // node 的兄弟节点
      other = parent->rb_right;

      // 如果兄弟节点为红色，将其节点置为黑色
      // 父节点置为红色，以父节点为中心左旋
      // 同时将父节点的右孩子设置为other
      if (RB_RED == other->rb_color) {
        other->rb_color  = RB_BLACK;
        parent->rb_color = RB_RED;
        __rb_lotate_left(parent, root);
        other = parent->rb_right;
      }

      // 判断other的左右孩子节点是否存在，或颜色是否为黑色
      // 如果是，将other节点置为红色，向上递归
      if ((!other->rb_left || other->rb_left->rb_color == RB_BLACK) &&
          (!other->rb_right || other->rb_right->rb_color == RB_BLACK)) {
        other->rb_color = RB_RED;
        node            = parent;
        parent          = node->rb_parent;
      } else {
        // 如果不是，判断是否存在右孩子，或右孩子颜色为黑色，将其左孩子的颜色置为黑色
        // other 节点置为红色，以other为旋转轴向右旋转
        // other 置为父节点的右孩子
        if (!other->rb_right || other->rb_right->rb_color == RB_BLACK) {
          register struct rb_node *o_left;
          if ((o_left = other->rb_left)) {
            o_left->rb_color = RB_BLACK;
          }

          other->rb_color = RB_RED;
          __rb_lotate_right(other, root);
          other = parent->rb_right;
        }

        // other 节点颜色置为父节点颜色
        // 父节点置为黑色
        other->rb_color  = parent->rb_color;
        parent->rb_color = RB_BLACK;

        if (other->rb_right) {
          other->rb_right->rb_color = RB_BLACK;
        }

        // 以parent为旋转轴右旋
        __rb_lotate_right(parent, root);
        node = root->rb_node;
        break;
      }

    } else {
      other = parent->rb_left;
      if (RB_RED == other->rb_color) {
        parent->rb_color = RB_RED;
        other->rb_color  = RB_BLACK;
        __rb_lotate_right(parent, root);
        other = parent->rb_left;
      }

      if ((!other->rb_left || other->rb_left->rb_color == RB_BLACK) &&
          (!other->rb_right || other->rb_right->rb_color == RB_BLACK)) {
        other->rb_color = RB_RED;
        node            = parent;
        parent          = node->rb_parent;
      } else {
        if (!other->rb_left || other->rb_left->rb_color == RB_BLACK) {
          register struct rb_node *o_right;

          if ((o_right = other->rb_right)) {
            o_right->rb_color = RB_BLACK;
          }
          other->rb_color = RB_RED;
          __rb_lotate_left(other, root);
          other = parent->rb_left;
        }

        other->rb_color  = parent->rb_color;
        parent->rb_color = RB_BLACK;
        if (other->rb_left) {
          other->rb_left->rb_color = RB_BLACK;
        }

        __rb_lotate_right(parent, root);
        node = root->rb_node;
        break;
      }
    }
  }
  if (node) {
    node->rb_color = RB_BLACK;
  }
}

void rb_erase(struct rb_node *node, struct rb_root *root) {
  struct rb_node *child, *parent;
  int             color;

  if (!node->rb_left) {
    child = node->rb_right;
  } else if (!node->rb_right) {
    child = node->rb_right;
  } else {
    struct rb_node *old = node, *left;

    node = node->rb_right;

    while ((left = node->rb_left)) {
      node = left;
    }

    child  = node->rb_right;
    parent = node->rb_parent;
    color  = node->rb_color;

    if (child) {
      child->rb_parent = parent;
    }

    if (parent) {
      if (parent->rb_left == node) {
        parent->rb_left = node;
      } else {
        parent->rb_right = node;
      }
    } else {
      root->rb_node = child;
    }

    if (old == node->rb_parent) {
      parent = node;
    }

    node->rb_color  = old->rb_color;
    node->rb_left   = old->rb_left;
    node->rb_parent = old->rb_parent;
    node->rb_right  = old->rb_right;

    if (old->rb_parent) {
      if (old->rb_parent->rb_left == old) {
        old->rb_parent->rb_left = node;
      } else {
        old->rb_parent->rb_right = node;
      }

    } else {
      root->rb_node = node;
    }

    old->rb_left->rb_parent = node;

    if (old->rb_right) {
      old->rb_right->rb_parent = node;
    }

    goto color;
  }

  parent = node->rb_parent;
  color  = node->rb_color;

  if (child) {
    child->rb_parent = child;
  }

  if (parent) {
    if (node == parent->rb_left) {
      parent->rb_left = child;
    } else {
      parent->rb_right = child;
    }
  } else {
    root->rb_node = child;
  }

color:
  if (color == RB_BLACK) {
    __rb_erase_color(child, parent, root);
  }
}

/*
 * This function returns the first node (in sort order) of the tree.
 */
struct rb_node *rb_first(struct rb_root *root) {
  struct rb_node *n;

  n = root->rb_node;
  if (!n) {
    return (struct rb_node *)0;
  }

  while (n->rb_left) {
    n = n->rb_left;
  }

  return n;
}

struct rb_node *rb_last(struct rb_root *root) {
  struct rb_node *n;
  n = root->rb_node;

  if (!n) {
    return (struct rb_node *)0;
  }

  while (n->rb_right) {
    n = n->rb_right;
  }

  return n;
}

struct rb_node *rb_next(struct rb_node *node) {
  /* If we have a right-hand child, go down and then left as far
     as we can. */
  if (node->rb_right) {
    node = node->rb_right;
    while (node->rb_left) {
      node = node->rb_left;
    }
    return node;
  }
  /* No right-hand children.  Everything down and left is
     smaller than us, so any 'next' node must be in the general
     direction of our parent. Go up the tree; any time the
     ancestor is a right-hand child of its parent, keep going
     up. First time it's a left-hand child of its parent, said
     parent is our 'next' node. */
  while (node->rb_parent && node->rb_parent->rb_right) {
    node = node->rb_parent;
  }

  return node->rb_parent;
}

struct rb_node *rb_rev(struct rb_node *node) {
  /* If we have a left-hand child, go down and then right as far
     as we can. */
  if (node->rb_left) {
    while (node->rb_right) {
      node = node->rb_right;
    }
    return node;
  }

  /* No left-hand children. Go up till we find an ancestor which
     is a right-hand child of its parent */
  while (node->rb_parent && node->rb_parent->rb_left) {
    node = node->rb_parent;
  }

  return node->rb_parent;
}

void rb_replace_node(struct rb_node *victm,
                     struct rb_node *newnode,
                     struct rb_root *root) {
  struct rb_node *parent = victm->rb_parent;
  /* Set the surrounding nodes to point to the replacement */
  if (parent) {
    if (victm == parent->rb_left) {
      parent->rb_left = newnode;
    } else {
      parent->rb_right = newnode;
    }
  } else {
    root->rb_node = newnode;
  }

  if (victm->rb_left) {
    newnode->rb_left = victm->rb_left;
  }

  if (victm->rb_right) {
    newnode->rb_right = victm->rb_right;
  }

  /* Copy the pointers/colour from the victim to the replacement */
  *victm = *newnode;
}