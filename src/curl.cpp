#include "curl.hpp"

#include <cstring>
#include <stdexcept>

#define CURL_NO_OLDIES
#include "curl/curl.h"

osmview::curl_easy::curl_easy()
{
    data_.error_buffer_[0] = '\0';
    if (!data_.handle_) {
        throw std::runtime_error("Curl: error in curl_easy_init");
    }
    default_setup();
}

osmview::curl_easy& osmview::curl_easy::set_url(const char* url)
{
    curl_easy_setopt(handle(), CURLOPT_URL, url);
    return *this;
}

osmview::curl_easy& osmview::curl_easy::set_write_function(osmview::curl_easy::write_function_type f, void* data)
{
    curl_easy_setopt(handle(), CURLOPT_WRITEFUNCTION, f);
    curl_easy_setopt(handle(), CURLOPT_WRITEDATA, data);
    return *this;
}

osmview::curl_easy& osmview::curl_easy::set_user_agent(const char* user_agent)
{
    curl_easy_setopt(handle(), CURLOPT_USERAGENT, user_agent);
    return *this;
}

osmview::curl_easy& osmview::curl_easy::set_private(void* data)
{
    curl_easy_setopt(handle(), CURLOPT_PRIVATE, data);
    return *this;
}

osmview::curl_easy& osmview::curl_easy::reset()
{
    curl_easy_reset(handle());
    return default_setup();
}

osmview::curl_easy& osmview::curl_easy::default_setup()
{
    curl_easy_setopt(handle(), CURLOPT_ERRORBUFFER, data_.error_buffer_.get());
    // this is needed for mutithreaded applications
    curl_easy_setopt(handle(), CURLOPT_NOSIGNAL, 1l);
    curl_easy_setopt(handle(), CURLOPT_FAILONERROR, 1l);
    curl_easy_setopt(handle(), CURLOPT_TIMEOUT, 2l);
    curl_easy_setopt(handle(), CURLOPT_FOLLOWLOCATION, 1l);
    return *this;
}

void* osmview::curl_easy::get_private() const
{
    char* data;
    curl_easy_getinfo(handle(), CURLINFO_PRIVATE, &data);
    return data;
}

const char* osmview::curl_easy::error_message() const
{
    return data_.error_buffer_.get();
}

osmview::curl_easy::curl_easy(curl_easy_handle&& data)
    : data_(std::move(data))
{
}

osmview::curl_multi::curl_multi()
    : handle_(curl_multi_init())
{
    if (!handle_) {
        throw std::runtime_error("Curl: error in curl_multi_init");
    }
}

int osmview::curl_multi::perform()
{
    int still_running = 0;
    CURLMcode code = curl_multi_perform(handle(), &still_running);
    if (code != CURLM_OK) {
        throw std::runtime_error(std::string("Curl: error in curl_multi_perform:") + curl_multi_strerror(code));
    }
    return still_running;
}

std::optional<osmview::curl_multi::message> osmview::curl_multi::get_message()
{
    int nmessages;
    CURLMsg* msg;

    msg = curl_multi_info_read(handle(), &nmessages);

    if (msg == nullptr) {
        return std::nullopt;
    }

    return message(msg);
}

osmview::curl_easy_handle osmview::curl_multi::add(curl_easy&& easy)
{
    if (curl_multi_add_handle(handle(), easy.handle()) != CURLM_OK) {
        throw std::runtime_error("Curl: error in curl_multi_add_handle");
    }
    return curl_easy_handle(std::move(easy.data_));
}

osmview::curl_easy osmview::curl_multi::remove(osmview::curl_easy_handle&& easy_handle)
{
    if (curl_multi_remove_handle(handle(), easy_handle.handle()) != CURLM_OK) {
        throw std::runtime_error("Curl: error in curl_multi_remove_handle");
    }
    return curl_easy(std::move(easy_handle));
}

osmview::curl_global::curl_global()
{
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        throw std::runtime_error("Curl: error in curl_global_init");
    }
}

osmview::curl_global::~curl_global()
{
    curl_global_cleanup();
}

void osmview::curl_easy_handle::deleter::operator()(CURL* h) const { curl_easy_cleanup(h); }

void osmview::curl_multi::deleter::operator()(CURLM* h) const { curl_multi_cleanup(h); }

bool osmview::curl_multi::message::success()
{
    return message_->data.result == CURLE_OK;
}

const char* osmview::curl_multi::message::error_message()
{
    return curl_easy_strerror(message_->data.result);
}

void* osmview::curl_multi::message::get_private() const
{
    char* data;
    curl_easy_getinfo(message_->easy_handle, CURLINFO_PRIVATE, &data);
    return data;
}

osmview::curl_easy_handle::curl_easy_handle()
    : handle_(curl_easy_init())
    , error_buffer_(new char[CURL_ERROR_SIZE])
{
}
