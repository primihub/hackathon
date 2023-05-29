
#include "utils.h"
/**
@brief read one layer .fp file  into int array

@param fm: opened fp file
@param lmeta: layer meta
@param t_layer_parameters: layer parameters
*/
void read_fp_parameters_layer(std::ifstream& fm,
                              Layer_meta lmeta,
                              Layer_parameter& t_layer_parameter) {
    TensorShape ts_w, ts_b;
    if (lmeta.cal_type == CAL_MATMUL_SP) {
        ts_w = lmeta.fcmeta.weight_shape;
        ts_b = TensorShape({lmeta.bias_len});
        int p_numels = ts_w.num_elements();
        int64_t* fp_weights = new int64_t[p_numels];
        fm.read((char*)fp_weights, sizeof(int64_t) * p_numels);
        t_layer_parameter.t_weights =
            Tensor<uint64_t>::Wrap((uint64_t*)fp_weights, ts_w);

        if (lmeta.bias_len > 0) {
            int64_t* fp_bias = new int64_t[ts_b.length()];
            fm.read((char*)fp_bias, sizeof(int64_t) * ts_b.length());
            t_layer_parameter.t_bias =
                Tensor<uint64_t>::Wrap((uint64_t*)fp_bias, ts_b);
        }
    
    } else if (lmeta.cal_type == CAL_LAYERNORM) {
        ts_w = TensorShape({lmeta.weights_len});
        ts_b = TensorShape({lmeta.bias_len});
        int p_numels = ts_w.num_elements();
        int64_t* fp_weights = new int64_t[p_numels];
        fp_weights = new int64_t[p_numels];
        fm.read((char*)fp_weights, sizeof(int64_t) * p_numels);
        t_layer_parameter.t_weights =
            Tensor<uint64_t>::Wrap((uint64_t*)fp_weights, ts_w);
            
        if (lmeta.bias_len > 0) {
            int64_t* fp_bias = new int64_t[ts_b.length()];
            fm.read((char*)fp_bias, sizeof(int64_t) * ts_b.length());
            t_layer_parameter.t_bias =
                Tensor<uint64_t>::Wrap((uint64_t*)fp_bias, ts_b);
        }
    }


    // verify
    // int verify_len = 10;
    // if (lmeta.cal_type == CAL_MATMUL_SP) {
    //     int cout_len = t_layer_parameter.t_weights.cols();
    //     cout_len = cout_len < verify_len ? cout_len : verify_len;
    //     for (size_t i = 0; i < cout_len; i++) {
    //         std::cout << (int64_t)t_layer_parameter.t_weights.data()[i] << " ";
    //     }
    //     std::cout << std::endl;
    //     if (lmeta.bias_len > 0) {
    //         int cout_len = t_layer_parameter.t_bias.length();
    //         cout_len = cout_len < verify_len ? cout_len : verify_len;
    //         for (size_t i = 0; i < cout_len; i++) {
    //             std::cout << (int64_t)t_layer_parameter.t_bias.data()[i] << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // } else if (lmeta.cal_type == CAL_LAYERNORM) {
    //     int cout_len = t_layer_parameter.t_weights.length();
    //     cout_len = cout_len < verify_len ? cout_len : verify_len;
    //     for (size_t i = 0; i < cout_len; i++) {
    //         std::cout << (int64_t)t_layer_parameter.t_weights.data()[i] << " ";
    //     }
    //     std::cout << std::endl;
    //     if (lmeta.bias_len > 0) {
    //         int cout_len = t_layer_parameter.t_bias.length();
    //         cout_len = cout_len < verify_len ? cout_len : verify_len;
    //         for (size_t i = 0; i < cout_len; i++) {
    //             std::cout << (int64_t)t_layer_parameter.t_bias.data()[i] << " ";
    //         }
    //         std::cout << std::endl;
    //     }
    // }
    
}
/**
@brief read .fp file to int array

@param in_model: a pytorch model, ".fp"
@param transformer_meta: meta from the ".mconf" file
@param t_transformer_parameters: parameters
*/
void read_fp_parameters_transformer(string in_file,
                               Transformer_meta transformer_meta,
                               Transformer_parameter& t_transformer_parameters) {
    std::cout << "read_fp_parameters" << std::endl;
    std::ifstream fm(in_file, std::ios::in | std::ios::binary);
    if (!fm) {
        std::cout << "cannot open " << in_file << std::endl;
    }
    t_transformer_parameters.t_encoder_parameters =
        new Layer_parameter*[transformer_meta.num_encoders];
    for (size_t n = 0; n < transformer_meta.num_encoders; n++) {
        t_transformer_parameters.t_encoder_parameters[n] =
            new Layer_parameter[transformer_meta.encoder_metas[n].num_layers];
        for (size_t i = 0; i < transformer_meta.encoder_metas[n].num_layers; i++) {
            read_fp_parameters_layer(fm, transformer_meta.encoder_metas[n].layer_metas[i],
                                     t_transformer_parameters.t_encoder_parameters[n][i]);
        }
    }
    t_transformer_parameters.t_pooler_parameters =
        new Layer_parameter[transformer_meta.pooler_meta.num_layers];
    for (size_t i = 0; i < transformer_meta.pooler_meta.num_layers; i++) {
        read_fp_parameters_layer(fm, transformer_meta.pooler_meta.layer_metas[i],
                                 t_transformer_parameters.t_pooler_parameters[i]);
    }
    t_transformer_parameters.t_fc_parameters =
        new Layer_parameter[transformer_meta.fc_meta.num_layers];
    for (size_t i = 0; i < transformer_meta.fc_meta.num_layers; i++) {
        read_fp_parameters_layer(fm, transformer_meta.fc_meta.layer_metas[i],
                                 t_transformer_parameters.t_fc_parameters[i]);
    }

    fm.close();
}

