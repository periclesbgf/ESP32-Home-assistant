#include "tensorflow/lite/experimental/microfrontend/lib/kiss_fft_int16.h"
#include "tensorflow/lite/experimental/microfrontend/lib/fft.h"
#include "tensorflow/lite/experimental/microfrontend/lib/fft_util.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/kernels/conv.h"
#include "tensorflow/lite/micro/kernels/fully_connected.h"
#include "tensorflow/lite/micro/kernels/softmax.h"

#include "model_data.h"

#include "tf_utils.h"
#include <iostream>

#ifdef __cplusplus
extern "C++" {
#endif

constexpr int kTensorArenaSize = 1024 * 256;  // Exemplo: 256KB
uint8_t tensor_arena[kTensorArenaSize];


void imprime() {
    std::cout << "Olá, Mundo!";
}
#if 0
    layers.Input(shape=input_shape),
    layers.Resizing(32, 32), # Redimensiona a imagem (pixels)
    norm_layer,
    //layers.Conv2D(32, 3, activation='relu'), # Etapa de convolução, afim de extrair recursos locais dos espectrogramas
    //layers.Conv2D(64, 3, activation='relu'), # Etapa de convolução, afim de extrair recursos locais dos espectrogramas
    //layers.MaxPooling2D((2,2)), # Reduz a resolução espacial do espectrograma
    //layers.Dropout(0.25), # Reduzindo a probabilidade de overfitting
    //layers.Flatten(), # Converte o formato de dados em uma matriz unidimensional
    //layers.Dense(128, activation='relu'), # Modela as relações entre os recursos extraídos
    //layers.Dropout(0.5),
    //layers.Dense(num_labels, activation='softmax', name='output_layer')
#endif

void create_model()
{
    constexpr int kMaxOpRegistrations = 10;  // Ajuste conforme necessário
    tflite::MicroMutableOpResolver<kMaxOpRegistrations> op_resolver;

    op_resolver.AddConv2D();
    op_resolver.AddMaxPool2D();
    op_resolver.AddFullyConnected();
    op_resolver.AddSoftmax();


    const tflite::Model* model = ::tflite::GetModel(saved_modelo_optimizado_tflite); // Modelo convertido

    if (model->version() != TFLITE_SCHEMA_VERSION) {
    // Tratar incompatibilidade de versão do modelo
    }

    tflite::ErrorReporter error_reporter;
    uint8_t tensor_arena[kTensorArenaSize];

    tflite::MicroInterpreter interpreter(model, op_resolver, tensor_arena, kTensorArenaSize, &error_reporter);

    // Aloque tensores
    TfLiteStatus allocate_status = interpreter.AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        // Tratar falha na alocação de tensores
        return -1;
    }

    // Execute inferência com o modelo (passando os dados de entrada pelo modelo e produz os resultados de saída)
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        // Trate falha na execução da inferência
        return -1;
    }

    // Faça algo com as saídas do modelo

    return 0;
}



#ifdef __cplusplus
}
#endif

