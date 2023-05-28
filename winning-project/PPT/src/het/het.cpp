#pragma once

#include <fstream>
#include <iostream>
#include <thread>

#define SCI_OT 1
#define USE_CHEETAH 1

#include "utils.h"
#include "torch/script.h"

// using namespace std;
using namespace seal;
using namespace sci;
using namespace gemini;

int party = 0;
int bitlength = 41;
int num_threads = 4;
int port = 32000;
string address = "127.0.0.1";
int num_rows = 256;
int current_dim = 784;
int filter_precision = 16;

int bw = 41;
int shift = 16;
int num_relu = 256;
int num_relu_orig = 256;
int b = 4;
PRG128 prg;
int emb_len = 15;
int ver = 0;

auto torch_shift = torch::tensor({1<<16},torch::kDouble);
auto variance_epsilon = torch::tensor({1e-12},torch::kDouble);
int one_party = 1;

/**
 * @brief HE matrix mul, y(num_vec,W)=w(H,W)[x(num_vec,W)],
   note the input is generally save as row-vector, the weight is generally save
 as col-vector, i.e.(H,W), fc is done as y = x*(W.T), where y is also row-vector
 * @param H: H(height) of w
 * @param W: W(width) of w
 * @param num_vec: num of vectors, each vector's size is the same with
 * @param w: weight flatten, length=H*W
 * @param x: input flatten, length=num_vec*W
 * @param y: output flatten, length=num_vec*H
*/
void matmul_sp(int H,
               int W,
               int num_vec,
               uint64_t* w,
               uint64_t* x,
               uint64_t*& y,
               bool is_shared_input) {
    CheetahLinear::FCMeta meta;
    TensorShape weight_shape({H, W});
    TensorShape input_shape({num_vec, W});
    meta.input_shape = TensorShape({W});
    meta.weight_shape = weight_shape;
    meta.is_shared_input = is_shared_input;
    y = new uint64_t[num_vec * H];
    Tensor<uint64_t> t_w = Tensor<uint64_t>::Wrap(w, weight_shape);
    for (long r = 0; r < num_vec; ++r) {
        uint64_t* input_row = x + r * W;
        Tensor<uint64_t> t_input_vec =
            Tensor<uint64_t>::Wrap(input_row, meta.input_shape);
        Tensor<uint64_t> out_vec;
        cheetah_linear->fc(t_input_vec, t_w, meta, out_vec);
        std::copy_n(out_vec.data(), H, y + r * H);
    }
}

void test_fc() {
    CheetahLinear::FCMeta fcmeta;
    int in_shape, H, W;
    TensorShape ts_in, ts_w;
    uint64_t *x, *w;
    Tensor<uint64_t> t_x, t_w;

    int in_shapes[] = {768, 768, 768, 768, 3072};
    int Hs[] = {64, 768, 768 * 3, 3072, 768};
    int* Ws = in_shapes;

    for (size_t n = 0; n < 5; n++) {
        in_shape = in_shapes[n];
        H = Hs[n], W = Ws[n];
        ts_in = TensorShape({in_shape});
        ts_w = TensorShape({H, W});
        x = new uint64_t[in_shape];
        w = new uint64_t[H * W];
        for (size_t i = 0; i < in_shape; i++)
            x[i] = i;
        for (size_t i = 0; i < H; i++)
            for (size_t j = 0; j < W; j++)
                w[i * W + j] = i + j;
        t_x = Tensor<uint64_t>::Wrap(x, ts_in);
        t_w = Tensor<uint64_t>::Wrap(w, ts_w);
        fcmeta.input_shape = ts_in;
        fcmeta.weight_shape = ts_w;
        fcmeta.is_shared_input = false;
        INIT_TIMER;
        START_TIMER;
        Tensor<uint64_t> t_y;
        cheetah_linear->fc(t_x, t_w, fcmeta, t_y);

        printf("(%d, %d) * (%d)", H, W, in_shape);
        STOP_TIMER("test_fc");
    }
}

void test_matmul_sp() {
    CheetahLinear::FCMeta fcmeta;
    int in_shape, H, W;
    TensorShape ts_in, ts_w;
    uint64_t *x, *w, *y;
    Tensor<uint64_t> t_x, t_w;

    int in_shapes[] = {768, 768, 768, 768, 3072};
    int Hs[] = {64, 768, 768 * 3, 3072, 768};
    int* Ws = in_shapes;
    int num_vec = 32;

    for (size_t n = 0; n < 5; n++) {
        H = Hs[n], W = Ws[n];
        ts_in = TensorShape({in_shape});
        ts_w = TensorShape({H, W});
        x = new uint64_t[num_vec * W];
        w = new uint64_t[H * W];
        for (size_t i = 0; i < num_vec; i++)
            for (size_t j = 0; j < W; j++)
                x[i * W + j] = i + j;
        for (size_t i = 0; i < H; i++)
            for (size_t j = 0; j < W; j++)
                w[i * W + j] = i + j;

        INIT_TIMER;
        START_TIMER;
        matmul_sp(H, W, num_vec, w, x, y, false);
        printf("(%d, %d) * (%d, %d)", num_vec, W, H, W);
        STOP_TIMER("test_fc");
    }
}


