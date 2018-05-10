#ifndef SDD_MANAGER_H_
#define SDD_MANAGER_H_
#include <functional>
#include <vector>
#include <stack>
#include <unordered_map>
#include "sdd_common.h"
#include "sdd_node.h"
#include "sdd_vtree.h"
#include "sdd_nodetable.h"
#include "cache_table.h"
using namespace std; 


namespace sdd {

class Sdd;

class SddManager {
public:
    SddManager(const VTree& vtree, const unsigned int cache_size = 1U << 16) 
        : vtree_(vtree), 
          cache_table_(cache_size),
          sdd_node_table_()
        {

            assert(&vtree_!=NULL);
            //cout << "vtree_ not null"<<endl;
            //cout << "create a sdd for manager!!";
        }


    // Apply operations
    // Sdd sdd_intersection(const Sdd& lhs, const Sdd& rhs);
    // Sdd sdd_union(const Sdd& lhs, const Sdd& rhs);
    // Sdd sdd_difference(const Sdd& lhs, const Sdd& rhs);
    // Sdd sdd_orthogonal_join(const Sdd& lhs, const Sdd& rhs);
    
    Sdd sdd_And(const Sdd& lhs, const Sdd& rhs);
    Sdd sdd_Or(const Sdd& lhs, const Sdd& rhs);

    
    // Apply operations with a variable.
    // Sdd sdd_change(const Sdd& sdd, const addr_t var);
    // Sdd sdd_filter_contain(const Sdd& sdd, const addr_t var);
    // Sdd sdd_filter_not_contain(const Sdd& sdd, const addr_t var);
    
    // Restore implicit partitions.
    //Sdd sdd_to_explicit_form(const Sdd& sdd);

    // increment reference counter
    void inc_sddnode_refcount_at(const addr_t idx) {
        if (idx < 0) return;
        sdd_node_table_.get_node_at(idx).inc_ref_count(*this);
    }
    // decrement refcount
    void dec_sddnode_refcount_at(const addr_t idx) {
        if (idx < 0) return;
        sdd_node_table_.get_node_at(idx).dec_ref_count(*this);
    }
    
    // garbage collection 
    // delete nodes whose refcount is 0.
    void gc();


    SddNode& get_sddnode_at(const addr_t idx)  {
        return sdd_node_table_.get_node_at(idx);
    }
    const SddNode& get_sddnode_at(const addr_t idx) const {
        return sdd_node_table_.get_node_at(idx);
    }


    


    // make terminal/variable sdds.
    Sdd make_sdd_literal(const addr_t literal);
    //Sdd make_sdd_baseset();
    //Sdd make_sdd_empty();
    Sdd make_sdd_true();
    Sdd make_sdd_false();

    // make sdd representing power set whose base set 
    // consists of leaves of vtree_node.
    Sdd make_sdd_powerset(const int vtree_node);

    // model counting
    unsigned long long count_solution(const addr_t sdd) const;

    // size of sdds.
    unsigned long long size(const addr_t sdd) const;
    std::vector<std::vector<int>> calc_setfamily(const addr_t sdd) const;


    void export_sdd_txt(const addr_t sdd, std::ostream& os) const;
    void export_sdd_dot(const addr_t sdd, std::ostream& os) const;

private:
    addr_t make_sdd_literal_inner(const addr_t literal);
    addr_t sdd_to_explicit_form_inner(const addr_t sdd);
    std::vector<SddElement> compress_candidates(const std::vector<std::pair<addr_t, addr_t>>&
                                                  new_decomp_candidates);

    //addr_t sdd_apply_withvar(const Operation& op, const addr_t sdd, const addr_t var);


    addr_t make_sdd_decomposition(std::vector<SddElement>&& decomp_nodes, const int vtree_node);
    addr_t calc_primes_union(const std::vector<SddElement>& decomp);
    addr_t make_sdd_powerset_inner(const int vtree_node);

    unsigned long long count_solution_inner(const addr_t sdd, std::unordered_map<addr_t, unsigned long long>& cache) const;
    std::vector<std::vector<int>> calc_setfamily_inner(const addr_t sdd, std::unordered_map<addr_t, std::vector<std::vector<int>>>& cache) const; 

    void export_sdd_txt_inner(const addr_t sdd, std::ostream& os, 
                               std::unordered_set<addr_t>& found, 
                               const size_t empty_id, 
                               const size_t false_id) const;

    addr_t sdd_apply_negate(const addr_t lhs);
    addr_t sdd_apply_and(const addr_t lhs, const addr_t rhs);
    addr_t sdd_apply_xor(const addr_t lhs, const addr_t rhs);
    addr_t sdd_apply_or(const addr_t lhs, const addr_t rhs);

    VTree vtree_;
    CacheTable cache_table_;
    SddNodeTable sdd_node_table_;

};

}
#endif // SDD_MANAGER_H_