/**
@brief read .fp file to int array
@param in_file: a pytorch model, ".fp"
@param transformer_meta: meta from the ".mconf" file
@param t_data: data in tensor type
*/
void read_fp_data_transformer(string in_file,
                         Transformer_meta transformer_meta,
                         Tensor<uint64_t>& t_data) {
    std::cout << "read_fp_data" << std::endl;

    int p_numels = transformer_meta.encoder_metas[0].layer_metas[0].fcmeta.input_shape.num_elements();
    int64_t* fp_data = new int64_t[p_numels];
    std::ifstream fm(in_file, std::ios::in | std::ios::binary);
    if (!fm) {
        std::cout << "cannot open " << in_file << std::endl;
    }
    fm.read((char*)fp_data, sizeof(int64_t) * p_numels);
    t_data = Tensor<uint64_t>::Wrap(
        (uint64_t*)fp_data, transformer_meta.encoder_metas[0].layer_metas[0].fcmeta.input_shape);
    fm.close();

    // verify
    for (size_t j = 0; j < 100; j++) {
        std::cout << (int64_t)fp_data[j] << " ";
    }
    std::cout << std::endl;
}

/**
@brief generate zero array, in first layer, 
input is shared as input=input+0, server holds zeros
@param ts: TensorShape of input
@param t_data: data in tensor type
*/
void generate_data_zeors(TensorShape ts, Tensor<uint64_t>& t_data) {
    int data_len = ts.num_elements();
    uint64_t* data = new uint64_t[data_len];
    memset(data, 0, sizeof(uint64_t) * data_len);
    t_data = Tensor<uint64_t>::Wrap(data, ts);
}

