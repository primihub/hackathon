from concurrent.futures import ThreadPoolExecutor
import time
import socket
from importlib import import_module
import sys
sys.path.append('.')
from models.bert import Config
import numpy as np
import json 
import torch
from hashlib import sha256
import random
import math


OP = "OP"
DATA = "data"
UTF = "UTF-8"
config = Config("THUCNews")
# padding符号, bert中综合信息符号
PAD, CLS = '[PAD]', '[CLS]'
IP = '127.0.0.1'
PORT = 12546
BUFLEN = 419430400
bit_len = 1026#>1024
q = 359538626972463181545861038157804946723595395788461314546860162315465351611001926265416954644815072042240227759742786715317579537628833244985694861278948248755535786849730970552604439202492188238906165904170011537676301364684925762947826221081654474326701021369172596479894491876959432609670712659248451897847
phi = q-1
scale = pow(2,20)
n = 21128
k = 32
d = 768
classes = [
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
    ]

offline_time = 0
online_time = 0


with open("./c_x_new.json",'r') as f:
    json_msg = json.load(f)
c0 = json_msg['c']
pt0 = json_msg['pt']
x0 = json_msg['x']
with open("./a.json",'r') as f:
    json_msg = json.load(f)
a0 = json_msg['a']
a_0 = json_msg['a_']


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

    return input_ids,extended_attention_mask

