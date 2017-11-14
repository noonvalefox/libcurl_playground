#ifndef HREF_EXTRACTOR_H
#define HREF_EXTRACTOR_H

#include <curl/curl.h>
#include "htmlstreamparser.h"

#include <functional>
#include <vector>
#include <string>

class href_extractor
{
    char tag[1], attr[4], val[128];
    CURL *curl;
    HTMLSTREAMPARSER *hsp;

    // default filter: let everything pass
    std::function<bool(std::string)> filter = ([](std::string) {
        return true;
    });

    static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *extractor);

  public:
    href_extractor();
    ~href_extractor();
    std::vector<std::string> &extract(char *link);
    void setFilter(std::function<bool(std::string)> f);

    std::vector<std::string> extractedLinks;
};

#endif /* HREF_EXTRACTOR_H */