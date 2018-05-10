#include "epd_manager.h"

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include "epd.h"
using namespace std;
namespace epd {

Epd EpdManager::make_sdd_true() {
    //cout << "true" <<endl;
    return Epd(EPD_TRUE, *this);
}

Epd EpdManager::make_sdd_false() {
    //cout << "false" <<endl;
    return Epd(EPD_FALSE, *this); 
}

Epd EpdManager::make_sdd_all() {
    return Epd(EPD_ALL, *this);
}

Epd EpdManager make_sdd_empty() {
    return Epd(EPD_EMPTY, *this);
}


// addr_t SddManager::make_sdd_literal_inner(const addr_t literal) {
//     const int v_id = vtree_.find_literal_node_id(literal);
//     return sdd_node_table_.make_or_find_literal(literal, v_id);
// }

void EpdManager::gc() {
    epd_node_table_.gc();
    epd_cache_table_.clear_cache();
    epd_cache_table_neg_.clear_cache();
    sdd_to_epd_cache_table_.clear_cache();//?
}


Epd EpdManager::epd_And(const Epd& lhs, const Epd& rhs) {
    epd_addr_t res =  epd_apply_and(lhs.epd_addr(), rhs.epd_addr());
    return Epd(res, *this);
}

Epd EpdManager::epd_Or(const Epd& lhs, const Epd& rhs) {
    epd_addr_t res =  epd_apply_or(lhs.epd_addr(), rhs.epd_addr());
    return Epd(res, *this);
}

Epd EpdManager::epd_Neg(const Epd& lhs) {
    epd_addr_t res = epd_apply_negate(lhs.epd_addr());
}

Epd EpdManager::make_pos_epd(const Sdd& lhs) {
    epd_addr_t res = epd_apply_pos_epd(lhs.epd_addr());
}

Epd EpdManager::make_neg_epd(const Sdd& lhs) {
    epd_addr_t res = epd_apply_neg_epd(lhs.epd_addr());
}


epd_addr_t EpdManager::make_epd_decomposition(std::vector<EpdElement>&& decomp_nodes, const int vtree_node) {
    assert(!decomp_nodes.empty());
    return epd_node_table_.make_or_find_decomp(std::move(decomp_nodes), vtree_node);
}

epd_addr_t EpdManager::make_epd_disjunction(std::vector<epd_addr_t>&& disjunction_nodes, const int vtree_node) {
    assert(!disjunction_nodes.empty());
    return epd_node_table_.make_or_find_disjunction(std::move(disjunction_nodes), vtree_node);
}                               

epd_addr_t EpdManager::epd_apply_and(const epd_addr_t lhs, const epd_addr_t rhs)
{
    // if(lhs == rhs) return lhs;
    // if(lhs == EPD_TRUE) return rhs;
    // if(rhs == EPD_TRUE) return lhs;
    // if(lhs == EPD_FALSE || rhs == EPD_FALSE) return EPD_FALSE;
    // if(lhs == EPD_EMPTY || rhs == EPD_EMPTY) return ; 
    // if(lhs == EPD_ALL) return ;

    // const EpdNode& l_node = get_epdnode_at(lhs);
    // const EpdNode& r_node = get_epdnode_at(rhs);

    // // cache check
    // {
    //     epd_addr_t cache = epd_cache_table_.read_cache(Operation::AND, lhs, rhs);
    //     if (cache != EPD_NULL) {
    //         return cache;
    //     }
    // }


    // if (l_node.type() == EpdNodeType::DISJUNC && r_node.type() == EpdNodeType::DISJUNC){
    //     std::vector<epd_addr_t> disjunc_l;
    //     std::vector<epd_addr_t> disjunc_r;

    //     const epd_addr_t l_vnode = l_node.vtree_node_id();
    //     const epd_addr_t r_vnode = r_node.vtree_node_id();

    //     std::vector<epd_addr_t> new_disjunction_candidates;

    //     for (const auto l_elem : disjunc_l) {
    //         for (const auto r_elem : disjunc_r) {
                
    //             epd_addr_t new_d = sdd_apply_and(l_elem, r_elem);
    //             if (new_d == SDD_FALSE) continue;  //何时跳出循环跳出循环
    //             new_disjunction_candidates.emplace_back(new_d);
    //         }
    //     }

    //     auto new_disjunction = compress_disjunction(new_disjunction_candidates);
    //     epd_addr_t result_node = make_epd_disjunction(std::move(new_disjunction), depend_vtree_node_id);
    //     epd_cache_table_.write_cache(Operation::AND, lhs, rhs, result_node);
    //     return result_node;
    // }


}

epd_addr_t EpdManager::epd_apply_or(const epd_addr_t lhs, const epd_addr_t rhs)
{

}

epd_addr_t EpdManager::epd_apply_negate(const epd_addr_t lhs, const epd_addr_t rhs)
{

}

epd_addr_t EpdManager::epd_apply_pos_epd(const epd_addr_t lhs, const epd_addr_t rhs)
{

}

epd_addr_t EpdManager::epd_apply_neg_epd(const epd_addr_t lhs, const epd_addr_t rhs)
{

}



// addr_t SddManager::sdd_apply_xor(const addr_t lhs,const addr_t rhs)
// {
//     if(lhs == rhs) return SDD_FALSE;

//     if(lhs == SDD_FALSE) return rhs;
//     if(rhs == SDD_FALSE) return lhs;

//     if (lhs == SDD_TRUE)
//     {
//         const SddNode& node = get_sddnode_at(lhs);
//         if(node.type()==NodeType::LIT)
//         {
//             return make_sdd_literal_inner(-node.literal());
//         }
//     }

//     if (rhs == SDD_TRUE)
//     {
//         const SddNode& node = get_sddnode_at(lhs);
//         if(node.type()==NodeType::LIT)
//         {
//             return make_sdd_literal_inner(-node.literal());
//         }
//     }


//     // cache check
//     {
//         addr_t cache = cache_table_.read_cache(Operation::XOR, lhs, rhs);
//         if (cache != SDD_NULL) {
//             return cache;
//         }
//     }
//     const SddNode& l_node = get_sddnode_at(lhs);
//     const SddNode& r_node = get_sddnode_at(rhs);
//     // setup decomposition nodes;
//     std::vector<SddElement> decomp_l;
//     std::vector<SddElement> decomp_r;

//     const addr_t l_vnode = l_node.vtree_node_id();
//     const addr_t r_vnode = r_node.vtree_node_id();

//         //4 types
//      addr_t  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);

//         if (l_vnode == r_vnode) {
//             decomp_l = l_node.decomposition();
//             decomp_r = r_node.decomposition();
//         }

//         else if (l_vnode == depend_vtree_node_id) {
//             if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
//                 decomp_l = l_node.decomposition();                
//                 decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
//             }  else {
//                 decomp_l = l_node.decomposition();
//                 decomp_r = {{SDD_TRUE, rhs}};
//             }
//         }

//         else if (r_vnode == depend_vtree_node_id) {

//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = r_node.decomposition();

//             }  else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = r_node.decomposition();
                
//             }
//         }

//         else { //depend node is a common ancestor
//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = {{SDD_TRUE, rhs}};

                
//             } else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};

//             }
//         }


    
//     std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

//         for (const auto l_elem : decomp_l) {
//             for (const auto r_elem : decomp_r) {
                
//                 addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
//                 if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
//                 addr_t new_s = sdd_apply_xor(l_elem.second, r_elem.second);
//                 new_decomp_candidates.emplace_back(new_p, new_s);
//             }
//         }

//     // compression

//     auto new_decomposition = compress_candidates(new_decomp_candidates);


//     addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
//                                                  depend_vtree_node_id);
//     cache_table_.write_cache(Operation::XOR, lhs, rhs, result_node);
//     return result_node;

// }

// addr_t SddManager::sdd_apply_and(const addr_t lhs, const addr_t rhs) {

//     if(lhs == rhs) return lhs;

//     if (lhs == SDD_FALSE || rhs == SDD_FALSE) return SDD_FALSE;
//     if(lhs == SDD_TRUE) return rhs;
//     if(rhs == SDD_TRUE) return lhs;

//     const SddNode& l_node = get_sddnode_at(lhs);
//     const SddNode& r_node = get_sddnode_at(rhs);

//     if (l_node.type() == NodeType::LIT && r_node.type() == NodeType::LIT) {
//         if (llabs(l_node.literal()) == llabs(r_node.literal())) {
//             if (l_node.literal() > 0) {
//                     return lhs;
//                 }
//                 else 
//                     return rhs;
//                 }
//         }

//     // cache check
//     {
//         addr_t cache = cache_table_.read_cache(Operation::AND, lhs, rhs);
//         if (cache != SDD_NULL) {
//             return cache;
//         }
//     }
//     // setup decomposition nodes;
//     std::vector<SddElement> decomp_l;
//     std::vector<SddElement> decomp_r;

//     const addr_t l_vnode = l_node.vtree_node_id();
//     const addr_t r_vnode = r_node.vtree_node_id();

//         //4 types
//      addr_t  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);

//         if (l_vnode == r_vnode) {
//             decomp_l = l_node.decomposition();
//             decomp_r = r_node.decomposition();
//         }

//         else if (l_vnode == depend_vtree_node_id) {
//             if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
//                 decomp_l = l_node.decomposition();                
//                 decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
//             }  else {
//                 decomp_l = l_node.decomposition();
//                 decomp_r = {{SDD_TRUE, rhs}};
//             }
//         }

//         else if (r_vnode == depend_vtree_node_id) {

//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = r_node.decomposition();

//             }  else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = r_node.decomposition();
                
//             }
//         }

//         else { //depend node is a common ancestor
//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = {{SDD_TRUE, rhs}};

                
//             } else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};

//             }
//         }


    
//     std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

//         for (const auto l_elem : decomp_l) {
//             for (const auto r_elem : decomp_r) {
                
//                 addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
//                 if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
//                 addr_t new_s = sdd_apply_and(l_elem.second, r_elem.second);
//                 new_decomp_candidates.emplace_back(new_p, new_s);
//             }
//         }

//     // compression

//     auto new_decomposition = compress_candidates(new_decomp_candidates);


//     addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
//                                                  depend_vtree_node_id);
//     cache_table_.write_cache(Operation::AND, lhs, rhs, result_node);
//     return result_node;
// }

// addr_t SddManager::sdd_apply_or(const addr_t lhs, const addr_t rhs) {

//     if(lhs == rhs) return lhs;

//     if (lhs == SDD_TRUE || rhs == SDD_TRUE) return SDD_TRUE;
//     if(lhs == SDD_FALSE) return rhs;
//     if(rhs == SDD_FALSE) return lhs;

//     const SddNode& l_node = get_sddnode_at(lhs);
//     const SddNode& r_node = get_sddnode_at(rhs);

//     if (l_node.type() == NodeType::LIT && r_node.type() == NodeType::LIT) {
//         if (llabs(l_node.literal()) == llabs(r_node.literal())) {
//                 return SDD_TRUE;
//                 }
//         }

//     // cache check
//     {
//         addr_t cache = cache_table_.read_cache(Operation::OR, lhs, rhs);
//         if (cache != SDD_NULL) {
//             return cache;
//         }
//     }

//     // setup decomposition nodes;
//     std::vector<SddElement> decomp_l;
//     std::vector<SddElement> decomp_r;

//     const addr_t l_vnode = l_node.vtree_node_id();
//     const addr_t r_vnode = r_node.vtree_node_id();

//         //4 types
//      addr_t  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);

//      //cout << endl << "depend_vid = " << depend_vtree_node_id<<endl<<endl;

//         if (l_vnode == r_vnode) {
//             decomp_l = l_node.decomposition();
//             decomp_r = r_node.decomposition();
//         }

//         else if (l_vnode == depend_vtree_node_id) {
//             if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
//                 decomp_l = l_node.decomposition();                
//                 decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
//             }  else {
//                 decomp_l = l_node.decomposition();
//                 decomp_r = {{SDD_TRUE, rhs}};
//             }
//         }

//         else if (r_vnode == depend_vtree_node_id) {

//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = r_node.decomposition();

//             }  else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = r_node.decomposition();
                
//             }
//         }

//         else { //depend node is a common ancestor
//             if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
//                 decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
//                 decomp_r = {{SDD_TRUE, rhs}};

                
//             } else {
//                 decomp_l = {{SDD_TRUE, lhs}};
//                 decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};
//             }
//         }


    
//     std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

//         for (const auto l_elem : decomp_l) {
//             for (const auto r_elem : decomp_r) {
                
//                 addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
//                 if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
//                 addr_t new_s = sdd_apply_or(l_elem.second, r_elem.second);
//                 new_decomp_candidates.emplace_back(new_p, new_s);
//             }
//         }

//     // compression

//     auto new_decomposition = compress_candidates(new_decomp_candidates);


//     addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
//                                                  depend_vtree_node_id);
//     cache_table_.write_cache(Operation::OR, lhs, rhs, result_node);
//     return result_node;
// }

std::vector<EpdElement> EpdManager::compreess_decomp(const std::vector<std::pair<epd_addr_t, epd_addr_t>>& new_decomp_candidates)
{

}
std::vector<epd_addr_t> EpdManager::compreess_disjunction(const std::vector<epd_addr_t>& new_disjunction_candidates)
{

}


// std::vector<SddElement> SddManager::compress_candidates(const std::vector<std::pair<addr_t, addr_t>>&
//                                                           new_decomp_candidates) {
//     std::unordered_map<addr_t, std::vector<addr_t>> same_sub_groups;
//     for (auto p : new_decomp_candidates) {
//         if (same_sub_groups.find(p.second) == same_sub_groups.end()) {
//             same_sub_groups.emplace(p.second, std::vector<addr_t>{p.first});
//         } else {
//             same_sub_groups.at(p.second).push_back(p.first);
//         }
//     }
//     std::vector<SddElement> new_decomposition;

//     for (auto& p : same_sub_groups) {
//         auto same_sub_group = p.second;

//         addr_t combined = SDD_FALSE;
//         for (auto i : same_sub_group) {
//             addr_t combined_prev = combined;
//             combined = sdd_apply_or(combined_prev, i);
//         }
//         new_decomposition.emplace_back(combined, p.first);
//     }
//     return new_decomposition;
// }

unsigned long long Epdmanager::count_solution(const epd_addr_t) const {

}

unsigned long long EpdManager::count_solution_inner(const epd_addr_t epd) const {
    
}

// unsigned long long SddManager::count_solution(const addr_t sdd) const {
//     std::unordered_map<addr_t, unsigned long long> cache;
//     return count_solution_inner(sdd, cache);
// }


// unsigned long long SddManager::count_solution_inner(const addr_t sdd, 
//                                                      std::unordered_map<addr_t, 
//                                                      unsigned long long>& cache) const {
//     //if (sdd == SDD_EMPTY) return 1LLU;
//     //if (sdd == SDD_FALSE) return 0LLU;

//     if (sdd == SDD_TRUE) return 1LLU;
//     if (sdd == SDD_FALSE) return 0LLU;

//     auto r = cache.find(sdd);
//     if (r != cache.end()) {
//         return r->second;
//     }
    
//     const SddNode& n = get_sddnode_at(sdd);
//     if (n.type() == NodeType::LIT) {
//         if (n.literal() < 0) {
//             cache.emplace(sdd, 2LLU);
//             return 2LLU;
//         } else {
//             cache.emplace(sdd, 1LLU);
//             return 1LLU;
//         }
//     }
//     else { // n.type() == NodeType::DECOMP
//         unsigned long long c = 0LLU;
//         for (const auto e : n.decomposition()) {
//             auto p_c = count_solution_inner(e.first, cache);
//             auto s_c = count_solution_inner(e.second, cache);
//             c += p_c * s_c;
//         }
//         cache.emplace(sdd, c);
//         return c;
//     }
// }

unsigned long long int EpManager::size(const epd_addr_t epd) const {
    if (epd < 0) return 0;
    
    std::unordered_set<epd_addr_t> epd_nodes;
    std::stack<epd_addr_t> epd_unexpanded;
    std::unordered_set<addr_t> sdd_nodes;
    std::stack<addr_t> sdd_unexpanded;

    epd_unexpanded.push(epd);
    while (!epd_unexpanded.empty()) {
        epd_addr_t e = epd_unexpanded.top();
        epd_unexpanded.pop();

        epd_nodes.insert(e);
        if (e < 0) {
            continue;
        }                
        const EpdNode& n = epd_node_table_.get_node_at(e);
        if (n.type() == EpdNodeType::DECOMP) {
            for (const EpdElement e : n.decomposition()) {
                if (sdd_nodes.find(e.first) == sdd_nodes.end()) {
                    sdd_nodes.insert(e.first);
                    sdd_unexpanded.push(e.first);
                }
                if (epd_nodes.find(e.second) == epd_nodes.end()) {
                    epd_nodes.insert(e.second);
                    unexpanded.push(e.second);
                }
            }
        }
        else if(n.type() == EpdNodeType::DISJUNC) {
            for(const epd_addr_t e : n.disjunction()) {
                if (epd_nodes.find(e) == epd_nodes.end()) {
                    epd_nodes.insert(e);
                    epd_unexpanded.push(e);
                }
            }
        }
    }

    unsigned long long int size = 0LLU;
    for (const auto i : epd_nodes) {
        if (i < 0) continue;
        const EpdNode& n = get_epdnode_at(i);
        if (n.type() == EpdNodeType::DECOMP) {
            size += n.decomposition().size();
        }
        else if(n.type() == EpdNodeType::DISJUNC) {
            size += n.disjunction().size();
        }
    }
    for(const auto i : sdd_nodes) {
        if(i<0) continue;
        const SddNode& n = get_sddnode_at(i);
        if(n.type() == EpdNodeType::DECOMP){
            size+= n.decomposition().size();
        }
    }  
    return size;
}

}//