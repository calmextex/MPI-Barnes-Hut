#include <iostream>
#include "quadtree.h"

// Adapted from https://www.youtube.com/watch?v=-OLQlDHCMgM and https://www.geeksforgeeks.org/quad-tree
// This structure is designed to efficiently solve the given problem using a QuadTree data structure.

const double G = 0.0001; // Gravitational constant

QuadTree::QuadTree(int tree_level, double region_size, double region_x, double region_y) {
    level = tree_level;
    nodeData.regionSize = region_size;
    nodeData.regionCenterX = region_x;
    nodeData.regionCenterY = region_y;
    nodeData.totalMass = 0;
    nodeData.centerOfMassX = 0;
    nodeData.centerOfMassY = 0;
    isLeafNode = true;
    containedBody = nullptr;
    children.resize(0);
}

QuadTree::~QuadTree() {
    containedBody = nullptr; // Clear the pointer to the associated body
}


void QuadTree::insertBody(bodies& newBody) {
    if (newBody.mass == -1) return; // Skip lost bodies

    if (isLeafNode) {
        if (!containedBody) {
            containedBody = &newBody;
            nodeData.totalMass = newBody.mass;
            nodeData.centerOfMassX = newBody.x;
            nodeData.centerOfMassY = newBody.y;
        } else {
            isLeafNode = false;
            subdivide();
            dispatchBody(*containedBody);
            containedBody = nullptr;
            updateCenterOfMass(newBody);
            dispatchBody(newBody);
        }
    } else {
        updateCenterOfMass(newBody);
        dispatchBody(newBody);
    }
}

bool QuadTree::contains(const bodies& b) const {
    return b.x >= nodeData.regionCenterX && 
           b.x < nodeData.regionCenterX + nodeData.regionSize &&
           b.y >= nodeData.regionCenterY && 
           b.y < nodeData.regionCenterY + nodeData.regionSize;
}

void QuadTree::updateCenterOfMass(const bodies& b) {
    double newTotalMass = nodeData.totalMass + b.mass;
    if (newTotalMass > 0) {
        nodeData.centerOfMassX = (nodeData.centerOfMassX * nodeData.totalMass + b.x * b.mass) / newTotalMass;
        nodeData.centerOfMassY = (nodeData.centerOfMassY * nodeData.totalMass + b.y * b.mass) / newTotalMass;
        nodeData.totalMass = newTotalMass;
        
        //std::cout << "Updated COM: mass=" << totalMass 
        //         << " pos=(" << centerOfMassX << "," << centerOfMassY << ")\n";
    }
}

void QuadTree::subdivide() {
    double halfSize = nodeData.regionSize / 2.0;

    // Create four child nodes: NW, NE, SW, SE
    children.emplace_back(std::make_shared<QuadTree>(level + 1, halfSize, 
                         nodeData.regionCenterX, nodeData.regionCenterY + halfSize));
    children.emplace_back(std::make_shared<QuadTree>(level + 1, halfSize, 
                         nodeData.regionCenterX + halfSize, nodeData.regionCenterY + halfSize));
    children.emplace_back(std::make_shared<QuadTree>(level + 1, halfSize, 
                         nodeData.regionCenterX, nodeData.regionCenterY));
    children.emplace_back(std::make_shared<QuadTree>(level + 1, halfSize, 
                         nodeData.regionCenterX + halfSize, nodeData.regionCenterY));
}

void QuadTree::dispatchBody(bodies& newBody) {
    for (auto& childNode : children) {
        if (childNode->contains(newBody)) {
            childNode->insertBody(newBody);
            break;
        }
    }
}

void QuadTree::calculateNetForce(bodies& b, double theta) {
     if (b.mass == -1 || nodeData.totalMass == 0) return;

    // If we're at a leaf node with a body
    if (isLeafNode && containedBody) {
        // Skip self-interaction
        if (containedBody->id == b.id) return;
        
        // Get distance components
        auto dist = calcDistance(b.x, b.y, containedBody->x, containedBody->y);
        
        // Calculate gravitational force magnitude
        double forceMagnitude = (G * containedBody->mass * b.mass) /
                               (dist.distance * dist.distance);
        
        // Calculate force components
        double fx = forceMagnitude * dist.dx / dist.distance;
        double fy = forceMagnitude * dist.dy / dist.distance;
        
        // Apply force to both bodies (symmetric update)
        b.addForce(fx, fy);
        // Only update the contained body if it's in our process's range
        // This prevents race conditions in parallel execution -- optomization
        if (containedBody->mass != -1) {
            containedBody->addForce(-fx, -fy);  // Negative signs for equal and opposite forces
        }
    }
    // For non-leaf nodes, check to see if we can use center of mass approximation
    else if (!isLeafNode) {
        auto dist = calcDistance(b.x, b.y, nodeData.centerOfMassX, nodeData.centerOfMassY);
        
        if ((nodeData.regionSize / dist.distance) < theta) {
            // Use center of mass approximation
            double forceMagnitude = (G * nodeData.totalMass * b.mass) /
                                  (dist.distance * dist.distance);
            
            double fx = forceMagnitude * dist.dx / dist.distance;
            double fy = forceMagnitude * dist.dy / dist.distance;
            
            b.addForce(fx, fy);
        } else {
            // Recursively compute forces from children
            for (const auto& childNode : children) {
                if (childNode) {
                    childNode->calculateNetForce(b, theta);
                }
            }
        }
    }
}