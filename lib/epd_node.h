#ifndef EPD_NODE_H_
#define EPD_NODE_H_
#include <vector>
#include <iostream>
#include "epd_common.h"
#include "epd_vtree.h"
#include <unordered_set>
#include <type_traits>
#include <functional>

namespace epd {

class EpdManager;

using EpdElement = std::pair<epd_addr_t, epd_addr_t>;

class EpdNode {
public:
    EpdNode() : 
        type_(EpdNodeType::UNUSED), 
        disjunction_(), 
        decomp_(),
        vtree_node_id_(-1), 
        refcount_(0) {}

    EpdNode(const std::vector<EpdElement>& decomp, const int vtree_node_id) :
        type_(EpdNodeType::DECOMP),
        disjunction_(),
        decomp_(decomp),
        vtree_node_id_(vtree_node_id),
        refcount_(0) {}

    EpdNode(const std::vector<EpdElement>&& decomp, const int vtree_node_id) :
        type_(EpdNodeType::DECOMP),
        disjunction_(),
        decomp_(std::move(decomp)),
        vtree_node_id_(vtree_node_id),
        refcount_(0) {}

    EpdNode(const std::vector<epd_addr_t>& disjunction, const int vtree_node_id) :
        type_(EpdNodeType::DISJUNC),
        disjunction_(disjunction),
        decomp_(),
        vtree_node_id_(vtree_node_id),
        refcount_(0) {}

    EpdNode(const epd_addr_t constant, const int vtree_node_id) : 
        type_(EpdNodeType::CONSTANT),
        disjunction_(),
        decomp_(),
        vtree_node_id_(vtree_node_id),
        refcount_(0) {}
    
    EpdNode(const EpdNode& obj) :
        type_(obj.type_), 
        disjunction_(obj.disjunction_),
        decomp_(obj.decomp_),
        vtree_node_id_(obj.vtree_node_id_),
        refcount_(obj.refcount_) {}

    EpdNode(EpdNode&& obj) :
        type_(obj.type_), 
        disjunction_(obj.disjunction_),
        decomp_(std::move(obj.decomp_)),
        vtree_node_id_(obj.vtree_node_id_), 
        refcount_(obj.refcount_) {}

    // set the node as Unused
    // (appear only in cache.
    void deactivate() {
        type_ = EpdNodeType::UNUSED;
        disjunction_.clear();
        decomp_.clear();
        vtree_node_id_ = -1;
        refcount_ = 0;
    }

    //void operator=(const EpdNode& obj)  = delete;

    // set unused not to the value of obj.
    void activate(EpdNode&& obj) {
        assert(type_ == EpdNodeType::UNUSED);
        type_ = obj.type_;
        vtree_node_id_ = obj.vtree_node_id_;
        disjunction_ = std::move(obj.disjunction_);           
        decomp_ = std::move(obj.decomp_);
        refcount_ = obj.refcount_;
    }
    
    bool operator==(const EpdNode& n) const {
        if (type_ != n.type_) return false;
        if (type_ == EpdNodeType::DISJUNC) {
            return vtree_node_id_ == n.vtree_node_id_ &&
                disjunction_ == n.disjunction_;
        } else {
            return vtree_node_id_ == n.vtree_node_id_ &&
                decomp_ == n.decomp_;
        }
    }

    EpdNodeType type() const { return type_; }
    const std::vector<EpdElement>& decomposition() const { return decomp_; }
    const std::vector<epd_addr_t>& disjunction() const {return disjunction_;}
    epd_addr_t constant() const {return constant_;}
    int vtree_node_id() const { return vtree_node_id_; }

    unsigned int refcount() const { return refcount_; }

    // increment/decrement reference counter.
    // the reference counter is used in gc().
    void inc_ref_count(EpdManager& mgr);
    void dec_ref_count(EpdManager& mgr);

private:
    EpdNodeType type_; // nodetype
    std::vector<EpdElement> decomp_; // decomposition(represented as pairs of epdnoes)
    std::vector<epd_addr_t> disjunction_;
    epd_addr_t constant_;

    int vtree_node_id_;
    unsigned int refcount_;
};


} // namespace epd

namespace std {
template <> struct hash<epd::EpdNode> {
    std::size_t operator()(const epd::EpdNode& n) const {
        size_t h = 0;
        if (n.type() == epd::EpdNodeType::DISJUNC) {
            for (const auto& e : n.disjunction()) {
                epd::hash_combine(h, hash<epd::epd_addr_t>()(e));
            }
            epd::hash_combine(h, hash<int>()(n.vtree_node_id()));
            return h;
        } else if (n.type() == epd::EpdNodeType::DECOMP) {
            for (const auto& e : n.decomposition()) {
                epd::hash_combine(h, hash<epd::epd_addr_t>()(e.first));
                epd::hash_combine(h, hash<epd::epd_addr_t>()(e.second));
            }
            epd::hash_combine(h, hash<int>()(n.vtree_node_id()));
        } 
        return h;
    }
};
}


#endif // EPD_NODE_H_
