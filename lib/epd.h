#ifndef EPD_H_
#define EPD_H_

#include "epd_manager.h"
#include <iostream>

namespace epd {

class Epd {
public:
    Epd(const epd_addr_t epd_addr, EpdManager& manager) : 
        epd_addr_(epd_addr), mngr_(manager) { 
        mngr_.inc_epdnode_refcount_at(epd_addr_);
    } 
    Epd(const Epd& obj) : 
        epd_addr_(obj.epd_addr_), mngr_(obj.mngr_) {
        mngr_.inc_epdnode_refcount_at(epd_addr_);
    }
    Epd(const Epd&& obj) : 
        epd_addr_(obj.epd_addr_), mngr_(obj.mngr_) {
        mngr_.inc_epdnode_refcount_at(epd_addr_);
    }
    ~Epd() {
        mngr_.dec_epdnode_refcount_at(epd_addr_);
    }
    Epd& operator=(const Epd& obj) {
        if (epd_addr_ == obj.epd_addr_) return *this;
        mngr_.dec_epdnode_refcount_at(epd_addr_);
        epd_addr_ = obj.epd_addr_;
        mngr_.inc_epdnode_refcount_at(epd_addr_);
        return *this;
    }
    unsigned long long int count_solution() const {
        return mngr_.count_solution(epd_addr_);
    }

    unsigned long long int size() const {
        cout << "epd_addr_: "<<epd_addr_ <<endl;
        return mngr_.size(epd_addr_);
    }
    epd_addr_t epd_addr() const { return epd_addr_; }

private:
    epd_addr_t epd_addr_;
    EpdManager& mngr_;
};
}

#endif // EPD_H_
