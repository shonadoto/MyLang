#include "LecsicalAnalyzer.h"

class LecsicalAnalyzer::StateMachine {
private:
    std::vector<StateMachine*> links_;
    LecsicalEnum terminal_;
public:
    StateMachine(LecsicalEnum terminal) :terminal_(terminal), links_(std::vector<StateMachine*>(UCHAR_MAX, NULL)) {}

    void setLink(std::string str, StateMachine* node) {
        for (auto& c : str) {
            links_[c] = node;
        }
    }

    StateMachine* getLink(char c) {
        return links_[c];
    }

    LecsicalEnum getTerminal() { return terminal_; }
};

class LecsicalAnalyzer::LecsicalError {
private:
    int line_number_, char_number_;
    std::string file_;
public:
    LecsicalError(std::string file, int line_number, int char_number) : file_(file), line_number_(line_number), char_number_(char_number) {}
    void sendError() {
        std::cout << "File: " << file_ << '\n';
        std::cout << "Line: " << line_number_ << ", character: " << char_number_ << ".\n";
        std::cout << "Lecsical error: Unexpected character.\n\n";
    }
};

LecsicalAnalyzer::LecsicalAnalyzer(Path* source) {
    file_ = source;
    program_input_ = std::ifstream(file_->fullPath());
    if (!program_input_.is_open()) {
        throw "File error: \"" + file_->fullPath() + "\" do not exist.\n";
    }
    createReserved();
    createStateMachine();
}

void LecsicalAnalyzer::createReserved() {
    for (auto s : {
        "bool",
        "char",
        "int",
        "float",
        "string",
        "range",
        "array",
        "void",

        "return",
        "break",
        "continue",

        "if",
        "while",
        "do",
        "for",
        "when",

        "import",

        "in",
        "it",
        "print",

        "input"
        }) {
        reserved.insert(s);
    }
    return;
}

void LecsicalAnalyzer::createStateMachine() {

    int number_of_states = 40;

    machine_.resize(number_of_states);
    terminals_.resize(number_of_states);

    struct Rule
    {
        int from, to;
        std::string symbols;
    };

    std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",
        digits = "0123456789",
        shielded = "abfnrtv\"\'\\",
        all = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789 !@#$%^&*()-+/<>?|~`{}[]:;.,";


    // Rules for changing states
    std::vector<Rule> rules = {
        // Name
        {0, 1, letters},
        {1, 1, letters + digits},

        // Int or Float
        {0, 2, digits},
        {2, 2, digits}, // int
        {2, 3, "."}, // int_point
        {3, 37, digits}, // float
        {37, 37, digits}, // float
        {37, 4, "Ee"}, // none
        {4, 39, "+-"}, // none
        {39, 5, digits}, // float
        {4, 5, digits}, // float
        {5, 5, digits}, // float

        // String
        {0, 6, "\""},
        {6, 6, all},
        {6, 7, "\\"},
        {7, 6, shielded},
        {6, 8, "\""},

        // Char
        {0, 9, "\'"},
        {9, 10, all},
        {9, 12, "\\"},
        {12, 10, shielded},
        {10, 11, "\'"},

        // Comma
        {0, 13, ","},

        // Colon
        {0, 14, ":"},

        // Semi-Colon
        {0, 15, ";"},

        // Square braces
        {0, 16, "[]"},

        // Round braces
        {0, 17, "()"},

        // Curly braces
        {0, 18, "{}"},

        // Space
        {0, 19, " \t\n"},

        // Operations
        {0, 20, "*"},
        {20, 21, "*"},
        {20, 22, "="},
        {21, 22, "="},
        {0, 23, "+-%^!="},
        {23, 22, "="},
        {0, 22, "~"},
        {0, 24, "<"},
        {24, 25, "<"},
        {25, 22, "="},
        {24, 22, "="},
        {0, 26, ">"},
        {26, 27, ">"},
        {27, 22, "="},
        {26, 22, "="},
        {0, 28, "&"},
        {28, 22, "&="},
        {0, 29, "|"},
        {29, 22, "|="},

        // Point
        {0, 32, "."},
        {32, 36, "." },
        {3, 38, "."},

        // Comments
        {0, 33, "/"},
        {33, 34, "/"},
        {33, 22, "="}
    };

    // terminals for states
    {
        terminals_[0] = LecsicalEnum::NONE;
        terminals_[1] = LecsicalEnum::NAME;
        terminals_[2] = LecsicalEnum::CONST_INT;
        terminals_[3] = LecsicalEnum::INT_POINT;
        terminals_[4] = LecsicalEnum::NONE;
        terminals_[5] = LecsicalEnum::CONST_FLOAT;
        terminals_[6] = LecsicalEnum::NONE;
        terminals_[7] = LecsicalEnum::NONE;
        terminals_[8] = LecsicalEnum::CONST_STRING;
        terminals_[9] = LecsicalEnum::NONE;
        terminals_[10] = LecsicalEnum::NONE;
        terminals_[11] = LecsicalEnum::CONST_CHAR;
        terminals_[12] = LecsicalEnum::NONE;
        terminals_[13] = LecsicalEnum::COMMA;
        terminals_[14] = LecsicalEnum::COLON;
        terminals_[15] = LecsicalEnum::SEMI_COLON;
        terminals_[16] = LecsicalEnum::SQUARE_BRACE;
        terminals_[17] = LecsicalEnum::ROUND_BRACE;
        terminals_[18] = LecsicalEnum::CURLY_BRACE;
        terminals_[19] = LecsicalEnum::SPACE;
        terminals_[20] = LecsicalEnum::OPERATION;
        terminals_[21] = LecsicalEnum::OPERATION;
        terminals_[22] = LecsicalEnum::OPERATION;
        terminals_[23] = LecsicalEnum::OPERATION;
        terminals_[24] = LecsicalEnum::OPERATION;
        terminals_[25] = LecsicalEnum::OPERATION;
        terminals_[26] = LecsicalEnum::OPERATION;
        terminals_[27] = LecsicalEnum::OPERATION;
        terminals_[28] = LecsicalEnum::OPERATION;
        terminals_[29] = LecsicalEnum::OPERATION;
        terminals_[30] = LecsicalEnum::NONE;
        terminals_[31] = LecsicalEnum::NONE;
        terminals_[32] = LecsicalEnum::POINT;
        terminals_[33] = LecsicalEnum::OPERATION;
        terminals_[34] = LecsicalEnum::COMMENT;
        terminals_[35] = LecsicalEnum::NONE;
        terminals_[36] = LecsicalEnum::OPERATION;
        terminals_[37] = LecsicalEnum::CONST_FLOAT;
        terminals_[38] = LecsicalEnum::INT_DOUBLE_POINT;
        terminals_[39] = LecsicalEnum::NONE;
    }

    // Creating StateMachine nodes
    for (int i = 0; i < number_of_states; i++) {
        machine_[i] = new StateMachine(terminals_[i]);
    }

    // Setting rules to StateMachine nodes
    for (auto& rule : rules) {
        machine_[rule.from]->setLink(rule.symbols, machine_[rule.to]);
    }

    return;
}

