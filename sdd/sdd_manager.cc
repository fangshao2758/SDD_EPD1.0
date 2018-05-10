#include "sdd_manager.h"

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include "sdd.h"
using namespace std;
namespace sdd {

Sdd SddManager::make_sdd_literal(const addr_t literal) {
    addr_t res = make_sdd_literal_inner(literal);
    return Sdd(res, *this);
}

Sdd SddManager::make_sdd_true() {
    //cout << "true" <<endl;
    return Sdd(SDD_TRUE, *this);
}

Sdd SddManager::make_sdd_false() {
    //cout << "false" <<endl;
    return Sdd(SDD_FALSE, *this); 
}

addr_t SddManager::make_sdd_literal_inner(const addr_t literal) {
    const int v_id = vtree_.find_literal_node_id(literal);
    return sdd_node_table_.make_or_find_literal(literal, v_id);
}

void SddManager::gc() {
    sdd_node_table_.gc();
    cache_table_.clear_cache();
}


Sdd SddManager::sdd_And(const Sdd& lhs, const Sdd& rhs) {
    addr_t res =  sdd_apply_and(lhs.addr(), rhs.addr());
    return Sdd(res, *this);
}

Sdd SddManager::sdd_Or(const Sdd& lhs, const Sdd& rhs) {
    addr_t res =  sdd_apply_or(lhs.addr(), rhs.addr());
    return Sdd(res, *this);
}


addr_t SddManager::make_sdd_decomposition(std::vector<SddElement>&& decomp_nodes, 
                                            const int vtree_node) {
    assert(!decomp_nodes.empty());
    return sdd_node_table_.make_or_find_decomp(std::move(decomp_nodes), vtree_node);
}


addr_t SddManager::sdd_apply_negate(const addr_t lhs)
{

    return sdd_apply_xor(lhs,SDD_TRUE);
}

addr_t SddManager::sdd_apply_xor(const addr_t lhs,const addr_t rhs)
{
    if(lhs == rhs) return SDD_FALSE;

    if(lhs == SDD_FALSE) return rhs;
    if(rhs == SDD_FALSE) return lhs;

    if (lhs == SDD_TRUE)
    {
        const SddNode& node = get_sddnode_at(rhs);
        if(node.type()==NodeType::LIT)
        {
            return make_sdd_literal_inner(-node.literal());
        }
        else if(node.type() == NodeType::DECOMP)
        {
            std::vector<std::pair<addr_t, addr_t>> new_decomposition;

            for (const auto r_elem : node.decomposition()) {          
                addr_t new_s = sdd_apply_xor(SDD_TRUE, r_elem.second);
                new_decomposition.emplace_back(r_elem.first, new_s);
            }

    // compression
            addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
                                                   node.vtree_node_id());
            cache_table_.write_cache(Operation::XOR, lhs, rhs, result_node);
            return result_node;
        }
    }

