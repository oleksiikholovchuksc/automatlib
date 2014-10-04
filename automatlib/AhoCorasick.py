"""
Implementation of the aho-corasick algorithm.
Google the paper "String Matching: An Aid to Bibliographic Search" for details.
"""

__author__ = "vortexxx192@gmail.com"


from string import printable as alphabet


class AhoCorasickAutomaton:
    class __Node:
        """
        Description of every DFA vertex.
        """
        def __init__(self):
            self.next = [-1 for _ in alphabet]    # transition function
            self.is_end_state = False
            self.prev_state_idx = 0     # by default, each vertex has root as a parent
            self.fail_fun = 0           # by default, suffix link of each vertex points to root
            self.symbol = '$'     # by which symbol we came to this state. Undefined for root


    def __init__(self, words=[]):
        self.vertices = []
        self.words = words

        # construction of the AC-automaton consists of the following phases:
        # 1. building a trie of input words
        for word in self.words:
            self.__add_to_trie(word)

        # 2. computing the failure function (suffix links)
        self.__compute_failure_fun()

        # 3. compressing suffix links and actually
        # turning the trie with suffix links to DFA
        self.__compress_suff_links()


    def search(self, text):
        """
        Search for occurences of words in text.

        :param word: str, text where the search will be performed.
        :return: list of pairs (position, word)
        """

        pass


    def __add_to_trie(self, s):
        """
        Add string s to the trie.

        :param s: str, string to be added in trie.
        :return: None
        """

        pass


    def __compute_failure_fun(self):
        """
        Add suffix link to each automaton's vertex

        :return: None
        """

        pass


    def __compress_suff_links(self):
        """
        Turn trie with suffix links to DFA

        :return: None
        """

        pass