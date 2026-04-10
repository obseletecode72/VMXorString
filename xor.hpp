#pragma once
#pragma optimize("t", on)
#pragma inline_depth(255)
#pragma inline_recursion(on)

typedef unsigned long long size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifdef _MSC_VER
#define VMSTR_FORCEINLINE __forceinline
extern "C" unsigned __int64 __rdtsc(void);
#pragma intrinsic(__rdtsc)
#else
#define VMSTR_FORCEINLINE __attribute__((always_inline)) inline
#include <x86intrin.h>
#endif

namespace minivm {
    template <size_t... Is>
    struct index_sequence {};

    template <size_t N, size_t... Is>
    struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, Is...> {};

    template <size_t... Is>
    struct make_index_sequence_impl<0, Is...> {
        typedef index_sequence<Is...> type;
    };

    template <size_t N>
    using make_index_sequence = typename make_index_sequence_impl<N>::type;

    struct OpcodesArray {
        uint8_t data[7];
    };

    VMSTR_FORCEINLINE void m_memcpy(void* dest, const void* src, size_t n) {
        char* d = (char*)dest;
        const char* s = (const char*)src;
        for (size_t i = 0; i < n; ++i) {
            d[i] = s[i];
        }
    }

    VMSTR_FORCEINLINE void m_wmemcpy(wchar_t* dest, const wchar_t* src, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            dest[i] = src[i];
        }
    }

    VMSTR_FORCEINLINE bool m_strcmp(const char* s1, const char* s2) {
        while (*s1 && (*s1 == *s2)) {
            s1++;
            s2++;
        }
        return *(const unsigned char*)s1 == *(const unsigned char*)s2;
    }

    VMSTR_FORCEINLINE bool m_wcscmp(const wchar_t* s1, const wchar_t* s2) {
        while (*s1 && (*s1 == *s2)) {
            s1++;
            s2++;
        }
        return *s1 == *s2;
    }

    constexpr OpcodesArray generate_opcodes(uint32_t seed) {
        OpcodesArray ops = {0};
        uint32_t s = seed ^ 0x66B5A10F;
        for (int i = 0; i < 7; i++) {
            bool unique;
            uint8_t cand;
            do {
                s = s * 1664525 + 1013904223;
                cand = (uint8_t)((s >> 16) & 0xFF);
                unique = true;
                for (int j = 0; j < i; j++) {
                    if (ops.data[j] == cand) {
                        unique = false;
                        break;
                    }
                }
            } while(!unique);
            ops.data[i] = cand;
        }
        return ops;
    }

    template <size_t CodeLen>
    struct Bytecode {
         uint8_t opcodes[7];
         uint8_t code[CodeLen];
    };

    template <size_t Len, class L, size_t... I>
    constexpr auto compile_vm_impl(L l, uint32_t seed, index_sequence<I...>) {
        OpcodesArray opcodes = generate_opcodes(seed);
        const uint8_t OP_ADD = opcodes.data[0];
        const uint8_t OP_SUB = opcodes.data[1];
        const uint8_t OP_XOR = opcodes.data[2];
        const uint8_t OP_ROL = opcodes.data[3];
        const uint8_t OP_ROR = opcodes.data[4];
        const uint8_t OP_OUT = opcodes.data[5];
        const uint8_t OP_END = opcodes.data[6];

        uint8_t code[Len * 8 + 1] = {0};
        size_t pc = 0;
        uint32_t s = seed;
        char str[Len] = { l()[I]... };
        
        uint8_t current_val = 0;
        
        for (size_t i = 0; i < Len; ++i) {
            uint8_t target = str[i];
            
            s = s * 1664525 + 1013904223;
            uint8_t op1 = (s >> 16) % 5;
            s = s * 1664525 + 1013904223;
            uint8_t k1 = (s & 0xFF);
            
            if (op1 == 0) {
                current_val += k1;
                code[pc++] = OP_ADD;
                code[pc++] = k1;
            } else if (op1 == 1) {
                current_val -= k1;
                code[pc++] = OP_SUB;
                code[pc++] = k1;
            } else if (op1 == 2) {
                current_val ^= k1;
                code[pc++] = OP_XOR;
                code[pc++] = k1;
            } else if (op1 == 3) {
                k1 &= 7;
                current_val = (uint8_t)((current_val << k1) | (current_val >> (8 - k1)));
                code[pc++] = OP_ROL;
                code[pc++] = k1;
            } else if (op1 == 4) {
                k1 &= 7;
                current_val = (uint8_t)((current_val >> k1) | (current_val << (8 - k1)));
                code[pc++] = OP_ROR;
                code[pc++] = k1;
            }

            s = s * 1664525 + 1013904223;
            uint8_t op2 = (s >> 16) % 5;
            s = s * 1664525 + 1013904223;
            uint8_t k2 = (s & 0xFF);
            
            if (op2 == 0) {
                current_val += k2;
                code[pc++] = OP_ADD;
                code[pc++] = k2;
            } else if (op2 == 1) {
                current_val -= k2;
                code[pc++] = OP_SUB;
                code[pc++] = k2;
            } else if (op2 == 2) {
                current_val ^= k2;
                code[pc++] = OP_XOR;
                code[pc++] = k2;
            } else if (op2 == 3) {
                k2 &= 7;
                current_val = (uint8_t)((current_val << k2) | (current_val >> (8 - k2)));
                code[pc++] = OP_ROL;
                code[pc++] = k2;
            } else if (op2 == 4) {
                k2 &= 7;
                current_val = (uint8_t)((current_val >> k2) | (current_val << (8 - k2)));
                code[pc++] = OP_ROR;
                code[pc++] = k2;
            }

            s = s * 1664525 + 1013904223;
            uint8_t r_op = (s >> 16) % 3;
            if (r_op == 0) {
                code[pc++] = OP_ADD;
                code[pc++] = (uint8_t)(target - current_val);
            } else if (r_op == 1) {
                code[pc++] = OP_XOR;
                code[pc++] = (uint8_t)(target ^ current_val);
            } else if (r_op == 2) {
                code[pc++] = OP_SUB;
                code[pc++] = (uint8_t)(current_val - target);
            }
            
            s = s * 1664525 + 1013904223;
            uint8_t fake_arg = (s & 0xFF);
            code[pc++] = OP_OUT;
            code[pc++] = fake_arg;
            
            current_val = (uint8_t)(target ^ fake_arg); 
        }
        code[pc] = OP_END;
        
        Bytecode<Len * 8 + 1> res{};
        for (int i = 0; i < 7; i++) {
            res.opcodes[i] = opcodes.data[i];
        }
        for (size_t i = 0; i <= pc; ++i) {
            res.code[i] = code[i];
        }
        return res;
    }

    template <size_t Len, class L>
    constexpr auto compile_vm(L l, uint32_t seed) {
        return compile_vm_impl<Len>(l, seed, make_index_sequence<Len>{});
    }

    template <size_t Len, class L, size_t... I>
    constexpr auto compile_vm_w_impl(L l, uint32_t seed, index_sequence<I...>) {
        OpcodesArray opcodes = generate_opcodes(seed);
        const uint8_t OP_ADD = opcodes.data[0];
        const uint8_t OP_SUB = opcodes.data[1];
        const uint8_t OP_XOR = opcodes.data[2];
        const uint8_t OP_ROL = opcodes.data[3];
        const uint8_t OP_ROR = opcodes.data[4];
        const uint8_t OP_OUT = opcodes.data[5];
        const uint8_t OP_END = opcodes.data[6];

        uint8_t code[Len * 16 + 1] = {0};
        size_t pc = 0;
        uint32_t s = seed;
        wchar_t str[Len] = { l()[I]... };
        
        uint8_t current_val = 0;
        
        for (size_t i = 0; i < Len; ++i) {
            uint16_t wc = str[i];
            for (int b = 0; b < 2; ++b) {
                uint8_t target = (b == 0) ? (wc & 0xFF) : ((wc >> 8) & 0xFF);
                
                s = s * 1664525 + 1013904223;
                uint8_t op1 = (s >> 16) % 5;
                s = s * 1664525 + 1013904223;
                uint8_t k1 = (s & 0xFF);
                
                if (op1 == 0) {
                    current_val += k1;
                    code[pc++] = OP_ADD;
                    code[pc++] = k1;
                } else if (op1 == 1) {
                    current_val -= k1;
                    code[pc++] = OP_SUB;
                    code[pc++] = k1;
                } else if (op1 == 2) {
                    current_val ^= k1;
                    code[pc++] = OP_XOR;
                    code[pc++] = k1;
                } else if (op1 == 3) {
                    k1 &= 7;
                    current_val = (uint8_t)((current_val << k1) | (current_val >> (8 - k1)));
                    code[pc++] = OP_ROL;
                    code[pc++] = k1;
                } else if (op1 == 4) {
                    k1 &= 7;
                    current_val = (uint8_t)((current_val >> k1) | (current_val << (8 - k1)));
                    code[pc++] = OP_ROR;
                    code[pc++] = k1;
                }

                s = s * 1664525 + 1013904223;
                uint8_t op2 = (s >> 16) % 5;
                s = s * 1664525 + 1013904223;
                uint8_t k2 = (s & 0xFF);
                
                if (op2 == 0) {
                    current_val += k2;
                    code[pc++] = OP_ADD;
                    code[pc++] = k2;
                } else if (op2 == 1) {
                    current_val -= k2;
                    code[pc++] = OP_SUB;
                    code[pc++] = k2;
                } else if (op2 == 2) {
                    current_val ^= k2;
                    code[pc++] = OP_XOR;
                    code[pc++] = k2;
                } else if (op2 == 3) {
                    k2 &= 7;
                    current_val = (uint8_t)((current_val << k2) | (current_val >> (8 - k2)));
                    code[pc++] = OP_ROL;
                    code[pc++] = k2;
                } else if (op2 == 4) {
                    k2 &= 7;
                    current_val = (uint8_t)((current_val >> k2) | (current_val << (8 - k2)));
                    code[pc++] = OP_ROR;
                    code[pc++] = k2;
                }

                s = s * 1664525 + 1013904223;
                uint8_t r_op = (s >> 16) % 3;
                if (r_op == 0) {
                    code[pc++] = OP_ADD;
                    code[pc++] = (uint8_t)(target - current_val);
                } else if (r_op == 1) {
                    code[pc++] = OP_XOR;
                    code[pc++] = (uint8_t)(target ^ current_val);
                } else if (r_op == 2) {
                    code[pc++] = OP_SUB;
                    code[pc++] = (uint8_t)(current_val - target);
                }
                
                s = s * 1664525 + 1013904223;
                uint8_t fake_arg = (s & 0xFF);
                code[pc++] = OP_OUT;
                code[pc++] = fake_arg;
                
                current_val = (uint8_t)(target ^ fake_arg);
            }
        }
        code[pc] = OP_END;
        
        Bytecode<Len * 16 + 1> res{};
        for (int i = 0; i < 7; i++) {
            res.opcodes[i] = opcodes.data[i];
        }
        for (size_t i = 0; i <= pc; ++i) {
            res.code[i] = code[i];
        }
        return res;
    }

    template <size_t Len, class L>
    constexpr auto compile_vm_w(L l, uint32_t seed) {
        return compile_vm_w_impl<Len>(l, seed, make_index_sequence<Len>{});
    }

    constexpr uint32_t get_seed(uint32_t off1, uint32_t off2) {
        constexpr const char* time_str = __TIME__;
        constexpr uint32_t time_hash = 
            (time_str[0] * 1) + (time_str[1] * 2) + 
            (time_str[3] * 3) + (time_str[4] * 4) + 
            (time_str[6] * 5) + (time_str[7] * 6);
            
        uint32_t h =  0x811c9dc5 ^ off1;
        h *= 0x1000193; 
        h ^= off2;
        h *= 0x1000193;
        h ^= time_hash;
        h *= 0x1000193;
        return h;
    }
}

