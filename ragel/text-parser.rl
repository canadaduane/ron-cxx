#include "text.hpp"
#include <iostream>

namespace ron {

%% machine RON;
%% write data;


Status TextFrame::Cursor::Next () {

    Atoms& atoms = op_.atoms_;

    int line=1;
    typedef const unsigned char* iterator;

    switch (cs) {
        case RON_error:
            if (off_!=0) {
                return Status::BAD_STATE;
            }
            %% write init;
            break;

        case RON_FULL_STOP:
            cs = RON_error;
            return Status::ENDOFFRAME;

        default:
            break;
    }

    if (data().size()<=off_) {
        cs = RON_error;
        return Status::ENDOFFRAME;
    }

    Slice body{data()};
    iterator pb = (iterator)body.buf_;
    iterator p = pb + off_;
    iterator pe = pb + body.size();
    iterator eof = pe;
    iterator lineb = pb;
    Slice intb{p,0};
    Slice floatb{p,0};
    Slice strb{p,0};
    Slice uuidb{p,0};
    iterator lastintb{0};
    char term{0};
    Slice value, origin;
    char variety, version;

    atoms.clear();
    op_.AddAtom(prev_id_.inc());
    op_.AddAtom(prev_id_);

    //std::cerr<<"starting with "<<cs<<" ["<<p<<"]\n";

    %%{
    include TEXT_FRAME "./text-grammar.rl";
    main := TEXT_FRAME ;
    write exec;
    }%%

    off_ = p-pb;

    if (op_.size()) prev_id_ = op_.id();

    //std::cerr << "ending with [" <<p<<"] state "<<cs<<" "<<op_.size()<<" atoms "<<(pe-p)<<" bytes left, prev_id_ "<<prev_id_.str()<<'\n';

    if (term && cs!=RON_error) {
        op_.term_ = TERM(strchr(TERM_PUNCT, term) - TERM_PUNCT); // FIXME gen a fn
        return Status::OK;
    } else {
        cs = RON_error;
        char msg[64];
        size_t msglen = sprintf(msg, "syntax error at line %d col %d (offset %d)", line, (int)(p-lineb), (int)(p-pb));
        return Status::BAD_STATE.comment(std::string{msg, msglen});
    }

}

}
