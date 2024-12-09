#include <algorithm>
#include <map>
#include <vector>
#include <set>

#include "../include/substring-finder.h"

namespace sdtmaps {

SuffixAutomaton::SuffixAutomaton(const std::vector<QString> &strings) {
    makeInitialState();
    for (const auto &str : strings) {
        addString(str);
    }
}

std::vector<QString> SuffixAutomaton::findAllStringsContaining(const QString &substring) const {
    // Feed substring into the automaton
    int currentState = 0;
    for (char c : substring.toStdString()) {
        if (!states[currentState].transitions.contains(c)) {
            return {}; // Substring not accepted
        }
        currentState = states[currentState].transitions.at(c);
    }

    // Perform BFS to collect all reachable strings
    std::set<int> visited;
    std::vector<QString> result;
    std::vector<int> stack;
    stack.push_back(currentState);

    while (!stack.empty()) {
        int state = stack.back();
        stack.pop_back();
        if (visited.contains(state)) {
            continue;
        }
        visited.insert(state);
        // Collect strings from this state
        result.insert(result.end(), states[state].ends.begin(), states[state].ends.end());

        // Add neighboring states to the stack
        for (const auto &[ch, nextState] : states[state].transitions) {
            stack.push_back(nextState);
        }
    }

    // Remove duplicates (optional, depending on input guarantees)
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());

    return result;
}

void SuffixAutomaton::makeInitialState() {
    states.clear();
    states.emplace_back(); // Create the initial state
}

void SuffixAutomaton::addString(const QString &str) {
    lastState = 0;
    for (char c : str.toStdString()) {
        addCharacter(c);
    }

    // Mark all states corresponding to the suffixes of this string
    int current = lastState;
    while (current != -1) {
        states[current].ends.push_back(str);
        current = states[current].link;
    }
}

void SuffixAutomaton::addCharacter(char c) {
    int current = states.size();
    states.emplace_back();
    states[current].length = states[lastState].length + 1;

    int p = lastState;
    while (p != -1 && !states[p].transitions.contains(c)) {
        states[p].transitions[c] = current;
        p = states[p].link;
    }

    if (p == -1) {
        states[current].link = 0; // Link to the initial state
    } else {
        int q = states[p].transitions[c];
        if (states[p].length + 1 == states[q].length) {
            states[current].link = q;
        } else {
            size_t clone = states.size();
            states.emplace_back(states[q]);
            states[clone].length = states[p].length + 1;

            while (p != -1 && states[p].transitions[c] == q) {
                states[p].transitions[c] = clone;
                p = states[p].link;
            }

            states[q].link = states[current].link = clone;
        }
    }
    lastState = current;
}


}