class VmStringRunner {
    static constexpr size_t MAX_N = 256;
    alignas(16) char buf[MAX_N];
    size_t len;
public:
    template <size_t CodeLen>
    VMSTR_FORCEINLINE VmStringRunner(const minivm::Bytecode<CodeLen>& bc) {
        uint8_t reg = 0;
        size_t out_idx = 0;
        
        const uint8_t op_add = bc.opcodes[0];
        const uint8_t op_sub = bc.opcodes[1];
        const uint8_t op_xor = bc.opcodes[2];
        const uint8_t op_rol = bc.opcodes[3];
        const uint8_t op_ror = bc.opcodes[4];
        const uint8_t op_out = bc.opcodes[5];
        const uint8_t op_end = bc.opcodes[6];

        uint64_t tsc1 = __rdtsc();

        for (size_t pc = 0; pc < CodeLen; ) {
            uint8_t op = static_cast<volatile const uint8_t&>(bc.code[pc]);
            if (op == op_end) {
                break;
            }
            uint8_t arg = static_cast<volatile const uint8_t&>(bc.code[pc+1]);
            
            if (op == op_add) {
                reg = (uint8_t)(reg + arg);
            } else if (op == op_sub) {
                reg = (uint8_t)(reg - arg);
            } else if (op == op_xor) {
                reg ^= arg;
            } else if (op == op_rol) {
                reg = (uint8_t)((reg << arg) | (reg >> (8 - arg)));
            } else if (op == op_ror) {
                reg = (uint8_t)((reg >> arg) | (reg << (8 - arg)));
            } else if (op == op_out) {
                buf[out_idx++] = (char)(reg); 
                reg = (uint8_t)(reg ^ arg);
            }
            pc += 2;
        }
        len = out_idx > 0 ? (out_idx - 1) : 0; 

        uint64_t tsc2 = __rdtsc();
        if (tsc1 == tsc2 || (tsc2 - tsc1) > 0xFFFFFF) {
            for (size_t i = 0; i < MAX_N; ++i) buf[i] = 0;
            len = 0;
        }
    }
    
