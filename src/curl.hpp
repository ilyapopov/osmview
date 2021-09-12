#ifndef CURL_HPP
#define CURL_HPP

#include <cstddef>
#include <curl/curl.h>
#include <memory>
#include <optional>
#include <string_view>

struct CURLMsg;

namespace osmview {

class curl_global {
public:
    curl_global();
    curl_global(const curl_global&) = delete;
    curl_global(curl_global&&) = delete;
    curl_global& operator=(const curl_global&) = delete;
    curl_global& operator=(curl_global&&) = delete;

    ~curl_global();
};

class curl_easy;
class curl_easy_in_multi;
class curl_multi;

struct curl_easy_data {
    struct deleter {
        void operator()(CURL* h) const;
    };
    curl_easy_data();

    std::unique_ptr<CURL, deleter> handle_;
    std::unique_ptr<char[]> error_buffer_;
};

class curl_easy {
public:
    curl_easy();

    using write_function_type = size_t (*)(char* ptr, size_t size, size_t nmemb,
        void* userdata);

    curl_easy& set_url(const char* url);
    curl_easy& set_write_function(write_function_type f, void* data);
    template <typename Callable>
    curl_easy& set_write_callback(Callable& f)
    {
        set_write_function(
            [](char* ptr, size_t /*size*/, size_t nmemb, void* userdata) {
                auto f = reinterpret_cast<Callable*>(userdata);
                return (*f)(std::string_view(ptr, nmemb));
            },
            reinterpret_cast<void*>(&f));
        return *this;
    }
    curl_easy& set_user_agent(const char* user_agent);
    curl_easy& set_private(void* data);
    curl_easy& reset();
    curl_easy& default_setup();

    void* get_private() const;

    const char* error_message() const;

    friend class curl_multi;

private:
    curl_easy(curl_easy_data&& data);
    CURL* handle() const { return data_.handle_.get(); }
    curl_easy_data data_;
};

class curl_easy_in_multi {
public:
    friend class curl_multi;

private:
    curl_easy_in_multi(curl_easy_data&& data);
    CURL* handle() const { return data_.handle_.get(); }
    curl_easy_data data_;
};

class curl_multi {
public:
    using curlm_t = void;

    class message {
        const CURLMsg* message_;

    public:
        explicit message(const CURLMsg* message)
            : message_(message)
        {
        }
        bool success();
        const char* error_message();
        void* handle() const;
        void* get_private() const;
    };

    curl_multi();

    curl_easy_in_multi add(curl_easy&& easy);

    curl_easy remove(curl_easy_in_multi&& easy_handle);

    int perform();

    std::optional<message> get_message();

private:
    curlm_t* handle() const { return handle_.get(); }

    struct deleter {
        void operator()(curlm_t* h) const;
    };

    std::unique_ptr<curlm_t, deleter> handle_;
};

} // namespace osmview

#endif // CURL_HPP
