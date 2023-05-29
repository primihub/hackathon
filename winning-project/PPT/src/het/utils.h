#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#include "library_fixed.h"
#include "library_fixed_uniform.h"

#include "cheetah/cheetah-api.h"
#include "gemini/cheetah/tensor.h"
#include "gemini/cheetah/tensor_shape.h"

// using namespace std;
using namespace seal;
using namespace sci;
using namespace gemini;

struct Layer_parameter {
    Tensor<uint64_t> t_weights;
    Tensor<uint64_t> t_bias;
};

struct Transformer_parameter {
    Layer_parameter** t_encoder_parameters;
    Layer_parameter* t_pooler_parameters;
    Layer_parameter* t_fc_parameters;
};

#define CAL_MATMUL_SP 0
#define CAL_MATMUL_SS 1
#define CAL_LAYERNORM 2
#define CAL_SOFTMAX 3
#define CAL_TANH 4
#define CAL_GELU 5



struct Layer_meta {
    int cal_type;                  // defined CAL_TYPE_XX
    CheetahLinear::FCMeta fcmeta;  // for CAL_MATMUL_SP
    int weights_len;               // for layernorm
    int bias_len;                  // 0 if no
    int scale_type = 1;            // just for linear layers, 0 if no
    int num_heads;
    int num_words;
    TensorShape ishape;  // just for non-linear
};
struct Meta {
    int num_layers;
    Layer_meta* layer_metas;
};

struct Transformer_meta {
    int num_encoders;
    Meta* encoder_metas;
    Meta pooler_meta;
    Meta fc_meta;
};

void generate_data_zeors(TensorShape ts, Tensor<uint64_t>& t_data);
void generate_parameters_shape(Transformer_meta transformer_meta,
                               Transformer_parameter& Transformer_parameter);

void read_fp_data_transformer(string in_file,
                              Transformer_meta transformer_meta,
                              Tensor<uint64_t>& t_data);

void read_fp_parameters_layer(std::ifstream& fm,
                              Layer_meta lmeta,
                              Layer_parameter& t_layer_parameter);
void read_fp_parameters_transformer(
    string in_file,
    Transformer_meta transformer_meta,
    Transformer_parameter& t_Transformer_parameters);

void read_conf_layer(FILE* fc, bool is_shared_input, Layer_meta& lmeta);
void read_conf_transformer(string params_conf,
                           Transformer_meta& transformer_meta);
void check_conf_layer(Layer_meta lmeta);
void check_conf_transformer(Transformer_meta PSPNetMeta);

void print_array(uint64_t* data,
                 int shift,
                 int channels = 1,
                 int rows = 1,
                 int cols = 10,
                 int nums_in_one_row = 10);

void test_verify(uint64_t* data,
                 int data_len,
                 int party,
                 sci::NetIO* io,
                 int shift,
                 int channels = 1,
                 int rows = 1,
                 int cols = 10,
                 int nums_in_one_row = 10);
void test_verify(Tensor<uint64_t> data,
                 int party,
                 sci::NetIO* io,
                 int shift,
                 int channels = 1,
                 int rows = 1,
                 int cols = 10,
                 int nums_in_one_row = 10);
