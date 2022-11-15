#ifndef RBBST_H
#define RBBST_H

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
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
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
		void BSTinsert(const std::pair<const Key, Value> &newPair); //regular BST insert (no rotations)
		AVLNode<Key, Value>* BSTremove(const Key &key); //regular BST remove (no rotations), return AVLNode of subtree root
		int findHeight(AVLNode<Key, Value>* a); //finds the height of the subtree starting from the passed in node
		void rightRotate(AVLNode<Key, Value>* y); //performs the right rotation
		void leftRotate(AVLNode<Key, Value>* x); //performs the left rotation

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
		if(this->root_ == NULL){ //if the tree is empty
			this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
			return;
		}
		else if(BinarySearchTree<Key, Value>::internalFind(new_item.first) != NULL){ //if the key already exists within the tree
			Node<Key, Value>* toUpdate = BinarySearchTree<Key, Value>::internalFind(new_item.first);
			toUpdate->setValue(new_item.second);
			return;
		}

		//if we get here, we know we have to add this node in somewhere (so we use BST insert and make the balance)
		BSTinsert(new_item);

		AVLNode<Key, Value>* subtreeRoot = (AVLNode<Key, Value>*) BinarySearchTree<Key, Value>::internalFind(new_item.first);
		//subtreeRoot = subtreeRoot->getParent();
		Key newsKey = new_item.first;

		while(subtreeRoot != NULL){
			int leftHeight = findHeight(subtreeRoot->getLeft());
			int rightHeight = findHeight(subtreeRoot->getRight());
			subtreeRoot->setBalance((1+std::max(leftHeight, rightHeight))); //sets balance_ to the height of the biggest subtree


			if((std::abs(leftHeight-rightHeight))>1){ //if the difference between the two trees is >1 (so it is unbalanced)

				if(rightHeight>leftHeight){ //if the right subtree is longer than the left subtree, then it is either needs a left rotation or a right-left rotation
					Key child = subtreeRoot->getRight()->getKey();
					if(newsKey > child){ //new node is to the right of the "current root's" right child (zig-zig): needs a left rotation
						leftRotate(subtreeRoot);
					}
					else if(newsKey < child){ //new node is to the left of the "current root's" right child (zig-zag): needs a right rotation and then a left
						rightRotate(subtreeRoot->getRight()); //turns the tree into a zig-zig
						leftRotate(subtreeRoot);
					}
				}

				else if(leftHeight>rightHeight){ //if the left subtree is longer than the right subtree, then it either needs a right rotation or a left-right rotation
					Key child = subtreeRoot->getLeft()->getKey();
					if(newsKey < child){ //new node is to the left of the "current root's" left child (zag-zag): needs a right rotation
						rightRotate(subtreeRoot);
					}
					else if(newsKey > child){ //new node is to the right of the "current root's" left zhild (zag-zig): needs a left rotation and then a right
						leftRotate(subtreeRoot->getLeft()); //turns the tree into a zag-zag
						rightRotate(subtreeRoot);
					}
				}
			}

			subtreeRoot = subtreeRoot->getParent(); //iterate to update the balances above
		}
		
		/*AVLNode<Key, Value>* rootroot = (AVLNode<Key, Value>*)this->root_;
		rootroot->setBalance(1+std::max(findHeight(subtreeRoot->getLeft()), findHeight(subtreeRoot->getRight()))); //finalizes the balance_ factor of the now root
		return;*/
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
		if(BinarySearchTree<Key, Value>::internalFind(key) == NULL){
			return;
		}

		AVLNode<Key, Value>* subtreeRoot = BSTremove(key); //remove the node and return the root of the subtree that needs to be checked (aka removed's parent)

		if(this->root_ == NULL){ //if we just removed the last element, we're done
			return;
		}

		//update balances of the left and right
		if(subtreeRoot->getRight() != NULL){ 
			subtreeRoot->getRight()->setBalance(1+std::max(findHeight(subtreeRoot->getRight()->getLeft()), findHeight(subtreeRoot->getRight()->getRight())));
		}
		if(subtreeRoot->getLeft() != NULL){
			subtreeRoot->getLeft()->setBalance(1+std::max(findHeight(subtreeRoot->getLeft()->getLeft()), findHeight(subtreeRoot->getLeft()->getRight())));
		}

		while(subtreeRoot != NULL){
			//int leftHeight = subtreeRoot->getLeft()->getBalance();
			//int rightHeight = subtreeRoot->getRight()->getBalance();
			int leftHeight = findHeight(subtreeRoot->getLeft());
			int rightHeight = findHeight(subtreeRoot->getRight());
			subtreeRoot->setBalance((1+std::max(leftHeight, rightHeight))); //sets balance_ to the height of the biggest subtree

			if((std::abs(leftHeight-rightHeight))>1){ //if the difference between the two trees is >1 (so it is unbalanced)
				AVLNode<Key, Value>* child = NULL;
				AVLNode<Key, Value>* grandchild = NULL;

				if(rightHeight>leftHeight){ //if the right child is the larger subtree
					child = subtreeRoot->getRight();
				}
				else if(leftHeight>rightHeight){ //if the left child is the larger subtree
					child = subtreeRoot->getLeft();
				}

				if(findHeight(child->getRight())>findHeight(child->getLeft())){ //if the right grandchild is the larger subtree
					grandchild = child->getRight();
				}
				else if(findHeight(child->getLeft())>findHeight(child->getRight())){ //if the left child is the larger subtree
					grandchild = child->getLeft();
				}
				else if(findHeight(child->getRight()) == findHeight(child->getLeft())){ //the grandchildren are equal heights
					if(subtreeRoot->getLeft()==child){
						grandchild = child->getLeft();
					}
					else if(subtreeRoot->getRight()==child){
						grandchild = child->getRight();
					}
				}

				//if(grandchild != NULL){ //account for no grandchild?? hard-coded, remove later
					if(rightHeight>leftHeight){ //if the right subtree is longer than the left subtree, then it is either needs a left rotation or a right-left rotation
						if(grandchild->getKey() > child->getKey()){  //if the grandchild key is greater than the child, then we know its zig-zig
							leftRotate(subtreeRoot);
						}
						else if(grandchild->getKey() < child->getKey()){ //zig-zag
							rightRotate(subtreeRoot->getRight()); //turns the tree into a zig-zig
							leftRotate(subtreeRoot);
						}
					}

					else if(leftHeight>rightHeight){ //if the left subtree is longer than the right subtree, then it either needs a right rotation or a left-right rotation
						if(grandchild->getKey() < child->getKey()){  //if the grandchild key is greater than the child, then we know its zag-zag
							rightRotate(subtreeRoot);
						}
						else if(grandchild->getKey() > child->getKey()){  //zag-zig
							leftRotate(subtreeRoot->getLeft()); //turns the tree into a zag-zag
							rightRotate(subtreeRoot);
						}
					}
				//}
			}
			subtreeRoot = subtreeRoot->getParent(); //advance up the tree
		}

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::BSTinsert(const std::pair<const Key, Value> &newPair){ //copied from bst.h
	bool done = false;
	AVLNode<Key, Value>* toUpdate = (AVLNode<Key, Value>*)this->root_;

	while(!done){ //traverse to find the correct spot
		if(newPair.first>toUpdate->getKey()){ //go right
			if(toUpdate->getRight()==NULL){ //if there's an empty spot, put it there and finish
				toUpdate->setRight(new AVLNode<Key, Value>(newPair.first, newPair.second, toUpdate));
				done = true;
			}
			else{ //no empty spot, continuing going down this tree
				toUpdate = toUpdate->getRight();
			}
		}

		else if(newPair.first<toUpdate->getKey()){ //go left
			if(toUpdate->getLeft()==NULL){ //if there's an empty spot, put it there and finish
				toUpdate->setLeft(new AVLNode<Key, Value>(newPair.first, newPair.second, toUpdate));
				done = true;
			}
			else{ //no empty spot, continue going down this tree
				toUpdate = toUpdate->getLeft();
			}
		}
	}
}