    if (rhs == SDD_TRUE)
    {
        const SddNode& node = get_sddnode_at(lhs);
        if(node.type()==NodeType::LIT)
        {
            return make_sdd_literal_inner(-node.literal());
        }
        else if(node.type() == NodeType::DECOMP)
        {
            std::vector<std::pair<addr_t, addr_t>> new_decomposition;

            for (const auto l_elem : node.decomposition()) {          
                addr_t new_s = sdd_apply_xor(SDD_TRUE, l_elem.second);
                new_decomposition.emplace_back(l_elem.first, new_s);
            }

    // compression
            addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
                                                   node.vtree_node_id());
            cache_table_.write_cache(Operation::XOR, lhs, rhs, result_node);
            return result_node;
        }
    }


    // cache check
    {
        addr_t cache = cache_table_.read_cache(Operation::XOR, lhs, rhs);
        if (cache != SDD_NULL) {
            return cache;
        }
    }
    const SddNode& l_node = get_sddnode_at(lhs);
    const SddNode& r_node = get_sddnode_at(rhs);
    //cout <<"manager_xor://"<<"lhs: "<<lhs << "rhs: " <<rhs<<endl<<endl;
    // setup decomposition nodes;
    std::vector<SddElement> decomp_l;
    std::vector<SddElement> decomp_r;

    const int l_vnode = l_node.vtree_node_id();
    const int r_vnode = r_node.vtree_node_id();

    // cout <<endl<< "sdd_manager.cc_xor file" <<endl;
    // cout << "l_vnode: " << l_vnode << "r_vnode: " << r_vnode <<endl<<endl; 
        //4 types
     int  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);

        if (l_vnode == r_vnode) {
            decomp_l = l_node.decomposition();
            decomp_r = r_node.decomposition();
        }

        else if (l_vnode == depend_vtree_node_id) {
            if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
                decomp_l = l_node.decomposition();                
                decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
            }  else {
                decomp_l = l_node.decomposition();
                decomp_r = {{SDD_TRUE, rhs}};
            }
        }

        else if (r_vnode == depend_vtree_node_id) {

            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = r_node.decomposition();

            }  else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = r_node.decomposition();
                
            }
        }

        else { //depend node is a common ancestor
            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = {{SDD_TRUE, rhs}};

                
            } else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};

            }
        }


    
    std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

        for (const auto l_elem : decomp_l) {
            for (const auto r_elem : decomp_r) {
                
                addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
                if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
                addr_t new_s = sdd_apply_xor(l_elem.second, r_elem.second);
                new_decomp_candidates.emplace_back(new_p, new_s);
            }
        }

    // compression

    auto new_decomposition = compress_candidates(new_decomp_candidates);

    if (new_decomposition.size() == 1) {
        // zero-suppression rule
        SddElement e = new_decomposition[0];
            cache_table_.write_cache(Operation::XOR, lhs, rhs, e.second);
            return e.second;
    }
    else if(new_decomposition.size() == 2) {
        SddElement e1 = new_decomposition[0];
        SddElement e2 = new_decomposition[1];
        if(e1.second == SDD_TRUE && e2.second == SDD_FALSE)
        {
            cache_table_.write_cache(Operation::XOR,lhs,rhs,e1.first);
            return e1.first;
        }
        else if(e1.second == SDD_FALSE && e2.second == SDD_TRUE)
        {
            cache_table_.write_cache(Operation::XOR,lhs,rhs,e2.first);
            return e2.first;
        }
    }

    addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
                                                 depend_vtree_node_id);
    cache_table_.write_cache(Operation::XOR, lhs, rhs, result_node);
    return result_node;

}

