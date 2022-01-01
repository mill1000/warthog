#ifndef _WARTHOG_
#define _WARTHOG_

#include <string>
#include <unordered_map>
#include <assert.h>

#include "mongoose.h"

class Warthog
{
  public:
    class Timer
    {
      public:
        struct mg_timer mg_timer;

        typedef void (*timer_handler_t)(Timer* timer, void* fn_data);

        void init(uint32_t ms, uint32_t flags, timer_handler_t handler, void* fn_data = nullptr)
        {
          this->handler = handler;
          this->fn_data = fn_data;

          // Init time with static handler
          mg_timer_init(&mg_timer, ms, flags, timer_event_handler, this);
        }

        void release()
        {
          // Free the timer from Mongoose
          mg_timer_free(&mg_timer);
        }

      private:
        static void timer_event_handler(void* fn_data)
        {
          assert(fn_data);
          Timer* timer = (Timer*)fn_data;
          timer->handler(timer, timer->fn_data);
        }

        timer_handler_t handler;
        void* fn_data;
    };

    static void http_send_redirect(struct mg_connection* c, int code, const std::string& uri)
    {
      std::string redirect = "Location: " + uri + "\r\n";
      mg_http_reply(c, code, redirect.c_str(), "");
    }
    
    static void http_connect_get(struct mg_mgr* mgr, const std::string& url, mg_event_handler_t handler, void* fn_data = nullptr)
    {
      // Fetch the host and URI components
      struct mg_str host = mg_url_host(url.c_str());
      const char* uri = mg_url_uri(url.c_str());

      // Build the request
      std::string request = "GET " + std::string(uri) + " HTTP/1.0\r\n";
      request += "Host: " + std::string(host.ptr, host.len) + "\r\n";
      request += "\r\n";

      // Create connection
      struct mg_connection* c = mg_http_connect(mgr, url.c_str(), handler, fn_data);

      // Immediately write request assuming it will send once connected
      mg_printf(c, request.c_str());
    }

    static void http_connect_post(struct mg_mgr* mgr, const std::string& url, mg_event_handler_t handler, void* fn_data = nullptr, const std::string& headers = "", const std::string& body = "")
    {
      // Fetch the host and URI components
      struct mg_str host = mg_url_host(url.c_str());
      const char* uri = mg_url_uri(url.c_str());

      // Build the request
      std::string request = "POST " + std::string(uri) + " HTTP/1.0\r\n";
      request += "Host: " + std::string(host.ptr, host.len) + "\r\n";
      request += "Content-Length: " + std::to_string(body.length()) + "\r\n";
      request += headers;
      request += "\r\n";

      // Create connection
      struct mg_connection* c = mg_http_connect(mgr, url.c_str(), handler, fn_data);

      // Immediately write request assuming it will send once connected
      mg_printf(c, request.c_str());
      mg_printf(c, body.c_str());
    }

    static int http_status_code(const struct mg_http_message* hm)
    {
      return std::stoi(std::string(hm->uri.ptr, hm->uri.len));
    }

    static std::string http_status_message(const struct mg_http_message* hm)
    {
      return std::string(hm->proto.ptr, hm->proto.len);
    }

    void register_endpoint(const std::string& uri, mg_event_handler_t handler, void* fn_data = nullptr)
    {
      endpoints[uri] = {.handler = handler, .fn_data = fn_data};
    }

    void register_default(mg_event_handler_t handler, void* fn_data = nullptr)
    {
      default_endpoint = {.handler = handler, .fn_data = fn_data};
    }

    static void mongoose_event_handler(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
    {
      assert(fn_data);
      ((Warthog*)fn_data)->resolve_endpoint(c, ev, ev_data);
    }

  private:
    void resolve_endpoint(struct mg_connection* c, int ev, void* ev_data)
    {
      if (ev != MG_EV_HTTP_MSG)
      {
        // Call default for non-HTTP event
        if (default_endpoint.handler)
          default_endpoint.handler(c, ev, ev_data, default_endpoint.fn_data);

        return;
      }

      struct mg_http_message* hm = (struct mg_http_message*) ev_data;

      // Attempt to match URI against known endpoints
      for (const auto& kv : endpoints)
      {
        if (mg_http_match_uri(hm, kv.first.c_str()))
        {
          if (kv.second.handler)
            kv.second.handler(c, ev, ev_data, kv.second.fn_data);

          return;
        }
      }

      // Call default if no endpoints matched
      if (default_endpoint.handler)
        default_endpoint.handler(c, ev, ev_data, default_endpoint.fn_data);
    }

    typedef struct endpoint_t
    {
      mg_event_handler_t handler;
      void* fn_data;
    } endpoint_t;

    typedef struct request_handle_t
    {
      std::string request;
      mg_event_handler_t handler;
      void* fn_data;
    } request_handle_t;

    std::unordered_map<std::string, endpoint_t> endpoints;
    endpoint_t default_endpoint;
};

#endif