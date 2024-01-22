#ifndef QUADTREE_H
#define QUADTREE_H
#include<vector>
class QuadTreeNode {
public:
    int xmin, xmax, ymin, ymax;
    double zmax;
    std::vector<QuadTreeNode* >children;  // 子节点
    QuadTreeNode* father = nullptr;
    QuadTreeNode(int _xmax, int _xmin, int _ymax, int _ymin) {
        xmin = _xmin;
        xmax = _xmax;
        ymin = _ymin;
        ymax = _ymax;
        zmax = 1.0f;
    }
    ~QuadTreeNode() {
        release();
    }
    QuadTreeNode* buildTree(int xmax, int xmin, int ymax, int ymin) {
        if (xmax < xmin || ymax < ymin) {
            printf_s("构建四叉树出错!\n");
        }
        if (xmax == xmin && ymax == ymin) {
            return new QuadTreeNode(xmax, xmin, ymax, ymin);
        }
        QuadTreeNode* node = new QuadTreeNode(xmax, xmin, ymax, ymin);
        int xmid = (xmax + xmin) / 2;
        int ymid = (ymax + ymin) / 2;
        //左上角是(0,0)
        //右下角
        if (ymax > ymid && xmax > xmid) {
            auto child = buildTree(xmax, xmid+1, ymax, ymid+1);
            node->children.push_back(child);
            child->father = node;
        }
        //左下角
        if (ymax > ymid && xmin <= xmid) {
            auto child = buildTree(xmid, xmin, ymax, ymid+1);
            node->children.push_back(child);
            child->father = node;
        }
        //右上角
        if (ymin <= ymid && xmax > xmid) {
            auto child = buildTree(xmax, xmid+1, ymid, ymin);
            node->children.push_back(child);
            child->father = node;
        }
        //左上角
        if (ymin <= ymid && xmin <= xmid) {
            auto child = buildTree(xmid, xmin, ymid, ymin);
            node->children.push_back(child);
            child->father = node;
        }
        return node;
    }

    void release(){
		for (QuadTreeNode* child : children)
		{
			if (child != NULL) child->release();
		}
		children.clear();
	}
    bool isCover(int xmax, int xmin, int ymax, int ymin) {
        if (this->xmax >= xmax && this->xmin <= xmin && this->ymax >= ymax && this->ymin <= ymin) {
            return true;
        }
        return false;
    }
    bool isContain(int xmax, int xmin, int ymax, int ymin) { //只要“含有”就返回true
        if (isCover(xmax, xmin, ymax, ymin)) return true; //完全覆盖
        if (xmin <= this->xmax && xmax >= this->xmin && ymin <= this->ymax && ymax >= this->ymin) {
            return true;
        }
        return false;
    }
};
void updateZpyramid(double cur_z, QuadTreeNode* cur_node);
QuadTreeNode* findSmallestNode(int xmax, int xmin, int ymax, int ymin, QuadTreeNode* root);
bool isClip(int xmax, int xmin, int ymax, int ymin, double zmin, QuadTreeNode* node);
#endif