template<class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::BSTremove(const Key &key){ //copied from bst.h
	AVLNode<Key, Value>* toRemove = (AVLNode<Key, Value>*) BinarySearchTree<Key, Value>::internalFind(key);
	AVLNode<Key, Value>* p = toRemove->getParent(); //keep the parent so that we can use it later in 'remove fix' (remove())


	if(toRemove->getLeft() == NULL && toRemove->getRight() == NULL){ //no children
		if(toRemove == this->root_){ //if this is the root
			this->root_ = NULL;
		}
		else if(toRemove->getParent()->getLeft() == toRemove){ //if this node is the left child of its parent
			toRemove->getParent()->setLeft(NULL);
		}
		else if(toRemove->getParent()->getRight() == toRemove){ //if this node is the right child of its parent
			toRemove->getParent()->setRight(NULL);
		}
	}

	else if((toRemove->getLeft() != NULL && toRemove->getRight() == NULL) || (toRemove->getLeft() == NULL && toRemove->getRight() != NULL)){ //if only one child
		if(toRemove->getLeft() != NULL && toRemove->getRight() == NULL){ //left child, no right child
			AVLNode<Key, Value>* n = toRemove->getLeft();
			if(toRemove->getParent()==NULL){ //if this is the root 
				this->root_ = n;
				n->setParent(NULL);
			}
			else{ 
				if(toRemove->getParent()->getRight()==toRemove){ //if the node is the right child of its parent
					toRemove->getParent()->setRight(n); //make the child be the new right child
				}
				else if(toRemove->getParent()->getLeft()==toRemove){ //if the node is the left child of its parent
					toRemove->getParent()->setLeft(n); //make the child be the new left child
				}
				n->setParent(toRemove->getParent()); //make the parent of the new child the parent
			}
		}

		else if(toRemove->getLeft() == NULL && toRemove->getRight() != NULL){ //right child, no left child
			AVLNode<Key, Value>* n = toRemove->getRight();
			if(toRemove->getParent()==NULL){ //if this is the root
				this->root_ = n;
				n->setParent(NULL);
			}
			else{ 
				if(toRemove->getParent()->getRight()==toRemove){ //if the node is the right child of its parent
					toRemove->getParent()->setRight(n); //make the child be the new right child
				}
				else if(toRemove->getParent()->getLeft()==toRemove){ //if the node is the left child of its parent
					toRemove->getParent()->setLeft(n); //make the child be the new left child
				}
				n->setParent(toRemove->getParent()); //make the parent of the new child the parent
			}
		}
	}

	else{ //both children exist
		AVLNode<Key, Value>* pred = (AVLNode<Key, Value>*)BinarySearchTree<Key, Value>::predecessor(toRemove);
		nodeSwap(toRemove, pred);
		p = toRemove->getParent(); // !!!UPDATE PARENT AFTER SWAPPING TO MAKE SURE WE RETURN THE CORRECT NODE TO LOOK AT IN REMOVE() !!
		if(toRemove->getLeft() == NULL && toRemove->getRight() == NULL){ //leaf (0 children case)
			if(toRemove->getParent()==NULL){ //if we're at the root
				this->root_ = NULL;
			}
			else if(toRemove->getParent()->getLeft() == toRemove){ //if this node is the left child of its parent
				toRemove->getParent()->setLeft(NULL);
			}
			else if(toRemove->getParent()->getRight() == toRemove){ //if this node is the right child of its parent
				toRemove->getParent()->setRight(NULL);
			}
		}
		else if(toRemove->getLeft() != NULL && toRemove->getRight() == NULL){ //left child, no right child (1 child case)
			Node<Key, Value>* n = toRemove->getLeft();
			if(toRemove->getParent()==NULL){ //if this is the root
				this->root_ = n;
				n->setParent(NULL);
			}
			else{ 
				if(toRemove->getParent()->getRight()==toRemove){ //if the node is the right child of its parent
					toRemove->getParent()->setRight(n); //make the child be the new right child
				}
				else if(toRemove->getParent()->getLeft()==toRemove){ //if the node is the left child of its parent
					toRemove->getParent()->setLeft(n); //make the child be the new left child
				}
				n->setParent(toRemove->getParent()); //make the parent of the new child the parent
			}
		}
	}

	delete toRemove;

	if(p==NULL && this->root_!=NULL){ //we ended up deleting the root
		p = (AVLNode<Key, Value>*)this->root_; //we're going to have to check the entire tree
	}
	else if(this->root_ == NULL){ //we removed the root and the tree is empty now
		return NULL;
	}
	return p;

}

