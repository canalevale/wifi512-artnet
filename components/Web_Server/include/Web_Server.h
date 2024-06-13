#ifndef ___WEB_SERVER_H__
#define ___WEB_SERVER_H__

#include "esp_http_server.h"
#include "esp_log.h"
#include "NVS_ESP.h"

esp_err_t root_handler_get(httpd_req_t *req);
esp_err_t root_handler_post(httpd_req_t *req);
void start_webserver(void);
void stop_webserver(httpd_handle_t server);

#endif //___WEB_SERVER_H__