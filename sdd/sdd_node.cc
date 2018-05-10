#include "sdd_node.h"
#include "sdd_manager.h"
#include <assert.h>
#include <sstream>

namespace sdd {


void SddNode::inc_ref_count(SddManager& mgr) {
    if (type_ == NodeType::LIT) return;

    refcount_++;
    if (refcount_ == 1) {
        for (const auto& e : decomp_) {
            if (e.first >= 0) {
                SddNode& p = mgr.get_sddnode_at(e.first);
                p.inc_ref_count(mgr);
            }
            if (e.second >= 0) {
                SddNode& s = mgr.get_sddnode_at(e.second);
                s.inc_ref_count(mgr);
            }
        }
    }
}

void SddNode::dec_ref_count(SddManager& mgr) {
    if (type_ == NodeType::LIT) return;

    refcount_--;
    if (refcount_ == 0) {
        for (const auto& e : decomp_) {
            if (e.first >= 0) {
                SddNode& p = mgr.get_sddnode_at(e.first);
                p.dec_ref_count(mgr);
            }
            if (e.second >= 0) {
                SddNode& s = mgr.get_sddnode_at(e.second);
                s.dec_ref_count(mgr);
            }
        }
    }
}


} // namespace sdd;
