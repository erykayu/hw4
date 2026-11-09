#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#include <set>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
void equalPathsHelper(Node* root, int d, set<int>& res);

bool equalPaths(Node * root)
{
  set <int> res;
  equalPathsHelper(root, 0, res);
  return res.size() <= 1; 
}

void equalPathsHelper(Node* root, int d, set<int>& res)
{
  if (root == nullptr) return; 

  if (root->left == nullptr && root ->right == nullptr)
  {
    res.insert(d);
    return; 
  }

  if (root -> left) equalPathsHelper(root->left, d+1, res);  
  if (root -> right) equalPathsHelper(root->right, d+1, res);  
}