/**
 * @brief matrix mul in plain, y(num_vec,H)=x(num_vec,W)w.T(W,H), 
 * note the input is generally save as row-vector, the weight is
 * generally save as col-vector, i.e.(H,W), linear is done as y = x*(W.T), 
 * where y is also row-vector
 * @param H: H(height) of w
 * @param W: W(width) of w
 * @param num_vec: num of vectors, each vector's size is the same with W
 * @param w: weight flatten, length=H*W
 * @param b: bias flatten, length=H
 * @param x: input flatten, length=num_vec*W
 * @param y: output flatten, length=num_vec*H
*/
void linear(int H, int W, int num_vec, uint64_t* w, uint64_t* b, uint64_t* x, uint64_t*& y) {

    auto torch_w = torch::from_blob(w,{H,W},torch::kInt64);
    auto torch_b = torch::from_blob(b,{H},torch::kInt64);
    auto torch_x = torch::from_blob(x,{num_vec,W},torch::kInt64);
    auto torch_y = torch::matmul(torch_x,torch_w.t());
    torch_y = torch_y.bitwise_right_shift(shift);
    torch_y = torch_y + torch_b;
    y = new uint64_t[torch_y.numel()];
    std::copy_n((uint64_t*)torch_y.data_ptr(), torch_y.numel(), y);
}
/**
 * @brief het one layer
 * @param t_in: input in Tensor type
 * @param lmeta: layer meta
 * @param t_layer_parameter: layer parameter
 * @param t_out: output in Tensor type
 */
void het_layer_one_party(Tensor<uint64_t> t_in,
               Layer_meta lmeta,
               Layer_parameter t_layer_parameter,
               Tensor<uint64_t>& t_out) {
    uint64_t mask_module = prime_mod - 1;
    // if (party == 1)
    //     std::cout << "lmeta.cal_type: " << lmeta.cal_type << std::endl;
    uint64_t* out;
    switch (lmeta.cal_type) {
        case CAL_MATMUL_SP: {
            if(one_party){
                linear(lmeta.fcmeta.weight_shape.dim_size(0),
                        lmeta.fcmeta.weight_shape.dim_size(1), lmeta.fcmeta.input_shape.dim_size(0),
                        t_layer_parameter.t_weights.data(),
                        t_layer_parameter.t_bias.data(), t_in.data(), out);
            }else{
                matmul_sp(lmeta.fcmeta.weight_shape.dim_size(0),
                        lmeta.fcmeta.weight_shape.dim_size(1), lmeta.fcmeta.input_shape.dim_size(0),
                        t_layer_parameter.t_weights.data(), t_in.data(), out,
                        lmeta.fcmeta.is_shared_input);
            }
            // std::cout << lmeta.num_words <<std::endl;
            t_out = Tensor<uint64_t>::Wrap(out, 
                        TensorShape({lmeta.fcmeta.input_shape.dim_size(0),lmeta.fcmeta.weight_shape.dim_size(0)}));
            break;
        }

        default:
            break;
    }

    // verify
    if(one_party)
        print_array(t_out.data(), shift);
    else
        test_verify(t_out, party, io, shift);
}


/**
 * @brief torch Tensor to gemini Tensor
 * @param torch_tensor: torch Tensor
 * @param torch_shift: gemini::FC uses fixed-point number in BFV, this is: int(float-point * torch_shift) = fixed-point
 */
Tensor<uint64_t> torch_to_gemini(torch::Tensor torch_tensor, torch::Tensor torch_shift=torch_shift){
    auto t_data_pre = (torch_tensor*torch_shift).toType(torch::kInt64).contiguous();
    uint64_t* t_data_pre2 = new uint64_t[t_data_pre.numel()];
    std::copy_n((uint64_t*)t_data_pre.data_ptr(), t_data_pre.numel(), t_data_pre2);
    TensorShape ts;
    if(torch_tensor.dim()==1){
        ts = TensorShape({torch_tensor.size(0)});
    }else if(torch_tensor.dim()==2){
        ts = TensorShape({torch_tensor.size(0),torch_tensor.size(1)});
    }else if(torch_tensor.dim()==3){
        ts = TensorShape({torch_tensor.size(0),torch_tensor.size(1),torch_tensor.size(2)});
    }
    return Tensor<uint64_t>::Wrap((uint64_t*)t_data_pre2,ts);
    
}

/**
 * @brief torch Tensor to gemini Tensor
 * @param torch_tensor: torch Tensor
 * @param torch_shift: gemini::FC uses fixed-point number in BFV, this is: int(float-point * torch_shift) = fixed-point
 */
torch::Tensor gemini_to_torch(Tensor<uint64_t> t_tensor, torch::Tensor torch_shift=torch_shift){
    torch::IntArrayRef shape;
    if(t_tensor.dims()==1){
        return torch::from_blob(t_tensor.data(),{t_tensor.dim_size(0)},torch::kInt64)/torch_shift;
    }else if(t_tensor.dims()==2){
        return torch::from_blob(t_tensor.data(),{t_tensor.dim_size(0),t_tensor.dim_size(1)},torch::kInt64)/torch_shift;
    }else if(t_tensor.dims()==3){
        return torch::from_blob(t_tensor.data(),{t_tensor.dim_size(0),t_tensor.dim_size(1),t_tensor.dim_size(2)},torch::kInt64)/torch_shift;
    }
    
}

/**
 * @brief comm
 */
