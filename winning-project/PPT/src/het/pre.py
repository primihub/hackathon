
from random import random
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset,DataLoader
from torch import device, optim
import numpy as np
import cv2
import os
import struct
import time

from importlib import import_module
import sys
sys.path.append('.')
from models.bert import Config
UTF = "UTF-8"
config = Config("THUCNews")
PAD, CLS = '[PAD]', '[CLS]'

filter_precision = 16
SCALE_NUM = 2**filter_precision

from pytorch_pretrained.modeling import BertEmbeddings, BertConfig


def pre_data(sth: str ,pad_size=32):
    content = sth.strip()
    token = config.tokenizer.tokenize(content)
    token = [CLS] + token
    seq_len = len(token)
    mask = []
    token_ids = config.tokenizer.convert_tokens_to_ids(token)
    if pad_size:
        if len(token) < pad_size:
            mask = [1] * len(token_ids) + [0] * (pad_size - len(token))
            token_ids += ([0] * (pad_size - len(token)))
        else:
            mask = [1] * pad_size
            token_ids = token_ids[:pad_size]
            seq_len = pad_size
    input_ids = torch.LongTensor([token_ids,]).to(config.device)
    token_type_ids = None
    attention_mask = torch.LongTensor([mask,]).to(config.device)
    output_all_encoded_layers=False
    if attention_mask is None:
        attention_mask = torch.ones_like(input_ids)
    if token_type_ids is None:
        token_type_ids = torch.zeros_like(input_ids)
    extended_attention_mask = attention_mask.unsqueeze(1).unsqueeze(2)
    extended_attention_mask = extended_attention_mask.to(dtype=torch.float32) # fp16 compatibility
    extended_attention_mask = (1.0 - extended_attention_mask) * -10000.0

    return input_ids,token_type_ids

def split_embedding(emb_path):
    config = Config("THUCNews")
    model_name = "bert"
    x = import_module('models.' + model_name)
    model = x.Model(config).to(config.device)
    model.load_state_dict(torch.load(config.save_path, map_location=torch.device('cpu')))
    embeddings = model.bert.embeddings
    torch.save(embeddings.state_dict(), emb_path)


def embedding_and_to_fp(emb_path, input_text, out_file, out_config=None):
    config_file = "bert_pretrain/bert_config.json"
    config  = BertConfig.from_json_file(config_file)
    embeddings = BertEmbeddings(config)
    embeddings.load_state_dict(torch.load(emb_path, map_location=torch.device('cpu')))
    embeddings.eval()
    input_ids,token_type_ids = pre_data(input_text)
    embedding_output = embeddings(input_ids, token_type_ids)

    with open(out_file,"w") as fm:
        embedding_output = SCALE_NUM*embedding_output
        numel = embedding_output.numel()
        embedding_output = embedding_output.round().long().numpy()
        size = embedding_output.shape
        embedding_output = embedding_output.reshape(-1)
        print(size,numel,embedding_output.shape[0],embedding_output[:10],embedding_output.dtype)
        embedding_output.tofile(fm)



def trans_pytorch_parameters_transformer(out_model, out_config=None):
    ''' trans a pytorch model to binary form of fixed-point integer
    paramaters:
    out:
        string out_model: out model, ".fp"
        string out_model: out model config, ".mconf", show name_size of the model's layers
    '''
    
    config = Config("THUCNews")
    # padding符号, bert中综合信息符号
    PAD, CLS = '[PAD]', '[CLS]'
    model_name = "bert"
    x = import_module('models.' + model_name)
    model = x.Model(config).to(config.device)
    model.load_state_dict(torch.load(config.save_path, map_location=torch.device('cpu')))
    # model = model.eval()

    if(out_config is None):
        out_config=out_model[:-3]+".mconf"
        
    print(out_model, out_config)
    
    sd = model.state_dict()
    with open(out_model,"w") as fm:
        for name,params in sd.items():
            if("encoder" in name or "pooler" in name or "fc" in name):
                params = SCALE_NUM*params
                numel = params.numel()
                params = params.round().long().numpy()
                size = params.shape
                params = params.reshape(-1)
                print(name,size,numel,params.shape[0],params[:10],params.dtype)
                params.tofile(fm)

    MATMUL_SP = 0
    MATMUL_SS = 1
    LAYERNORM = 2
    SOFTMAX = 3
    TANH = 4
    GELU = 5

    num_encoders = len(model.bert.encoder.layer)
    w_qkv = model.bert.encoder.layer[0].attention.self.query.weight.shape
    b_qkv = model.bert.encoder.layer[0].attention.self.query.bias.shape
    with open(out_config,"w") as fc:
        fc.write(f"{num_encoders}\n")
        for n in range(num_encoders):
            num_layers = 12
            fc.write(f"{num_layers}\n")
            for i in range(num_layers):
                if(i<3):
                    fc.write(f"{MATMUL_SP} {w_qkv[0]} {w_qkv[1]} {b_qkv[0]} {2} {32} {768}")
                elif(i==3):
                    fc.write(f"{MATMUL_SS} {768} {768} {12}")
                elif(i==4):
                    fc.write(f"{SOFTMAX} {32} {12} {3} {12} {32} {32}")
                elif(i==5):
                    fc.write(f"{MATMUL_SS} {768} {768} {12}")
                elif(i==6):
                    fc.write(f"{MATMUL_SP} {768} {768} {768} {2} {32} {768}")
                elif(i==7):
                    fc.write(f"{LAYERNORM} {768} {768} {2} {32} {768}")
                elif(i==8):
                    fc.write(f"{MATMUL_SP} {3072} {768} {3072} {2} {32} {768}")
                elif(i==9):
                    fc.write(f"{GELU} {2} {32} {3072}")
                elif(i==10):
                    fc.write(f"{MATMUL_SP} {768} {3072} {768} {2} {32} {3072}")
                elif(i==11):
                    fc.write(f"{LAYERNORM} {768} {768} {2} {32} {768}")
                fc.write("\n")
        fc.write(f"{2}\n")
        fc.write(f"{MATMUL_SP} {768} {768} {768} {2} {1} {768}\n")
        fc.write(f"{TANH} {2} {1} {768}\n")
        fc.write(f"{1}\n")
        fc.write(f"{MATMUL_SP} {10} {768} {10} {2} {1} {768}\n")
        

if __name__=="__main__":
    split_embedding("emb.pth")
    trans_pytorch_parameters_transformer("het/models/transformer.fp")
    input = "女超联赛重启 上海女足三球大胜"
    embedding_and_to_fp("emb.pth",input,"in.fp")