    ~VmStringRunner() {
        for (size_t i = 0; i < MAX_N; ++i) {
            static_cast<volatile char*>(buf)[i] = 0;
        }
    }
    VmStringRunner(const VmStringRunner&) = delete;
    VmStringRunner& operator=(const VmStringRunner&) = delete;
    VmStringRunner(VmStringRunner&&) = default;
    VmStringRunner& operator=(VmStringRunner&&) = default;

    VMSTR_FORCEINLINE const char* c_str() const { return buf; }
    VMSTR_FORCEINLINE size_t size() const { return len; }

    VMSTR_FORCEINLINE operator const char*() const { return buf; }
    VMSTR_FORCEINLINE bool operator==(const char* rhs) const { return minivm::m_strcmp(buf, rhs); }
    VMSTR_FORCEINLINE bool operator!=(const char* rhs) const { return !minivm::m_strcmp(buf, rhs); }
};

template <size_t CodeLen>
class VmString {
    minivm::Bytecode<CodeLen> bc;
public:
    VMSTR_FORCEINLINE constexpr VmString(const minivm::Bytecode<CodeLen>& code) : bc(code) {}
    VMSTR_FORCEINLINE VmStringRunner run() const { return VmStringRunner(bc); }
    
    VMSTR_FORCEINLINE operator const char*() const {
        static thread_local char proxy_buffer[16][256];
        static thread_local size_t proxy_idx = 0;
        size_t idx = proxy_idx++ % 16;
        auto runner = run();
        size_t copy_len = runner.size() < 255 ? runner.size() : 255;
        minivm::m_memcpy(proxy_buffer[idx], runner.c_str(), copy_len);
        proxy_buffer[idx][copy_len] = '\0';
        return proxy_buffer[idx];
    }
    