void comm(int sender, uint64_t* data, int data_len){
    uint64_t moduloMask = prime_mod - 1;
    if(party == sender){
        io->send_data(data,sizeof(uint64_t)*data_len);
    }else{
        uint64_t* data_comm = new uint64_t[data_len];
        io->recv_data(data_comm,sizeof(uint64_t)*data_len);
        for (int i = 0; i < data_len; i++) {
            data[i] = (data[i] + data_comm[i]) & moduloMask;
            if (data[i] > prime_mod / 2) {
                data[i] -= prime_mod;
            }
        }
        delete[] data_comm;
    }
}


/** @brief state
 * 
 */
void fstate(int state){
    FILE* fp;
    if(state==1){
        fp = fopen("state.txt","a+");
    }else{
        fp = fopen("state.txt","w");
    }
    fprintf(fp,"%d ", state);
    fclose(fp);

}

/**
 * @brief het one layer
 * @param t_in: input in Tensor type
 * @param lmeta: layer meta
 * @param t_layer_parameter: layer parameter
 * @param t_out: output in Tensor type
 */
void het_layer(Tensor<uint64_t> t_in,
               Layer_meta lmeta,
               Layer_parameter t_layer_parameter,
               Tensor<uint64_t>& t_out) {
    uint64_t mask_module = prime_mod - 1;
    //if (party == 2) {std::cout << "lmeta.cal_type: " << lmeta.cal_type << std::endl;}
    uint64_t* out;
    switch (lmeta.cal_type) {
        case CAL_MATMUL_SP: {
            // std::cout<<party<<" " <<t_layer_parameter.t_weights.dims()<<" "
            //  <<t_layer_parameter.t_weights.dim_size(0)<<" "
            //  <<t_layer_parameter.t_weights.dim_size(1)<<std::endl;
            // std::cout<<party<<" " <<t_in.dims()<<" "
            //  <<t_in.dim_size(0)<<" "
            //  <<t_in.dim_size(1)<<std::endl;
            //  std::cout<<party<<" " <<t_layer_parameter.t_weights.data()[0]<<" "
            //  <<t_in.data()[0]<<" "
            //  <<t_layer_parameter.t_bias.data()[0]<<std::endl;

            matmul_sp(lmeta.fcmeta.weight_shape.dim_size(0),
                    lmeta.fcmeta.weight_shape.dim_size(1), lmeta.fcmeta.input_shape.dim_size(0),
                    t_layer_parameter.t_weights.data(), t_in.data(), out,
                    lmeta.fcmeta.is_shared_input);
            //io
            comm(1,out,lmeta.fcmeta.input_shape.dim_size(0)*lmeta.fcmeta.weight_shape.dim_size(0));
            if(party==2){
                auto torch_y = torch::from_blob(out,{lmeta.fcmeta.input_shape.dim_size(0),lmeta.fcmeta.weight_shape.dim_size(0)},torch::kInt64)/torch_shift/torch_shift;
                auto torch_b = gemini_to_torch(t_layer_parameter.t_bias);
                auto torch_out = torch_y + torch_b;
                t_out = torch_to_gemini(torch_out);
            }
            
            break;
        }
        
        default:
            break;
    }

    // verify
    // if(party==2){print_array(t_out.data(), shift);}
}



/**
 * @brief het
 */
