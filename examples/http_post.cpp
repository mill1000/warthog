#include <iostream>

#include "warthog.h"

int main(void)
{
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  bool run = true;

  std::cout << "POSTing to http://httpbin.org/post." << std::endl;
  Warthog::http_connect_post(&mgr, "http://httpbin.org/post", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    if (ev == MG_EV_HTTP_MSG)
    {
      // Print received response
      struct mg_http_message* hm = (struct mg_http_message*) ev_data;

      // Print status code and message
      std::cout << "POST status code: " << std::to_string(Warthog::http_status_code(hm)) << std::endl;
      std::cout << "POST status message: " << Warthog::http_status_message(hm) << std::endl;

      // Print entire message
      std::cout << std::string(hm->message.ptr, hm->message.len) << std::endl;

      // Close connection
      c->is_closing = 1;

      // Stop the main loop
      (*(bool*) fn_data) = false;
    }
  }, &run, "", "Hello world.");

  mg_http_listen(&mgr, "0.0.0.0:8080", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    // Nothing
  }, nullptr);

  while (run)
    mg_mgr_poll(&mgr, 1000);

  std::cout << "Shutting down mongoose." << std::endl;
  mg_mgr_free(&mgr);
}