def H1(x):
    res = int(sha256(x.to_bytes(((len(hex(x))-2+ 2-1) //2),"big")).hexdigest()*4,16)%q
    if res == 0:
        return 1
    return res
    

def H2(x):
    return int(sha256(x.to_bytes(((len(hex(x))-2+ 2-1) //2),"big")).hexdigest()[-8:],16)


def rand_phi_q():
    while(True):
        s = random.randint(2,q-2)
        if(math.gcd(s,phi)==1):
            return s


def s_offline_main():
    s_offline_main_time = time.time()
    model_name = "bert"
    x = import_module('models.' + model_name)
    model = model = x.Model(config).to(config.device)
    model.load_state_dict(torch.load(config.save_path, map_location=torch.device('cpu')))
    model = model.eval()

    # 获得三个embedding层
    Tw = model.bert.embeddings.word_embeddings.state_dict()['weight'].numpy()
    Tp = model.bert.embeddings.position_embeddings.state_dict()['weight'].numpy()
    Tt = model.bert.embeddings.token_type_embeddings.state_dict()['weight'].numpy()

    print("SER>> 离线计算")
    time1 = time.time()
    
    R1 = random.randint(0,(1<<32)-1)/scale
    R2 = random.randint(0,(1<<32)-1)/scale
    w = ((Tw + R1)*scale).astype(np.int64) 
    pt = Tp[:32] + Tt[0] + R2
    x = random.randint(1,q-1)
    ids = [i for i in range(n)]
    h2 = [0]*n
    pstep = 100
    h2 = list(map(lambda i: H2(pow(H1(i),x,q)), ids))
    h2 = np.array(h2)
    c = (w.T ^ h2).T 
    with open("./test.json",'w') as f:
        json.dump({'c':c.tolist(),'x':x,'pt':pt.tolist()},f)
    
    
    s_offline_main_time = time.time()-s_offline_main_time
    print(f"SER>> 离线计算主要消耗时间{s_offline_main_time:.4f}s")
    
    
    


def s_offline():
    global offline_time
    

    offline_time = time.time()
    model_name = "bert"
    x = import_module('models.' + model_name)
    model = model = x.Model(config).to(config.device)
    model.load_state_dict(torch.load(config.save_path, map_location=torch.device('cpu')))
    model = model.eval()

    # 获得三个embedding层
    Tw = model.bert.embeddings.word_embeddings.state_dict()['weight'].numpy()
    Tp = model.bert.embeddings.position_embeddings.state_dict()['weight'].numpy()
    Tt = model.bert.embeddings.token_type_embeddings.state_dict()['weight'].numpy()

    print("SER>> 离线计算")
    time1 = time.time()
    
    R1 = random.randint(0,(1<<32)-1)/scale
    R2 = random.randint(0,(1<<32)-1)/scale
    w = ((Tw + R1)*scale).astype(np.int64) 
    pt = Tp[:32] + Tt[0] + R2
    x = random.randint(1,q-1)
    ids = [i for i in range(n)]
    h2 = [0]*n
    pstep = 100
    h2 = list(map(lambda i: H2(pow(H1(i),x,q)), ids))
    h2 = np.array(h2)
    c = (w.T ^ h2).T 
    with open("./test.json",'w') as f:
        json.dump({'c':c.tolist(),'x':x,'pt':pt.tolist()},f)

    # c = c0
    # pt = pt0

    s_offline_cal_time = time.time() - time1
    print(f"SER>> 离线计算消耗时间{s_offline_cal_time:.4f}s")
    
    ListenSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ListenSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    ListenSocket.bind((IP, PORT))
    num_client = 1
    ListenSocket.listen(num_client)
    print(f"SER>> 服务器启动成功，在{PORT}端口等待客户端连接...")
    DataSocket, addr = ListenSocket.accept()
    print("SER>> 接收一个客户端连接：", addr)
    print("SER>> offline S=>R: {c, pt}")
    time1 = time.time()
    tosend = json.dumps({'c':c.tolist(),'pt':pt.tolist()})
    DataSocket.send(tosend.encode(UTF))
    s_offline_com_time = time.time() - time1
    
    print(f"SER>> 离线通信消耗时间{s_offline_com_time:.4f}s")
    print(f"SER>> 离线运行时间{s_offline_cal_time+s_offline_com_time:.4f}s")
    print(f"SER>> 离线运行结束")
    
    DataSocket.close()
    ListenSocket.close()

def c_offline():
    global offline_time
    DataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        try:
            DataSocket.connect((IP, PORT))
            break
        except:
            continue
    print("CNT>> 连接到服务器")
    
    print("CNT>> 离线计算")
    time1 = time.time()
    a = [rand_phi_q() for _ in range(k)]
    a_ = [pow(aj,-1,phi) for aj in a]
    c_offline_cal_time = time.time() - time1
    with open("./a.json",'w') as f:
        json.dump({'a':a,'a_':a_},f)

    print("CNT>> offline R<=S: {c, pt}")
    time1 = time.time()
    msg = b""
    while(True):
        hear = DataSocket.recv(BUFLEN)
        msg += hear
        if(hear[-1]==ord('}')):
            break
    json_msg = json.loads(msg)
    c = json_msg['c']
    pt = json_msg['pt']
    c_offline_com_time = time.time() - time1
    
    offline_time = time.time() - offline_time
    if(offline_time<(c_offline_cal_time+c_offline_com_time)):
        offline_time = c_offline_cal_time+c_offline_com_time
    print(f"CNT>> 离线计算消耗时间{c_offline_cal_time:.4f}s")
    print(f"CNT>> 离线通信消耗时间{c_offline_com_time:.4f}s")
    print(f"CNT>> 离线总运行时间{c_offline_cal_time+c_offline_com_time:.4f}s")
    print(f"CNT>> 离线运行结束")

    DataSocket.close()




def s_online():
    model_name = "bert"
    x = import_module('models.' + model_name)
    model = model = x.Model(config).to(config.device)
    model.load_state_dict(torch.load(config.save_path, map_location=torch.device('cpu')))
    model = model.eval()
    ListenSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ListenSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    ListenSocket.bind((IP, PORT))
    num_client = 1
    ListenSocket.listen(num_client)
    print(f"SER>> 服务器启动成功，在{PORT}端口等待客户端连接...")
    DataSocket, addr = ListenSocket.accept()
    print("SER>> 接收一个客户端连接：", addr)

    x = x0

    print("SER>> online S<=R:A")
    time1 = time.time()
    msg = b""
    while(True):
        hear = DataSocket.recv(BUFLEN)
        msg += hear
        if(hear[-1]==ord('}')):
            break
    json_msg = json.loads(msg)
    A = json_msg['A']
    s_online_com_time1 = (time.time()-time1)
    print(f"SER>> online S<=R:A 通信消耗时间{s_online_com_time1:.4f}s")

    print("SER>> online 计算D")
    time1 = time.time()
    D = [pow(Aj,x,q) for Aj in A]
    s_online_cal_time1 = (time.time()-time1)
    print(f"SER>> online 计算D 消耗时间{s_online_cal_time1:.4f}s")

    
    print("SER>> online S=>R:D")
    time1 = time.time()
    tosend = json.dumps({'D':D})
    DataSocket.send(tosend.encode(UTF))
    s_online_com_time2 = (time.time()-time1)
    print(f"SER>> online S=>R:D 通信消耗时间{s_online_com_time2:.4f}s")

    # print(f"SER>> OTS消耗时间{time.time()-time1:.4f}")
    
    print("SER>> online S<=R: normalized data")
    time1 = time.time()
    msg = b""
    while(True):
        hear = DataSocket.recv(BUFLEN)
        msg += hear
        if(hear[-1]==ord('}')):
            break
    json_msg = json.loads(msg)
    normalized_data = json_msg['normalized_data']
    extended_attention_mask = json_msg['extended_attention_mask']
    s_online_com_time3 = (time.time()-time1)
    print(f"SER>> online S<=R: normalized data 通信消耗时间{s_online_com_time3:.4f}s")

    # 神经网络预测
    print("SER>> online 后续神经网络推理")
    time1 = time.time()
    normalized_data = torch.Tensor(normalized_data)
    extended_attention_mask = torch.Tensor(extended_attention_mask)
    embedding_output = model.bert.embeddings.LayerNorm.weight * normalized_data + model.bert.embeddings.LayerNorm.bias
    embedding_output=embedding_output.unsqueeze(0)
    encoded_layers = model.bert.encoder(embedding_output,extended_attention_mask,False)
    sequence_output = encoded_layers[-1]
    pooled_output = model.bert.pooler(sequence_output)
    encoded_layers = encoded_layers[-1]
    out = model.fc(pooled_output)
    predict = torch.max(out.data, 1)[1][0]
    s_online_cal_time2 = (time.time()-time1)
    print(f"SER>> online 后续神经网络推理 消耗时间{s_online_cal_time2:.4f}s")
    
    print("SER>>","预测结果为：",classes[predict])

    # 发送结果
    print("SER>> online S=>R: predict")
    time1 = time.time()
    tosend = json.dumps({'predict':predict.item()})
    DataSocket.send(tosend.encode(UTF))
    s_online_com_time4 = (time.time()-time1)
    print(f"SER>> online S=>R: predict 通信消耗时间{s_online_com_time4:.4f}s")

    s_online_cal_time = s_online_cal_time1+s_online_cal_time2
    s_online_com_time = s_online_com_time1+s_online_com_time2+s_online_com_time3+s_online_com_time4
    print(f"SER>> 在线计算消耗时间{s_online_cal_time:.4f}s")
    print(f"SER>> 在线通信消耗时间{s_online_com_time:.4f}s")
    print(f"SER>> 在线总运行时间{s_online_cal_time+s_online_com_time:.4f}s")

    print("SER>> 服务端在线运行结束")

    DataSocket.close()
    ListenSocket.close()
    

def c_online(data):
    global online_time
    DataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        try:
            DataSocket.connect((IP, PORT))
            break
        except:
            continue

    c = c0
    pt = pt0
    a = a0
    a_ = a_0
    online_time = time.time()

    print("CNT>> online pre_data")
    time1 = time.time()
    input_ids, extended_attention_mask=pre_data(data)
    input_ids = input_ids.numpy().tolist()[0]
    extended_attention_mask = extended_attention_mask.numpy().tolist()
    c_online_cal_time1 = (time.time()-time1)
    print(f"CNT>> online pre_data计算消耗时间{c_online_cal_time1:.4f}s")

    print("CNT>> online 计算A")
    time1 = time.time()
    A = [pow(H1(input_ids[j]),a[j],q) for j in range(k)]
    c_online_cal_time2 = (time.time()-time1)
    print(f"CNT>> online 计算A计算消耗时间{c_online_cal_time2:.4f}s")

    print("CNT>> online R=>S:A")
    time1 = time.time()
    tosend = json.dumps({'A':A})
    DataSocket.send(tosend.encode(UTF))
    c_online_com_time1 = (time.time()-time1)
    print(f"CNT>> online R=>S:A 通信消耗时间{c_online_com_time1:.4f}s")

    print("CNT>> online R<=S:D")
    time1 = time.time()
    msg = b""
    while(True):
        hear = DataSocket.recv(BUFLEN)
        msg += hear
        if(hear[-1]==ord('}')):
            break
    json_msg = json.loads(msg)
    D = json_msg['D']
    c_online_com_time2 = (time.time()-time1)
    print(f"CNT>> online R<=S:D 通信消耗时间{c_online_com_time2:.4f}s")

    print("CNT>> online 计算K和m")
    time1 = time.time()
    K = [pow(D[j],a_[j],q) for j in range(k)]
    c_sigma = [c[input_ids[j]] for j in range(k)]  # (32,21128)
    h2_ = [H2(Kj) for Kj in K]
    c_sigma = np.array(c_sigma)
    h2_ = np.array(h2_)
    m_sigma = np.float32(((c_sigma.T ^ h2_).T)/scale) + pt
    c_online_cal_time3 = (time.time()-time1)
    print(f"CNT>> online 计算K和m 计算消耗时间{c_online_cal_time3:.4f}s")

    print("CNT>> online 计算norm")
    time1 = time.time()
    embeddings = torch.Tensor(m_sigma)
    u = embeddings.reshape(-1).mean(-1, keepdim=True)
    s = (embeddings - u).pow(2).reshape(-1).mean(-1, keepdim=True)
    normalized_data = (embeddings - u) / torch.sqrt(s + 1e-12)
    normalized_data = normalized_data.numpy().tolist()
    c_online_cal_time4 = (time.time()-time1)
    print(f"CNT>> online 计算norm 计算消耗时间{c_online_cal_time4:.4f}s")

    print(f"CNT>> online R=>S: normalized data")
    time1 = time.time()
    tosend = json.dumps({'normalized_data':normalized_data,'extended_attention_mask':extended_attention_mask})
    DataSocket.send(tosend.encode(UTF))
    c_online_com_time3 = (time.time()-time1)
    print(f"CNT>> online R=>S:normalized data 通信消耗时间{c_online_com_time3:.4f}s")

    print("CNT>> online R<=S: predict")
    time1 = time.time()
    hear = DataSocket.recv(BUFLEN)
    json_msg = json.loads(hear)
    predict = json_msg['predict']
    c_online_com_time4 = (time.time()-time1)
    print(f"CNT>> online R=>S:normalized data 消耗时间{c_online_com_time4:.4f}s")

    c_online_cal_time = c_online_cal_time1+c_online_cal_time2+c_online_cal_time3+c_online_cal_time4
    c_online_com_time = c_online_com_time1+c_online_com_time2+c_online_com_time3+c_online_com_time4
    
    online_time = time.time() - online_time
    print(f"CNT>> 在线计算消耗时间{c_online_cal_time:.4f}s")
    print(f"CNT>> 在线通信消耗时间{c_online_com_time:.4f}s")
    print(f"CNT>> 在线总运行时间{c_online_cal_time+c_online_com_time:.4f}s")

    print("CNT>> 客户端在线运行结束")

    DataSocket.close()

def test_mpct_time_multi_processes(role):

    offline_time0 = time.time()
    if(role=="1"):
        s_offline()
    else:
        c_offline()
    offline_time = time.time()-offline_time0
    print(f"离线总运行时间{offline_time:.4f}s")

    online_time0 = time.time()
    text = "新一届政治局常委亮相"
    if(role=="1"):
        s_online()
    else:
        c_online(text)
    online_time = time.time()-online_time0
    print(f"在线总运行时间{online_time:.4f}s")
    
    print(f"总运行时间{offline_time+online_time:.4f}s")

def test_mpct_time_multi_threads():
    time0 = time.time()
    with ThreadPoolExecutor(2)as t:
        client = t.submit(c_offline)
        server = t.submit(s_offline)
    print(f"离线总运行时间{offline_time:.4f}s")

    text = "新一届政治局常委亮相"
    with ThreadPoolExecutor(2)as t:
        client = t.submit(c_online,text)
        server = t.submit(s_online)
    print(f"在线总运行时间{online_time:.4f}s")
    
    print(f"总运行时间{online_time+offline_time:.4f}s")



    


if __name__=="__main__":
    # role = sys.argv[1]
    # print(role)
    # test_mpct_time_multi_processes(role=role)
    test_mpct_time_multi_threads()