// Victoria Johnston ~ PG3 ~ vjohnst1

#ifndef BINTREE_HPP
#define BINTREE_HPP

#include <utility>
#include <string>
#include <vector>
using namespace std;

class bintree {
  // A binary search tree for locations in Lineland.

  // Notes: 
  // - Assume a flat, one-dimensional world with locations from -180 to 180.
  // - All locations and distances are measured in the same units (degrees).
private:
	typedef struct node {
		string name;
		float location;
		node * left;
		node * right;
	} node;
	typedef struct range {
		float start;
		float end;
		range(float s,float e):start(s),end(e){};
	} range;
	float minRange = -180;
	float maxRange = 180;
	node * root;
public:
  	// Default constructor
  	bintree() {
		root = NULL;
  	}

  	// Copy constructor
	bintree(const bintree &t) {
   		// use copyFromRoot to help copy every node using recursion starting at the root of t
		// it will return the pointer to the root of the copied tree
		root = copyFromRoot(t.root);
  	}

	node* copyFromRoot(const node *currentroot) {
		// check root is not null. if it is return null.
		if (currentroot == NULL){
			return NULL;
		}
		// declare new node (node to copy to)
        	node * newnode = new node;
		// copy over name and location
		newnode->location = currentroot->location;
		newnode->name = currentroot->name;
		// make the left and right children the new roots and use recursion to copy over the rest of tree
		newnode->left = copyFromRoot(currentroot->left);
		newnode->right = copyFromRoot(currentroot->right);
		// return node
		return newnode;
	}

  	// Destructor
  	~bintree() {
		clearNode(root);
		root = NULL;
  	}

	// recursive funstion which clears every node left and right of current node
	void clearNode(node * &n){
		if (n!=NULL){
			// clear left and right children of current node
			clearNode(n->left);
			clearNode(n->right);
			// delete current node
			delete n;
		}
	}

  	// Copy assignment is implemented using the copy-swap idiom
  	// https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Copy-and-swap
  	friend void swap(bintree &t1, bintree &t2) {
   	using std::swap;
    		// Swap all data members here, e.g.,
    		swap(t1.root, t2.root);
    		// Pointers should be swapped -- but not the things they point to.
  	}

  	bintree &operator= (bintree other) {
    		// You don't need to modify this function.
    		swap(*this, other);
    		return *this;
  	}

	void insert(const std::string& name, double p) {
	// Insert an element with name `name` and location `p`.
		if (root == NULL){
			root = createNode(name,p);
		} else {
			recurseInsert(root,name,p);
		}
	}

	void recurseInsert(node * &currentroot, const std::string& name, double location){
		// if the root is 0, insert the node in
		if (currentroot == NULL){
			currentroot = createNode(name,location);
		} else {
			if (location > currentroot->location){
				return recurseInsert(currentroot->right,name,location);
			} else {
				return recurseInsert(currentroot->left,name,location);
			}
		}
	}

	// function which creates a node with given data and returns it. Default node will have no children
	node * createNode(const std::string& name, double location){
		node * newnode = new node;
		newnode->location = location;
		newnode->name = name;
		newnode->right = NULL;
		newnode->left = NULL;
		return newnode;
	}

  	void within_radius(double p, double r, std::vector<std::string> &result) const {
  	  // Search for elements within the range `p` plus or minus `r`.
  	  // Clears `result` and puts the elements in `result`.
  	  // Postcondition: `result` contains all (and only) elements of the
  	  // tree, in any order, that lie within the range `p` plus or minus
  	  // `r`.
		result.clear(); //make sure results is empty
  		range srange(p-r,p+r); //starting srange
		range brange(minRange,maxRange); //starting brange
		search(srange,root,brange,result);
  	}

	void search(range srange,node* currentnode,range brange,vector<string> &result) const{
		if (currentnode == NULL){
			return;
		}
		// check to see if srange and brange overlap
		if ((srange.start<=brange.end && brange.start<=srange.end)||(brange.start<=srange.end && srange.start<=brange.end)){
			// check if node location is in srange. If it is add the name to results
			if (currentnode->location>=srange.start && currentnode->location<=srange.end){
				result.push_back(currentnode->name);
			}
			// create new ranges for recursive searches
			range leftrange(brange.start,currentnode->location);
			range rightrange(currentnode->location,brange.end);
			// use recursion to check left and right child
			search(srange,currentnode->left,leftrange,result);
			search(srange,currentnode->right,rightrange,result);
		}
	}
};

#endif
