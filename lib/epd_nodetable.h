#ifndef EPD_NODETABLE_H_
#define EPD_NODETABLE_H_
#include "epd_node.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>
#include <algorithm>


namespace epd {
class EpdNodeTable {
public:
    EpdNodeTable() :
        epd_nodes_(),
        uniq_table_(),
        avail_() {}

    EpdNode& get_node_at(const epd_addr_t i) {
        auto& n = epd_nodes_[i];
        return n;
    }
    const EpdNode& get_node_at(const epd_addr_t i) const {
        auto& n = epd_nodes_[i];
        return n;
    }

    epd_addr_t make_or_find_disjunction(std::vector<epd_addr_t>&& disjunction, const int v_id) {
        std::sort(disjunction.begin(), disjunction.end());
        EpdNode new_node(std::move(disjunction), v_id);
        return make_or_find_inner(std::move(new_node));
    }

    epd_addr_t make_or_find_decomp(std::vector<EpdElement>&& decomp, const int v_id) {
        std::sort(decomp.begin(), decomp.end());
        EpdNode new_node(std::move(decomp), v_id);
        return make_or_find_inner(std::move(new_node));
    }

    epd_addr_t make_or_find_inner(EpdNode&& new_node) {   //需要怎么实现
        auto res = uniq_table_.find(new_node);
        if (res != uniq_table_.end()) {
            return res->second;
        }

        size_t node_id = new_node_id();
        uniq_table_.emplace(new_node, node_id);
        epd_nodes_[node_id].activate(std::move(new_node));
        return node_id;
    }

    std::vector<epd_addr_t> gc() {    //对disjunction的gc
        
        std::vector<epd_addr_t> deleted;
        for (size_t i = 0; i < epd_nodes_.size(); i++) {
            auto& node = epd_nodes_[i];
            if (node.type() == EpdNodeType::DECOMP && node.refcount() == 0 ||
                node.type() == EpdNodeType::DISJUNC && node.refcount() == 0)
            {
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
            epd_nodes_.emplace_back();
            return epd_nodes_.size()-1;
        } else {
            size_t id = avail_.top();
            avail_.pop();
            return id;
        }
    }

    size_t node_array_size() const {
        return epd_nodes_.size();
    }
    
private:
    std::vector<EpdNode> epd_nodes_;
    std::unordered_map<EpdNode, epd_addr_t> uniq_table_;
    std::stack<size_t> avail_;
};
}

#endif // ENDIF
