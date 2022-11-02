#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool checkHeight(Node* root, int depth, int& initialLeaf);

bool equalPaths(Node * root)
{
  // Add your code below
	if(root==NULL){
		return true;
	}

	int initialLeaf = 0;

	return checkHeight(root, 1, initialLeaf);

}

bool checkHeight(Node* root, int depth, int& initialLeaf) {
	if(root==NULL){
		return true;
	}

	if(root->left == NULL && root->right == NULL){ //if we're at the leaf node

		if(initialLeaf==0){ //if this is our first encounter with a leaf node
			initialLeaf = depth; //the baseline leaf node (initialLeaf) is set to this
		}

		else if(depth==initialLeaf){ //if the leaf node is the same as the others
			return true;
		}

		else{ //if this leaf node is different
			return false;
		}

	}
	
	return checkHeight(root->left, depth+1, initialLeaf) && checkHeight(root->right, depth+1, initialLeaf);

}

