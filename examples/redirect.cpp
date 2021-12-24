#include <iostream>

#include "warthog.h"

int main(void)
{
  Warthog warthog;

  warthog.register_endpoint("/home", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    mg_http_reply(c, 200, NULL, "Welcome home!");
  });

  warthog.register_default([](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    if (ev == MG_EV_HTTP_MSG)
    {
      std::cout << "Redirecting to /home." << std::endl;
      Warthog::http_send_redirect(c, 302, "/home");
    }
  });

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  std::cout << "Mongoose listening on port 8080." << std::endl;
  mg_http_listen(&mgr, "0.0.0.0:8080", Warthog::mongoose_event_handler, &warthog);

  while (1)
    mg_mgr_poll(&mgr, 1000);

  mg_mgr_free(&mgr);
}