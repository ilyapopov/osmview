#ifndef CURL_HPP
#define CURL_HPP

#include <cstddef>
#include <functional>
#include <memory>

#include "curl/curl.h"

namespace osmview
{

class curl_global
{
public:
    curl_global();
    curl_global(const curl_global &) = delete;
    curl_global(curl_global &&) = delete;
    curl_global &operator=(const curl_global &) = delete;
    curl_global &operator=(curl_global &&) = delete;

    ~curl_global();
};

class curl_easy
{
    struct deleter
    {
        void operator()(CURL *h) const { curl_easy_cleanup(h); }
    };

    std::unique_ptr<CURL, deleter> handle_;

    char error_buffer_[CURL_ERROR_SIZE];

public:
    curl_easy();

    CURL *handle() const { return handle_.get(); }

    using callback_type = size_t (*)(char *ptr, size_t size, size_t nmemb,
                                void *userdata);

    void setup_download(const char *url, callback_type f, void *data);

    const char *error_message(CURLcode code) const;
};

class curl_multi
{
    struct deleter
    {
        void operator()(CURLM *h) const { curl_multi_cleanup(h); }
    };

    std::unique_ptr<CURLM, deleter> handle_;

public:
    curl_multi();

    CURLM *handle() const { return handle_.get(); }

    void add(curl_easy &easy);

    void remove(curl_easy &easy);

    int perform(bool repeat = false);

    int process_info(std::function<bool(CURL*, CURLcode)> on_done);
};

} // namespace osmview

#endif // CURL_HPP
