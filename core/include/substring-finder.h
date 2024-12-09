#ifndef SUBSTRING_FINDER_H
#define SUBSTRING_FINDER_H

#include <vector>
#include <map>
#include <QString>

namespace sdtmaps {

class SuffixAutomaton {
public:
    struct State {
        int length = 0;                       // Length of the longest string in this state
        int link = -1;                        // Suffix link (points to a state)
        std::map<char, int> transitions;      // Transitions for characters
        std::vector<QString> ends;        // Strings ending in this state
    };

    SuffixAutomaton() = default;
    explicit SuffixAutomaton(const std::vector<QString> &strings);
    std::vector<QString> findAllStringsContaining(const QString &substring) const;

private:
    std::vector<State> states; // All states in the automaton
    int lastState = 0;         // Last active state

    void initialState();
    void addString(const QString &str);
    void addCharacter(char c);
};

}

#endif //SUBSTRING_FINDER_H
