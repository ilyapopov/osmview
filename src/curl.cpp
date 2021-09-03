#include "curl.hpp"

#include <cstring>
#include <stdexcept>

#include "curl/curl.h"

osmview::curl_easy::curl_easy()
    : handle_(curl_easy_init()),
      error_buffer_(new char[CURL_ERROR_SIZE])
{
    error_buffer_[0] = '\0';
    if (!handle_)
    {
        throw std::runtime_error("Curl: error in curl_easy_init");
    }
    curl_easy_setopt(handle(), CURLOPT_ERRORBUFFER, error_buffer_.get());
    // this is needed for mutithreaded applications
    curl_easy_setopt(handle(), CURLOPT_NOSIGNAL, 1l);
    curl_easy_setopt(handle(), CURLOPT_FAILONERROR, 1l);
    curl_easy_setopt(handle(), CURLOPT_TIMEOUT, 2l);
    curl_easy_setopt(handle(), CURLOPT_FOLLOWLOCATION, 1l);
}

osmview::curl_easy &osmview::curl_easy::set_url(const char *url)
{
    curl_easy_setopt(handle(), CURLOPT_URL, url);
    return *this;
}

osmview::curl_easy &osmview::curl_easy::set_callback(osmview::curl_easy::callback_type f, void *data)
{
    curl_easy_setopt(handle(), CURLOPT_WRITEFUNCTION, f);
    curl_easy_setopt(handle(), CURLOPT_WRITEDATA, data);
    return *this;
}

osmview::curl_easy &osmview::curl_easy::set_user_agent(const char *user_agent)
{
    curl_easy_setopt(handle(), CURLOPT_USERAGENT, user_agent);
    return *this;
}

const char *osmview::curl_easy::error_message() const
{
    return error_buffer_.get();
}

osmview::curl_multi::curl_multi() : handle_(curl_multi_init())
{
    if (!handle_)
    {
        throw std::runtime_error("Curl: error in curl_multi_init");
    }
}

int osmview::curl_multi::perform(bool repeat)
{
    int still_running = 0;
    CURLMcode code;
    do
    {
        code = curl_multi_perform(handle(), &still_running);
    } while (repeat && code == CURLM_CALL_MULTI_PERFORM);
    if (code != CURLM_OK && code != CURLM_CALL_MULTI_PERFORM)
    {
        throw std::runtime_error("Curl: error in curl_multi_perform");
    }
    return still_running;
}

std::optional<osmview::curl_multi::message> osmview::curl_multi::get_message()
{
    int nmessages;
    CURLMsg *msg;

    msg = curl_multi_info_read(handle(), &nmessages);

    if (msg == nullptr)
    {
        return std::nullopt;
    }

    return message(msg);
}

void osmview::curl_multi::add(osmview::curl_easy &easy)
{
    if (curl_multi_add_handle(handle(), easy.handle()) != CURLM_OK)
    {
        throw std::runtime_error("Curl: error in curl_multi_add_handle");
    }
}

void osmview::curl_multi::remove(osmview::curl_easy &easy)
{
    if (curl_multi_remove_handle(handle(), easy.handle()) != CURLM_OK)
    {
        throw std::runtime_error("Curl: error in curl_multi_remove_handle");
    }
}

osmview::curl_global::curl_global()
{
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
    {
        throw std::runtime_error("Curl: error in curl_global_init");
    }
}

osmview::curl_global::~curl_global()
{
    curl_global_cleanup();
}

void osmview::curl_easy::deleter::operator()(CURL *h) const { curl_easy_cleanup(h); }

void osmview::curl_multi::deleter::operator()(CURLM *h) const { curl_multi_cleanup(h); }

bool osmview::curl_multi::message::success()
{
    return message_->data.result == CURLE_OK;
}

const char *osmview::curl_multi::message::error_message()
{
    return curl_easy_strerror(message_->data.result);
}

osmview::curl_easy::curl_t *osmview::curl_multi::message::handle()
{
    return message_->easy_handle;
}
