#ifndef ron_text_hpp
#define ron_text_hpp
#include "op.hpp"

namespace ron {

class TextFrame {

    const std::string data_;

public:

    TextFrame() : data_{} {}
    explicit TextFrame(const std::string& data) : data_{data} {}
    explicit TextFrame(const std::string&& data) : data_{data} {}

    const std::string& data() const { return data_; }

    class Cursor {
        const TextFrame& frame_;
        Op op_;
        int position;

        int cs;
        int atm, dgt, off;
        half_t hlf;

        static constexpr int RON_FULL_STOP = 255;
        static constexpr int SPEC_SIZE = 2; // open RON

    public:
        explicit Cursor(const TextFrame& host) :
            frame_{host}, op_{RON::TEXT_OPEN, TERM::RAW}, 
            cs{0}, atm{0}, dgt{0}, hlf{VALUE}, off{0}, position{-1} 
            {
                Next();
            }
        const Op& op() const { return op_; }
        const TextFrame& frame() const { return frame_; }
        bool Next();
        const std::string& data() const { return frame_.data(); }
    };

    class Builder {
        Op op_;
        std::string data_;

        void WriteInt(int64_t value);
        void WriteFloat(double value);
        void WriteUuid(const Uuid& value);
        void WriteString(const std::string& value);

    public:

        Builder() : data_{}, op_{RON::TEXT_OPEN, TERM::RAW} {}

        void AddOp(const Op& op, const std::string& back_buf);

        void AddAll(Cursor& cur, const std::string& back_buf);

        const TextFrame frame() const { return TextFrame{data_}; }
    };

    Cursor cursor() const {
        return Cursor{*this};
    }

    static constexpr char ESC = '\\';

    static void unescape (std::string& to, const char* buf, fsize_t size);

    static void escape (std::string& to, const char* buf, fsize_t size);

    static inline void escape (std::string& to, const std::string& str) {
        escape(to, str.data(), (fsize_t)str.size());
    }

    static inline void unescape (std::string& to, const std::string& str) {
        unescape(to, str.data(), (fsize_t)str.size());
    }

};

}

#endif