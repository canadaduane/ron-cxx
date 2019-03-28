#ifndef CPP_KEY_HPP
#define CPP_KEY_HPP

#include "../ron/form.hpp"
#include "../ron/uuid.hpp"

namespace ron {

/**
 *  A 128-bit key has the UUID and the RDT/reducer id.
 *  UUID origin flag bits are always RON_UUID/TIME, hence reused for the RDT
 *  id. The bit layout is: origin(60) variety(4) value(60) reducer(4)
 *  * yarns, scans
 *  * unknown-type fetch
 *  * prefix compression: value tail bits, reducer bits
 *  * same cf for chains(yarns), objects
 *  * origin-locality
 */
struct Key {
    uint64pair bits;

    static constexpr size_t SIZE = sizeof(bits);

    Key() = default;

    Key(const Key& b) = default;

    explicit Key(uint64pair b) : bits{b} {}  // NOLINT

    Key(Uuid id, FORM form) {
        bits.second = id.origin()._64 & Word::PAYLOAD_BITS;
        bits.first = id.value()._64 & Word::PAYLOAD_BITS;
        bits.second |= bits.first << 60;
        bits.first >>= 4;
        bits.first |= uint64_t(form) << 56;
    }

    Key(Uuid id, Uuid form) : Key{id, uuid2form(form)} {}

    inline uint64pair be() const {
        return uint64pair{htobe64(bits.first), htobe64(bits.second)};
    }

    static Key be(Slice data) {
        assert(data.size() == sizeof(bits));
        auto b = *reinterpret_cast<const uint64pair*>(data.data());
        return Key{uint64pair{be64toh(b.first), be64toh(b.second)}};
    }

    inline bool operator<(const Key& b) const { return bits < b.bits; }

    inline bool operator==(const Key& b) const { return bits == b.bits; }
    inline bool operator!=(const Key& b) const { return bits != b.bits; }

    inline FORM form() const { return FORM(bits.first >> 56); }

    inline Uuid id(enum UUID uuid_flags = TIME) const {
        return Uuid{
            ((bits.first << 4) & Word::PAYLOAD_BITS) | (bits.second >> 60),
            (bits.second & Word::PAYLOAD_BITS) | (uint64_t(uuid_flags) << 60)};
    }

    String str() const {
        return '*' + form2uuid(form()).str() + '#' + id().str();
    }
};

static Key END_KEY{FATAL, ERROR_NO_FORM};

}  // namespace ron

#endif  // CPP_KEY_HPP
