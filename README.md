# Warthog
A minimalist C++ support library for Mongoose Web Server.

## Motivation
[Mongoose](https://github.com/cesanta/mongoose) is a popular, powerful embedded webserver. However, starting with the 7.X release series, some favorite features were removed. Warthog is a simple project designed to restore some of these features to ease transition from the 6.X releases to the latest 7.X release.

## Features
### HTTP Endpoints
Warthog support HTTP endpoints via it's `register_endpoint` and `register_default` member functions. These functions mimics the old functionality provided by `mg_register_http_endpoint`. Each endpoint (including the default) can take an optional user data pointer which is passed to the event handler.

```c++
// Construct a warthog
Warthog warthog;

// Register an endpoint at /api1
uint32_t api_data = 0xD00D;
warthog.register_endpoint("/api1", api1_handler, &api_data);

// Register a default handler
warthog.register_default(default_handler);

struct mg_mgr mgr;
mg_mgr_init(&mgr);

// Setup mongoose to listen with warthog as it's callback
mg_http_listen(&mgr, "0.0.0.0:8080", Warthog::mongoose_event_handler, &warthog);
```

## Redirect
Warthog provides a static function `Warthog::http_send_redirect` which mimics the old functionality provided by `mg_http_send_redirect` to easily redirect clients.

```c++
Warthog::http_send_redirect(c, 302, "/api1");
```

## HTTP GET
Warthog provides a static function `Warthog::http_connect_get` which allows easy HTTP GET requests similar to the old functionality provided by `mg_connect_http`. Each request accepts a event handler and optional user data handler.

```c++
struct mg_mgr mgr;
mg_mgr_init(&mgr);

// Fetch a website
Warthog::http_connect_get(&mgr, "http://google.com", get_handler);
```