    VMSTR_FORCEINLINE const char* decrypted() const {
        return (const char*)(*this);
    }
    
    VMSTR_FORCEINLINE bool operator==(const char* rhs) const { return run() == rhs; }
    VMSTR_FORCEINLINE bool operator!=(const char* rhs) const { return run() != rhs; }
    VMSTR_FORCEINLINE bool operator<(const char* rhs) const { return minivm::m_strcmp(run().c_str(), rhs) < 0; }
    VMSTR_FORCEINLINE bool operator>(const char* rhs) const { return minivm::m_strcmp(run().c_str(), rhs) > 0; }
    VMSTR_FORCEINLINE bool operator<=(const char* rhs) const { return minivm::m_strcmp(run().c_str(), rhs) <= 0; }
    VMSTR_FORCEINLINE bool operator>=(const char* rhs) const { return minivm::m_strcmp(run().c_str(), rhs) >= 0; }
    
    friend VMSTR_FORCEINLINE bool operator==(const char* lhs, const VmString<CodeLen>& rhs) { return rhs == lhs; }
    friend VMSTR_FORCEINLINE bool operator!=(const char* lhs, const VmString<CodeLen>& rhs) { return rhs != lhs; }
    friend VMSTR_FORCEINLINE bool operator<(const char* lhs, const VmString<CodeLen>& rhs) { return rhs > lhs; }
    friend VMSTR_FORCEINLINE bool operator>(const char* lhs, const VmString<CodeLen>& rhs) { return rhs < lhs; }
    friend VMSTR_FORCEINLINE bool operator<=(const char* lhs, const VmString<CodeLen>& rhs) { return rhs >= lhs; }
    friend VMSTR_FORCEINLINE bool operator>=(const char* lhs, const VmString<CodeLen>& rhs) { return rhs <= lhs; }
};

