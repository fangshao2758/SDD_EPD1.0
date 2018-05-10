#ifndef SDD_H_
#define SDD_H_

#include "sdd_manager.h"
#include <iostream>

namespace sdd {

class Sdd {
public:
    Sdd(const addr_t addr, SddManager& manager) : 
        addr_(addr), mngr_(manager) { 
        mngr_.inc_sddnode_refcount_at(addr_);
    } 
    Sdd(const Sdd& obj) : 
        addr_(obj.addr_), mngr_(obj.mngr_) {
        mngr_.inc_sddnode_refcount_at(addr_);
    }
    Sdd(const Sdd&& obj) : 
        addr_(obj.addr_), mngr_(obj.mngr_) {
        mngr_.inc_sddnode_refcount_at(addr_);
    }
    ~Sdd() {
        mngr_.dec_sddnode_refcount_at(addr_);
    }
    Sdd& operator=(const Sdd& obj) {
        if (addr_ == obj.addr_) return *this;
        mngr_.dec_sddnode_refcount_at(addr_);
        addr_ = obj.addr_;
        mngr_.inc_sddnode_refcount_at(addr_);
        return *this;
    }
    unsigned long long int count_solution() const {
        return mngr_.count_solution(addr_);
    }

    unsigned long long int size() const {
        cout << "addr_: "<<addr_ <<endl;
        return mngr_.size(addr_);
    }
    addr_t addr() const { return addr_; }

    void export_txt(std::ostream& os) const {
        mngr_.export_sdd_txt(addr_, os);
    }
    void export_dot(std::ostream& os) const {
        mngr_.export_sdd_dot(addr_, os);
    }

private:
    addr_t addr_;
    SddManager& mngr_;
};
}

#endif // SDD_H_
