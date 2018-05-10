#ifndef EPD_CACHE_TABLE_H_
#define EPD_CACHE_TABLE_H_
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include "sdd_common.h"
#include "epd_node.h"
#include "epd_common.h"
#include "sdd_node.h"

namespace epd {

class EpdCacheTable {

    using epd_cache_entry = std::tuple<Operation, epd_addr_t, epd_addr_t, epd_addr_t>;  //and,or
    using epd_cache_entry_neg = std::tuple<epd_addr_t,epd_addr_t>;  //neg
    using sdd_to_epd_cache_entry = std::tuple<sdd::addr_t, epd_addr_t>;  //sdd_to_epd

public:

    EpdCacheTable() : epd_cache_table_(INIT_SIZE), epd_cache_table_neg_(INIT_SIZE), sdd_to_epd_cache_table_(INIT_SIZE) {}
    EpdCacheTable(const int init_size) : epd_cache_table_(init_size), epd_cache_table_neg_(init_size), sdd_to_epd_cache_table_(init_size) {}


    void clear_cache() {
        for (auto it =  epd_cache_table_.begin(); it != epd_cache_table_.end(); ++it) {
            *it = std::make_tuple(Operation::NULLOP, -1, -1, -1);
        }
    }
    
    void clear_cache() {
        for (auto it = epd_cache_table_neg_.begin(); it != epd_cache_table_neg_.end(); ++it) {
            *it = std::make_tuple(-1, -1);
        }
    }

    void clear_cache() {
        for (auto it = sdd_to_epd_cache_table_.begin(); it != sdd_to_epd_cache_table_.end(); ++it) {
            *it = std::make_tuple(TransType::NULLOL, -1, -1);
        }
    }

    void write_cache(const Operation op, const epd_addr_t lhs, const epd_addr_t rhs, const epd_addr_t res)  //and,or
    {
        auto key = calc_key(op, lhs, rhs);
        epd_cache_table_[key] = std::make_tuple(op, lhs, rhs, res);
    }

    void write_cache(const epd_addr_t lhs,const epd_addr_t res)   //neg
    {
        auto key = calc_key(lhs);
        epd_cache_table_neg_[key] = std::make_tuple(lhs, res);

    }

    void write_cache(const TransType ol, const sdd::addr_t lhs, const epd_addr_t res)  //2 types  sdd_to_epd(positive,negate)
    {
        auto key = calc_key(ol, lhs);
        sdd_to_epd_cache_table_[key] = std::make_tuple(ol, lhs);

    }

    epd_addr_t read_cache(const Operation op, const epd_addr_t lhs, const epd_addr_t rhs)
    {
        auto key = calc_key(op, lhs, rhs);
        auto res = epd_cache_table_[key];
        
        if (std::get<0>(res) == op &&
            std::get<1>(res) == lhs &&
            std::get<2>(res) == rhs) {
            return std::get<3>(res);
        }
        return EPD_NULL;
    }

    epd_addr_t read_cache(const epd_addr_t lhs)
    {
        auto key = calc_key(lhs);
        auto res = epd_cache_table_neg_[key];

        if(std::get<0>(res) == lhs)
        {
            return std::get<1>(res);
        }
        return EPD_NULL;

    }

    epdepd_addr_t read_cache(const TransType ol, const sdd::addr_t lhs)
    {
        auto key = calc_key(ol,lhs);
        auto res = sdd_to_epd_cache_table_[key];

        if(std::get<0>(res) == ol &&
           std::get<1>(res) == lhs){
            return std::get<2>(res);
        }
        return EPD_NULL;
    }


    void extend_table() {
        const auto prev_size = epd_cache_table_.size();
        epd_cache_table_.reserve(epd_cache_table_.size() << TABLE_EXTEND_FACTOR);
        for (size_t k = 1; k < (1U << TABLE_EXTEND_FACTOR); k++) {
            for (size_t i = 0; i < prev_size; i++) {
                epd_cache_table_.push_back(epd_cache_table_[i]);
            }
        }
    }

    void extend_table() {
        const auto prev_size = epd_cache_table_neg_.size();
        epd_cache_table_neg_.reserve(epd_cache_table_neg_.size() << TABLE_EXTEND_FACTOR);
        for (size_t k = 1; k < (1U << TABLE_EXTEND_FACTOR); k++) {
            for (size_t i = 0; i < prev_size; i++) {
                epd_cache_table_neg_.push_back(epd_cache_table_neg_[i]);
            }
        }
    }

    void extend_table() {
        const auto prev_size = sdd_to_epd_cache_table_.size();
        sdd_to_epd_cache_table_.reserve(sdd_to_epd_cache_table_.size() << TABLE_EXTEND_FACTOR);
        for (size_t k = 1; k < (1U << TABLE_EXTEND_FACTOR); k++) {
            for (size_t i = 0; i < prev_size; i++) {
                sdd_to_epd_cache_table_.push_back(sdd_to_epd_cache_table_[i]);
            }
        }
    }     

private:
    const unsigned int TABLE_EXTEND_FACTOR = 2;
    const unsigned int INIT_SIZE = 1U<<8;
    std::vector<epd_cache_entry> epd_cache_table_;
    std::vector<sdd_to_epd_cache_entry> sdd_to_epd_cache_table_;
    std::vector<epd_cache_entry_neg> epd_cache_table_neg_;

    size_t calc_key(const Operation op, const epd_addr_t lhs,  const epd_addr_t rhs) {  //and,or
        size_t key = 0;
        hash_combine(key, std::hash<int>()(static_cast<int>(op)));
        hash_combine(key, std::hash<size_t>()(lhs));
        hash_combine(key, std::hash<size_t>()(rhs));
        return key % epd_cache_table_.size();
    }

    size_t calc_key(const epd_addr_t lhs) {   //neg
        size_t key = 0;
        hash_combine(key, std::hash<size_t>()(lhs));
        return key % epd_cache_table_neg_.size();
    }

    size_t calc_key(const TransType ol, const sdd::addr_t lhs) {
        size_t key = 0;
        hash_combine(key, std::hash<int>()(static_cast<int>(ol)));
        hash_combine(key, std::hash<size_t>()(lhs));
        return key % sdd_to_epd_cache_table_.size();
    }

};
} // namespace epd;
#endif //EPD_CACHE_TABLE_H_
