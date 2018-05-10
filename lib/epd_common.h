#ifndef EPD_COMMON_H_
#define EPD_COMMON_H_
#include <utility>
#include <unordered_set>

namespace epd {
using epd_addr_t = long long int;

constexpr epd_addr_t EPD_TRUE = -1;
constexpr epd_addr_t EPD_FALSE = -2;
constexpr epd_addr_t EPD_ALL = -3;
constexpr epd_addr_t EPD_EMPTY = -4;
constexpr epd_addr_t EPD_NULL = -5;

enum class EpdNodeType : char{DISJUNC, DECOMP, CONSTANT, UNUSED};

enum  class Operation : char
{
    NULLOP,
    AND,             //EPD的三种apply操作
    OR,
    NEG,
};

enum class TransType : char
{
    NULLOL,
	POS,
	NEG,
};

inline void hash_combine(size_t& seed, size_t value) {
    seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

} // namespce epd
#endif // EPD_COMMON_H
