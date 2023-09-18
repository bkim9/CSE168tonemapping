#include "bvh.h"
#include <vector>

int construct_bvh(const std::vector<BBox> &boxes,
                  std::vector<BVHNode> &node_pool) {
    if (boxes.size() == 1) {
        BVHNode node;
        node.left_node_id = node.right_node_id = -1;
        node.primitive_id = boxes[0].id;
        node.box = boxes[0];
        node_pool.push_back(node);
        return node_pool.size() - 1;
    }

    BBox big_box;
    for (const BBox &b : boxes) {
        big_box = merge(big_box, b);
    }
    int axis = largest_axis(big_box);
    std::vector<BBox> local_boxes = boxes;
    std::sort(local_boxes.begin(), local_boxes.end(),
        [&](const BBox &b1, const BBox &b2) {
            Vector3 center1 = (b1.p_max + b1.p_min) / Real(2);
            Vector3 center2 = (b2.p_max + b2.p_min) / Real(2);
            return center1[axis] < center2[axis];
        });
    std::vector<BBox> left_boxes(
    	local_boxes.begin(),
    	local_boxes.begin() + local_boxes.size() / 2);
    std::vector<BBox> right_boxes(
    	local_boxes.begin() + local_boxes.size() / 2,
        local_boxes.end());

    BVHNode node;
    node.box = big_box;
    node.left_node_id = construct_bvh(left_boxes, node_pool);
    node.right_node_id = construct_bvh(right_boxes, node_pool);
    node.primitive_id = -1;
    node_pool.push_back(node);
    return node_pool.size() - 1;
}
