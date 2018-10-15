#ifndef CPP_SLICE_H
#define CPP_SLICE_H
#include <cstdint>
#include <utility>
#include <cassert>
#include <cstring>
#include <functional>

namespace ron {

typedef uint32_t fsize_t;
typedef std::pair<fsize_t, fsize_t> frange_t;

struct slice_t {
    const char *buf_;
    fsize_t size_;

    slice_t (const char* buf, fsize_t size) : buf_{buf}, size_{size} {}
    slice_t () : buf_{nullptr}, size_{0} {}
    slice_t (const slice_t& orig) : buf_{orig.buf_}, size_{orig.size_} {}
    slice_t (const std::string& str, const frange_t& range) :
        buf_{str.data()+range.first}, size_{range.second} {}

    inline char operator[](fsize_t idx) const {
        assert(idx < size_);
        return buf_[idx];
    }

    inline fsize_t size() const {
        return size_;
    }

    bool operator == (const slice_t b) const {
        return size()==b.size() && strncmp(buf_, b.buf_, size())==0;
    }

    size_t hash () const {
        static constexpr int shift = sizeof(size_t)==8 ? 3 : 2;
        static constexpr auto sz_hash_fn = std::hash<size_t>{};
        static constexpr auto char_hash_fn = std::hash<char>{};
        size_t ret = 0;
        int c = size_ >> shift;
        auto* szbuf = (size_t*) buf_;
        for(int i=0; i<c; i++) {
            ret ^= sz_hash_fn(szbuf[i]);
        }
        for(int i=c<<shift; i<size_; i++) {
            ret ^= char_hash_fn(buf_[i]);
        }
        return ret;
    }
};

}

namespace std {

    template <>
    struct hash<ron::slice_t> {
        size_t operator()(ron::slice_t const& s) const noexcept {
            return s.hash();
        }
    };

}
#endif //CPP_SLICE_H