addr_t SddManager::sdd_apply_and(const addr_t lhs, const addr_t rhs) {

    if(lhs == rhs) return lhs;

    if (lhs == SDD_FALSE || rhs == SDD_FALSE) return SDD_FALSE;
    if(lhs == SDD_TRUE) return rhs;
    if(rhs == SDD_TRUE) return lhs;

    const SddNode& l_node = get_sddnode_at(lhs);
    const SddNode& r_node = get_sddnode_at(rhs);

    if (l_node.type() == NodeType::LIT && r_node.type() == NodeType::LIT) {
        if (llabs(l_node.literal()) == llabs(r_node.literal())) {
            if (l_node.literal() > 0) {
                    return lhs;
                }
                else 
                    return rhs;
                }
        }

    // cache check
    {
        addr_t cache = cache_table_.read_cache(Operation::AND, lhs, rhs);
        if (cache != SDD_NULL) {
            return cache;
        }
    }
    // setup decomposition nodes;
    std::vector<SddElement> decomp_l;
    std::vector<SddElement> decomp_r;

    const int l_vnode = l_node.vtree_node_id();
    const int r_vnode = r_node.vtree_node_id();

        //4 types
    // cout <<endl<< "sdd_manager.cc_and file" <<endl;
    // cout << "l_vnode: " << l_vnode << "r_vnode: " << r_vnode <<endl<<endl; 
    int  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);


        if (l_vnode == r_vnode) {
            decomp_l = l_node.decomposition();
            decomp_r = r_node.decomposition();
        }

        else if (l_vnode == depend_vtree_node_id) {
            if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
                decomp_l = l_node.decomposition();                
                decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
            }  else {
                decomp_l = l_node.decomposition();
                decomp_r = {{SDD_TRUE, rhs}};
            }
        }

        else if (r_vnode == depend_vtree_node_id) {

            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = r_node.decomposition();

            }  else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = r_node.decomposition();
                
            }
        }

        else { //depend node is a common ancestor
            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = {{SDD_TRUE, rhs}};

                
            } else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};

            }
        }


    
    std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

        for (const auto l_elem : decomp_l) {
            for (const auto r_elem : decomp_r) {
                
                addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
                if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
                addr_t new_s = sdd_apply_and(l_elem.second, r_elem.second);
                new_decomp_candidates.emplace_back(new_p, new_s);
            }
        }

    // compression

    auto new_decomposition = compress_candidates(new_decomp_candidates);

    if (new_decomposition.size() == 1) {
        // zero-suppression rule
        SddElement e = new_decomposition[0];
            cache_table_.write_cache(Operation::AND, lhs, rhs, e.second);
            return e.second;
    }
    else if(new_decomposition.size() == 2) {
        SddElement e1 = new_decomposition[0];
        SddElement e2 = new_decomposition[1];
        if(e1.second == SDD_TRUE && e2.second == SDD_FALSE)
        {
            cache_table_.write_cache(Operation::AND,lhs,rhs,e1.first);
            return e1.first;
        }
        else if(e1.second == SDD_FALSE && e2.second == SDD_TRUE)
        {
            cache_table_.write_cache(Operation::AND,lhs,rhs,e2.first);
            return e2.first;
        }
    }

    addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
                                                 depend_vtree_node_id);
    cache_table_.write_cache(Operation::AND, lhs, rhs, result_node);
    return result_node;
}

