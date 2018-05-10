#ifndef SDD_COMMON_H_
#define SDD_COMMON_H_
#include <utility>
#include <unordered_set>

namespace sdd {
using addr_t = long long int;


constexpr addr_t SDD_FALSE = -2;
constexpr addr_t SDD_NULL = -3;
constexpr addr_t SDD_TRUE = -1;

enum class NodeType : char{LIT, DECOMP, UNUSED};

enum  class Operation : char
{
    NULLOP,
    AND,             //SDD的三种apply操作
    OR,
    XOR,
};


inline void hash_combine(size_t& seed, size_t value) {
    seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

} // namespce sdd
#endif // SDD_COMMON_H