void LecsicalAnalyzer::createTokens() {
    std::string line;
    try
    {
        for (int line_number = 1; std::getline(program_input_, line); line_number++) {
            line += "\n";
            StateMachine* cur_state = machine_[0];
            LecsicalEnum prev_terminal = LecsicalEnum::NONE;
            std::string cur_lecsem = "";
            for (int i = 0; i < line.size(); ++i) {
                prev_terminal = cur_state->getTerminal();
                cur_state = cur_state->getLink(line[i]);
                if (cur_state == NULL && prev_terminal == LecsicalEnum::NONE) {
                    throw LecsicalError(file_->fullPath(), line_number + 1, i + 1);
                }
                else if (cur_state == NULL) {
                    if (prev_terminal == LecsicalEnum::NAME &&
                        std::find(reserved.begin(), reserved.end(), cur_lecsem) != reserved.end()) {
                        prev_terminal = LecsicalEnum::RESERVED;
                    }
                    if (cur_lecsem == "true" || cur_lecsem == "false") {
                        prev_terminal = LecsicalEnum::CONST_BOOL;
                    }
                    if (cur_lecsem == "in") {
                        prev_terminal = LecsicalEnum::OPERATION;
                    }
                    if (prev_terminal == LecsicalEnum::COMMENT) {
                        break;
                    }
                    if (prev_terminal == LecsicalEnum::INT_POINT) {
                        cur_lecsem.pop_back();
                        tokens_.push_back(LecsicalToken(cur_lecsem, LecsicalEnum::CONST_INT, line_number, i - cur_lecsem.size() - 1, file_->fullPath()));
                        tokens_.push_back(LecsicalToken(".", LecsicalEnum::POINT, line_number, i - 1, file_->fullPath()));
                    }
                    else if (prev_terminal == LecsicalEnum::INT_DOUBLE_POINT) {
                        cur_lecsem.pop_back();
                        cur_lecsem.pop_back();

                        tokens_.push_back(LecsicalToken(cur_lecsem, LecsicalEnum::CONST_INT, line_number, i - cur_lecsem.size() - 1, file_->fullPath()));
                        tokens_.push_back(LecsicalToken("..", LecsicalEnum::OPERATION, line_number, i - 1, file_->fullPath()));
                    }
                    else if (prev_terminal != LecsicalEnum::SPACE)
                        tokens_.push_back(LecsicalToken(cur_lecsem, prev_terminal, line_number, i - cur_lecsem.size(), file_->fullPath()));
                    i--;
                    cur_state = machine_[0];
                    cur_lecsem = "";
                }
                else {
                    cur_lecsem += line[i];
                }
            }
        }
        //tokens_.insert(tokens_.begin(), LecsicalToken(file_->fullPath(), LecsicalEnum::DEFINE, -1, -1, file_->fullPath()));
        //tokens_.insert(tokens_.begin(), LecsicalToken(file_->fullPath(), LecsicalEnum::IFNDEF, -1, -1, file_->fullPath()));
        //tokens_.push_back(LecsicalToken(file_->fullPath(), LecsicalEnum::ENDIF, -1, -1, file_->fullPath()));
        program_input_.close();
    }
    catch (LecsicalError& error)
    {
        error.sendError();
        throw std::string("");
    }
}

std::vector<LecsicalToken> LecsicalAnalyzer::getTokens() {
    return tokens_;
}

void LecsicalAnalyzer::debugTokens() {
    for (auto& token : tokens_) {
        token.tokenDebug();
    }
}