/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2012 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* <DESC>
 * Uses the "Streaming HTML parser" to extract the href pieces in a streaming
 * manner from a downloaded HTML.
 * </DESC>
 */
/*
 * The HTML parser is found at http://code.google.com/p/htmlstreamparser/
 */

#include <cstdio>
#include <curl/curl.h>
#include <htmlstreamparser.h>

#include <iostream>
#include <vector>
#include <string>
#include <functional>

using namespace std;

class href_extractor
{
  char tag[1], attr[4], val[128];
  CURL *curl;
  HTMLSTREAMPARSER *hsp;

  // default filter: let everything pass
  function<bool(string)> filter = ([](string){
    return true;
  });

  static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *extractor)
  {
    // extract members from object
    auto *hsp = static_cast<href_extractor*>(extractor)->hsp;
    auto filter = static_cast<href_extractor*>(extractor)->filter;
    auto &list = static_cast<href_extractor*>(extractor)->extractedLinks;

    size_t realsize = size * nmemb, p;
    for (p = 0; p < realsize; p++) {
      html_parser_char_parse(hsp, ((char *)buffer)[p]);
      if (html_parser_cmp_tag(hsp, "a", 1))
        if (html_parser_cmp_attr(hsp, "href", 4))
          if (html_parser_is_in(hsp, HTML_VALUE_ENDED))
          {
            html_parser_val(hsp)[html_parser_val_length(hsp)] = '\0';
            //printf("%s\n", html_parser_val((HTMLSTREAMPARSER *)hsp));
            char* link = html_parser_val(hsp);
            
            if(filter(link)) 
              list.push_back(link);
          }
    }
    return realsize;
  }

  public:
  vector<string> extractedLinks;

  href_extractor()
  {
    curl = curl_easy_init();
    hsp = html_parser_init();

    html_parser_set_tag_to_lower(hsp, 1);
    html_parser_set_attr_to_lower(hsp, 1);
    html_parser_set_tag_buffer(hsp, tag, sizeof(tag));
    html_parser_set_attr_buffer(hsp, attr, sizeof(attr));
    html_parser_set_val_buffer(hsp, val, sizeof(val) - 1);
  }

  ~href_extractor() {
    curl_easy_cleanup(curl);
    html_parser_cleanup(hsp);
  }

  vector<string>& extract(char *link) {
    extractedLinks.clear();

    curl_easy_setopt(curl, CURLOPT_URL, link);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this); // static callback workaround: pass pointer to object
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_perform(curl);

    return extractedLinks;
  }

  void setFilter(function<bool(string)> f) {
    filter = f;
  }

};

int main(int argc, char *argv[]) {

  if(argc != 2) {
    cout << "usage: " << argv[0] << " <wikipedia-url>" << endl;
    return 0;
  }
  href_extractor ex;
  ex.setFilter([](string str) {
      return str.find("/wiki/") == 0
          //&& str.find(".") == string::npos
          && str.find(":") == string::npos // filter out files, category and special sites
          ;
  });

  auto list = ex.extract(argv[1]);

  

  for(auto entry : list) {
    cout << entry << endl;
  }
  cout << "number of links: " << list.size() << endl;


 return 0;  
}
