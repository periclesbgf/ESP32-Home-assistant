idf_component_register(SRCS "src/main.c" "src/gpio.c" "src/dht11.c" "src/inmp441.c"
                            "src/wifi.c" "src/tcp_client_v4.c" "src/udp_client.c"
                            "src/webserver.c" "src/mqtt_demo_mutual_auth.c" "src/tf_utils.cc"
                    INCLUDE_DIRS "." "./include"
)

#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)

#target_compile_options(${COMPONENT_LIB} PRIVATE
#-Wno-maybe-uninitialized
#-Wno-missing-field-initializers
#-Wno-error=sign-compare
#-Wno-error=double-promotion
#-Wno-type-limits)