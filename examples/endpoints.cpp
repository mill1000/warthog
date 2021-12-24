#include <iostream>

#include "warthog.h"

int main(void)
{
  Warthog warthog;

  // Register an endpoint at /api1
  const char* api1_message = "Hello";
  warthog.register_endpoint("/api1", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    mg_http_reply(c, 200, NULL, "API1 handler says %s.\n", (const char*)fn_data);
  }, (void*)api1_message);

  // Register an endpoint at /api2
  const char* api2_message = "Bonjour";
  warthog.register_endpoint("/api2", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    mg_http_reply(c, 200, NULL, "API2 handler says %s.\n", (const char*)fn_data);
  }, (void*)api2_message);

  // Setup a default handler
  const char* default_message = "Holla";
  warthog.register_default([](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    if (ev == MG_EV_HTTP_MSG)
      mg_http_reply(c, 200, NULL, "Default handler says %s.\n", (const char*)fn_data);
  }, (void*)default_message);

  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  std::cout << "Mongoose listening on port 8080." << std::endl;
  mg_http_listen(&mgr, "0.0.0.0:8080", Warthog::mongoose_event_handler, &warthog);

  while (1)
    mg_mgr_poll(&mgr, 1000);

  mg_mgr_free(&mgr);
}