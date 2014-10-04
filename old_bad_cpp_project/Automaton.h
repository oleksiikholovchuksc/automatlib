#ifndef _AUTOMATON_H_
#define _AUTOMATON_H_

#include <vector>
#include <map>
#include <list>
#include <fstream>
#include <stdexcept>

#include "SimpleAhoCorasick.h"

template <class Key, class Value>
class Automaton {
public:
  Automaton() {
    init();

  }

  Automaton(const std::vector<std::pair<std::vector<Key>, Value> >& wordsToAdd) {
    init();
    for(int i = 0; i < wordsToAdd.size(); ++i)
      add(wordsToAdd[i]);
  }

  Automaton(const std::vector<std::pair<std::string, Value> >& wordsToAdd) {
    init();
    for(int i = 0; i < wordsToAdd.size(); ++i)
      add(wordsToAdd[i]);
  }

  Automaton(const Automaton& automaton) {
    init();
    // blah blah do something here
  }

  ~Automaton() {
    for(typename std::list<AC_POINTER>::iterator it = automata.begin();
	it != automata.end(); ++it) {
      delete *it;
    }
  }

  void add(const std::pair<std::vector<Key>, Value>& newWord) {
    std::vector<std::pair<std::vector<Key>, Value> > words;
    words.push_back(newWord);

    
    typename std::list<AC_POINTER>::iterator placeItHere = automata.begin();
    while(placeItHere != automata.end()) {
      if(*placeItHere == NULL) break;
      placeItHere++;
    }
    if(placeItHere == automata.end()) {
      automata.push_back(NULL);
      placeItHere = automata.end(); 
      --placeItHere;
    }
    
    
    typename std::list<AC_POINTER>::iterator collect = placeItHere;
    if(collect != automata.begin()) --collect;
    for(;;) {
      if(collect == automata.begin() && *collect) {
	for(int i = 0; i < (*collect)->setOfWords.size(); ++i) {
	  words.push_back((*collect)->setOfWords.at(i));
	}
	delete *collect;
	*collect = NULL;
	break;
      }
      
      if(!(*collect)) {
	break;
      }

      for(int i = 0; i < (*collect)->setOfWords.size(); ++i) {
        words.push_back((*collect)->setOfWords.at(i));
      }
      delete *collect;
      *collect = NULL;
      --collect;
    }

    *placeItHere = new SimpleAhoCorasick<Key, Value>(words);
  }

  void add(const std::pair<std::string, Value>& newWord) {
    std::vector<Key> v(newWord.first.begin(), newWord.first.end());
    add(std::make_pair(v, newWord.second));
  }

  
  std::vector<Value> recognize(const std::vector<Key>& word) {
    std::vector<Value> result;
    for(typename std::list<AC_POINTER>::iterator it = automata.begin();
	it != automata.end(); ++it) {
      if(*it) {
	std::vector<Value> out = (*it)->recognize(word);
	for(int i = 0; i < out.size(); ++i)
	  result.push_back(out[i]);
      }
    }
    return result;
  }

  // have no idea how slow it is
  std::vector<Value> recognize(const std::string& word) {
    return recognize(std::vector<Key>(word.begin(), word.end()));
  }


  std::vector<std::pair<std::vector<Key>, Value> > getSetOfWords() const {
    
  }
  

  void saveToFile(const char* path) {
    std::ofstream OUT
      (
       path,
       std::ios_base::out |
       std::ios_base::binary |
       std::ios_base::trunc
      );

    if(!OUT.good())
      throw std::invalid_argument("Unable to open file");

    OUT << automata.size();
    for(typename std::list<AC_POINTER>::iterator it = automata.begin();
	it != automata.end(); ++it) {
	writeAutomaton(*it, OUT);
    }

    OUT.close();
  }


  void loadFromFile(const char* path) {
    std::ifstream IN
      (
       path, 
       std::ios_base::in |
       std::ios_base::binary
      );

    for(typename std::list<AC_POINTER>::iterator it = automata.begin();
	it != automata.end(); ++it) {
      delete *it;
      *it = NULL;
    }
    
    automata.clear();

    int newListSize;
    IN >> newListSize;
    automata.resize(newListSize, NULL);
    
    for(typename std::list<AC_POINTER>::iterator it = automata.begin();
	it != automata.end(); ++it) {
      *it = readAutomaton(IN);
    }

    IN.close();
  }

  
private:
  typedef SimpleAhoCorasick<Key, Value>* AC_POINTER;
  std::list<AC_POINTER> automata;

  void init() {
    automata.push_back(AC_POINTER(NULL));
  }

  void writeAutomaton(const AC_POINTER* a, std::ofstream& OUT) {
    if(!a) {
      OUT << 0;
      return;
    }

        outStream << setOfWords.size() << '\n';
    for(int i = 0; i < setOfWords.size(); ++i) {
      outStream << setOfWords[i].first.size() << '\n';
      for(int j = 0; j < setOfWords[i].first.size(); ++j)
        outStream << setOfWords[i].first[j] << '\n';
      outStream << setOfWords[i].second << '\n';
    }
    
    outStream << trieSize + 1 << '\n';
    for(int i = 0; i <= trieSize; ++i) {
      outStream << trie[i].fail << '\n' << trie[i].pred << '\n';
      outStream << trie[i].symb << '\n';
      outStream << trie[i].out << '\n';
      
      outStream << trie[i].go.size() << '\n';
      for(typename std::map<Key, int>::iterator it = trie[i].go.begin();
	  it != trie[i].go.end(); ++it) {
	outStream << it->first << '\n';
	outStream << it->second << '\n';
      } 
    }

  }

  AC_POINTER readAutomaton(std::ifstream& IN) {
    return NULL;
  }
};

#endif
