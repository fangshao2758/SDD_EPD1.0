#include "epd_node.h"
#include "epd_manager.h"
#include <assert.h>
#include <sstream>

namespace epd {

//没有递归积

void EpdNode::inc_ref_count(EpdManager& mgr) {
    if (type_ == EpdNodeType::CONSTANT) return;

    refcount_++;

    if (type_ == EpdNodeType::DISJUNC)
    {
        if(refcount_ == 1){
            for(const auto& e : disjunction_){
                if(e >= 0){
                    EpdNode& p = mgr.get_epdnode_at(e);
                    p.inc_ref_count(mgr);
                }
            }
        }
    }

    else if(type_ == EpdNodeType::DECOMP){
        if (refcount_ == 1) {
            for (const auto& e : decomp_) {
                if (e.first >= 0) {
                    EpdNode& p = mgr.get_epdnode_at(e.first);
                    p.inc_ref_count(mgr);
                }
                if (e.second >= 0) {
                    EpdNode& s = mgr.get_epdnode_at(e.second);
                    s.inc_ref_count(mgr);
                }
            }
        }
    }

}


void EpdNode::dec_ref_count(EpdManager& mgr) {
    if (type_ == EpdNodeType::CONSTANT) return;

    refcount_--;

    if (type_ == EpdNodeType::DISJUNC)
    {
        if(refcount_ == 0){
            for(const auto& e : disjunction_){
                if(e >= 0){
                    EpdNode& p = mgr.get_epdnode_at(e);
                    p.dec_ref_count(mgr);
                }
            }
        }
    }
    
    else if(type_ == EpdNodeType::DECOMP){ 
        if (refcount_ == 0) {
            for (const auto& e : decomp_) {
                if (e.first >= 0) {
                    EpdNode& p = mgr.get_epdnode_at(e.first);
                    p.dec_ref_count(mgr);
                }
                if (e.second >= 0) {
                    EpdNode& s = mgr.get_epdnode_at(e.second);
                    s.dec_ref_count(mgr);
                }
            }
        }
    }
}


} // namespace epd;
