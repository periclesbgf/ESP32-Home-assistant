idf_component_register(SRCS "src/main.c" "src/gpio.c" "src/dht11.c" "src/inmp441.c"
                            "src/wifi.c" "src/tcp_client_v4.c" "src/udp_client.c"
                            "src/webserver.c" "src/mqtt_client_eden.c"
                    INCLUDE_DIRS "." "./include")

