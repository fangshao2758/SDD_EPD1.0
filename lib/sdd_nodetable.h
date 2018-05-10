#ifndef SDD_NODETABLE_H_
#define SDD_NODETABLE_H_
#include "sdd_node.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <algorithm>


namespace sdd {
class SddNodeTable {
public:
    SddNodeTable() :
        sdd_nodes_(),
        uniq_table_(),
        avail_() {}

    SddNode& get_node_at(const addr_t i) {
        auto& n = sdd_nodes_[i];
        return n;
    }
    const SddNode& get_node_at(const addr_t i) const {
        auto& n = sdd_nodes_[i];
        return n;
    }

    addr_t make_or_find_literal(const addr_t literal, const int v_id) {
        SddNode new_node(literal, v_id);
        return make_or_find_inner(std::move(new_node));
    }

    addr_t make_or_find_decomp(std::vector<SddElement>&& decomp, const int v_id) {
        std::sort(decomp.begin(), decomp.end());
        SddNode new_node(std::move(decomp), v_id);
        return make_or_find_inner(std::move(new_node));
    }

    addr_t make_or_find_inner(SddNode&& new_node) {
        auto res = uniq_table_.find(new_node);
        if (res != uniq_table_.end()) {
            return res->second;
        }

        size_t node_id = new_node_id();
        uniq_table_.emplace(new_node, node_id);
        sdd_nodes_[node_id].activate(std::move(new_node));
        return node_id;
    }

    std::vector<addr_t> gc() {
        
        std::vector<addr_t> deleted;
        for (size_t i = 0; i < sdd_nodes_.size(); i++) {
            auto& node = sdd_nodes_[i];
            if (node.type() == NodeType::DECOMP && 
                node.refcount() == 0) {
                uniq_table_.erase(node);
                node.deactivate();
                avail_.push(i);
                deleted.push_back(i);
            }
        }
        return deleted;
    }

    size_t new_node_id()  {
        if (avail_.empty()) {
            sdd_nodes_.emplace_back();
            return sdd_nodes_.size()-1;
        } else {
            size_t id = avail_.top();
            avail_.pop();
            return id;
        }
    }

    size_t node_array_size() const {
        return sdd_nodes_.size();
    }
    
private:
    std::vector<SddNode> sdd_nodes_;
    std::unordered_map<SddNode, addr_t> uniq_table_;
    std::stack<size_t> avail_;
};
}

#endif // ENDIF