template<class Key, class Value>
int AVLTree<Key, Value>::findHeight(AVLNode<Key, Value>* a){
	if (a==NULL){
		return 0; //no node is 0 
	}
	return a->getBalance(); //the balance is updated through setBalance(), usually end up setting the balance within the functions where it is used
}

template<class Key, class Value>
void AVLTree<Key, Value>::rightRotate(AVLNode<Key, Value>* y){ //based off of slide 24 in L14_BalancedBST_AVL.pdf
	AVLNode<Key, Value>* p = y->getParent();
	AVLNode<Key, Value>* x = y->getLeft();
	AVLNode<Key, Value>* b = x->getRight();

	if(y == this->root_){
		this->root_ = x;
	}
	
	y->setLeft(b); //b (x's child) becomes y's left child (since b < y and y's left child is no longer x)
	if(x->getRight() != NULL){
		
		b->setParent(y); //set b's parent to be y
	}

	y->setParent(x); //make y's parent x
	x->setParent(p); //make x's parent p (former parent of y)
	x->setRight(y); //make x's right child y 

		//update the balances, starting from the bottom
	y->setBalance(1+std::max(findHeight(y->getLeft()), findHeight(y->getRight())));
	x->setBalance(1+std::max(findHeight(x->getLeft()), findHeight(x->getRight()))); 

	if(p!=NULL){ //if y was not the root
		if(p->getLeft()==y){ //if y was the left child of p, make p's new left child x
			p->setLeft(x);
		}
		else if(p->getRight()==y){ //if y was the right child of p, make p's new right child x
			p->setRight(x);
		}
		p->setBalance(1+std::max(findHeight(p->getLeft()), findHeight(p->getRight()))); //update the balance of p
	}
	
}