void het(int embedding_len) {
    if (party == 2) {if(ver){fstate(0);}}
    Transformer_meta transformer_meta;
    Tensor<uint64_t> t_data;
    Transformer_parameter t_transformer_parameters;
    read_conf_transformer("het/models/transformer.mconf",
                          transformer_meta);
    
    string labels[] = {
        "finance",
        "realty",
        "stocks",
        "education",
        "science",
        "society",
        "politics",
        "sports",
        "game",
        "entertainment"
    };
    // read parameters and data

    if (party == sci::ALICE) {
        read_fp_parameters_transformer(
            "het/models/transformer.fp",
            transformer_meta, t_transformer_parameters);
        generate_data_zeors(
            transformer_meta.encoder_metas[0].layer_metas[0].fcmeta.input_shape,
            t_data);

    } else {  // BOB
        read_fp_data_transformer("het/in.fp",
                                transformer_meta, t_data);
        read_fp_parameters_transformer(
            "het/models/transformer.fp",
            transformer_meta, t_transformer_parameters);
        // generate_parameters_shape(transformer_meta, t_transformer_parameters);
        embedding_len = 15;
    }

    StartComputation();

    // encoder
    int num_encoders = transformer_meta.num_encoders;
    int num_heads = 12;
    int num_emb_dims = 768;
    int num_words = 32;
    int dk = 64;
    Tensor<uint64_t> t_encoder_res;

    torch::Tensor embeddings = torch::zeros({1,1,32});
    for(int j = embedding_len ; j < 32;j++){
        embeddings[0][0][j] = -10000.0;
    }

    Tensor<uint64_t> t_y6, t_y7, t_y8,t_y9,t_y10,t_y12;
    
    int cur = 2;
    for (size_t n = 0; n < num_encoders; n++) {

        // std::cout<<" t_data 0 "<<n<<std::endl;
        // print_array(t_data.data(),1<<16,1,32,768,768);

        // if (party == 1)
            // std::cout << "encoder n: " << n << std::endl;
        int num_layers = transformer_meta.encoder_metas[n].num_layers;
        Tensor<uint64_t>* t_y = new Tensor<uint64_t>[num_layers + 1];
        int i = 0;
        if(party==1){
            generate_data_zeors(
                transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.input_shape,
                t_data);
        }
        t_y[0] = t_data;
        // in fact, these three matmul can be done as matmul_qkv
        
        // {
        //     i = 0;
        //     if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
        //     het_layer(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
        //                 t_transformer_parameters.t_encoder_parameters[n][i],
        //                 t_y[i + 1]);
            
        //     i = 1;
        //     if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
        //     het_layer(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
        //                 t_transformer_parameters.t_encoder_parameters[n][i],
        //                 t_y[i + 1]);
        //     i = 2;
        //     if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
        //     het_layer(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
        //                 t_transformer_parameters.t_encoder_parameters[n][i],
        //                 t_y[i + 1]);
        // }
        
        if (party == 2) {if(ver){fstate(1);};}
        for (; i < 3; i++)
        {
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;}
            het_layer(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
                    t_transformer_parameters.t_encoder_parameters[n][i],
                    t_y[i+1]);
        }
        if(party==2){
            
            if(ver){fstate(cur++);}
            auto torch_Q = torch::from_blob(t_y[1].data(),{num_words,num_emb_dims},torch::kInt64)/torch_shift;
            auto torch_K = torch::from_blob(t_y[2].data(),{num_words,num_emb_dims},torch::kInt64)/torch_shift;
            auto torch_V = torch::from_blob(t_y[3].data(),{num_words,num_emb_dims},torch::kInt64)/torch_shift;

            auto torch_q = torch_Q.view({num_words,num_heads,64});
            auto torch_qs = torch_q.permute({1,0,2});

            // std::cout<<" qs "<<n<<std::endl;
            // std::cout << torch_qs.sizes() <<std::endl;
            // std::cout << torch_qs <<std::endl;

            auto torch_k = torch_K.view({num_words,num_heads,64});
            auto torch_ks = torch_k.permute({1,0,2});

            // std::cout<<" ks "<<n<<std::endl;
            // std::cout << torch_ks.sizes() <<std::endl;
            // std::cout << torch_ks <<std::endl;

            auto torch_v = torch_V.view({num_words,num_heads,64});
            auto torch_vs = torch_v.permute({1,0,2});

            // std::cout<<" vs "<<n<<std::endl;
            // std::cout << torch_vs.sizes() <<std::endl;
            // std::cout << torch_vs <<std::endl;
            
            // 由于矩阵相乘，每个值从左移16位变为左移32位，故在单步进行位移，防止数值过大
            i = 3;
            auto torch_attention_scores = torch::matmul(torch_qs,torch_ks.permute({0,2,1}));
            // if (party == 2) {std::cout << "torch_attention_scores: " << i << std::endl;print_array(torch_to_gemini(torch_attention_scores).data(),shift);}

            // 分母
            auto torch_denominator = torch::tensor({8.0},torch::kDouble);
            auto attention_scores = torch_attention_scores/torch_denominator;
            auto torch_y4 = attention_scores + embeddings;
            
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(torch_to_gemini(torch_y4).data(),shift);}
            // std::cout<<" torch_y4 "<<n<<std::endl;
            // std::cout<<torch_y4.sizes()<<std::endl;
            // std::cout<<torch_y4<<std::endl;

            i = 4;
            auto torch_y5 = torch::softmax(torch_y4,2);
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(torch_to_gemini(torch_y5).data(),shift);}

            // std::cout<<" torch_y5 "<<n<<std::endl;
            // std::cout<<torch_y5.sizes()<<std::endl;
            // std::cout<<torch_y5<<std::endl;

            i = 5;
            
            auto torch_y6 = torch::matmul(torch_y5,torch_vs);
            torch_y6 = torch_y6.permute({1,0,2}).contiguous();
            torch_y6 = torch_y6.view({num_words,num_emb_dims});

            // std::cout<<" torch_y6 "<<n<<std::endl;
            // std::cout<<torch_y6.sizes()<<std::endl;
            // std::cout<<torch_y6<<std::endl;
            
            t_y6 = torch_to_gemini(torch_y6);
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(t_y6.data(),shift);}
        }
        i = 6;
        // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;}
        if(party==1){
            generate_data_zeors(
                transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.input_shape,
                t_y6);
        }
        // 输入： torch_y6
        //auto torch_Q = torch::from_blob(t_y[1].data(),{num_words,num_emb_dims},torch::kInt64);
        
        if (party == 2) {if(ver){fstate(1);};}
        het_layer(t_y6,transformer_meta.encoder_metas[n].layer_metas[i],
            t_transformer_parameters.t_encoder_parameters[n][i],t_y7);
        
        torch::Tensor torch_y8;
        if(party==2){
            if(ver){fstate(cur++);}
            i = 7;
            // LayerNorm  输入： torch_y7,t_y[0]
            auto torch_y7 = gemini_to_torch(t_y7);
            // std::cout<<" torch_y7 "<<n<<std::endl;
            // std::cout<<torch_y7.sizes()<<std::endl;
            // std::cout<<torch_y7<<std::endl;
            auto torch_y0 = gemini_to_torch(t_y[0]);
            auto x = torch_y0+torch_y7;
            auto u = x.mean(-1,true);
            auto s = (x - u).pow(2).mean(-1, true);
            x = (x - u) / (s + variance_epsilon).sqrt();
            auto torch_w = gemini_to_torch(t_transformer_parameters.t_encoder_parameters[n][i].t_weights); 
            auto torch_b = gemini_to_torch(t_transformer_parameters.t_encoder_parameters[n][i].t_bias); 
            // 点乘
            torch_y8 = torch_w*x+torch_b;
            t_y8 = torch_to_gemini(torch_y8);
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(t_y8.data(),shift);}
        }
        
        // std::cout<<" torch_y8 "<<n<<std::endl;
        // std::cout<<torch_y8.sizes()<<std::endl;
        // std::cout<<torch_y8<<std::endl;
           
        i = 8; 
        // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;}
        if(party==1){
            generate_data_zeors(
                transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.input_shape,
                t_y8);
        }
        
        if (party == 2) {if(ver){fstate(1);};}
        het_layer(t_y8,transformer_meta.encoder_metas[n].layer_metas[i],
            t_transformer_parameters.t_encoder_parameters[n][i],t_y9);
        // 输入： torch_y7

        if(party==2){
            if(ver){fstate(cur++);}
            i = 9;
            auto torch_y9 = gemini_to_torch(t_y9);
            auto torch_y10 = torch::gelu(torch_y9);
            t_y10 = torch_to_gemini(torch_y10);
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(t_y10.data(),shift);}
        }

        // std::cout<<" torch_y9 "<<n<<std::endl;
        // std::cout<<torch_y9.sizes()<<std::endl;
        // std::cout<<torch_y9<<std::endl;

        i = 10;
        // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;}
        if(party==1){
            generate_data_zeors(
                transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.input_shape,
                t_y10);
        }
        transformer_meta.encoder_metas[n].layer_metas[i].fcmeta.is_shared_input = 0;
        Tensor<uint64_t> t_y11;
        if (party == 2) {if(ver){fstate(1);};}
        het_layer(t_y10,transformer_meta.encoder_metas[n].layer_metas[i],
            t_transformer_parameters.t_encoder_parameters[n][i],t_y11);

        if(party==2){
            if(ver){fstate(cur++);}
            i = 11;
            auto torch_y11 = gemini_to_torch(t_y11);
            // 输入： torch_y10,torch_y8
            auto x = torch_y11+torch_y8;
            auto u = x.mean(-1,true);
            auto s = (x - u).pow(2).mean(-1, true);
            x = (x - u) / (s + variance_epsilon).sqrt();
            auto torch_w = gemini_to_torch(t_transformer_parameters.t_encoder_parameters[n][i].t_weights); 
            auto torch_b = gemini_to_torch(t_transformer_parameters.t_encoder_parameters[n][i].t_bias); 
            // 点乘
            auto torch_y12 = torch_w*x+torch_b;
            t_y12 = torch_to_gemini(torch_y12);
            t_data = t_y12;
            // if (party == 2) {std::cout << "encoder n i: " << n << " " << i << std::endl;print_array(t_y12.data(),shift);}
        }

        // std::cout<<" torch_y10 "<<n<<std::endl;
        // std::cout<<torch_y10.sizes()<<std::endl;
        // std::cout<<torch_y10<<std::endl;

        
        
        // std::cout<<" torch_y11 "<<n<<std::endl;
        // std::cout<<torch_y11.sizes()<<std::endl;
        // std::cout<<torch_y11<<std::endl;

        // 将 t_data 从 torch::kFloat 转为 Tensor<uint64_t>
        // std::cout<<" t_data 1 "<<n<<std::endl;
        // print_array(t_data.data(),1<<shift,1,num_words,num_emb_dims,num_emb_dims);
        
    }
    // std::cout<<" encoder:"<<std::endl;
    // print_array(t_data.data(),1<<shift,1,num_words,num_emb_dims,num_emb_dims);
    // std::cout<<std::endl;

    // pooler
    Tensor<uint64_t> t_pooler1, t_pooler2;
    {
        int num_layers = transformer_meta.pooler_meta.num_layers;
        int i = 0;
        // if (party == 2) {std::cout << "pooler i: "<< " " << i << std::endl;}
        if(party==1){
            generate_data_zeors(
                transformer_meta.pooler_meta.layer_metas[i].fcmeta.input_shape,
                t_data);
        }
        if (party == 2) {if(ver){fstate(1);};}
        het_layer(t_data, transformer_meta.pooler_meta.layer_metas[i], 
            t_transformer_parameters.t_pooler_parameters[i],t_pooler1);
        // 输入： torch_y7
        

        if(party==2){
            if(ver){fstate(cur++);}
            i = 1;
            auto torch_pooler1 = gemini_to_torch(t_pooler1);
            auto torch_pooler2 = torch::tanh(torch_pooler1);
            t_pooler2 = torch_to_gemini(torch_pooler2);
            t_data = t_pooler2;
            // if (party == 2) {std::cout << "pooler i: " << i << std::endl;print_array(t_pooler2.data(),shift);}
        }
        // print_array(t_data.data(),1<<shift,1,1,num_emb_dims,num_emb_dims);
    }

    // fc
    {
        int num_layers = transformer_meta.fc_meta.num_layers;
        int i = 0;
        // if (party == 2) {std::cout << "fc i: " << i << std::endl;}
        if(party==1){
            generate_data_zeors(
                transformer_meta.pooler_meta.layer_metas[i].fcmeta.input_shape,
                t_data);
        }
        Tensor<uint64_t> t_fc1;
        if (party == 2) {if(ver){fstate(1);};}
        het_layer(t_data, transformer_meta.fc_meta.layer_metas[i], 
            t_transformer_parameters.t_fc_parameters[i],t_fc1);
        
        
        // if (party == 2) {std::cout << "inference ok: " << std::endl;}
        if(party==2){
            if(ver){fstate(cur++);}
            auto torch_fc1 = gemini_to_torch(t_fc1);
            std::cout<<torch_fc1.sizes()<<std::endl;
            std::cout<<torch_fc1<<std::endl;
            
            auto res = torch::argmax(torch_fc1[0]).item().toInt();
            // std::cout<<res<<std::endl;
            std::cout<<"label: "<<labels[res]<<std::endl;
            
            FILE* fp = fopen("result.txt","w");
            fprintf(fp,"%d", res);
            fclose(fp);
            fstate(-1);
        }
        
        
    }
}

