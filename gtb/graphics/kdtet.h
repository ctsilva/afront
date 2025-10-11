#ifndef __KDTREE_OBJECT_H
#define __KDTREE_OBJECT_H

#include <gtb/stlext.h>
#include <gtb/graphics/box3.hpp>

GTB_BEGIN_NAMESPACE

//
// KDTree that stores tetrahedrons
// Supported operations
//  - Creation of a tree
//  - Search for the tetrahedron that contains a point
//
class KDTET
{
public:
  //    typedef ATET t_object;
    typedef t_object* p_object;

    KDTET(int max_objects_in_node);

    // Insert a single object
    // Currently supported for an empty tree only!
    void Insert(p_object pobj);

    // Build the tree
    void MakeTree();

    p_object FindTet(const Point3& p);

protected:
    struct Node
    {
        Node() {}
        virtual ~Node() {}

        virtual Node* MakeTree(int max_objects_in_leaf) {};

        enum node_type {leaf, tree, single} type; // Types of nodes, so we do not have recursion
    };

    struct TreeNode : public Node
    {
        ~TreeNode() { delete l; delete r; }

        Node* MakeTree(int max_objects_in_leaf);

//    protected:
        // Which axis is cut by this node
        // Numbering complies with VX, VY, VZ defined in algebra3.h
        enum t_axis {XAxis=0, YAxis, ZAxis}; 

        double cut_point;       // where along the axis is this node cut.
                                // Absolute coordinates.

        Node *l, *r;            // Left and right sons
        t_axis axis;
    }; // TreeNode

    struct LeafNode : public Node
    {
        LeafNode() {}
        void Insert(p_object pobj) { _objects.push_back(v); }
        
        // search if the leaf holds an object that contains a point
        // if so, return true and the object., otherwise tet=0
        bool Contains(const Point3& p, p_object& tet);

        Node* MakeTree(int max_objects_in_leaf);

    protected:
        typedef std::vector<p_object> t_objects_list;
        t_objects_list _objects; // List of objects in the node

    }; // LeafNode

    /////////////////////////////////
    // The tree!!!
    aptr<Node> _tree;
    int _max_objects_in_node;
};

GTB_END_NAMESPACE

#endif // __KDTREE_OBJECT_H