/**
@brief because the functions in SCI/cheetah check the shapes in both SERVER and
CLIENT, the shapes in CLIENT are given corresponding to the meta but the
parameters are ignored since CLIENT doesn't know the parameters.
@param transformer_meta: the meta
@param t_transformer_parameters: the parameters
*/
void generate_parameters_shape(Transformer_meta transformer_meta,
                               Transformer_parameter& t_transformer_parameters) {
    t_transformer_parameters.t_encoder_parameters = new Layer_parameter*[transformer_meta.num_encoders];
    for (size_t n = 0; n < transformer_meta.num_encoders; n++) {
        t_transformer_parameters.t_encoder_parameters[n] = new Layer_parameter[transformer_meta.encoder_metas[n].num_layers];
    }
    for (size_t n = 0; n < transformer_meta.num_encoders; n++) {
        for (int i = 0; i < transformer_meta.encoder_metas[n].num_layers; i++) {
            if (transformer_meta.encoder_metas[n].layer_metas[i].cal_type == CAL_MATMUL_SP) {
                t_transformer_parameters.t_encoder_parameters[n][i].t_weights.Reshape(transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.weight_shape);
            
            } else if (transformer_meta.encoder_metas[n].layer_metas[i].cal_type == CAL_LAYERNORM) {
                t_transformer_parameters.t_encoder_parameters[n][i].t_weights.Reshape(TensorShape({transformer_meta.encoder_metas[n].layer_metas[i].weights_len}));
            }
        }
    }
    t_transformer_parameters.t_pooler_parameters = new Layer_parameter[transformer_meta.pooler_meta.num_layers];
    for (int i = 0; i < transformer_meta.pooler_meta.num_layers; i++) {
        if (transformer_meta.pooler_meta.layer_metas[i].cal_type == CAL_MATMUL_SP) {
            t_transformer_parameters.t_pooler_parameters[i].t_weights.Reshape(transformer_meta.pooler_meta.layer_metas[i].fcmeta.weight_shape);
        
        } else if (transformer_meta.pooler_meta.layer_metas[i].cal_type == CAL_LAYERNORM) {
            t_transformer_parameters.t_pooler_parameters[i].t_weights.Reshape(TensorShape({transformer_meta.pooler_meta.layer_metas[i].weights_len}));
        }
    }
    t_transformer_parameters.t_fc_parameters = new Layer_parameter[transformer_meta.pooler_meta.num_layers];
    for (int i = 0; i < transformer_meta.pooler_meta.num_layers; i++) {
        if (transformer_meta.pooler_meta.layer_metas[i].cal_type == CAL_MATMUL_SP) {
            t_transformer_parameters.t_fc_parameters[i].t_weights.Reshape(transformer_meta.pooler_meta.layer_metas[i].fcmeta.weight_shape);
        
        } else if (transformer_meta.pooler_meta.layer_metas[i].cal_type == CAL_LAYERNORM) {
            t_transformer_parameters.t_fc_parameters[i].t_weights.Reshape(TensorShape({transformer_meta.fc_meta.layer_metas[i].weights_len}));
        }
    }
}

/**
@brief read one layer in confile(config file) into a layer_meta
@param fc: opened confile
@param is_shared_input: true if is_shared else flase
@param lmeta: layer meta
*/
void read_conf_layer(FILE* fc, bool is_shared_input, Layer_meta&  lmeta) {
    int Co, Ci, C, H, W, numels, in_dims, in_sizes[3], padding;
    fscanf(fc, "%d", &lmeta.cal_type);
    switch (lmeta.cal_type) {
        case CAL_MATMUL_SP:
            fscanf(fc, "%d %d %d %d", &H, &W, &lmeta.bias_len,
                   &in_dims);
            for (size_t i = 0; i < in_dims; i++)
                fscanf(fc, "%d", &in_sizes[i]);
            lmeta.fcmeta.input_shape = TensorShape({in_sizes[0], in_sizes[1]});
            lmeta.fcmeta.weight_shape = TensorShape({H, W});
            lmeta.fcmeta.is_shared_input = is_shared_input;
            break;
        case CAL_MATMUL_SS:
            fscanf(fc, "%d %d %d", &H, &W, &lmeta.num_heads);
            lmeta.ishape = TensorShape({H, W});
            break;
        case CAL_LAYERNORM:
            fscanf(fc, "%d %d  %d", &lmeta.weights_len, &lmeta.bias_len, &in_dims);
            for (size_t i = 0; i < in_dims; i++)
                fscanf(fc, "%d", &in_sizes[i]);
            lmeta.ishape = TensorShape({in_sizes[0], in_sizes[1]});
            break;
        case CAL_SOFTMAX:
            fscanf(fc, "%d %d %d", &lmeta.num_words, &lmeta.num_heads,  &in_dims);
            for (size_t i = 0; i < in_dims; i++)
                fscanf(fc, "%d", &in_sizes[i]);
            lmeta.ishape = TensorShape({in_sizes[0], in_sizes[1], in_sizes[2]});
            break;
        case CAL_TANH:
            fscanf(fc, "%d", &in_dims);
            for (size_t i = 0; i < in_dims; i++)
                fscanf(fc, "%d", &in_sizes[i]);
            lmeta.ishape = TensorShape({in_sizes[0], in_sizes[1]});
            break;
        case CAL_GELU:
            fscanf(fc, "%d", &in_dims);
            for (size_t i = 0; i < in_dims; i++)
                fscanf(fc, "%d", &in_sizes[i]);
            lmeta.ishape = TensorShape({in_sizes[0], in_sizes[1]});
            break;

        default:
            break;
    }
}