/**
 * @brief het
 */
void het_one_party() {
    Transformer_meta transformer_meta;
    Tensor<uint64_t> t_data;
    Transformer_parameter t_transformer_parameters;
    read_conf_transformer("het/models/transformer.mconf",
                          transformer_meta);
    int embedding_len = 0 ; 
    string labels[] = {
        "finance",
        "realty",
        "stocks",
        "education",
        "science",
        "society",
        "politics",
        "sports",
        "game",
        "entertainment"
    };
    // read parameters and data
    if(one_party){
        read_fp_parameters_transformer(
            "het/models/transformer.fp",
            transformer_meta, t_transformer_parameters);
        read_fp_data_transformer("het/in.fp",
                                 transformer_meta, t_data);
        embedding_len = 15;
    }
    else{
        if (party == sci::ALICE) {
            read_fp_parameters_transformer(
                "het/models/transformer.fp",
                transformer_meta, t_transformer_parameters);
            generate_data_zeors(
                transformer_meta.encoder_metas[0].layer_metas[0].fcmeta.input_shape,
                t_data);
            embedding_len = 15;

        } else {  // BOB
            read_fp_data_transformer("het/in.fp",
                                    transformer_meta, t_data);
            generate_parameters_shape(transformer_meta, t_transformer_parameters);
        }

        StartComputation();
    }

    // encoder
    int num_encoders = transformer_meta.num_encoders;
    Tensor<uint64_t> t_encoder_res;

    // 初始准备
    torch::Tensor embeddings = torch::zeros({1,1,32});
    for(int j = embedding_len ; j < 32;j++){
        embeddings[0][0][j] = -10000.0;
    }
    auto torch_shift = torch::tensor({1<<16});

    for (size_t n = 0; n < num_encoders; n++) {

        // std::cout<<" t_data 0 "<<n<<std::endl;
        // print_array(t_data.data(),1<<16,1,32,768,768);

        // if (party == 1)
            // std::cout << "encoder n: " << n << std::endl;
        int num_layers = transformer_meta.encoder_metas[n].num_layers;
        Tensor<uint64_t>* t_y = new Tensor<uint64_t>[num_layers + 1];
        t_y[0] = t_data;
        int i = 0;
        // in fact, these three matmul can be done as matmul_qkv
        
        {
            i = 0;
            if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
            het_layer_one_party(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
                        t_transformer_parameters.t_encoder_parameters[n][i],
                        t_y[i + 1]);
            
            i = 1;
            if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
            het_layer_one_party(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
                        t_transformer_parameters.t_encoder_parameters[n][i],
                        t_y[i + 1]);
            i = 2;
            if (party == 2){std::cout << "encoder n i: " << n << " " << i << std::endl;}
            het_layer_one_party(t_y[0], transformer_meta.encoder_metas[n].layer_metas[i],
                        t_transformer_parameters.t_encoder_parameters[n][i],
                        t_y[i + 1]);
        }
        i = 3;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        auto torch_Q = torch::from_blob(t_y[1].data(),{32,768},torch::kInt64);
        auto torch_K = torch::from_blob(t_y[2].data(),{32,768},torch::kInt64);
        auto torch_V = torch::from_blob(t_y[3].data(),{32,768},torch::kInt64);

        auto torch_q = torch_Q.view({32,12,64});
        auto torch_qs = torch_q.permute({1,0,2});

        // std::cout<<" qs "<<n<<std::endl;
        // std::cout << torch_qs.sizes() <<std::endl;
        // std::cout << torch_qs <<std::endl;

        auto torch_k = torch_K.view({32,12,64});
        auto torch_ks = torch_k.permute({1,0,2});

        // std::cout<<" ks "<<n<<std::endl;
        // std::cout << torch_ks.sizes() <<std::endl;
        // std::cout << torch_ks <<std::endl;

        auto torch_v = torch_V.view({32,12,64});
        auto torch_vs = torch_v.permute({1,0,2});

        // std::cout<<" vs "<<n<<std::endl;
        // std::cout << torch_vs.sizes() <<std::endl;
        // std::cout << torch_vs <<std::endl;
        
        // if(party==1)
        //     io->send_data(torch_Q.data_ptr(),sizeof(uint64_t)*torch_Q.numel());
        // else
        //     uint64_t* rec;
        //     io->recv_data(rec,sizeof(uint64_t)*torch_Q.numel());
        
        i = 4;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // 由于矩阵相乘，每个值从左移16位变为左移32位，故在单步进行位移，防止数值过大
        auto torch_attention_scores = torch::matmul(torch_qs/torch_shift,torch_ks.permute({0,2,1})/torch_shift);

        // 理论应为sqrtf64(double(transformer_meta.encoder_metas[n].layer_metas->num_heads))，这里直接用8了
        // 分母
        auto torch_denominator = torch::tensor({8.0},torch::kDouble);
        auto attention_scores = torch_attention_scores/torch_denominator;
        auto torch_y4 = attention_scores + embeddings;
        // std::cout<<" torch_y4 "<<n<<std::endl;
        // std::cout<<torch_y4.sizes()<<std::endl;
        // std::cout<<torch_y4<<std::endl;

        i = 5;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        auto torch_y5 = torch::softmax(torch_y4,2);

        // std::cout<<" torch_y5 "<<n<<std::endl;
        // std::cout<<torch_y5.sizes()<<std::endl;
        // std::cout<<torch_y5<<std::endl;

        i = 6;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        
        auto torch_y6 = torch::matmul(torch_y5,torch_vs/torch_shift);
        torch_y6 = torch_y6.permute({1,0,2}).contiguous();
        torch_y6 = torch_y6.view({32,768});

        // std::cout<<" torch_y6 "<<n<<std::endl;
        // std::cout<<torch_y6.sizes()<<std::endl;
        // std::cout<<torch_y6<<std::endl;

        i = 7;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // 输入： torch_y6
        //auto torch_Q = torch::from_blob(t_y[1].data(),{32,768},torch::kInt64);
        auto torch_w = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_weights.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0),
            transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(1)},
            torch::kInt64
        );
        auto torch_b = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_bias.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0)},
            torch::kInt64
        );
        auto torch_y7 = torch::matmul(torch_y6,torch_w.t()/torch_shift)+(torch_b/torch_shift);

        // std::cout<<" torch_y7 "<<n<<std::endl;
        // std::cout<<torch_y7.sizes()<<std::endl;
        // std::cout<<torch_y7<<std::endl;
        
        i = 8;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // LayerNorm  输入： torch_y7,t_y[0]
        // 得到的结果即为attention_output
        auto t_y0 = torch::from_blob(t_y[0].data(),{32,768},torch::kInt64)/torch_shift;
        auto x = t_y0+torch_y7;
        auto u = x.mean(-1,true);
        auto s = (x - u).pow(2).mean(-1, true);
        x = (x - u) / (s + variance_epsilon).pow(0.5);
        torch_w = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_weights.data(),
            {768},
            torch::kInt64
        );
        torch_b = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_bias.data(),
            {768},
            torch::kInt64
        );
        // 点乘
        auto torch_y8 = (torch_w/torch_shift)*x+(torch_b/torch_shift);
        
        // std::cout<<" torch_y8 "<<n<<std::endl;
        // std::cout<<torch_y8.sizes()<<std::endl;
        // std::cout<<torch_y8<<std::endl;
        
        i = 9;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // 输入： torch_y7
        torch_w = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_weights.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0),
            transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(1)},
            torch::kInt64
        );
        torch_b = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_bias.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0)},
            torch::kInt64
        );
        auto torch_y9 = torch::matmul(torch_y8,torch_w.t()/torch_shift)+(torch_b/torch_shift);
        torch_y9 = torch::gelu(torch_y9);

        // std::cout<<" torch_y9 "<<n<<std::endl;
        // std::cout<<torch_y9.sizes()<<std::endl;
        // std::cout<<torch_y9<<std::endl;

        i = 10;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // 输入： torch_y9
        torch_w = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_weights.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0),
            transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(1)},
            torch::kInt64
        );
        torch_b = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_bias.data(),
            {transformer_meta.encoder_metas[n].layer_metas[i-1].fcmeta.weight_shape.dim_size(0)},
            torch::kInt64
        );
        auto torch_y10 = torch::matmul(torch_y9,torch_w.t()/torch_shift)+(torch_b/torch_shift);

        // std::cout<<" torch_y10 "<<n<<std::endl;
        // std::cout<<torch_y10.sizes()<<std::endl;
        // std::cout<<torch_y10<<std::endl;

        i = 11;
        // if (party == 1)
            // std::cout << "encoder n i: " << n << " " << i << std::endl;
        // 输入： torch_y10,torch_y8
        x = torch_y10+torch_y8;
        u = x.mean(-1,true);
        s = (x - u).pow(2).mean(-1, true);
        x = (x - u) / (s + variance_epsilon).pow(0.5);
        torch_w = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_weights.data(),
            {768},
            torch::kInt64
        );
        torch_b = torch::from_blob(
            t_transformer_parameters.t_encoder_parameters[n][i-1].t_bias.data(),
            {768},
            torch::kInt64
        );
        // 点乘
        auto torch_y11 = (torch_w/torch_shift)*x+(torch_b/torch_shift);
        
        // std::cout<<" torch_y11 "<<n<<std::endl;
        // std::cout<<torch_y11.sizes()<<std::endl;
        // std::cout<<torch_y11<<std::endl;

        // 将 t_data 从 torch::kFloat 转为 Tensor<uint64_t>
        auto t_data_pre = (torch_y11*torch_shift).toType(torch::kInt64).contiguous();
        uint64_t* t_data_pre2 = new uint64_t[t_data_pre.numel()];
        std::copy_n((uint64_t*)t_data_pre.data_ptr(), t_data_pre.numel(), t_data_pre2);
        t_data = Tensor<uint64_t>::Wrap((uint64_t*)t_data_pre2,TensorShape({32,768}));

        // std::cout<<" t_data 1 "<<n<<std::endl;
        // print_array(t_data.data(),1<<16,1,32,768,768);
        
    }
    // std::cout<<" encoder:"<<std::endl;
    // print_array(t_data.data(),1<<16,1,32,768,768);
    // std::cout<<std::endl;

    // pooler
    {
        int num_layers = transformer_meta.pooler_meta.num_layers;
        int i = 0;

        auto torch_pooler = torch::from_blob(t_data.data(),{32,768},torch::kInt64);

        auto torch_w = torch::from_blob(
            t_transformer_parameters.t_pooler_parameters[i].t_weights.data(),
            {transformer_meta.pooler_meta.layer_metas[i].fcmeta.weight_shape.dim_size(0),
            transformer_meta.pooler_meta.layer_metas[i].fcmeta.weight_shape.dim_size(1)},
            torch::kInt64
        );

        auto torch_b = torch::from_blob(
            t_transformer_parameters.t_pooler_parameters[i].t_bias.data(),
            {transformer_meta.pooler_meta.layer_metas[i].fcmeta.weight_shape.dim_size(0)},
            torch::kInt64
        );

        auto torch_pooler1 = torch::matmul(torch_pooler[0]/torch_shift,torch_w.t()/torch_shift)+(torch_b/torch_shift);

        // std::cout<<" torch_pooler1:"<<std::endl;
        // std::cout<<torch_pooler1.sizes()<<std::endl;
        // std::cout<<torch_pooler1<<std::endl;

        i = 1;
        auto torch_pooler2 = torch::tanh(torch_pooler1);

        // std::cout<<" torch_pooler2:"<<std::endl;
        // std::cout<<torch_pooler2.sizes()<<std::endl;
        // std::cout<<torch_pooler2<<std::endl;
        // std::cout<<std::endl;

        // 类型转换
        auto t_data_pre = (torch_pooler2*torch_shift).toType(torch::kInt64).contiguous();
        uint64_t* t_data_pre2 = new uint64_t[t_data_pre.numel()];
        std::copy_n((uint64_t*)t_data_pre.data_ptr(), t_data_pre.numel(), t_data_pre2);
        t_data = Tensor<uint64_t>::Wrap((uint64_t*)t_data_pre2,TensorShape({32,768}));
        // print_array(t_data.data(),1<<16,1,1,768,768);
    }

    // fc
    {
        int num_layers = transformer_meta.fc_meta.num_layers;
        int i = 0;

        auto torch_fc = torch::from_blob(t_data.data(),{768},torch::kInt64);

        auto torch_w = torch::from_blob(
            t_transformer_parameters.t_fc_parameters[i].t_weights.data(),
            {transformer_meta.fc_meta.layer_metas[i].fcmeta.weight_shape.dim_size(0),
            transformer_meta.fc_meta.layer_metas[i].fcmeta.weight_shape.dim_size(1)},
            torch::kInt64
        );

        auto torch_b = torch::from_blob(
            t_transformer_parameters.t_fc_parameters[i].t_bias.data(),
            {transformer_meta.fc_meta.layer_metas[i].fcmeta.weight_shape.dim_size(0)},
            torch::kInt64
        );

        auto torch_fc1 = torch::matmul(torch_fc/torch_shift,torch_w.t()/torch_shift)+(torch_b/torch_shift);
        // std::cout<<torch_fc1.sizes()<<std::endl;
        // std::cout<<torch_fc1<<std::endl;

        auto res = torch::argmax(torch_fc1, 0);
        // std::cout<<res<<std::endl;
        
    }
}

