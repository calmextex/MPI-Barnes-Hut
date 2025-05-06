#ifndef QUADTREE_H
#define QUADTREE_H

#include <vector>
#include <memory>
#include "bodies.h"

// Separate frequently accessed data into a cache-aligned struct -- OPTIMIZATION FOR EC
// This should minimize pointer chasing where possible. Original implementation had each variable in struct within the private method.

struct alignas(64) TreeNodeData {  // 64-byte cache line alignment
    double centerOfMassX;
    double centerOfMassY;
    double totalMass;
    double regionSize;
    double regionCenterX;
    double regionCenterY;
};

class QuadTree {
private:
    // Group frequently accessed data together in cache-friendly struct
    TreeNodeData nodeData;
    
    
    int level;
    bool isLeafNode;
    bodies* containedBody;
    std::vector<std::shared_ptr<QuadTree>> children;

    // Helper method for force calculations
    inline void calculateForceFromNode(bodies& b, double theta) const;

public:
    QuadTree(int tree_level, double region_size, double region_x, double region_y);
    ~QuadTree();

    bool isLeaf() const { return isLeafNode; }
    bool contains(const bodies& b) const;
    void insertBody(bodies& newBody);
    void updateCenterOfMass(const bodies& b);
    void calculateNetForce(bodies& b, double theta);
    void subdivide();
    void dispatchBody(bodies& newBody);
};

#endif // QUADTREE_H