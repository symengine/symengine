// Automatically generated

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

// $insert class_ih
#include "scanner.ih"

// $insert namespace-open
namespace SymEngine
{

// s_ranges__: use (unsigned) characters as index to obtain
//           that character's range-number.
//           The range for EOF is defined in a constant in the
//           class header file
size_t const ScannerBase::s_ranges__[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  5,  5,  5,  5,  5,
    6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 17, 17, 18, 19, 20, 21, 22, 23, 23, 23, 23, 24, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 27,
    28, 29, 30, 30, 30, 30, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 34, 35, 36, 37, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
    38, 38, 38, 38, 38, 38, 38, 38, 38,
};

// s_dfa__ contains the rows of *all* DFAs ordered by start state.  The
// enum class StartCondition__ is defined in the baseclass header
// StartCondition__::INITIAL is always 0.  Each entry defines the row to
// transit to if the column's character range was sensed. Row numbers are
// relative to the used DFA, and d_dfaBase__ is set to the first row of
// the subset to use.  The row's final two values are respectively the
// rule that may be matched at this state, and the rule's FINAL flag. If
// the final value equals FINAL (= 1) then, if there's no continuation,
// the rule is matched. If the BOL flag (8) is also set (so FINAL + BOL (=
// 9) is set) then the rule only matches when d_atBOL is also true.
int const ScannerBase::s_dfa__[][42] = {
    // INITIAL
    {-1, 1, 2,  3,  4,  3,  5,  3, 5,  5,  6,  5,  5,  5,
     7,  5, 8,  3,  9,  10, 11, 3, 12, 13, 13, 13, 3,  5,
     13, 3, 13, 13, 13, 3,  5,  3, 5,  3,  13, -1, -1, -1}, // 0
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 9,  -1}, // 1
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8,  -1}, // 2
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1}, // 3
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, -1}, // 4
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  -1}, // 5
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  -1}, // 6
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1}, // 7
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     15, -1, 8,  -1, -1, -1, -1, -1, -1, 16, 17, 16, -1, -1,
     16, -1, 16, 17, 16, -1, -1, -1, -1, -1, 16, -1, 7,  -1}, // 8
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, 18, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  -1}, // 9
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, 19, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 11, -1}, // 10
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, 20, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  -1}, // 11
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1,  -1}, // 12
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 13, -1, -1, -1, -1, -1, -1, 13, 13, 13, -1, -1,
     13, -1, 13, 13, 13, -1, -1, -1, -1, -1, 13, -1, 5,  -1}, // 13
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 14, -1, -1, -1, -1, -1, -1, 16, 17, 16, -1, -1,
     16, -1, 16, 17, 16, -1, -1, -1, -1, -1, 16, -1, 7,  -1}, // 14
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 14, -1, -1, -1, -1, -1, -1, 16, 16, 16, -1, -1,
     16, -1, 16, 16, 16, -1, -1, -1, -1, -1, 16, -1, 7,  -1}, // 15
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 16, -1, -1, -1, -1, -1, -1, 16, 16, 16, -1, -1,
     16, -1, 16, 16, 16, -1, -1, -1, -1, -1, 16, -1, 6,  -1}, // 16
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 21, -1, 21,
     -1, -1, 22, -1, -1, -1, -1, -1, -1, 16, 16, 16, -1, -1,
     16, -1, 16, 16, 16, -1, -1, -1, -1, -1, 16, -1, 6,  -1}, // 17
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 2,  -1}, // 18
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 4,  -1}, // 19
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 3,  -1}, // 20
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 21
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 22, -1, -1, -1, -1, -1, -1, 16, 16, 16, -1, -1,
     16, -1, 16, 16, 16, -1, -1, -1, -1, -1, 16, -1, 6,  -1}, // 22
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, 23, -1, -1, -1, -1, -1, -1, 16, 16, 16, -1, -1,
     16, -1, 16, 16, 16, -1, -1, -1, -1, -1, 16, -1, 7,  -1}, // 23
};

