#include "href_extractor.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {

  if (argc != 2) {
    cout << "usage: " << argv[0] << " <wikipedia-url>" << endl;
    return 0;
  }

  href_extractor ex;
  ex.setFilter([](string str) {
    return str.find("/wiki/") == 0 &&
           // filter out files, category and special sites
           str.find(":") == string::npos;
  });

  auto list = ex.extract(argv[1]);

  for (auto entry : list) {
    cout << entry << endl;
  }
  
  cout << "number of links: " << list.size() << endl;

  return 0;
}
