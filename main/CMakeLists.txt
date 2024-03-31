idf_component_register(SRCS "src/main.c" "src/gpio.c" "src/dht11.c" "src/inmp441.c"
                            "src/wifi.c" "src/tcp_client_v4.c" "src/udp_client.c"
                            "src/webserver.c" "src/mqtt_demo_mutual_auth.c" "src/voice.c" "esp-aws-iot"
                    INCLUDE_DIRS "." "./include" "esp-aws-iot"
                    REQUIRES esp_wifi driver esp_http_server spiffs esp_http_client esp-tls mqtt esp-tls)