class VmWStringRunner {
    static constexpr size_t MAX_N = 128;
    alignas(16) wchar_t buf[MAX_N];
    size_t len;
public:
    template <size_t CodeLen>
    VMSTR_FORCEINLINE VmWStringRunner(const minivm::Bytecode<CodeLen>& bc) {
        uint8_t reg = 0;
        size_t out_idx_bytes = 0;

        const uint8_t op_add = bc.opcodes[0];
        const uint8_t op_sub = bc.opcodes[1];
        const uint8_t op_xor = bc.opcodes[2];
        const uint8_t op_rol = bc.opcodes[3];
        const uint8_t op_ror = bc.opcodes[4];
        const uint8_t op_out = bc.opcodes[5];
        const uint8_t op_end = bc.opcodes[6];

        uint64_t tsc1 = __rdtsc();

        for (size_t pc = 0; pc < CodeLen; ) {
            uint8_t op = static_cast<volatile const uint8_t&>(bc.code[pc]);
            if (op == op_end) {
                break;
            }
            uint8_t arg = static_cast<volatile const uint8_t&>(bc.code[pc+1]);
            
            if (op == op_add) {
                reg = (uint8_t)(reg + arg);
            } else if (op == op_sub) {
                reg = (uint8_t)(reg - arg);
            } else if (op == op_xor) {
                reg ^= arg;
            } else if (op == op_rol) {
                reg = (uint8_t)((reg << arg) | (reg >> (8 - arg)));
            } else if (op == op_ror) {
                reg = (uint8_t)((reg >> arg) | (reg << (8 - arg)));
            } else if (op == op_out) {
                ((uint8_t*)buf)[out_idx_bytes++] = reg; 
                reg = (uint8_t)(reg ^ arg); 
            }
            pc += 2;
        }
        len = out_idx_bytes > 0 ? ((out_idx_bytes / 2) - 1) : 0;

        uint64_t tsc2 = __rdtsc();
        if (tsc1 == tsc2 || (tsc2 - tsc1) > 0xFFFFFF) {
            for (size_t i = 0; i < MAX_N; ++i) buf[i] = 0;
            len = 0;
        }
    }
    
    ~VmWStringRunner() {
        for (size_t i = 0; i < MAX_N; ++i) {
            static_cast<volatile wchar_t*>(buf)[i] = 0;
        }
    }
    VmWStringRunner(const VmWStringRunner&) = delete;
    VmWStringRunner& operator=(const VmWStringRunner&) = delete;
    VmWStringRunner(VmWStringRunner&&) = default;
    VmWStringRunner& operator=(VmWStringRunner&&) = default;

    VMSTR_FORCEINLINE const wchar_t* c_str() const { return buf; }
    VMSTR_FORCEINLINE size_t size() const { return len; }

    VMSTR_FORCEINLINE operator const wchar_t*() const { return buf; }
    VMSTR_FORCEINLINE bool operator==(const wchar_t* rhs) const { return minivm::m_wcscmp(buf, rhs); }
    VMSTR_FORCEINLINE bool operator!=(const wchar_t* rhs) const { return !minivm::m_wcscmp(buf, rhs); }
};

template <size_t CodeLen>
class VmWString {
    minivm::Bytecode<CodeLen> bc;
public:
    VMSTR_FORCEINLINE constexpr VmWString(const minivm::Bytecode<CodeLen>& code) : bc(code) {}
    VMSTR_FORCEINLINE VmWStringRunner run() const { return VmWStringRunner(bc); }
    
    VMSTR_FORCEINLINE operator const wchar_t*() const {
        static thread_local wchar_t proxy_buffer[16][256];
        static thread_local size_t proxy_idx = 0;
        size_t idx = proxy_idx++ % 16;
        auto runner = run();
        size_t copy_len = runner.size() < 255 ? runner.size() : 255;
        minivm::m_wmemcpy(proxy_buffer[idx], runner.c_str(), copy_len);
        proxy_buffer[idx][copy_len] = L'\0';
        return proxy_buffer[idx];
    }
    
