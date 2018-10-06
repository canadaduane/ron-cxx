#ifndef ron_frame_hpp
#define ron_frame_hpp
#include <vector>
#include <cassert>
#include "uuid.hpp"


namespace ron {

typedef std::vector<Atom> Atoms;

// An op in the nominal RON (open) coding.
// That's the internal format
struct Op {
    Atoms atoms_;
    TERM term_;
    RON coding_;
    Op(const Op& op) : atoms_{op.atoms_}, term_{op.term_} {}
    explicit Op(RON coding, TERM term) : 
    atoms_{}, coding_{coding}, term_{term} {
        atoms_.reserve((coding&RON::CLOSED)?4:6);
    }
    explicit Op(RON coding) : Op{coding, TERM::RAW} {}
    Op(const Uuid& id, const Uuid& ref) : Op{RON::NOMINAL_OPEN, TERM::HEADER} {
        atoms_.push_back(id);
        atoms_.push_back(ref);
    }
    Op(const Uuid& id, const Uuid& ref, const Value& v0) : Op{id, ref} {
        atoms_.push_back(v0);
        term_ = TERM::RAW;
    }
    Op(const Uuid& id, const Uuid& ref, const Value& v0, const Value& v1) : Op{id, ref, v0} {
        atoms_.push_back(v1);
    }
    const Uuid& id() const {
        return (Uuid&)atoms_[0];
    }
    const Uuid& ref() const {
        return (Uuid&)atoms_[1];
    }
    const Value& value(fsize_t idx) const {
        assert(idx<atoms_.size());
        return (Value&)atoms_[idx];
    }
    inline double value_float(fsize_t idx) const {
        return value(idx).float_value();
    } 
    inline int64_t value_int(fsize_t idx) const {
        return value(idx).int_value();
    }
    inline Uuid value_uuid(fsize_t idx) const {
        return reinterpret_cast<const Uuid&>(atoms_[idx]);
    }
    inline std::string value_string(fsize_t idx, const std::string& data) const {
        frange_t rng = ((Value&)atoms_[idx]).range();
        std::string ret = data.substr(rng.first, rng.second);
        return ret;
    }
    fsize_t size() const {
        return (fsize_t) atoms_.size();
    }
    void resize (fsize_t new_size = 2) { atoms_.resize(new_size); }
    Atoms& data() { return atoms_; }
    inline RON coding() const { return coding_; }
    // a nominal RON (closed) op, with a descriptor
    // mostly for outer interfaces / bindings
    Atom* closed(Uuid rdt, Uuid object) const;
    void SetEv(const Uuid& ev);
    void SetRef(const Uuid& ref);
    void AddValue(const Atom& value);
    void AddFloat(double value);
    void AddString(fsize_t from, fsize_t till);

};

typedef int Status; // FIXME


} // namespace ron

#endif