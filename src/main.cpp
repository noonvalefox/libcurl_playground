#include "href_extractor.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

bool filter(string str) {
  return str.find("/wiki/") == 0 &&
         // filter out files, category and special sites
         str.find(":") == string::npos;
}

int main(int argc, char *argv[]) {

  int depth = 1;

  if (argc < 2) {
    cout << "usage: " << argv[0] << " <wikipedia-url> [depth]" << endl;
    return 0;
  }
  if (argc >= 3) { // ignore other args
    depth = atoi(argv[2]);
  }

  string prefix = "wikipedia.org";

  href_extractor ex;
  ex.setFilter(&filter);

  set<string> wikipedia;
  set<string> front_1, front_2;
  auto *f1 = &front_1, *f2 = &front_2;

  // start point (root)
  f1->insert(string(argv[1]).substr(prefix.length()));

  // auto links = ex.extract(argv[1]);

  for (int i = 0; i < depth; i++) {
    for (auto entry : *f1) {
      // cout << entry << endl;
      auto links = ex.extract(prefix + entry);
      auto sizeBefore = f2->size();
      // add only new links to the next front
      set_difference(links.begin(), links.end(), wikipedia.begin(),
                     wikipedia.end(), inserter(*f2, f2->end()));
      cout << f2->size() - sizeBefore << "/" << links.size()
           << " new links extracted from " << entry << endl;
    }
    cout << "next front size: " << f2->size() << endl;

    // add new front to database and set up new front
    wikipedia.insert(f2->begin(), f2->end());
    f1->clear();
    swap(f1, f2);
  }

  cout << "total number of links: " << wikipedia.size() << endl;
  return 0;
}