    VMSTR_FORCEINLINE const wchar_t* decrypted() const {
        return (const wchar_t*)(*this);
    }

    VMSTR_FORCEINLINE bool operator==(const wchar_t* rhs) const { return run() == rhs; }
    VMSTR_FORCEINLINE bool operator!=(const wchar_t* rhs) const { return run() != rhs; }
    VMSTR_FORCEINLINE bool operator<(const wchar_t* rhs) const { return minivm::m_wcscmp(run().c_str(), rhs) < 0; }
    VMSTR_FORCEINLINE bool operator>(const wchar_t* rhs) const { return minivm::m_wcscmp(run().c_str(), rhs) > 0; }
    VMSTR_FORCEINLINE bool operator<=(const wchar_t* rhs) const { return minivm::m_wcscmp(run().c_str(), rhs) <= 0; }
    VMSTR_FORCEINLINE bool operator>=(const wchar_t* rhs) const { return minivm::m_wcscmp(run().c_str(), rhs) >= 0; }
    
    friend VMSTR_FORCEINLINE bool operator==(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs == lhs; }
    friend VMSTR_FORCEINLINE bool operator!=(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs != lhs; }
    friend VMSTR_FORCEINLINE bool operator<(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs > lhs; }
    friend VMSTR_FORCEINLINE bool operator>(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs < lhs; }
    friend VMSTR_FORCEINLINE bool operator<=(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs >= lhs; }
    friend VMSTR_FORCEINLINE bool operator>=(const wchar_t* lhs, const VmWString<CodeLen>& rhs) { return rhs <= lhs; }
};

#define XORSTR(s) []() { \
    constexpr auto __bc = minivm::compile_vm<sizeof(s)>([]() { return s; }, minivm::get_seed(__LINE__, __COUNTER__)); \
    return VmString<sizeof(s) * 8 + 1>(__bc); \
}()

#define XWOSTR(s) []() { \
    constexpr auto __bc_w = minivm::compile_vm_w<sizeof(s)/sizeof(wchar_t)>([]() { return s; }, minivm::get_seed(__LINE__, __COUNTER__)); \
    return VmWString<(sizeof(s)/sizeof(wchar_t)) * 16 + 1>(__bc_w); \
}()

#define RUN_XOR(s) (XORSTR(s).run().c_str())
#define RUN_XWO(s) (XWOSTR(s).run().c_str())

template<typename _Elem, typename _Traits, size_t _Len>
inline auto& operator<<(std::basic_ostream<_Elem, _Traits>& _Ostr, const VmString<_Len>& _Str) {
    return _Ostr << (const char*)_Str;
}

template<typename _Elem, typename _Traits, size_t _Len>
inline auto& operator<<(std::basic_ostream<_Elem, _Traits>& _Ostr, const VmWString<_Len>& _Str) {
    return _Ostr << (const wchar_t*)_Str;
}

template<typename _Traits, typename _Alloc, size_t _Len>
inline std::basic_string<char, _Traits, _Alloc> operator+(const std::basic_string<char, _Traits, _Alloc>& _Lhs, const VmString<_Len>& _Rhs) {
    return _Lhs + (const char*)_Rhs;
}

template<typename _Traits, typename _Alloc, size_t _Len>
inline std::basic_string<char, _Traits, _Alloc> operator+(const VmString<_Len>& _Lhs, const std::basic_string<char, _Traits, _Alloc>& _Rhs) {
    return std::basic_string<char, _Traits, _Alloc>((const char*)_Lhs) + _Rhs;
}

template<typename _Traits, typename _Alloc, size_t _Len>
inline std::basic_string<wchar_t, _Traits, _Alloc> operator+(const std::basic_string<wchar_t, _Traits, _Alloc>& _Lhs, const VmWString<_Len>& _Rhs) {
    return _Lhs + (const wchar_t*)_Rhs;
}

template<typename _Traits, typename _Alloc, size_t _Len>
inline std::basic_string<wchar_t, _Traits, _Alloc> operator+(const VmWString<_Len>& _Lhs, const std::basic_string<wchar_t, _Traits, _Alloc>& _Rhs) {
    return std::basic_string<wchar_t, _Traits, _Alloc>((const wchar_t*)_Lhs) + _Rhs;
}