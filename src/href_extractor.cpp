/**
 * author: Anton Baldin
 * date:  2017-11-13
 *
 * A href extractor with a buildin filter function.
 * Uses a HTML parser for finding the hrefs.
 *
 * The HTML parser is found at http://code.google.com/p/htmlstreamparser/
 */

#include "href_extractor.h"

using namespace std;

size_t href_extractor::write_callback(void *buffer, size_t size, size_t nmemb,
                                      void *extractor) {
  // extract members from object
  auto *hsp = static_cast<href_extractor *>(extractor)->hsp;
  auto filter = static_cast<href_extractor *>(extractor)->filter;
  auto &list = static_cast<href_extractor *>(extractor)->extractedLinks;

  size_t realsize = size * nmemb, p;
  for (p = 0; p < realsize; p++) {
    html_parser_char_parse(hsp, ((char *)buffer)[p]);
    if (html_parser_cmp_tag(hsp, "a", 1))
      if (html_parser_cmp_attr(hsp, "href", 4))
        if (html_parser_is_in(hsp, HTML_VALUE_ENDED)) {
          html_parser_val(hsp)[html_parser_val_length(hsp)] = '\0';
          // printf("%s\n", html_parser_val((HTMLSTREAMPARSER *)hsp));
          char *link = html_parser_val(hsp);

          if (filter(link))
            list.push_back(link);
        }
  }
  return realsize;
}

href_extractor::href_extractor() {
  curl = curl_easy_init();
  hsp = html_parser_init();

  html_parser_set_tag_to_lower(hsp, 1);
  html_parser_set_attr_to_lower(hsp, 1);
  html_parser_set_tag_buffer(hsp, tag, sizeof(tag));
  html_parser_set_attr_buffer(hsp, attr, sizeof(attr));
  html_parser_set_val_buffer(hsp, val, sizeof(val) - 1);
}

href_extractor::~href_extractor() {
  curl_easy_cleanup(curl);
  html_parser_cleanup(hsp);
}

vector<string> &href_extractor::extract(char *link) {
  extractedLinks.clear();

  curl_easy_setopt(curl, CURLOPT_URL, link);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA,
                   this); // static callback workaround: pass pointer to object
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  curl_easy_perform(curl);

  return extractedLinks;
}

void href_extractor::setFilter(function<bool(string)> f) { filter = f; }
