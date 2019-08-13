#ifndef CURL_HPP
#define CURL_HPP

#include <cstddef>
#include <memory>
#include <optional>

struct CURLMsg;

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
public:
    using curl_t = void;

    curl_easy();

    curl_t *handle() const { return handle_.get(); }

    using callback_type = size_t (*)(char *ptr, size_t size, size_t nmemb,
                                     void *userdata);

    void setup_download(const char *url, callback_type f, void *data);

    curl_easy &set_url(const char *url);
    curl_easy &set_callback(callback_type f, void *data);
    curl_easy &set_user_agent(const char *user_agent);

    const char *error_message() const;
private:
    struct deleter
    {
        void operator()(curl_t *h) const;
    };

    std::unique_ptr<curl_t, deleter> handle_;
    std::unique_ptr<char[]> error_buffer_;

};

class curl_multi
{
public:
    using curlm_t = void;

    class message
    {
        const CURLMsg *message_;
    public:
        explicit message(const CURLMsg *message): message_(message) {}
        bool success();
        const char * error_message();
        curl_easy::curl_t *handle();
    };

    curl_multi();

    curlm_t *handle() const { return handle_.get(); }

    void add(curl_easy &easy);

    void remove(curl_easy &easy);

    int perform(bool repeat = false);

    std::optional<message> get_message();

private:
    struct deleter
    {
        void operator()(curlm_t *h) const;
    };

    std::unique_ptr<curlm_t, deleter> handle_;

};

} // namespace osmview

#endif // CURL_HPP