int const (*ScannerBase::s_dfaBase__[])[42] = {
    s_dfa__ + 0,
};

size_t ScannerBase::s_istreamNr = 0;

// $insert inputImplementation
ScannerBase::Input::Input() : d_in(0), d_lineNr(1)
{
}

ScannerBase::Input::Input(std::istream *iStream, size_t lineNr)
    : d_in(iStream), d_lineNr(lineNr)
{
}

size_t ScannerBase::Input::get()
{
    switch (size_t ch = next()) // get the next input char
    {
        case '\n':
            ++d_lineNr;
        // FALLING THROUGH

        default:
            return ch;
    }
}

size_t ScannerBase::Input::next()
{
    size_t ch;

    if (d_deque.empty()) // deque empty: next char fm d_in
    {
        if (d_in == 0)
            return AT_EOF;
        ch = d_in->get();
        return *d_in ? ch : static_cast<size_t>(AT_EOF);
    }

    ch = d_deque.front();
    d_deque.pop_front();

    return ch;
}

void ScannerBase::Input::reRead(size_t ch)
{
    if (ch < 0x100) {
        if (ch == '\n')
            --d_lineNr;
        d_deque.push_front(ch);
    }
}

void ScannerBase::Input::reRead(std::string const &str, size_t fm)
{
    for (size_t idx = str.size(); idx-- > fm;)
        reRead(str[idx]);
}

ScannerBase::ScannerBase(std::istream &in, std::ostream &out)
    : d_filename("-"), d_out(new std::ostream(out.rdbuf())),
      // $insert interactiveInit
      d_in(0), d_input(new std::istream(in.rdbuf())), d_dfaBase__(s_dfa__)
{
}

void ScannerBase::switchStream__(std::istream &in, size_t lineNr)
{
    d_input.close();
    d_input = Input(new std::istream(in.rdbuf()), lineNr);
}

ScannerBase::ScannerBase(std::string const &infilename,
                         std::string const &outfilename)
    : d_filename(infilename),
      d_out(outfilename == "-"
                ? new std::ostream(std::cout.rdbuf())
                : outfilename == "" ? new std::ostream(std::cerr.rdbuf())
                                    : new std::ofstream(outfilename)),
      d_input(new std::ifstream(infilename)), d_dfaBase__(s_dfa__)
{
}

void ScannerBase::switchStreams(std::istream &in, std::ostream &out)
{
    switchStream__(in, 1);
    switchOstream(out);
}

void ScannerBase::switchOstream(std::ostream &out)
{
    *d_out << std::flush;
    d_out.reset(new std::ostream(out.rdbuf()));
}

// $insert debugFunctions
void ScannerBase::setDebug(bool onOff)
{
}

bool ScannerBase::debug() const
{
    return false;
}

void ScannerBase::redo(size_t nChars)
{
    size_t from = nChars >= length() ? 0 : length() - nChars;
    d_input.reRead(d_matched, from);
    d_matched.resize(from);
}

void ScannerBase::switchOstream(std::string const &outfilename)
{
    *d_out << std::flush;
    d_out.reset(outfilename == "-"
                    ? new std::ostream(std::cout.rdbuf())
                    : outfilename == "" ? new std::ostream(std::cerr.rdbuf())
                                        : new std::ofstream(outfilename));
}

void ScannerBase::switchIstream(std::string const &infilename)
{
    d_input.close();
    d_filename = infilename;
    d_input = Input(new std::ifstream(infilename));
    d_atBOL = true;
}

void ScannerBase::switchStreams(std::string const &infilename,
                                std::string const &outfilename)
{
    switchOstream(outfilename);
    switchIstream(infilename);
}

void ScannerBase::pushStream(std::istream &istr)
{
    std::istream *streamPtr = new std::istream(istr.rdbuf());
    p_pushStream("(istream)", streamPtr);
}