/**
@brief read confile(config file) into Transformer_meta,
python=>confile=>Transformer_meta=>C++
@param params_conf: a model's config, ".mconf"
@param meta: Transformer_meta for computing in C++
*/
void read_conf_transformer(string params_conf, Transformer_meta& transformer_meta) {
    std::cout << "read_conf" << std::endl;
    FILE* fc;
    if ((fc = fopen(params_conf.c_str(), "r")) == NULL) {
        std::cout << "cannot open " << params_conf << std::endl;
    };
    fscanf(fc, "%d", &transformer_meta.num_encoders);
    transformer_meta.encoder_metas = new Meta[transformer_meta.num_encoders];
    for (size_t n = 0; n < transformer_meta.num_encoders; n++) {
        fscanf(fc, "%d", &transformer_meta.encoder_metas[n].num_layers);
        transformer_meta.encoder_metas[n].layer_metas = new Layer_meta[transformer_meta.encoder_metas[n].num_layers];
        for (size_t i = 0; i < transformer_meta.encoder_metas[n].num_layers; i++) {
            read_conf_layer(fc, 0, transformer_meta.encoder_metas[n].layer_metas[i]);
        }
    }

    fscanf(fc, "%d", &transformer_meta.pooler_meta.num_layers);
    transformer_meta.pooler_meta.layer_metas =
        new Layer_meta[transformer_meta.pooler_meta.num_layers];
    for (size_t i = 0; i < transformer_meta.pooler_meta.num_layers; i++) {
        read_conf_layer(fc, 0, transformer_meta.pooler_meta.layer_metas[i]);

    }
    fscanf(fc, "%d", &transformer_meta.fc_meta.num_layers);
    transformer_meta.fc_meta.layer_metas =
        new Layer_meta[transformer_meta.fc_meta.num_layers];
    for (size_t i = 0; i < transformer_meta.fc_meta.num_layers; i++) {
        read_conf_layer(fc, 0, transformer_meta.fc_meta.layer_metas[i]);

    }
    

    fclose(fc);
}


/**
@brief check one layer's confile(config file) in a layer_meta
@param lmeta: layer meta
*/
void check_conf_layer(Layer_meta lmeta) {
    
    switch (lmeta.cal_type) {
        case CAL_MATMUL_SP:
            std::cout << lmeta.cal_type << " " << lmeta.fcmeta.weight_shape.dim_size(0) << " "
                 << lmeta.fcmeta.weight_shape.dim_size(1) << " "
                 << lmeta.bias_len << " "
                 << lmeta.fcmeta.input_shape.dims() << " ";
            
            for (size_t i = 0; i < lmeta.fcmeta.input_shape.dims(); i++)
                std::cout<< lmeta.fcmeta.input_shape.dim_size(i) << " ";
            std::cout<<std::endl;
            break;
        case CAL_MATMUL_SS:
            std::cout << lmeta.cal_type << " " << lmeta.ishape.dim_size(0) << " "
                 << lmeta.ishape.dim_size(1) << " "
                 << lmeta.num_heads <<std::endl;
            break;
        case CAL_LAYERNORM:
            std::cout << lmeta.cal_type << " " << lmeta.weights_len << " "
                 << lmeta.bias_len << " "
                 << lmeta.ishape.dims() << " ";
                
            for (size_t i = 0; i < lmeta.ishape.dims(); i++)
                std::cout<< lmeta.ishape.dim_size(i) << " ";
            std::cout<<std::endl;
            break;
        case CAL_SOFTMAX:
            std::cout << lmeta.cal_type << " " << lmeta.num_words << " "
                 << lmeta.num_heads << " "
                 << lmeta.ishape.dims() << " ";
                
            for (size_t i = 0; i < lmeta.ishape.dims(); i++)
                std::cout<< lmeta.ishape.dim_size(i) << " ";
            std::cout<<std::endl;
            break;
        case CAL_TANH:
            std::cout << lmeta.cal_type << " " << lmeta.ishape.dims() << " ";
                
            for (size_t i = 0; i < lmeta.ishape.dims(); i++)
                std::cout<< lmeta.ishape.dim_size(i) << " ";
            std::cout<<std::endl;
            break;
        case CAL_GELU:
            std::cout << lmeta.cal_type << " " << lmeta.ishape.dims() << " ";
                
            for (size_t i = 0; i < lmeta.ishape.dims(); i++)
                std::cout<< lmeta.ishape.dim_size(i) << " ";
            std::cout<<std::endl;
            break;

        default:
            break;
    }
}

