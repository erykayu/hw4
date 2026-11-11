#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void removeFix(AVLNode<Key, Value>* n, int diff);
    void rotateLeft(AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value>* n);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
  if (this->root_ == NULL)
  {
    this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
    return; 
  }

  AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
  AVLNode<Key, Value>* parent = NULL;
  while (curr != NULL)
  {
    parent = curr;
    if(new_item.first == curr->getKey()) 
    {
      curr -> setValue(new_item.second);
      return;
    }
    else if (new_item.first < curr->getKey()) curr = curr->getLeft();
    else curr = curr->getRight(); 
  }

  AVLNode<Key, Value>* n = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
  if (new_item.first < parent->getKey()) parent->setLeft(n);
  else parent->setRight(n);

  if (parent->getBalance() != 0) parent->setBalance(0);
  else
  {
    if (n == parent->getLeft()) parent->setBalance(-1);
    else parent->setBalance(1); 
    insertFix(parent, n); 
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
  AVLNode<Key, Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
  if (n==NULL) return; 

  if (n->getLeft() != NULL && n->getRight() != NULL)
  {
    AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(n));
    nodeSwap(n, pred); 
  }

  AVLNode<Key, Value>* p = n->getParent(); 
  int diff = 0; 
  if (p != NULL)
  {
    if (n == p->getLeft()) diff = 1;
    else diff = -1; 
  }

  AVLNode<Key, Value>* child; 
  if (n->getLeft() != NULL) child = n->getLeft();
  else child = n->getRight(); 
  if (child != NULL) child->setParent(p); 
  if (p==NULL) this->root_ = child; 
  else if (p->getLeft() == n) p->setLeft(child);
  else p->setRight(child);

  delete n;
  removeFix(p, diff); 
}

//insertfix make
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n)
{
  if (p == NULL || p->getParent() == NULL) return; 
  AVLNode<Key, Value>* g = p->getParent(); 

  if (p == g->getLeft()) g->updateBalance(-1);
  else g->updateBalance(1); 

  int8_t b = g->getBalance();
  if (b == 0) return;
  else if (b == -1 || b == 1) insertFix(g, p); 
  else if (b == -2) 
  {
    if (n == p->getLeft()) //zigzig
    {
      rotateRight(g);
      p->setBalance(0);
      g->setBalance(0);
    }
    else //zigzag
    {
      rotateLeft(p);
      rotateRight(g);
      int8_t nb = n->getBalance();
      if (nb == -1)
      {
        p->setBalance(0); g->setBalance(1);
      }
      else if (nb == 0)
      {
        p->setBalance(0); g->setBalance(0);
      }
      else
      {
        p->setBalance(-1); g->setBalance(0);
      }
      n -> setBalance(0);
    }
  }

  else 
  {
    //zigzig
    if (n == p->getRight())
    {
      rotateLeft(g);
      p->setBalance(0);
      g->setBalance(0);
    }
    else //zigzag
    {
      rotateRight(p);
      rotateLeft(g); 
      int8_t nb = n->getBalance();
      if (nb == 1)
      {
        p->setBalance(0); g->setBalance(-1);
      }
      else if (nb == 0)
      {
        p->setBalance(0); g->setBalance(0);
      }
      else
      {
        p->setBalance(1); g->setBalance(0);
      }
      n->setBalance(0);
    }
  }
}

//theremove fix 
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff)
{
  if (n == NULL) return;
  AVLNode<Key, Value>* p = n->getParent();
  int ndiff = 0;
  if (p != NULL)
  {
    if (n == p->getLeft()) ndiff = 1;
    else ndiff = -1; 
  }
  int b = n->getBalance() + diff;

  if (b == -2)
  {
    AVLNode<Key, Value>* c = n->getLeft();
    int8_t temp = c->getBalance();
    //zigzig
    if (temp == -1)
    {
      rotateRight(n);
      n->setBalance(0);
      c->setBalance(0);
      removeFix(p, ndiff); 
    }
    else if (temp == 0) //zigzig
    {
      rotateRight(n);
      n->setBalance(-1);
      c->setBalance(1); 
    }
    else //zigzag
    {
      AVLNode<Key, Value>* g = c->getRight(); 
      rotateLeft(c);
      rotateRight(n);
      int8_t temp2 = g->getBalance();
      if (temp2 == 1)
      {
        n->setBalance(0);
        c->setBalance(-1);
      }
      else if (temp2 == 0)
      {
        n->setBalance(0);
        c->setBalance(0);
      }
      else 
      {
        n->setBalance(1);
        c->setBalance(0);
      }
      g->setBalance(0);
      removeFix(p, ndiff); 
    }
  }
  else if (b ==2)
  {
    AVLNode<Key, Value>* c = n->getRight(); 
    int8_t temp3 = c->getBalance();
    if (temp3 == 1)
    {
      rotateLeft(n);
      n->setBalance(0);
      c->setBalance(0);
      removeFix(p, ndiff); 
    }
    else if (temp3 == 0)
    {
      rotateLeft(n);
      n->setBalance(1);
      c->setBalance(-1);
    }
    else 
    {
      AVLNode<Key, Value>* g = c->getLeft(); 
      rotateRight(c); 
      rotateLeft(n);
      int8_t temp4 = g->getBalance();
      if (temp4 == -1)
      {
        n->setBalance(0);
        c->setBalance(1);
      }
      else if (temp4 == 0)
      {
        n->setBalance(0);
        c->setBalance(0);
      }
      else
      {
        n->setBalance(-1);
        c->setBalance(0);
      }
      g->setBalance(0);
      removeFix(p, ndiff);
    }

  }

  else if (b == 0)
  {
    n->setBalance(0);
    removeFix(p, ndiff); 
  }
  else n->setBalance(b); 
}

//rotates
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n)
{
  AVLNode<Key,Value>* child = n->getRight();
  AVLNode<Key,Value>* parent = n->getParent();
  AVLNode<Key,Value>* middle = child->getLeft();

  child->setParent(parent);
  if (parent == NULL) this->root_ = child; 
  else if(parent->getLeft() == n) parent->setLeft(child); 
  else parent->setRight(child);

  child->setLeft(n);
  n->setParent(child);
  n->setRight(middle);
  if (middle != NULL) middle->setParent(n); 
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n)
{
  AVLNode<Key,Value>* child = n->getLeft();
  AVLNode<Key,Value>* parent = n->getParent();
  AVLNode<Key,Value>* middle = child->getRight();

  child->setParent(parent);
  if (parent == NULL) this->root_ = child; 
  else if(parent->getLeft() == n) parent->setLeft(child); 
  else parent->setRight(child);

  child->setRight(n);
  n->setParent(child);
  n->setLeft(middle);
  if (middle != NULL) middle->setParent(n); 
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
