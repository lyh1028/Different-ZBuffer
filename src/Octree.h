#ifndef OCTREE_H
#define OCTREE_H
#include<limits>
#include"HzbPolygen.h"
#include<vector>
class OctreeNode {
public:
    std::vector<HzbPolygen> polygens;
    OctreeNode* children[8];
    int xmax, xmin, ymax, ymin;
    float zmax, zmin;
    //bool is_leaf = false;
    OctreeNode(int _xmax, int _xmin, int _ymax, int _ymin, float _zmax, float _zmin, std::vector<HzbPolygen>& _polygens) : polygens(_polygens) {
        for (int i = 0; i < 8; ++i) {
            children[i] = nullptr;
        }
        //is_leaf = is_leaf;
        xmax = _xmax; xmin = _xmin;
        ymax = _ymax; ymin = _ymin;
        zmax = _zmax; zmin = _zmin;
    }

    ~OctreeNode() {
        release();
    }
    void release() {
        for (auto& child : children)
        {
            if (child != NULL) child->release();
        }
        for (int i = 0; i < 8; ++i) {
            delete children[i];
        }
    }
    OctreeNode* buildTree(int xmax, int xmin, int ymax, int ymin, float zmax, float zmin, std::vector<HzbPolygen>& polygens) {
        if (polygens.empty() || xmax < xmin || ymax < ymin || zmax < zmin) return nullptr;
        if (polygens.size() <= 25 || isEqual(xmax, xmin) || isEqual(ymax, ymin)) return new OctreeNode(xmax, xmin, ymax, ymin, zmax, zmin, polygens);
        
        float zmid = (zmax + zmin) / 2;
        int xmid = (xmax + xmin) / 2;
        int ymid = (ymax + ymin) / 2;
        std::vector<std::vector<HzbPolygen> > octree_polygens(9);
        for (auto& poly : polygens) {
            //0. Left-Down-Front
            if (poly.xmin <= xmid && poly.ymax > ymid && poly.zmin <= zmid) {
                octree_polygens[0].push_back(poly);
            }
            //1. Right-Down-Front
            else if (poly.xmax > xmid && poly.ymax > ymid && poly.zmin <= zmid) {
                octree_polygens[1].push_back(poly);
            }
            //2. Left-Up-Front
            else if (poly.xmin <= xmid && poly.ymin <= ymid && poly.zmin <= zmid) {
                octree_polygens[2].push_back(poly);
            }
            //3. Right-Up-Front
            else if (poly.xmax > xmid && poly.ymin <= ymid && poly.zmin <= zmid) {
                octree_polygens[3].push_back(poly);
            }
            //Left-Down-Back
            else if (poly.xmin <= xmid && poly.ymax > ymid && poly.zmax > zmid) {
                octree_polygens[4].push_back(poly);
            }
            //Right-Down-Back
            else if (poly.xmax > xmid && poly.ymax > ymid && poly.zmax > zmid) {
                octree_polygens[5].push_back(poly);
            }
            //Left-Up-Back
            else if (poly.xmin <= xmid && poly.ymin <= ymid && poly.zmax > zmid) {
                octree_polygens[6].push_back(poly);
            }
            //Right-Up-Back
            else if (poly.xmax > xmid && poly.ymin <= ymid && poly.zmax > zmid) {
                octree_polygens[7].push_back(poly);
            }
            else {
                octree_polygens[8].push_back(poly);//当前节点包含的面片
            }
        }
        OctreeNode* node = new OctreeNode(xmax, xmin, ymax, ymin, zmax, zmin, octree_polygens[8]);
        node->children[0] = buildTree(xmid, xmin, ymax, ymid, zmid, zmin, octree_polygens[0]);
        node->children[1] = buildTree(xmax, xmid, ymax, ymid, zmid, zmin, octree_polygens[1]);
        node->children[2] = buildTree(xmid, xmin, ymid, ymin, zmid, zmin, octree_polygens[2]);
        node->children[3] = buildTree(xmax, xmid, ymid, ymin, zmid, zmin, octree_polygens[3]);
        node->children[4] = buildTree(xmid, xmin, ymax, ymid, zmax, zmid, octree_polygens[4]);
        node->children[5] = buildTree(xmax, xmid, ymax, ymid, zmax, zmid, octree_polygens[5]);
        node->children[6] = buildTree(xmid, xmin, ymid, ymin, zmax, zmid, octree_polygens[6]);
        node->children[7] = buildTree(xmax, xmid, ymid, ymin, zmax, zmid, octree_polygens[7]);
        for (OctreeNode* child : node->children) {
            if (child) {
                node->zmax = std::max(node->zmax, child->zmax);
                node->zmin = std::min(node->zmin, child->zmin);
            }
        }
        return node;
        
    }
    bool isEqual(float a, float b) {
        constexpr float epsilon = std::numeric_limits<float>::epsilon();
        if (abs(b - a) < epsilon) return true;
        return false;
    }
};

#endif // ! AABB_H

