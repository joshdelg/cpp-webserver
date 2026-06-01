#include <string>

#include "parser.h"

std::ostream& operator<<(std::ostream& os, const HTTPRequest& req) {
    os << "HTTP Request" << std::endl;
    os << "Method: " << req.method << " Target: " << req.url << " Version: " << req.version << std::endl;
}

Parser::Parser(std::string raw_request) : _parser_status(Ok), _parser_state(Start), _raw_request(raw_request), _cursor(0) {}

HTTPRequest Parser::parse_request() {
    // Loop Invariant: Always at least 1 in bounds character to process
    while(cursor_in_bounds() && get_parser_status() == Ok) {
        switch (_parser_state) {
            case Start: {
                std::string ignore = consume_while(is_whitespace);

                if(!set_status_waiting_if_oob()) set_parser_state(MethodToken);
                
                break;
            }
            case MethodToken: {
                std::string token = consume_while(is_tchar);
                _req.method.append(token);

                if(!set_status_waiting_if_oob()) set_parser_state(MethodSpacePath);
                break;
            }
            case MethodSpacePath: {
                bool success = consume_one(is_space);

                if(!success) {
                    set_parser_status(Error);
                    break;
                }

                set_parser_state(PathSlash);
                set_status_waiting_if_oob();

                break;
            }
            case PathSlash: {
                bool success = consume_one(is_slash);

                if(!success) {
                    set_parser_status(Error);
                    break;
                }

                _req.url.append("/");
                set_parser_state(PathLetter);
                set_status_waiting_if_oob();
                
                break;
            }
            case PathLetter: {
                std::string token = consume_while(is_pchar);
                _req.url.append(token);

                if(set_status_waiting_if_oob()) break;

                // We're done -- decide where to transition next
                if(current_char_is('/')) {
                    set_parser_state(PathSlash);
                } else if(current_char_is(' ')) {
                    set_parser_state(PathSpaceVersion);
                } else { // TODO: Handle Query Params
                    set_parser_status(Error);
                }

                break;
            }
            case PathSpaceVersion: {
                bool success = consume_one(is_space);

                if(!success) {
                    set_parser_status(Error);
                    break;
                }

                set_parser_state(VersionH);
                set_status_waiting_if_oob();
                break;
            }
            case VersionH: {
                bool success = consume_one('H');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("H");
                set_parser_state(VersionT1);
                set_status_waiting_if_oob();
                break;
            }
            case VersionT1: {
                bool success = consume_one('T');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("T");
                set_parser_state(VersionT2);
                set_status_waiting_if_oob();
                break;
            }
            case VersionT2: {
                bool success = consume_one('T');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("T");
                set_parser_state(VersionP);
                set_status_waiting_if_oob();
                break;
            }
            case VersionP: {
                bool success = consume_one('P');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("P");
                set_parser_state(VersionSlash);
                set_status_waiting_if_oob();
                break;
            }
            case VersionSlash: {
                bool success = consume_one('/');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("/");
                set_parser_state(VersionDigit1);
                set_status_waiting_if_oob();
                break;
            }
            // TODO: More versions
            case VersionDigit1: {
                bool success = consume_one('1');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("1");
                set_parser_state(VersionDot);
                set_status_waiting_if_oob();
                break;
            }
            case VersionDot: {
                bool success = consume_one('.');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append(".");
                set_parser_state(VersionDigit2);
                set_status_waiting_if_oob();
                break;
            }
            case VersionDigit2: {
                bool success = consume_one('1');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                _req.version.append("1");
                set_parser_state(RequestLineCR);
                set_status_waiting_if_oob();
                break;
            }
            // TODO: Add optional CR
            case RequestLineCR: {
                bool success = consume_one('\r');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }

                set_parser_state(RequestLineLF);
                set_status_waiting_if_oob();
                break;
            }
            case RequestLineLF: {
                bool success = consume_one('\n');

                if(!success) {
                    set_parser_status(Error);
                    break;
                }
                
                set_parser_state(RequestLineCR);
                set_status_waiting_if_oob();
                break;
            }
            case FieldName: {
                // TODO: consume field name token
                break;
            }
            case FieldColon: {
                // TODO: consume ':' after field name
                break;
            }
            case ColonOWSFieldValue: {
                // TODO: consume optional whitespace after ':'
                break;
            }
            case FieldValue: {
                // TODO: consume field value
                break;
            }
            case FieldValueOWS: {
                // TODO: consume optional whitespace before field line end
                break;
            }
            case FieldLineEnd: {
                // TODO: consume CRLF after field line
                break;
            }
            case FieldLineEndEndMessageBody: {
                // TODO: consume CRLF between headers and message body
                break;
            }
            case MessageBody: {
                // TODO: consume message body
                break;
            }
            case Finished: {
                break;
            }

            // TEMP: Advance to the end so we can see if the earlier stuff parsed
            default: {
                _cursor++;
                break;
            }
        }
    }

    if(get_parser_status() != Error) set_parser_state(Finished);

    return _req;
}
