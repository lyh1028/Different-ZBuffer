#include"QuadTree.h"
void updateZpyramid(double cur_z, QuadTreeNode* cur_node){
        cur_node->zmax = cur_z;
        auto fa = cur_node->father;
        if (fa == nullptr) return;
        double fa_z = fa->zmax;
        while (fa != nullptr) {
            bool flag = 1;
            double new_z = 0.0f;
            for (auto child : fa->children) {
                new_z = std::max(new_z, child->zmax);
                if (child->zmax == fa_z) flag = 0;
            }
            if (flag) { //向上更新
                fa->zmax = new_z;
                fa = fa->father;
            }
            else return;
        }
 }

//找到cover多边形的最细粒度的node
QuadTreeNode* findSmallestNode(int xmax, int xmin, int ymax, int ymin, QuadTreeNode* root) {
    if (root == nullptr) return nullptr;
    bool is_cover = root->isCover(xmax, xmin, ymax, ymin);
    if (!is_cover) return nullptr;
    else {
        QuadTreeNode* smallest_node = root;
        bool flag = 1;
        auto children = root->children;
        for (auto child : children) {
            QuadTreeNode* node = findSmallestNode(xmax, xmin, ymax, ymin, child);
            if (node != nullptr) smallest_node = node;
        }
        return smallest_node;
    }  
}

bool isClip(int xmax, int xmin, int ymax, int ymin, double zmin, QuadTreeNode* node) {
    if (node == nullptr) return false;
    if (node->zmax <= zmin) return true; //应当剔除
    bool flag = true;
    bool res = true;
    //当前节点判断不出来，继续递归判断子节点
    for (auto child : node->children) {
        if (!node->isContain(xmax, xmin, ymax, ymin)) continue;
        flag = false;
        bool is_clip = isClip(xmax, xmin, ymax, ymin, zmin, child);
        res = res & is_clip;
        if (res == false) return false;
    }
    if (flag) return false;
    if (res) return true;
    return false;
}