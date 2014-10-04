/*  TODO:
 *  1. One and only one out-word corresponds to each terminal state, that's wrong.
 *  2. Duplicates may occur in vector returned by the `recognize` function.
 *  3. Create pure abstract class "DFA" with field "state" 
 *     and functions "getCurrent state" & "consumeSymbol".
 */

#ifndef _SIMPLEAHOCOR_H_
#define _SIMPLEAHOCOR_H_

#include <vector>
#include <map>
#include <queue>
#include <iostream>


template <class Key, class Value>
class SimpleAhoCorasick {
  
  template <class K, class V>
  friend class Automaton;

public:
  // keys recognizable by the automaton and values mapped to them
  std::vector<std::pair<std::vector<Key>, Value> > setOfWords;
  
  SimpleAhoCorasick(const std::vector<std::pair<std::vector<Key>, Value> >& wordsToAdd) {
    setOfWords.resize(wordsToAdd.size());
    for(int i = 0; i < wordsToAdd.size(); ++i)
      setOfWords[i] = wordsToAdd[i];

    build();
  }

  SimpleAhoCorasick(const std::vector<std::pair<std::string, Value> >& wordsToAdd) {
    setOfWords.resize(wordsToAdd.size());
    for(int i = 0; i < wordsToAdd.size(); ++i) {
      for(int j = 0; j < wordsToAdd[i].first.length(); ++j)
	setOfWords[i].first.push_back(wordsToAdd[i].first[j]);

      setOfWords[i].second = wordsToAdd[i].second;
    }

    build();
  }

  SimpleAhoCorasick(const SimpleAhoCorasick& copyIt) {
    setOfWords.resize(copyIt.setOfWords.size());
    for(int i = 0; i < copyIt.setOfWords.size(); ++i)
      setOfWords[i] = copyIt.setOfWords[i];

    build();
  }
  
  ~SimpleAhoCorasick() {
    // nothing to delete (yet...)
  }

  std::vector<Value> recognize(const std::vector<Key>& word) {
    std::vector<Value> result;

    int currState = 0;
    for(int i = 0; i < word.size(); ++i) {
      Key key = word[i];
      MapIter it = trie[currState].go.find(key);
      currState = (it != trie[currState].go.end()) ? it->second : 0;

      if(trie[currState].out != -1) {
	Value valueStoredHere = setOfWords[trie[currState].out].second;
	result.push_back(valueStoredHere);
      }
    }
    
    return result;
  }

  std::vector<Value> recognize(const std::string& word) {
    return recognize(std::vector<char> (word.begin(), word.end()));
  }


private:
  struct Node {
    std::map<Key, int> go;
    int fail, pred;
    Key symb;
    int out;

    Node() {
      out = -1;
      fail = pred = 0;
    }
  };

  std::vector<Node> trie;
  int trieSize;

  typedef typename std::map<Key, int>::iterator MapIter;

  void build() {
    trieSize = 0;

    for(int i = 0; i < setOfWords.size(); ++i)
      trieAdd(setOfWords[i], i);
    buildFailureFunc();
    buildNextFunc();
  }

  /*
   *  STEP 1: Adding all the words to trie (of prefix tree)
   */
  void trieAdd(const std::pair<std::vector<Key>, Value>& toAdd, int idx) {
    Node node; trie.push_back(node);   // adding the first node
    int currVertex = 0;
    Key key;
    for(int i = 0; i < toAdd.first.size(); ++i) {
      key = toAdd.first[i];
      if(trie[currVertex].go.find(key) == trie[currVertex].go.end()) {
	Node node; trie.push_back(node);   // add new node
	trie[currVertex].go[key] = ++trieSize;
	trie[trieSize].symb = key;
	trie[trieSize].pred = currVertex;
      }
      currVertex = trie[currVertex].go[key];
    }
    
    trie[currVertex].out = idx;
  }

  /*
   *  STEP 2: Building suffix links through breadth-first trie traversal
   */
  void buildFailureFunc() {
    std::queue<int> Q;
    Q.push(0);
    int currVertex, state;
    Key key;
    while(Q.empty() == false) {
      currVertex = Q.front(); Q.pop();
      for(MapIter it = trie[currVertex].go.begin(); it != trie[currVertex].go.end(); ++it) {
	Q.push(it->second);
      }

      // if current vertex is root or some vertex at level 1
      if(currVertex == 0 || trie[currVertex].pred == 0) {
	trie[currVertex].fail = 0;
	continue;
      }

      // building new suffix link
      key = trie[currVertex].symb;
      state = trie[trie[currVertex].pred].fail;
      while(state != 0 && trie[state].go.find(key) == trie[state].go.end())
	state = trie[state].fail;
      if(trie[state].go.find(key) != trie[state].go.end())
	state = trie[state].go[key];
      trie[currVertex].fail = state;
    }
  }

  /*
   *  STEP 3: Transforming trie to DFA
   */
  void buildNextFunc() {
    for(int state = 0; state <= trieSize; ++state) {
      int go_state = trie[state].fail;
      for(MapIter it = trie[go_state].go.begin(); it != trie[go_state].go.end(); ++it) {
	Key key = it->first;
	if(trie[state].go.find(key) == trie[state].go.end())
	  trie[state].go[key] = trie[go_state].go[key];
      }
    }
  }
};

#endif
