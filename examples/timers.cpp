#include <iostream>

#include "warthog.h"

int main(void)
{
  struct mg_mgr mgr;
  mg_mgr_init(&mgr);

  // Setup a timer to trigger at regular intervals
  uint32_t count = 0;
  Warthog::Timer heartbeat_timer;
  heartbeat_timer.init(500, MG_TIMER_REPEAT, [](Warthog::Timer* t, void* fn_data)
  {
    // Use fn_data to track ticks
    uint32_t* count = (uint32_t*)fn_data;
    *count += 1;

    // Increase period every 10 ticks
    if (*count % 5 == 0)
      t->mg_timer.period_ms += 100;

    // Stop this timer after a while
    if (*count == 20)
      t->release();

    std::cout << "Heartbeat timer. #: " << std::to_string(*count) << ", MS: " << std::to_string(t->mg_timer.period_ms) << std::endl;
  }, &count);

  // Setup a one-shot timer to stop the loop
  bool run = true;
  Warthog::Timer stop_timer;
  stop_timer.init(20000, 0, [](Warthog::Timer* t, void* fn_data)
  {
    std::cout << "Stopping loop." << std::endl;

    // Stop the main loop
    (*(bool*) fn_data) = false;
  }, &run);

  mg_http_listen(&mgr, "0.0.0.0:8080", [](struct mg_connection* c, int ev, void* ev_data, void* fn_data)
  {
    // Nothing
  }, nullptr);

  while (run)
    mg_mgr_poll(&mgr, 100);

  std::cout << "Shutting down mongoose." << std::endl;
  mg_mgr_free(&mgr);
}