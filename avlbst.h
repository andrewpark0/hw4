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
    int height(AVLNode<Key, Value>* node) const;
    AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
    void balance(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeHelperOneChild(AVLNode<Key, Value>* node);
    void removeHelperLeaf(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* predecessor(AVLNode<Key, Value>* current);
    void balanceRemove(AVLNode<Key, Value>* node);
    

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO 

    if(this->root_ == nullptr){
      this->root_= new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr);
    }
    else{
      AVLNode<Key,Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
      AVLNode<Key,Value>* parent = nullptr;

      while(current != nullptr){
        parent = current;
        if(new_item.first == current->getKey()){
          current->setValue(new_item.second);
          return;
        }
        else if(new_item.first < current->getKey()){
          current = current->getLeft();
        }
        else{
          current = current->getRight();
        }
      }

      AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

      if(newNode->getKey() < parent->getKey()){
        parent->setLeft(newNode);
      }
      else{
        parent->setRight(newNode);
      }
      balance(parent,newNode);
    }
    
}

//insert helpers

template<class Key, class Value>
int AVLTree<Key, Value>::height(AVLNode<Key, Value> *node)const{
  if(node == nullptr){
    return -1;
  }
  return std::max(height(node->getLeft()), height(node->getRight())) + 1;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node){
  
  if(node == nullptr || node->getLeft() == nullptr){
    return node;
  }

  AVLNode<Key,Value>* newRoot = node->getLeft();

  AVLNode<Key,Value>* parent = node->getParent();
  newRoot->setParent(parent);
  node->setLeft(newRoot->getRight());

  if(node->getLeft() != nullptr){
    node->getLeft()->setParent(node);
  }
  
  newRoot->setRight(node);

  if (parent != nullptr) {
    if (parent->getRight() == node) {
      parent->setRight(newRoot);
    } 
    else {
      parent->setLeft(newRoot);
    }
  } 
  else {
    this->root_ = newRoot; // Update the root if the original node is the root
  }
  node->setParent(newRoot);
  return newRoot;
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node){
  
  if(node == nullptr || node->getRight() == nullptr){
    return node;
  }

  AVLNode<Key,Value>* newRoot = node->getRight();

  AVLNode<Key,Value>* parent = node->getParent();
  newRoot->setParent(parent);
  node->setRight(newRoot->getLeft());

  if(node->getRight() != nullptr){
    node->getRight()->setParent(node);
  }
  
  newRoot->setLeft(node);

  if (parent != nullptr) {
    if (parent->getLeft() == node) {
      parent->setLeft(newRoot);
    } 
    else {
      parent->setRight(newRoot);
    }
  } 
  else {
    this->root_ = newRoot; // Update the root if the original node is the root
  }
  node->setParent(newRoot);
  return newRoot;
}

template<class Key, class Value>
void AVLTree<Key, Value>::balance(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child){
  
  if(child == nullptr || parent == nullptr){
    return;
  }
  
  int balanceFactor = height(parent->getLeft()) - height(parent->getRight());
  
  parent->updateBalance(balanceFactor);
  
  //left heavy
  if (balanceFactor > 1){
    
    if(height(child->getLeft()) > height(child->getRight())){
      rotateRight(parent);
        //left left
    } 
    rotateLeft(child);
    rotateRight(parent); //left left
    
  }
  //right heavy
  else if(balanceFactor < -1){
    
    if(height(child->getRight()) > height(child->getLeft())){
      rotateLeft(parent);
      //right right
    }
    rotateRight(child);
    rotateLeft(parent); //right left
  
  }
  
  balance(parent->getParent(), parent);
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key){
    // TODO
  AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
  if(nodeToRemove == nullptr){
    return;
  }
  
  AVLNode<Key, Value>* parent = nodeToRemove->getParent();
  //case 1: no children
  if(nodeToRemove->getLeft() == nullptr && nodeToRemove->getRight() == nullptr){
    if(nodeToRemove == this->root_){
      delete this->root_;
      this->root_ = nullptr;
      return;
    } 
    removeHelperLeaf(nodeToRemove);
    balanceRemove(parent);
    
  }
  //case 2: two children
  else if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr){
    AVLNode<Key, Value>* predecessorNode = predecessor(nodeToRemove);
    nodeSwap(nodeToRemove, predecessorNode);
    
    if(nodeToRemove->getLeft() == nullptr && nodeToRemove->getRight() == nullptr){
      removeHelperLeaf(nodeToRemove);
      if(predecessorNode == this->root_){
        balanceRemove(predecessorNode);
      }
      else{
        balanceRemove(parent);
      }
    }
    else{
      removeHelperOneChild(nodeToRemove);
      if(predecessorNode == this->root_){
        balanceRemove(predecessorNode);
      }
      else{
        balanceRemove(parent);
      }
    }
  }
  //case 3, one child
  else if(nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() == nullptr){
    removeHelperOneChild(nodeToRemove);
    balanceRemove(parent);
  }
  else if(nodeToRemove->getLeft() == nullptr && nodeToRemove->getRight() != nullptr){
    removeHelperOneChild(nodeToRemove);
    balanceRemove(parent);
  }
}

