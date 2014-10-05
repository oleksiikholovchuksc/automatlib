"""
Solution of the dynamic substring search problem.

Queries of the following two types can be processed:
1. Add new string to set S
2. Given string T, check which strings from S are present in T as substrings
"""

__author__ = "vortexxx192@gmail.com"

from AhoCorasick import AhoCorasickAutomaton as AC


class DynamicAC:
    def __init__(self):
        self.automata = []


    def add(self, word):
        """
        Add new string to the set.

        :param word: string to be added
        :return: None
        """

        if not self.automata:
            self.automata = [AC([word])]
        else:
            words = [word]    # set of words that new automaton will consist of

            for i in range(len(self.automata)):
                if self.automata[i]:
                    words.extend(self.automata[i].words)
                    self.automata[i] = None
                else:
                    self.automata[i] = AC(words)
                    break

                if i == len(self.automata) - 1:
                    self.automata.append(AC(words))


    def search(self, text):
        """
        Search for occurences of strings from set S in text.

        :param text: string for search
        :return: list of found words
        """

        found_words = set()
        for automaton in filter(lambda x: x is not None, self.automata):
            found_words |= set(automaton.search(text))

        return list(found_words)