/**
@brief check confile(config file) in Transformer_meta
@param meta: Transformer_meta for computing in C++
*/
void check_conf_transformer(Transformer_meta transformer_meta){
    int num_encoders = transformer_meta.num_encoders;
    std::cout<< num_encoders<< " "<<std::endl;
    for (size_t n = 0; n < num_encoders; n++)
    {
        Meta meta = transformer_meta.encoder_metas[n];
        int num_layers = meta.num_layers;
        std::cout<< num_layers<< " "<<std::endl;
        for (size_t i = 0; i < num_layers; i++)
        {
            check_conf_layer(meta.layer_metas[i]);
        }
        
    }
    std::cout<< transformer_meta.pooler_meta.num_layers<< " "<<std::endl;
    for (size_t i = 0; i < transformer_meta.pooler_meta.num_layers; i++)
    {
        check_conf_layer(transformer_meta.pooler_meta.layer_metas[i]);
    }
    std::cout<< transformer_meta.fc_meta.num_layers<< " "<<std::endl;
    for (size_t i = 0; i < transformer_meta.fc_meta.num_layers; i++)
    {
        check_conf_layer(transformer_meta.fc_meta.layer_metas[i]);
    }
    
    
}


void print_array(uint64_t* data,
                 int shift,
                 int channels,
                 int rows,
                 int cols,
                 int nums_in_one_row){

    int c_inds = rows * cols;
    if(nums_in_one_row>cols) nums_in_one_row=cols;
    for (size_t c = 0; c < channels; c++)
    {
        for (int i = 0; i < rows; i++) {
            for (size_t j = 0; j < nums_in_one_row; j++)
            {
                std::cout << (((double)((int64_t)data[c*c_inds + i*cols + j])) / (1 << shift)) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

/**
@brief verify a result, just means shares addition(i.e. a=a0+a1)
and also a>>15 to compare with python result

@param data: data in Tensor type
@param party: {Alice, Bob}, Bob sends his shares to Alice and Alice add it
to construct
@param io: the IO used to communication
@param shift: print data element>>shift
@param channels: 3 dims as C*H*W
@param rows: 3 dims as C*H*W
@param cols: 3 dims as C*H*W
@param nums_in_one_row: numbers in one row, to get a better view of matrix
*/
void test_verify(uint64_t* data,
                 int data_len,
                 int party,
                 sci::NetIO* io,
                 int shift,
                 int channels,
                 int rows,
                 int cols,
                 int nums_in_one_row) {
    uint64_t moduloMask = prime_mod - 1;
    if (party == sci::BOB) {
        io->send_data(data, sizeof(uint64_t) * data_len);
    } else  // party == ALICE
    {
        std::cout<<"verify: "<<"shape"<<"("<<channels<<", "<<rows<<", "<<cols<<")"<<std::endl;
        uint64_t* data_real = new uint64_t[data_len];
        io->recv_data(data_real, sizeof(uint64_t) * data_len);
        int verify_len = channels*rows*cols;
        for (int i = 0; i < data_len; i++) {
            // if (i < verify_len) {
            //     std::cout << party << " ";
            //     std::cout << data_real[i] << " ";
            //     std::cout << data[i] << " ";
            // }
            data_real[i] = (data_real[i] + data[i]) & moduloMask;
            if (data_real[i] > prime_mod / 2) {
                data_real[i] -= prime_mod;
            }
            // if (i < verify_len)
            //     std::cout << (data_real[i]) << " "
            //          << (((double)((int64_t)data_real[i])) / (1 << shift)) << std::endl;
        }
        print_array(data_real,shift,channels,rows,cols,nums_in_one_row);
    }

    io->flush();
}

/**
@brief verify a result, just means shares addition(i.e. a=a0+a1)
and also a>>shift to compare with python result

@param data: data in Tensor type
@param party: {Alice, Bob}, Bob sends his shares to Alice and Alice add it
to reconstruct
@param io: the IO used to communication
@param shift: print data element>>shift
@param channels: 3 dims as C*H*W
@param rows: 3 dims as C*H*W
@param cols: 3 dims as C*H*W
@param nums_in_one_row: numbers in one row, to get a better view of matrix
*/
void test_verify(Tensor<uint64_t> data,
            int party,
            sci::NetIO* io,
            int shift,
            int channels,
            int rows,
            int cols,
            int nums_in_one_row){
    test_verify(data.data(), 
                data.NumElements(), 
                party, 
                io, 
                shift, 
                channels,
                rows,
                cols,
                nums_in_one_row);
}


