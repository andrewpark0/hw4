#ifndef RECCHECK
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;

int checkEqualHelper(Node* node, int depth, int& targetDepth);
// You may add any prototypes of helper functions here


bool equalPaths(Node* root)
{
    // Add your code below
    if(!root){
      return true;
    }
    

    int targetDepth = -1;
    return checkEqualHelper(root,0,targetDepth);
}

int checkEqualHelper(Node* node, int depth, int& targetDepth){
  if(!node){
    return true;
  }

  if(!node->left && !node->right){
    if(targetDepth == -1){
      targetDepth = depth;
    }
    else{
      if(depth != targetDepth){
        return false;
      }
    }
  }

  return checkEqualHelper(node->left,depth + 1, targetDepth) &&
          checkEqualHelper(node->right, depth + 1, targetDepth);
}
