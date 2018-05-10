#ifndef EPD_MANAGER_H_
#define EPD_MANAGER_H_
#include <functional>
#include <vector>
#include <stack>
#include <unordered_map>
#include "epd_common.h"
#include "sdd_common.h"
#include "epd_node.h"
#include "sdd_node.h"
#include "epd_vtree.h"
#include "sdd_nodetable.h"
#include "epd_nodetable.h"
#include "epd_cache_table.h"
using namespace std; 


namespace epd {

class Epd;

class EpdManager {
public:
    EpdManager(const VTree& vtree, const unsigned int cache_size = 1U << 16) 
        : vtree_(vtree), 
          epd_cache_table_(cache_size),
          epd_cache_table_neg_(cache_size),
          sdd_to_epd_cache_table_(cache_size),
          epd_node_table_()
        {}

    Epd epd_And(const Epd& lhs, const Epd& rhs);
    Epd epd_Or(const Epd& lhs, const Epd& rhs);
    Epd epd_Neg(const Epd& lhs);
    Epd make_pos_epd(const Epd& lhs);
    Epd make_neg_epd(const Epd& lhs);


    // increment reference counter
    void inc_epdnode_refcount_at(const epd_addr_t idx) {
        if (idx < 0) return;
        epd_node_table_.get_node_at(idx).inc_ref_count(*this);
    }
    // decrement refcount
    void dec_epdnode_refcount_at(const epd_addr_t idx) {
        if (idx < 0) return;
        epd_node_table_.get_node_at(idx).dec_ref_count(*this);
    }
    
    // garbage collection 
    // delete nodes whose refcount is 0.
    void gc();


    EpdNode& get_epdnode_at(const epd_addr_t idx)  {
        return epd_node_table_.get_node_at(idx);
    }
    const EpdNode& get_epdnode_at(const epd_addr_t idx) const {
        return epd_node_table_.get_node_at(idx);
    }


    Epd make_epd_true();
    Epd make_epd_false();
    Epd make_epd_all();
    Epd make_epd_empty();

    // model counting
    unsigned long long count_solution(const epd_addr_t epd) const;

    // size of sdds.
    unsigned long long size(const epd_addr_t epd) const;

private:

    VTree vtree_;
    EpdCacheTable epd_cache_table_;
    EpdCacheTable epd_cache_table_neg_;
    EpdCacheTable sdd_to_epd_cache_table_;
    EpdNodeTable epd_node_table_;

    epd_addr_t epd_apply_and(const epd_addr_t lhs, const epd_addr_t rhs);
    epd_addr_t epd_apply_or(const epd_addr_t lhs, const epd_addr_t rhs);
    epd_addr_t epd_apply_negate(const epd_addr_t lhs);
    epd_addr_t epd_apply_pos_epd(const sdd::addr_t lhs);
    epd_addr_t epd_apply_neg_epd(const sdd::addr_t lhs);

    epd_addr_t make_epd_decomposition(std::vector<EpdElement>&& decomp_nodes, const int vtree_node);
    epd_addr_t make_epd_disjunction(std::vector<EpdElement>&& decomp_nodes, const int vtree_node);

    std::vector<EpdElement> compress_decomp(const std::vector<std::pair<epd_addr_t, epd_addr_t>>&new_decomp_candidates);
    std::vector<EpdElement> compress_disjunction(const std::vector<epd_addr_t>&new_disjunction_candidates);

    unsigned long long count_solution_inner(const epd_addr_t epd, std::unordered_map<epd_addr_t, unsigned long long>& cache) const;

};

}
#endif // EPD_MANAGER_H_