void ScannerBase::pushStream(std::string const &name)
{
    std::istream *streamPtr = new std::ifstream(name);
    if (!*streamPtr) {
        delete streamPtr;
        throw std::runtime_error("Cannot read " + name);
    }
    p_pushStream(name, streamPtr);
}

void ScannerBase::p_pushStream(std::string const &name, std::istream *streamPtr)
{
    if (d_streamStack.size() == s_maxSizeofStreamStack__) {
        delete streamPtr;
        throw std::length_error("Max stream stack size exceeded");
    }

    d_streamStack.push_back(StreamStruct{d_filename, d_input});
    d_filename = name;
    d_input = Input(streamPtr);
    d_atBOL = true;
}

bool ScannerBase::popStream()
{
    d_input.close();

    if (d_streamStack.empty())
        return false;

    StreamStruct &top = d_streamStack.back();

    d_input = top.pushedInput;
    d_filename = top.pushedName;
    d_streamStack.pop_back();

    return true;
}

// See the manual's section `Run-time operations' section for an explanation
// of this member.
ScannerBase::ActionType__ ScannerBase::actionType__(size_t range)
{
    d_nextState = d_dfaBase__[d_state][range];

    if (d_nextState != -1) // transition is possible
        return ActionType__::CONTINUE;

    if (knownFinalState()) // FINAL state reached
        return ActionType__::MATCH;

    if (d_matched.size())
        return ActionType__::ECHO_FIRST; // no match, echo the 1st char

    return range != s_rangeOfEOF__ ? ActionType__::ECHO_CH
                                   : ActionType__::RETURN;
}

void ScannerBase::accept(size_t nChars) // old name: less
{
    if (nChars < d_matched.size()) {
        d_input.reRead(d_matched, nChars);
        d_matched.resize(nChars);
    }
}

void ScannerBase::setMatchedSize(size_t length)
{
    d_input.reRead(d_matched, length); // reread the tail section
    d_matched.resize(length);          // return what's left
}

// At this point a rule has been matched.  The next character is not part of
// the matched rule and is sent back to the input.  The final match length
// is determined, the index of the matched rule is determined, and then
// d_atBOL is updated. Finally the rule's index is returned.
// The numbers behind the finalPtr assignments are explained in the
// manual's `Run-time operations' section.
size_t ScannerBase::matched__(size_t ch)
{
    d_input.reRead(ch);

    FinalData *finalPtr;

    if (not d_atBOL)             // not at BOL
        finalPtr = &d_final.std; // then use the std rule (3, 4)

    // at BOL
    else if (not available(d_final.std.rule)) // only a BOL rule avail.
        finalPtr = &d_final.bol;              // use the BOL rule (6)

    else if (not available(d_final.bol.rule)) // only a std rule is avail.
        finalPtr = &d_final.std;              // use the std rule (7)

    else if (                 // Both are available (8)
        d_final.bol.length != // check lengths of matched texts
        d_final.std.length    // unequal lengths, use the rule
        )                     // having the longer match length
        finalPtr = d_final.bol.length > d_final.std.length ? &d_final.bol
                                                           : &d_final.std;

    else // lengths are equal: use 1st rule
        finalPtr
            = d_final.bol.rule < d_final.std.rule ? &d_final.bol : &d_final.std;

    setMatchedSize(finalPtr->length);

    d_atBOL = d_matched.back() == '\n';

    return finalPtr->rule;
}

size_t ScannerBase::getRange__(int ch) // using int to prevent casts
{
    return ch == AT_EOF ? as<size_t>(s_rangeOfEOF__) : s_ranges__[ch];
}

// At this point d_nextState contains the next state and continuation is
// possible. The just read char. is appended to d_match
void ScannerBase::continue__(int ch)
{
    d_state = d_nextState;

    if (ch != AT_EOF)
        d_matched += ch;
}