addr_t SddManager::sdd_apply_or(const addr_t lhs, const addr_t rhs) {

    if(lhs == rhs) return lhs;

    if (lhs == SDD_TRUE || rhs == SDD_TRUE) return SDD_TRUE;
    if(lhs == SDD_FALSE) return rhs;
    if(rhs == SDD_FALSE) return lhs;

    const SddNode& l_node = get_sddnode_at(lhs);
    const SddNode& r_node = get_sddnode_at(rhs);

    if (l_node.type() == NodeType::LIT && r_node.type() == NodeType::LIT) {
        if (llabs(l_node.literal()) == llabs(r_node.literal())) {
                return SDD_TRUE;
                }
        }

    // cache check
    {
        addr_t cache = cache_table_.read_cache(Operation::OR, lhs, rhs);
        if (cache != SDD_NULL) {
            return cache;
        }
    }

    // setup decomposition nodes;
    std::vector<SddElement> decomp_l;
    std::vector<SddElement> decomp_r;

    const int l_vnode = l_node.vtree_node_id();
    const int r_vnode = r_node.vtree_node_id();

        //4 types
    // cout << "sdd_manager.cc_or file" <<endl;
    // cout << "l_vnode: " << l_vnode << "r_vnode: " << r_vnode <<endl; 
    int  depend_vtree_node_id = vtree_.get_depend_node(l_vnode, r_vnode);

     //cout << endl << "depend_vid = " << depend_vtree_node_id<<endl<<endl;

        if (l_vnode == r_vnode) {
            decomp_l = l_node.decomposition();
            decomp_r = r_node.decomposition();
        }

        else if (l_vnode == depend_vtree_node_id) {
            if (vtree_.is_left_descendant(depend_vtree_node_id, r_vnode)) {
                decomp_l = l_node.decomposition();                
                decomp_r = {{rhs, SDD_TRUE}, {sdd_apply_negate(rhs), SDD_FALSE}};
            }  else {
                decomp_l = l_node.decomposition();
                decomp_r = {{SDD_TRUE, rhs}};
            }
        }

        else if (r_vnode == depend_vtree_node_id) {

            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE}, {sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = r_node.decomposition();

            }  else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = r_node.decomposition();
                
            }
        }

        else { //depend node is a common ancestor
            if (vtree_.is_left_descendant(depend_vtree_node_id, l_vnode)) {
                decomp_l = {{lhs, SDD_TRUE},{sdd_apply_negate(lhs),SDD_FALSE}};
                decomp_r = {{SDD_TRUE, rhs}};

                
            } else {
                decomp_l = {{SDD_TRUE, lhs}};
                decomp_r = {{rhs, SDD_TRUE},{sdd_apply_negate(rhs),SDD_FALSE}};
            }
        }


    
    std::vector<std::pair<addr_t, addr_t>> new_decomp_candidates;

        for (const auto l_elem : decomp_l) {
            for (const auto r_elem : decomp_r) {
                
                addr_t new_p = sdd_apply_and(l_elem.first, r_elem.first);
                if (new_p == SDD_FALSE) continue;  //为什么SDD_NULL跳出循环
                
                addr_t new_s = sdd_apply_or(l_elem.second, r_elem.second);
                new_decomp_candidates.emplace_back(new_p, new_s);
            }
        }

    // compression

    auto new_decomposition = compress_candidates(new_decomp_candidates);

    if (new_decomposition.size() == 1) {
        // zero-suppression rule
        SddElement e = new_decomposition[0];
            cache_table_.write_cache(Operation::OR, lhs, rhs, e.second);
            return e.second;
    }
    else if(new_decomposition.size() == 2) {
        SddElement e1 = new_decomposition[0];
        SddElement e2 = new_decomposition[1];
        if(e1.second == SDD_TRUE && e2.second == SDD_FALSE)
        {
            cache_table_.write_cache(Operation::OR,lhs,rhs,e1.first);
            return e1.first;
        }
        else if(e1.second == SDD_FALSE && e2.second == SDD_TRUE)
        {
            cache_table_.write_cache(Operation::OR,lhs,rhs,e2.first);
            return e2.first;
        }
    }

    addr_t result_node = make_sdd_decomposition(std::move(new_decomposition),
                                                 depend_vtree_node_id);
    cache_table_.write_cache(Operation::OR, lhs, rhs, result_node);
    return result_node;
}


std::vector<SddElement> SddManager::compress_candidates(const std::vector<std::pair<addr_t, addr_t>>&
                                                          new_decomp_candidates) {
    std::unordered_map<addr_t, std::vector<addr_t>> same_sub_groups;
    for (auto p : new_decomp_candidates) {
        if (same_sub_groups.find(p.second) == same_sub_groups.end()) {
            same_sub_groups.emplace(p.second, std::vector<addr_t>{p.first});
        } else {
            same_sub_groups.at(p.second).push_back(p.first);
        }
    }
    std::vector<SddElement> new_decomposition;

    for (auto& p : same_sub_groups) {
        auto same_sub_group = p.second;

        addr_t combined = SDD_FALSE;
        for (auto i : same_sub_group) {
            addr_t combined_prev = combined;
            combined = sdd_apply_or(combined_prev, i);
        }
        new_decomposition.emplace_back(combined, p.first);
    }
    return new_decomposition;
}


unsigned long long SddManager::count_solution(const addr_t sdd) const {
    std::unordered_map<addr_t, unsigned long long> cache;
    return count_solution_inner(sdd, cache);
}


