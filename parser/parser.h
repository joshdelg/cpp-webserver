#include <cctype>
#include <string>
#include <unordered_map>
#include <iostream>

// State machine brainstorming:
// define transition map RequestLineMethd

enum ParserStatus {
    Ok,
    WaitingForInput,
    Error
};

/**
 * Even though the HTTP spec is a CFG, its flat. So, we can just parse using a state machine.
 * For XSpaceY or XOWSY, the state is the Space or OWS (Optional White Space) between X and Y
 */
enum ParserState {
    Start,
    // request-line
    // method
    MethodToken,
    MethodSpacePath,
    // target
    PathSlash,
    PathLetter, // TODO: Handle Query Params
    PathSpaceVersion,
    // HTTP version
    VersionH,
    VersionT1,
    VersionT2,
    VersionP,
    VersionSlash,
    VersionDigit1,
    VersionDot,
    VersionDigit2,
    RequestLineCR,
    RequestLineLF,
    // field line
    FieldName,
    FieldColon,
    ColonOWSFieldValue,
    FieldValue,
    FieldValueOWS,
    FieldLineEndCR,
    FieldLineEndLF,
    FieldLineNextOrEnd,
    // CRLF between field line and message body
    FieldLineEndEndMessageBodyCR,
    FieldLineEndEndMessageBodyLF,
    // message body
    MessageBody,

    Finished,
};

struct HTTPRequest {
    std::string method;
    std::string url;
    std::unordered_map<std::string, std::string> queries;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::optional<std::string> body;

    friend std::ostream& operator<<(std::ostream& os, const HTTPRequest& req);
};

std::ostream& operator<<(std::ostream& os, const HTTPRequest& req);

/** 
 * Class owns the life cycle of a single raw HTTP string that needs to be parsed.
*/
class Parser {
private:
    // Status is about the health of the object, rather than where in the parser we are
    ParserStatus _parser_status;
    // State is about specifically where in the HTTP CFG we are
    ParserState _parser_state;
    std::string _raw_request;

    HTTPRequest _req;
    
    int _cursor;

    // Temporary accumulators for fields that are "committed"
    std::string _current_field_name;
    std::string _current_field_value;

    void set_parser_status(ParserStatus new_status) { _parser_status = new_status; }
    ParserStatus get_parser_status() { return _parser_status; }

    void set_parser_state(ParserState new_state) { _parser_state = new_state; }
    ParserState get_parser_state() { return _parser_state; }

    bool cursor_in_bounds() { return _cursor < _raw_request.size(); }

    void warn_if_out_of_bounds() { 
        if(!cursor_in_bounds()) {
            std::cerr << "[Parser] WARNING: Consume invoked with violated bounds predicate" << std::endl;
        }
    }

    bool set_status_waiting_if_oob() {
        if(!cursor_in_bounds()) {
            set_parser_status(WaitingForInput);
            return true;
        }

        return false;
    }

    void error_or_set_state(bool success, ParserState state) {
        if(!success) {
            set_parser_status(Error);
        } else {
            set_parser_state(state);
        }
    }

    // TODO: Stop allocating new strings

    /** Helpers
     * Precondition: At least 1 byte is available
     * Only mutate cursor
     * 
    */
    template <typename Pred>
    std::string consume_while(Pred pred);

    template <typename Pred>
    bool consume_one(Pred pred);
    bool consume_one(const char c);

    template <typename Pred>
    bool current_char_is(Pred pred);
    bool current_char_is(const char c);

public:
    Parser(std::string raw_request);
    HTTPRequest parse_request();
};

template <typename Pred>
bool Parser::consume_one(Pred pred) {
    warn_if_out_of_bounds();

    return pred(_raw_request[_cursor++]);
}

// Since we're defining in the header and not a template func: must be inline
inline bool Parser::consume_one(const char c) {
    warn_if_out_of_bounds();

    return _raw_request[_cursor++] == c;
}

template <typename Pred>
std::string Parser::consume_while(Pred pred) {
    warn_if_out_of_bounds();

    std::string result = "";
    while(cursor_in_bounds() && pred(_raw_request[_cursor])) { 
        result += _raw_request[_cursor++];
    }

    return result;
}

template <typename Pred>
bool Parser::current_char_is(Pred pred) {
    return pred(_raw_request[_cursor]);
}

inline bool Parser::current_char_is(const char c) {
    return _raw_request[_cursor] == c;
}

/** Generic helpers */

// TODO: More generic kinds of whitespace

/**
 * Recall:
 * - static on free function = internal linkage. only visible to this TU
 * - inline on free function = bypass ODR rule. technically redundant
*/
static inline bool is_whitespace(const char c) {
    return c == ' ';
}

static inline bool is_space(const char c) {
    return c == ' ';
}

static inline bool is_slash(const char c) {
    return c == '/';
}

// TODO: Parse %xx URL encoded data
static inline bool is_unreserved(const char c) {
    return std::isalnum(c) || c == '-' || c == '.' || '_' || '~';
}

static inline bool is_subdelim(const char c) {
    return c == '!' || c == '$' || c == '&' || c == '\'' ||
           c == '(' || c == ')' || c == '*' || c == '+' ||
           c == ',' || c == ';' || c == '=';
            
}

static inline bool is_tchar(const char c) {
    return is_unreserved(c) ||
           c == '!' || c == '#' || c == '$' || c == '%' || c == '&' ||
           c == '\'' || c == '*' || c == '+' ||
           c == '^' || c == '`' || c == '|';
}

static inline bool is_pchar(const char c) {
    return is_unreserved(c) || is_subdelim(c) || ':' || '@';
}

// VCHAR = %x21-7E (any visible US-ASCII character)
static inline bool is_vchar(const char c) {
    return c >= 0x21 && c <= 0x7E;
}