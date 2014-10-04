#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <cstdlib>

#include "Automaton.h"

int main() {
  std::cout << "Usage:\nadd <key> <value>\nrec <key>" << "\n\n";

  Automaton<char, std::string> DFA;
  for(;;) {
    std::string choice;
    std::cin >> choice;
    if(choice == "add") {
      std::string key, val;
      std::cin >> key >> val;
      DFA.add(std::make_pair(key, val));
      std::cout << "Done.\n" << '\n';
    }
    else if(choice == "rec") {
      std::string key;
      std::cin >> key;
      std::vector<std::string> res = DFA.recognize(key);
      for(int i = 0; i < res.size(); ++i)
	std::cout << res[i] << '\n';
      std::cout << '\n';
    } else {
      std::cout << "Thanks. Bye." << '\n';
      exit(0);
    }
  }
}
