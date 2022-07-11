#ifndef LECSICAL_ANALYZER
#define LECSICAL_ANALYZER

#pragma once
#include "LecsicalToken.h"

class LecsicalAnalyzer
{

    /*
        Description:
        Lecsical analyzer creates StateMachine which separate text of the program on different lecsems
    */
private:

    class StateMachine;
    class LecsicalError;

    Path* file_;
    std::vector<LecsicalEnum> terminals_;
    std::vector<StateMachine*> machine_;
    std::vector<LecsicalToken> tokens_;
    std::ifstream program_input_;
    std::set<std::string> reserved;


    void createStateMachine(); // Creates StateMachine itself
    void createReserved(); // Creates array of reserved names which can not be used for naming variables, functions and etc.

public:
    LecsicalAnalyzer(Path* source);
    std::vector<LecsicalToken> getTokens();
    void createTokens();
    void debugTokens();
};


#endif // LECSICAL_ANALYZER