//remove helpers

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeHelperLeaf(AVLNode<Key, Value>* node){
  AVLNode<Key,Value>* parent = node->getParent();

  if(parent != nullptr){
    if(parent->getLeft() == node){
      parent->setLeft(nullptr);
    }
    else{
      parent->setRight(nullptr);
    }
  }
  else{
    this->root_ = nullptr;
  }
  delete node;

  
}

template<typename Key, typename Value>
void AVLTree<Key, Value>::removeHelperOneChild(AVLNode<Key, Value>* node){
  if(node == nullptr){
    return;
  }

  AVLNode<Key,Value>* child = nullptr;

  if(node->getLeft() != nullptr){
    child = node->getLeft();
  }
  else{
    child = node->getRight();
  }

  AVLNode<Key,Value>* parent = node->getParent();

  if(parent != nullptr && child != nullptr){
    if(parent->getLeft() == node){
      parent->setLeft(child);
    }
    else{
      parent->setRight(child);
    }
    child->setParent(parent);
  }
  else{
    if(parent != nullptr){
      if(parent->getLeft() == node){
        parent->setLeft(child);
      }
      else{
        parent->setRight(child);
      }
      if(child != nullptr){
        child->setParent(parent);
      }
    }
    else{
      this->root_ = child;
      if(child != nullptr){
        child->setParent(nullptr);
      }
    }
  }
  delete node;

}

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::predecessor(AVLNode<Key, Value>* current){
  //case 1: left subtree not empty
  if(current->getLeft() != nullptr){
    //traverse to bottom right node of left subtree
    AVLNode<Key, Value>* predecessor = current->getLeft();
    while(predecessor->getRight() != nullptr){
        predecessor = predecessor->getRight();
    }
    return predecessor;
  }

  //case 2: left subtree empty
  else{
    AVLNode<Key,Value>* parent = current->getParent();
    while(parent != nullptr && current != nullptr){
      if(current == parent->getRight()){
        return parent;
      }
      else{
        current = parent;
      }
    }
    return nullptr;
  }
}

template<class Key, class Value>
void AVLTree<Key, Value>::balanceRemove(AVLNode<Key, Value>* node){
  if(node == nullptr){
    return;
  }

  AVLNode<Key, Value>* parent = node->getParent();
  int leftHeight = height(node->getLeft());
  int rightHeight = height(node->getRight());
  int balanceFactor = leftHeight - rightHeight;

  node->updateBalance(balanceFactor);

  if(balanceFactor > 1){
    if(height(node->getLeft()->getLeft()) >= height(node->getLeft()->getRight())){
      rotateRight(node);
    }
    else{
      node->setLeft(rotateLeft(node->getLeft()));
      rotateRight(node);
    }
  }
  else if(balanceFactor < -1){
    if(height(node->getRight()->getRight()) >= height(node->getRight()->getLeft())){
      rotateLeft(node);
    }
    else{
      node->setRight(rotateRight(node->getRight()));
      rotateLeft(node);
    }
  }

  balanceRemove(parent);
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
