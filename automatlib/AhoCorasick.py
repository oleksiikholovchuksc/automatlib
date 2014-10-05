"""
Implementation of the aho-corasick algorithm.
Google the paper "String Matching: An Aid to Bibliographic Search" for details.
"""

__author__ = "vortexxx192@gmail.com"


from Queue import Queue
import json
from graphviz import Digraph


class AhoCorasickAutomaton:
    class __Node:
        """
        Description of every DFA vertex.
        """
        def __init__(self,
                     next={},
                     output_fun=[],
                     prev_vert_idx=0,
                     failure_fun=0,
                     symbol='$'):

            self.next = next    # transition function
            self.output_fun = output_fun
            self.prev_vert_idx = prev_vert_idx  # by default, each vertex has root as a parent
            self.failure_fun = failure_fun        # by default, suffix link of each vertex points to root
            self.symbol = symbol     # by which symbol we came to this state. Undefined for root


        def fromJSON(self, data):
            """
            Construct __Node from JSON data

            :param data: dictionary of values that new node will be built from
            :return: None
            """

            data = json.loads(data)

            self.next = data["next"]
            self.output_fun = data["output_fun"]
            self.prev_vert_idx = data["prev_vert_idx"]
            self.failure_fun = data["failure_fun"]
            self.symbol = data["symbol"]


    def __init__(self, words=[]):
        self.vertices = [self.__Node(next={}, output_fun=[])]
        self.words = words

        # construction of the AC-automaton consists of the following phases:
        # 1. building a trie of input words
        for word in self.words:
            self.__add_to_trie(word)

        # 2. computing the failure function (suffix links)
        self.__compute_failure_fun()

        # 3. turn trie with suffix links to DFA
        self.__build_remaining_transitions()


    def search(self, text):
        """
        Search for occurences of words in text.

        :param word: str, text where the search will be performed.
        :return: list of found words
        """

        v_idx = 0
        found_words = set()
        for c in text:
            v_idx = self.vertices[v_idx].next[c] if c in self.vertices[v_idx].next else 0
            found_words |= set(self.vertices[v_idx].output_fun)

        return list(found_words)


    def toJSON(self):
        """
        Serialize AC automaton

        :return: JSON string that represents the automaton
        """

        return json.dumps(self, default=lambda obj: obj.__dict__)


    @staticmethod
    def fromJSON(data):
        """
        Deserialize automaton.

        :param data: JSON string that represents automaton
        :return: AhoCorasick object
        """

        if data is None:
            return None

        data = json.loads(data)

        to_return = AhoCorasickAutomaton()
        to_return.words = []
        for word in data["words"]:
            to_return.words.append(word)

        to_return.vertices = []
        for vertex_data in data["vertices"]:
            to_return.vertices.append(AhoCorasickAutomaton.__Node(next={}, output_fun=[]))
            to_return.vertices[-1].fromJSON(json.dumps(vertex_data))

        return to_return


    def toGraphvizSource(self):
        """
        Generate graphviz code that can be used to draw the automaton.

        :return: graphviz code
        """
        dot = Digraph(engine="neato")
        dot.graph_attr.update(size="4,4")

        for v_idx in range(len(self.vertices)):
            vertex_style = "doublecircle" if len(self.vertices[v_idx].output_fun) > 0 else "circle"
            if v_idx == 0: vertex_style = "point"
            dot.node(str(v_idx), str(v_idx), shape=vertex_style)

        for v_idx in range(len(self.vertices)):
            for c in self.vertices[v_idx].next:
                to_idx = self.vertices[v_idx].next[c]
                dot.edge(str(v_idx), str(to_idx), label=c, constraint="false")

        dot.format = "png"
        dot.render('automaton')

        return dot.source


    def __add_to_trie(self, word):
        """
        Add string 'word' to the trie.

        :param word: str, string to be added in trie.
        :return: None
        """

        v_idx = 0    # index of current vertex
        for c in word:
            # if there is no transition by c, create it
            if c not in self.vertices[v_idx].next:
                self.vertices.append(self.__Node(prev_vert_idx=v_idx, symbol=c, next={}, output_fun=[]))
                self.vertices[v_idx].next[c] = len(self.vertices) - 1

            v_idx = self.vertices[v_idx].next[c]

        # mark last vertex of the word
        self.vertices[v_idx].output_fun.append(word)


    def __compute_failure_fun(self):
        """
        Add suffix link to each automaton's vertex

        :return: None
        """

        # let's perform the breadth-first search on trie
        bfs_queue = Queue()
        bfs_queue.put(0)    # root is the starting vertex

        while not bfs_queue.empty():
            # take vertex from queue
            v_idx = bfs_queue.get()

            # put all adjacent vertices to queue
            for adjacent_vert_idx in self.vertices[v_idx].next.values():
                bfs_queue.put(adjacent_vert_idx)

            # if current vertex isn't a root and doesn't have root as a direct predecessor
            if v_idx != 0 and self.vertices[v_idx].prev_vert_idx != 0:
                u_idx = self.vertices[self.vertices[v_idx].prev_vert_idx].failure_fun
                symb = self.vertices[v_idx].symbol

                # jump on suffix links
                while True:
                    if symb in self.vertices[u_idx].next:
                        self.vertices[v_idx].failure_fun = self.vertices[u_idx].next[symb]

                        # unite output functions
                        self.vertices[v_idx].output_fun = list(set(self.vertices[v_idx].output_fun).union(
                            set(self.vertices[self.vertices[u_idx].next[symb]].output_fun)
                        ))

                        u_idx = self.vertices[u_idx].next[symb]
                        for c in self.vertices[u_idx].next:
                            if c not in self.vertices[v_idx].next:
                                self.vertices[v_idx].next[c] = self.vertices[u_idx].next[c]
                        break

                    if u_idx == 0:
                        break

                    u_idx = self.vertices[u_idx].failure_fun


    def __build_remaining_transitions(self):
        """
        Build transitions such that there will be no necessity to jump over suffix links
        during search process

        :return: None
        """

        # get all characters in words
        chars = set()
        for word in self.words:
            for letter in word:
                chars.add(letter)

        for v_idx in range(len(self.vertices)):
            for c in chars:
                if v_idx == 0:
                    continue

                u_idx = self.vertices[v_idx].failure_fun

                while True:
                    if c not in self.vertices[v_idx].next and c in self.vertices[u_idx].next:
                        self.vertices[v_idx].next[c] = self.vertices[u_idx].next[c]

                    if u_idx == 0:
                        break

                    u_idx = self.vertices[u_idx].failure_fun