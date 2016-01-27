// Victoria Johnston ~ PG3 ~ vjohnst1

#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <utility>
#include <string>
#include <vector>
#include <math.h>

#include "distance.hpp"
using namespace std;

class quadtree {
  // A point quadtree for locations on Earth.

  // Notes: 
  // - Assume a spherical Earth. But assume that if you keep traveling
  //   west, you will hit longitude -180 and stop, and if you keep
  //   traveling east, you will hit longitude +180 and stop; you don't
  //   have to worry about wrapping around like Magellan.
  // - Locations are measured in degrees latitude and longitude, but
  //   distances are measured in miles.
private:
	typedef struct node {
		string name;
		point location;
		node *southeast;
		node *northeast;
		node *southwest;
		node *northwest;
	} node;
	typedef struct range {
		point sw;
		point ne;
		double swlon, swlat, nelon, nelat;
		range(point s,point n):sw(s),ne(n){
			swlon=sw.lon; swlat=sw.lat; nelon=ne.lon; nelat=ne.lat;
		};
	} range;
	node *root;
public:
  // See bintree.hpp for skeleton definitions of the default
  // constructor, copy constructor, destructor, and copy assignment
  // operator (and swap).

	// Default constructor
	quadtree(){
		root = NULL;
	}

	// Copy constructor. use copyFromRoot to copy tree using recursion
	quadtree(const quadtree &t){
		root = copyFromRoot(t.root);
	}

	// recursive function used by copy consturctor. 
	node* copyFromRoot(const node *currentnode){
		// check node is not null. if it is return null.
		if (currentnode == NULL){
			return NULL;
		}
		// declare new node (node to copy to) and copy to it
		node * newnode = createNode(currentnode->name,currentnode->location);
		// use recursion to copy over the rest of the tree
		newnode->southeast = copyFromRoot(currentnode->southeast);
		newnode->southwest = copyFromRoot(currentnode->southwest);
		newnode->northeast = copyFromRoot(currentnode->northeast);
		newnode->northwest = copyFromRoot(currentnode->northwest);
		// return node
		return newnode;
	}

	// Destuctor
	~quadtree(){
		clearNode(root);
		root = NULL;
	}

	// function which clears node using recursion
	void clearNode(node * &n){
		if (n!=NULL){
			clearNode(n->southeast);
			clearNode(n->northeast);
			clearNode(n->southwest);
			clearNode(n->northwest);
			delete n;
		}
	}

	// swap for copy constructor- copy-and-swap idiom
	friend void swap(quadtree &t1, quadtree &t2) {
	using std::swap;
		swap(t1.root, t2.root);
	}

	quadtree &operator= (quadtree other) {
		swap(*this,other);
		return *this;
	}

  	void insert(const std::string& name, point p) {
    	// Insert an element with name `name` and location `p`.
    		if (root == NULL) {
			root = createNode(name,p);
		} else {
			recurseInsert(root,name,p);
		}
  	}

	void recurseInsert(node *&currentnode, const std::string& name, point p){
		if (currentnode == NULL){
			// if end of tree is reached (node is null) add a new node
			currentnode = createNode(name,p);
		} else {
			// check if the node to insert is a quadrant
			if (p.lon<=(currentnode->location).lon && p.lat<=(currentnode->location).lat){
				//southwest
				return recurseInsert(currentnode->southwest,name,p);
			} else if (p.lon<=(currentnode->location).lon && p.lat>(currentnode->location).lat){
                                // northwest
				return recurseInsert(currentnode->northwest,name,p);
                        } else if (p.lon>(currentnode->location).lon && p.lat<=(currentnode->location).lat){
				// southeast
				return recurseInsert(currentnode->southeast,name,p);
                        } else {
                                // northeast
				return recurseInsert(currentnode->northeast,name,p);
                        }
		}
			
	}
	
	// function which create a new node with input data and returns it. all children start off as null
	node* createNode(const std::string& name, point p){
		node * newnode = new node;
		newnode->name = name;
		newnode->location = p;
		newnode->southwest = NULL;
		newnode->northwest = NULL;
		newnode->southeast = NULL;
		newnode->northeast = NULL;
		return newnode;
	}

  	void within_radius(point p, double r, std::vector<std::string> &result) const {
    	// Search for elements within the circle with center `p` (in
    	// degrees latitude and longitude) and radius `r` (in miles).
    	// Clears `result` and puts the elements in `result`.
    	// Postcondition: `result` contains all (and only) the elements of
    	// the tree, in any order, that lie within the circle defined by
    	// `p` and `r`.
    		// clear results
    		result.clear();
		// run recursive search function which takes brange as an input. starting brange is (-90,180),(90,180)
		search(root,p,r,result,range(point(-90,-180),point(90,180)));
  	}

	void search(node* currentnode, point p, double r, std::vector<std::string> &result,range brange) const {
		if (currentnode == NULL){
			return;
		}
		// check if any part of srange is in brange
		if (distance_to_box(p,brange.sw,brange.ne)<=r){
			// check if point is within radius
			if (distance(currentnode->location,p)<=r){
				result.push_back(currentnode->name);
			}
			// set location of the curretn node to a point
			point nloc = currentnode->location;
			// find the four different ranges
			range sw(brange.sw,nloc);
			range se(point(brange.swlat,nloc.lon),point(nloc.lat,brange.nelon));
			range ne(nloc,brange.ne);
			range nw(point(nloc.lat,brange.swlon),point(brange.nelat,nloc.lon));
			// recursive search
			if ((currentnode->southwest)!=NULL){
				search(currentnode->southwest,p,r,result,sw);
			}
			if ((currentnode->southeast)!=NULL){
				search(currentnode->southeast,p,r,result,se);
			}
			if ((currentnode->northwest)!=NULL){
				search(currentnode->northwest,p,r,result,nw);
			}
			if ((currentnode->northeast)!=NULL){
				search(currentnode->northeast,p,r,result,ne);
			}
		}
	}
};

#endif
