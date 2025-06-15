#include "RtWeakPtr.hpp"

INT getWeakPtr(INT addr) {
    Sexy::RtWeakPtr<INT> a = Sexy::RtWeakPtr<INT>();
    a.FromOther((Sexy::RtWeakPtr<INT>*)addr);
    return (INT)a.get();
}