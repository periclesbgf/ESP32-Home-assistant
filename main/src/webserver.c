#include "webserver.h"
#include "wifih.h"
#include "utils.h"

int sema5 = 0;

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/

static const char *TAG = "wifi softAP";


// Função para converter um caractere hexadecimal em decimal
static int hex2int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// Função para decodificar uma string codificada em URL
static void url_decode(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a'-'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a'-'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';

            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

const char *html = "<!DOCTYPE html><html><body>"
                   "<h1>Bem vindo(a) a configuracao do Eden, seu tutor!</h1>"
                   "<form action=\"/submit\" method=\"post\">"
                   "<label for=\"ssid\">SSID:</label><br>"
                   "<input type=\"text\" id=\"ssid\" name=\"ssid\"><br>"
                   "<label for=\"password\">Password:</label><br>"
                   "<input type=\"password\" id=\"password\" name=\"password\"><br><br>"
                   "<input type=\"submit\" value=\"Submit\">"
                   "</form>"
                   "</body></html>";

const char *html_form = "<!DOCTYPE html><html><body>"
                        "<h1>Configuracao da Rede</h1>"
                        "<form action=\"/submit\" method=\"post\">"
                        "Nome da rede:<br>"
                        "<input type=\"text\" name=\"ssid\"><br>"
                        "Senha da rede:<br>"
                        "<input type=\"password\" name=\"password\"><br><br>"
                        "<input type=\"submit\" value=\"Confirmar\">"
                        "</form>"
                        "</body></html>";


void store_credentials(const char *ssid, const char *password) 
{
    // Abre o namespace NVS
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("wifi_config", NVS_READWRITE, &nvs_handle));

    // Armazena as credenciais no NVS
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "password", password));

    // Fecha o namespace NVS
    nvs_commit(nvs_handle);
    nvs_close(nvs_handle);
}

// Handler para o método POST que recebe os dados do formulário
esp_err_t post_handler(httpd_req_t *req)
{
    char* buf;
    size_t buf_len;

    // Obtém o comprimento do conteúdo da requisição
    buf_len = req->content_len;

    // Aloca memória para armazenar o conteúdo da requisição
    buf = malloc(buf_len + 1);
    if (!buf) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Lê o conteúdo enviado pelo formulário
    int ret = httpd_req_recv(req, buf, buf_len);
    if (ret <= 0) { // Se houve erro ou está vazio
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            // Timeout
            httpd_resp_send_408(req);
        }
        free(buf);
        return ESP_FAIL;
    }

    buf[ret] = '\0'; // Null-terminate whatever we received and treat it like a string

    char ssid[32] = {0};
    char password[64] = {0};

    // Extrai os dados para as variáveis ssid e password
    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password, sizeof(password));

    // Decodifica a senha
    char decodedPassword[64] = {0}; // Certifique-se de que este array seja grande o suficiente para armazenar o resultado decodificado
    url_decode(decodedPassword, password);

    ESP_LOGI(TAG, "SSID recebido: %s", ssid);
    ESP_LOGI(TAG, "Senha recebida: %s", decodedPassword);

    store_credentials(ssid, decodedPassword);

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s", ssid, password);

    sema5 = 1;

    free(buf);

    // Envie uma resposta ao cliente
    const char* resp = "configuracao recebida com sucesso!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    // Aqui você poderia, por exemplo, configurar o SSID e a senha
    // com as novas informações recebidas

    return ESP_OK;
}

esp_err_t get_handler(httpd_req_t *req) {
    if (strcmp(req->uri, "/submit") == 0) {
        httpd_resp_send(req, html_form, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_handler,
    .user_ctx = NULL
};

httpd_uri_t uri_post = {
    .uri = "/submit",
    .method = HTTP_POST,
    .handler = post_handler,
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 4444;  // Define a porta do servidor para 4444

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registra o manipulador para requisições GET
        httpd_register_uri_handler(server, &uri_get);

        // Registra o manipulador para requisições POST
        httpd_register_uri_handler(server, &uri_post);
    } else {
        ESP_LOGI(TAG, "Erro ao iniciar o servidor web");
    }
    return server;
}

void stop_webserver(httpd_handle_t server) {
    ESP_LOGI(TAG, "PARANDO O WEBSERVER");
    while (sema5 == 1)
    {
        httpd_stop(server);
    }
}

esp_err_t wifi_init_softap(void)
{
    ESP_LOGI(TAG, "wifi_init_softap");

    esp_err_t status = ESP_FAIL;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);

    httpd_handle_t server = start_webserver();

    while (sema5 != 1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    httpd_stop(server);
    return ESP_OK;
}
