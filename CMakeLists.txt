# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/esp-aws-iot"
                         "esp-aws-iot/libraries/coreMQTT"
						 "esp-aws-iot/libraries/coreHTTP"
						 "esp-aws-iot/libraries/backoffAlgorithm"
						 "esp-aws-iot/libraries/common/posix_compat"
	)
project(HomeAssistant)


target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/certs/AmazonRootCA1.pem" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/certs/client.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "main/certs/client.key" TEXT)
