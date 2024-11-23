idf_component_register(SRCS "src/main.c" "src/gpio.c" "src/dht11.c" "src/inmp441.c"
                            "src/wifi.c" "src/tcp_client_v4.c" "src/udp_client.c"
                            "src/webserver.c" "src/webserver.c" "src/mqtt_client_eden.c" "src/voice.c"
                    INCLUDE_DIRS "." "./include"
                    REQUIRES esp_wifi driver esp_http_server spiffs esp_http_client esp-tls mqtt esp-tls libpeer)