template<class Key, class Value>
void AVLTree<Key, Value>::leftRotate(AVLNode<Key, Value>* x){ //based off of slide 23 in L14_BalancedBST_AVL.pdf
	AVLNode<Key, Value>* p = x->getParent();
	AVLNode<Key, Value>* y = x->getRight();
	AVLNode<Key, Value>* b = x->getRight()->getLeft();

	if(x == this->root_){
		this->root_ = y;
	}

	x->setRight(b); //b (y's child) becomes z's right child (since b > x and x's right child is no longer y)
	if(y->getLeft() != NULL){
		b->setParent(x); //set b's parent to be y
	}

	y->setParent(p); //make y's parent p (former parent of s)
	y->setLeft(x); //make y's left child x
	x->setParent(y); //make x's parent y

	//update the balances, starting from the bottom
	x->setBalance(1+std::max(findHeight(x->getLeft()), findHeight(x->getRight())));
	y->setBalance(1+std::max(findHeight(y->getLeft()), findHeight(y->getRight()))); 

	if(p!=NULL){ //if x was not the root
		if(p->getLeft()==x){ //if x was the left child of p, make p's new left child y
			p->setLeft(y);
		}
		else if(p->getRight()==x){ //if x was the right child of p, make p's new right child y
			p->setRight(y);
		}
		p->setBalance(1+std::max(findHeight(p->getLeft()), findHeight(p->getRight()))); //update the balance of p
	}

}


#endif
