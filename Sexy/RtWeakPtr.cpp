#include "RtWeakPtr.hpp"

int64_t getWeakPtr(int64_t addr) {
    Sexy::RtWeakPtr<int64_t> a = Sexy::RtWeakPtr<int64_t>();
    a.FromOther((Sexy::RtWeakPtr<int64_t>*)addr);
    return (int64_t)a.get();
}