unsigned long long SddManager::count_solution_inner(const addr_t sdd, 
                                                     std::unordered_map<addr_t, 
                                                     unsigned long long>& cache) const {
    //if (sdd == SDD_EMPTY) return 1LLU;
    //if (sdd == SDD_FALSE) return 0LLU;

    if (sdd == SDD_TRUE) return 1LLU;
    if (sdd == SDD_FALSE) return 0LLU;

    auto r = cache.find(sdd);
    if (r != cache.end()) {
        return r->second;
    }
    
    const SddNode& n = get_sddnode_at(sdd);
    if (n.type() == NodeType::LIT) {
        if (n.literal() < 0) {
            cache.emplace(sdd, 2LLU);
            return 2LLU;
        } else {
            cache.emplace(sdd, 1LLU);
            return 1LLU;
        }
    }
    else { // n.type() == NodeType::DECOMP
        unsigned long long c = 0LLU;
        for (const auto e : n.decomposition()) {
            auto p_c = count_solution_inner(e.first, cache);
            auto s_c = count_solution_inner(e.second, cache);
            c += p_c * s_c;
        }
        cache.emplace(sdd, c);
        return c;
    }
}


// std::vector<std::vector<int>> SddManager::calc_setfamily(const addr_t sdd) const {
//     std::unordered_map<addr_t, std::vector<std::vector<int>>> cache;
//     return calc_setfamily_inner(sdd, cache);
// }

// std::vector<std::vector<int>> 
// SddManager::calc_setfamily_inner(const addr_t sdd, 
//                                   std::unordered_map<addr_t, std::vector<std::vector<int>>>& cache) const {
//     //if (sdd == SDD_EMPTY) return std::vector<std::vector<int>>(1, std::vector<int>());
//     if (sdd == SDD_TRUE) return std::vector<std::vector<int>>(1, std::vector<int>());
//     if (sdd == SDD_FALSE) return std::vector<std::vector<int>>();

//     auto r = cache.find(sdd);
//     if (r != cache.end()) {
//         return r->second;
//     }

//     const SddNode& n = get_sddnode_at(sdd);
//     std::vector<std::vector<int>> v;
//     if (n.type() == NodeType::LIT) {
//         if (n.literal() < 0) {
//             v.push_back(std::vector<int>());
//             v.push_back({-n.literal()});
//         } else {
//             v.push_back({n.literal()});
//         }
//     }
//     else { // n.type() == NodeType::DECOMP
//         for (const auto e : n.decomposition()) {
//             const auto& p_v = calc_setfamily_inner(e.first, cache);
//             const auto& s_v = calc_setfamily_inner(e.second, cache);

//             for (const auto& x : p_v) {
//                 for (const auto& y : s_v) {
//                     auto z = x;
//                     z.insert(z.end(), y.begin(), y.end());
//                     v.emplace_back(move(z));
//                 }
//             }
//         }
//     }
//     cache.emplace(sdd, v);
//     return v;
    
// }



unsigned long long int SddManager::size(const addr_t sdd) const {
    if (sdd < 0) return 0;
    
    std::unordered_set<addr_t> nodes;
    std::stack<addr_t> unexpanded;
    unexpanded.push(sdd);
    while (!unexpanded.empty()) {
        addr_t e = unexpanded.top();
        unexpanded.pop();

        nodes.insert(e);
        if (e < 0) {
            continue;
        }                
        const SddNode& n = sdd_node_table_.get_node_at(e);
        if (n.type() == NodeType::DECOMP) {
            for (const SddElement e : n.decomposition()) {
                if (nodes.find(e.first) == nodes.end()) {
                    nodes.insert(e.first);
                    unexpanded.push(e.first);
                }
                if (nodes.find(e.second) == nodes.end()) {
                    nodes.insert(e.second);
                    unexpanded.push(e.second);
                }
            }
        }
    }
    unsigned long long int size = 0LLU;
    for (const auto i : nodes) {
        if (i < 0) continue;
        const SddNode& n = get_sddnode_at(i);
        if (n.type() == NodeType::DECOMP) {
            size += n.decomposition().size();
        }
    }
    return size;
}


}//