void ScannerBase::echoCh__(size_t ch)
{
    *d_out << as<char>(ch);
    d_atBOL = ch == '\n';
}

// At this point there is no continuation. The last character is
// pushed back into the input stream as well as all but the first char. in
// the buffer. The first char. in the buffer is echoed to stderr.
// If there isn't any 1st char yet then the current char doesn't fit any
// rules and that char is then echoed
void ScannerBase::echoFirst__(size_t ch)
{
    d_input.reRead(ch);
    d_input.reRead(d_matched, 1);
    echoCh__(d_matched[0]);
}

// Update the rules associated with the current state, do this separately
// for BOL and std rules.
// If a rule was set, update the rule index and the current d_matched
// length.
void ScannerBase::updateFinals__()
{
    size_t len = d_matched.size();

    int const *rf = d_dfaBase__[d_state] + s_finIdx__;

    if (rf[0] != -1) // update to the latest std rule
    {
        d_final.std = FinalData{as<size_t>(rf[0]), len};
    }

    if (rf[1] != -1) // update to the latest bol rule
    {
        d_final.bol = FinalData{as<size_t>(rf[1]), len};
    }
}

void ScannerBase::reset__()
{
    d_final = Final{FinalData{s_unavailable, 0}, FinalData{s_unavailable, 0}};

    d_state = 0;
    d_return = true;

    if (!d_more)
        d_matched.clear();

    d_more = false;
}

int Scanner::executeAction__(size_t ruleIdx) try {
    switch (ruleIdx) {
        // $insert actions
        case 0: {
#line 18 "scanner.ll"
            {
                return matched()[0];
            }
        } break;
        case 1: {
#line 21 "scanner.ll"
            {
                return Parser::POW;
            }
        } break;
        case 2: {
#line 24 "scanner.ll"
            {
                return Parser::LE;
            }
        } break;
        case 3: {
#line 27 "scanner.ll"
            {
                return Parser::GE;
            }
        } break;
        case 4: {
#line 30 "scanner.ll"
            {
                return Parser::EQ;
            }
        } break;
        case 5: {
#line 33 "scanner.ll"
            {
                *dval = std::string(matched());
                return Parser::IDENTIFIER;
            }
        } break;
        case 6: {
#line 37 "scanner.ll"
            {
                *dval = std::string(matched());
                return Parser::IMPLICIT_MUL;
            }
        } break;
        case 7: {
#line 41 "scanner.ll"
            {
                *dval = std::string(matched());
                return Parser::NUMERIC;
            }
        } break;
        case 8:
        case 9:
        case 10: {
#line 47 "scanner.ll"
            {
            }
        } break;
        case 11: {
#line 49 "scanner.ll"
            {
            }
        } break;
    }
    noReturn__();
    return 0;
} catch (Leave__ value) {
    return static_cast<int>(value);
}

int Scanner::lex__()
{
    reset__();
    preCode();

    while (true) {
        size_t ch = get__();           // fetch next char
        size_t range = getRange__(ch); // determine the range

        updateFinals__(); // update the state's Final info

        switch (actionType__(range)) // determine the action
        {
            case ActionType__::CONTINUE:
                continue__(ch);
                continue;

            case ActionType__::MATCH: {
                d_token__ = executeAction__(matched__(ch));
                if (return__()) {
                    print();
                    postCode(PostEnum__::RETURN);
                    return d_token__;
                }
                break;
            }

            case ActionType__::ECHO_FIRST:
                echoFirst__(ch);
                break;

            case ActionType__::ECHO_CH:
                echoCh__(ch);
                break;

            case ActionType__::RETURN:
                if (!popStream()) {
                    postCode(PostEnum__::END);
                    return 0;
                }
                postCode(PostEnum__::POP);
                continue;
        } // switch

        postCode(PostEnum__::WIP);

        reset__();
        preCode();
    } // while
}

void ScannerBase::print__() const
{
}

// $insert namespace-close
}