void test_libtorch(){

    std::cout << "Hello, World!" << std::endl;
    
    auto a = torch::tensor({{1, 2}, {3, 4}});
    std::cout << a << std::endl;
}


void test_read() {
    Transformer_meta transformer_meta;
    read_conf_transformer("het/models/transformer.mconf",
                          transformer_meta);
    check_conf_transformer(transformer_meta);
    Transformer_parameter t_transformer_parameters;
    read_fp_parameters_transformer(
        "het/models/transformer.fp", transformer_meta,
        t_transformer_parameters);
    Tensor<uint64_t> t_data;
    read_fp_data_transformer("het/in.fp",
                             transformer_meta, t_data);
}

int main(int argc, char** argv) {
    ArgMapping amap;
    amap.arg("r", party, "Role of party: ALICE = 1; BOB = 2");
    amap.arg("p", port, "Port Number");
    amap.arg("fp", filter_precision, "Filter Precision");
    amap.arg("ip", address, "IP Address of server (ALICE)");
    amap.arg("l", bitlength, "Bitlength of inputs");
    amap.arg("e", emb_len, "emb_len");
    amap.arg("v", ver, "verify info");
    amap.parse(argc, argv);

    // test_fc();
    // test_matmul_sp();
    // test_read();
    // test_libtorch();
    // test_linear_and_matmul();
    INIT_TIMER;
    START_TIMER;
    // het_one_party();
    het(emb_len);
    STOP_TIMER("het");
    

    return 0;
}