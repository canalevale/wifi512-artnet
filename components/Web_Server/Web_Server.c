#include <stdio.h>
#include "Web_Server.h"

static const char * TAG_Web = "WebServer";
const char *resp_str = "<!doctypehtml><html lang=en><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Nodo Wifi-DMX512</title><style>body{font-family:Arial,Helvetica,sans-serif;margin:0;padding:0;background-color:#111}.container{max-width:600px;margin:0 auto;padding:20px;background-color:#222;border-radius:5px;box-shadow:0 0 10px rgba(2,0,0,.1)}h1{text-align:center;font-style:normal;color:#f2f2f2;font-size:400%\%}h2,label[for=numero]{text-align:center;color:#d2f432;font-size:200%\%}form{color:#f2f2f2;text-align:center}label{display:block;font-weight:700;margin-bottom:5px}input[type=number],input[type=password],input[id=ssid],select{width:50%\%;padding:10px;margin-bottom:15px;border:none;border-radius:4px;text-align:center;color:#222;background-color:#f2f2f2}input[type=number]{width:10\%%}input[type=submit]{width:50%\%;padding:10px;background-color:#d2f432;size:150%\%;color:#222;border:none;border-radius:4px;cursor:pointer}input[type=submit]:hover{background-color:#b1d029}</style><br><br><h1>Nodo Wifi-DMX512</h1><form action=/ method=post><label for=numero>Universo Art-Net: %d</label> <input id=numero name=numero required type=number max=255 min=0><h2>Configuracion de Red</h2><label for=mode>Modo de Funcionamiento: %s</label> <select id=mode name=mode><option value=AP>Punto de Acceso (AP)<option value=STA>Estación (STA)</select><br><br><label for=ssid>SSID: %s</label> <input id=ssid name=ssid required><br><br><label for=password>Contraseña: %s</label> <input id=password name=password required type=password placeholder=\"(Minimo 8 caracteres)\"><br><br><input type=submit value=\"Guardar Configuración\"></form>";
char *resp_get;

u_int8_t subuni2;
char mode2[4];
char ssid2[32];
char pass2[64];
/* Manejador para la ruta raíz */
esp_err_t root_handler_get(httpd_req_t *req){
    
    get_nvs_mode(mode2);
    if(strcmp(mode2,"STA")==0){subuni2=get_nvs_sta_subuni();get_nvs_ssid_sta(ssid2);get_nvs_pass_sta(pass2);}
    if(strcmp(mode2,"AP")==0){subuni2=get_nvs_ap_subuni();get_nvs_ssid_ap(ssid2);get_nvs_pass_ap(pass2);}
    asiprintf(&resp_get,resp_str,subuni2,mode2,ssid2,pass2);
    //Envía una respuesta HTML al cliente 
    httpd_resp_send(req, resp_get, strlen(resp_get));
    ESP_LOGI(TAG_Web, "se envio HTML");
    return ESP_OK;

}
esp_err_t root_handler_post(httpd_req_t *req){
    //Recervo memoria
    char* buf;
    buf=malloc(req->content_len);
    //Recupero Buffer POST
    int ret2 = httpd_req_recv(req, buf, req->content_len);
    if (ret2 <= 0) {
        ESP_LOGE(TAG_Web, "Error al recibir datos POST");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[req->content_len]='\0';
    //Recupero los datos y Asigno los datos
    sscanf(buf, "numero=%hhd&mode=%32[^&]&ssid=%32[^&]&password=%64s", &subuni2, mode2, ssid2, pass2);
    //Libero memoria
    free(buf);

    //Verifico espacios en SSID
    for (int i = 0; ssid2[i] != '\0'; i++) {
        if (ssid2[i] == '+') {
            ssid2[i] = ' ';
        }
    }

    //Reenvio HTML
    asiprintf(&resp_get,resp_str,subuni2,mode2,ssid2,pass2);
    httpd_resp_send(req, resp_get, strlen(resp_get));

    // Imprimir los datos recibidos para verificar
    ESP_LOGI(TAG_Web, "SubUni:%d", subuni2);
    ESP_LOGI(TAG_Web, "Modo:%s", mode2);
    ESP_LOGI(TAG_Web, "SSID:%s", ssid2);
    ESP_LOGI(TAG_Web, "Contraseña:%s", pass2);

    if(strcmp(mode2,"STA")==0){escritura_nvs_sta(subuni2,ssid2,pass2);}
    if(strcmp(mode2,"AP")==0){escritura_nvs_ap(subuni2,ssid2,pass2);}
    ESP_LOGI(TAG_Web,"Reset....");
    vTaskDelay(500/portTICK_PERIOD_MS);
    esp_restart();
    return ESP_OK;
}
/* Configuración de la ruta */
httpd_uri_t root_uri_get = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler_get,
    .user_ctx  = NULL
};
/* Configuración de la ruta */
httpd_uri_t root_uri_post = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = root_handler_post,
    .user_ctx  = NULL
};


/* Inicialización del servidor HTTP */
void start_webserver(void)
{
    /* Configuración del servidor HTTP */
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    /* Inicializa el servidor HTTP */
    if (httpd_start(&server, &config) == ESP_OK) {
        /* Agrega la ruta raíz al servidor */
        httpd_register_uri_handler(server, &root_uri_get);
        httpd_register_uri_handler(server, &root_uri_post);
    }
}

/* Detiene el servidor HTTP */
void stop_webserver(httpd_handle_t server)
{
    /* Detiene el servidor HTTP */
    httpd_stop(server);
}


