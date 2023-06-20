# A simple implementation of BackPropagation Neural Network in PyTorch framework
# Attention: the BPNN's structure is stable in the code.

import torch
import torch.nn.functional as F
from torch.autograd import Variable
import torch.utils.data as Data
import numpy as np
import utils
import torch.nn as nn
import torch.nn.utils.rnn as rnnutils
import matplotlib.pyplot as plt
from math import sqrt
from sklearn.metrics import r2_score
from sklearn.preprocessing import MinMaxScaler,StandardScaler
import pandas as pd
import time
from sklearn.metrics import classification_report, roc_curve, auc
import numpy as np



device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")  # 数据放到gpu上还是cpu上

torch.manual_seed(0)
torch.cuda.manual_seed(0)
torch.cuda.manual_seed_all(0)
'''torch.manual_seed(229176228)
torch.cuda.manual_seed(229176228)
torch.cuda.manual_seed_all(229176228)'''



class Self_Attention(nn.Module):
    # input : batch_size * seq_len * input_dim
    # q : batch_size * input_dim * dim_k
    # k : batch_size * input_dim * dim_k
    # v : batch_size * input_dim * dim_v
    def __init__(self ,input_dim ,dim_k ,dim_v):
        super(Self_Attention ,self).__init__()
        self.q = nn.Linear(input_dim ,dim_k)
        self.k = nn.Linear(input_dim ,dim_k)
        self.v = nn.Linear(input_dim ,dim_v)
        self._norm_fact = 1 / sqrt(dim_k)


    def forward(self ,x):
        Q = self.q(x) # Q: batch_size * seq_len * dim_k
        K = self.k(x) # K: batch_size * seq_len * dim_k
        V = self.v(x) # V: batch_size * seq_len * dim_v
        # print("x", x)
        # print("Q", Q)
 #       print("K.permute", K)
    #    print("K转置", K.t())

  #      print("torch.bmm(Q ,K.permute(0 ,2 ,1))", K.permute(0, 2, 1))


#        atten = nn.Softmax(dim=-1)((torch.matmul(Q, K.t())) * self._norm_fact)#用来处理batch_size为1的时候情况
        atten = nn.Softmax(dim=-1)((torch.matmul(Q, K.permute(0, 2, 1))) * self._norm_fact)

        #  atten = nn.Softmax(dim=-1)((torch.matmul(Q, K.t())) * self._norm_fact) # Q * K.T() # batch_size * seq_len * seq_len
  #      print("atten", atten)
  #      print("atten[1][0]-before", atten[1][0])
  #      atten[1][0] = 1- atten[1][0]
  #      print("atten[1][0]-after", atten[1][0])
  #      print("atten", atten[0])

    #    print("V:", V.shape)
    #    print("atten[0]", atten)
        output = torch.matmul(atten, V) # Q * K.T() * V # batch_size * seq_len * dim_v
    #    print("vvvvvvvvvvv", V
  #      print("output", output)
        return output



class GRU(nn.Module):
    def __init__(self, input_size, hidden_size, output_size):
        super(GRU, self).__init__()
        self.input_size = input_size
        self.hidden_size = hidden_size

        # 设置可以训练的参数矩阵
        self.w_xr = torch.nn.Parameter(torch.Tensor(input_size, hidden_size))
        self.w_hr = torch.nn.Parameter(torch.Tensor(hidden_size, hidden_size))
        self.w_xz = torch.nn.Parameter(torch.Tensor(input_size, hidden_size))
        self.w_hz = torch.nn.Parameter(torch.Tensor(hidden_size, hidden_size))
        self.w_xh = torch.nn.Parameter(torch.Tensor(input_size, hidden_size))
        self.w_hh = torch.nn.Parameter(torch.Tensor(hidden_size, hidden_size))

        self.b_r = torch.nn.Parameter(torch.Tensor(hidden_size))
        self.b_z = torch.nn.Parameter(torch.Tensor(hidden_size))
        self.b_h = torch.nn.Parameter(torch.Tensor(hidden_size))

        self.attention = Self_Attention(hidden_size, hidden_size, hidden_size)

        self.liner1 = nn.Linear(hidden_size * 2, hidden_size)
        self.liner2 = nn.Linear(input_size, hidden_size)
        self.liner3 = nn.Linear(self.hidden_size, self.hidden_size)


        self.outlinear = nn.Sequential(
            nn.Linear(hidden_size, output_size)
        )  # 输出层

        self.reset_parameters()  # 初始化参数

    def reset_parameters(self):
        stdv = 1.0 / sqrt(self.hidden_size)
        for weight in self.parameters():
            torch.nn.init.uniform_(weight, -stdv, stdv)





    def forward(self, input):
        print("input:",input.shape)
        input = input.float()  # (batchsize,seqlen,inputsize)

        batch_size = input.size(0)  # 一个batch的大小
        step_size = input.size(1)  # 时间步

        # 初始化隐藏状态矩阵h为零矩阵
        h = torch.zeros(batch_size, self.hidden_size)

        # 这里面存放每一个时间步出来的h
        lisths = []

        # 一个时间步一个时间步的计算
        for i in range(step_size):
            # 取input每个时间步的数据
            x = input[:, i, :]
       #     print("x", x.shape)
       #     print("h", h.shape)
            # --------------------------------GRU核心公式-----------------------------------
            # x形状是(batchsize,inputsize),w_xz矩阵形状是(inputsize,hiddensize)
            # torch.mm是矩阵乘法，这样(torch.mm(x,self.w_xz)的形状是(batchsize,hiddensize)

 #           z = torch.sigmoid((torch.mm(x, self.w_xz) + torch.mm(h, self.w_hz) + self.b_z))
 #           r = torch.sigmoid((torch.mm(x, self.w_xr) + torch.mm(h, self.w_hr) + self.b_r))
 #           h_tilde = torch.tanh((torch.mm(x, self.w_xh) + torch.mm(r * h, self.w_hh) + self.b_h))

            x = self.liner2(x)
            h = self.liner3(h)
       #     print("h.shape", h.shape)
       #     print("x_before", x)
       #     print("h_after", h)
            h_stack = torch.stack((h, x), dim=1)

       #     print("h_stack", h_stack.shape)
       #     x = x.reshape(1, batch_size, x.shape[1])
       #    x = [x]
       #     print("x_after",x)
      #      print("h_before", h)
       #     h = h.reshape(1, batch_size, h.shape[1])


       #     h_cat1 = torch.cat((x, h))
       #     print("h_cat", h_cat1)
            zt = self.attention(h_stack)
      #      print("zt", zt)
      #      print("Zt[0]", zt[0][0])
      #      print("x[0]", x[0])
            for i in range(len(zt)):
                x[i] = zt[i][0] + x[i]
     #       x[0] = zt[0][0]+x[0]
      #      print("x", x)

            h = x
         #   print("h", h)
            #       print("1-z", 1-z)
            #   h = torch.cat((x, h), 1)
            # --------------------------------GRU核心公式-----------------------------------
            # h的形状是(batch_size,hidden_size)
            #     h = torch.tanh(self.liner1(h))
            # 把每个时间步出来的h都存到list里
            lisths.append(h)

        # 用torch.stack把装有tensor的list转为torch.tensor类型,dim=1是指从第二个维度转化，因为seqlen在第二个维度上
        # 所以hs的形状是(batchsize,seqlen,hiddensize)
        hs = torch.stack(lisths, dim=1)  # 全部cell所计算的隐藏状态的集合

        # 此时的h是最后一个时间步计算出来的h，可以用这个作为最后的输出
        output = self.outlinear(h)  # 最后一个单元的输出，接一个带有Sigmoid激活函数的线性层，因为我们的任务是分类任务
        # output矩阵的形状现在是(batchsize,outputsize)
    #    print("output.shape", output.shape)
        return output





def split_data(data, n):
    # 前n时刻的值为输入
    # 预测n+1个时刻的负荷值
    in_ = []
    out_ = []
    N = data.shape[0] - n
    for i in range(N):
        in_.append(data[i:i + n])
        out_.append(data[i + n])
    in_ = np.array(in_).reshape(len(in_), -1)
    out_ = np.array(out_).reshape(len(out_), -1)
    return in_, out_


if __name__ == "__main__":




#做评估的时候隐藏层选择100效果很好
    model = GRU(43, 100, 10)
    leaning_rate = 0.001
    max_epoches = 50
    batch_size = 128

    label = []
    test_label = []

    h0 = torch.randn(128, 10)

    X_train = np.loadtxt(open("lab/50w_train.csv"),
                         delimiter=",", skiprows=0)
    # X_train = torch.from_numpy(X_train)
    Y_Label = np.loadtxt(open("lab/train_label_onehot.csv"),
                         delimiter=",", skiprows=0)
    # Y_Label = torch.from_numpy(Y_Label)

    X_test = np.loadtxt(open('5W_test/10w_test_2.csv'), delimiter=",", skiprows=0)
    Y_test_Label = np.loadtxt(open('5W_test/test_label_onehot_2.csv'), delimiter=",", skiprows=0)

'''

   # print(Y_Label)



    X_train = X_train.reshape( X_train.shape[0], 1, X_train.shape[1])
 #   print("X_trainX_train", X_train)
    X_test = X_test.reshape(X_test.shape[0], 1, X_test.shape[1])

    #print('y' , Y_Label)
   # Y_Label = Y_Label.reshape(1, 1)


    use_gpu = torch.cuda.is_available()
   # print('X_train', len(X_train))
    trainLoader = torch.utils.data.DataLoader(dataset=X_train, batch_size=batch_size, shuffle=False)
    trainlabel = torch.utils.data.DataLoader(dataset=Y_Label, batch_size=batch_size, shuffle=False)

    testLoader = torch.utils.data.DataLoader(dataset=X_test, batch_size=batch_size, shuffle=False)
    testlabel = torch.utils.data.DataLoader(dataset=Y_test_Label, batch_size=batch_size, shuffle=False)

    '''for label_tarin in trainlabel:
        label.append(np.asarray(label_tarin))'''
    for label1 in trainlabel:
        label.append(label1)

    for test_label1 in testlabel:
        test_label.append(test_label1)

    #torch_dataset = Data.TensorDataset(data_tensor=trainLoader, target_tensor=trainlabel)
  #  loader = Data.DataLoader(dataset=torch_dataset, batch_size=1, shuffle=False)

    logger = utils.Logger(max_epoches, len(trainLoader))


    optimizer = torch.optim.Adam(model.parameters(), lr=leaning_rate)
    scheduler = torch.optim.lr_scheduler.ReduceLROnPlateau(optimizer)
    loss_function = torch.nn.CrossEntropyLoss()

    loss_list = []


    for epoch in range(max_epoches):
        i = 0
        avg_loss = 0
        cnt = 0
        for data  in trainLoader:
            instance = Variable(data).float()
            label_train = Variable(label[i]).long()
        #    print('len' , len(instance))

            output = model(instance)
            output1 = []
            '''for x1 in range(len(output)):
                output1.append(output[x1][0])'''



            #print('output' , torch.size(output))
       #     print(label_train)
         #   print("output", output)
         #   print("label_train", label_train)

            loss = loss_function(output, label_train)
           # print(label_train)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            avg_loss += loss
            cnt += 1
            i += 1

            logger.log({'loss': loss.data}, avg_loss / cnt, epoch)
        scheduler.step(avg_loss/cnt)
        loss_list.append(float(avg_loss/cnt))
    print('loss_list', loss_list)
   # np.savetxt('output/ARN_loss.csv', loss_list, delimiter=',')
   # test_x = Variable(test_x)
   # print(model.forward(test_x))




    j = 0
    num = 0
    cnt_test = 0
    correct = 0
    acc = 0
    total = 0
    y_pred = []
    y_real = []
    test_loss = 0
    avg_test_loss = []
    test_mae = 0
    avg_mae = []
    accracy = 0
 #   output_test = None
    star_time = time.time()
    for test_data in testLoader:

        input_test = Variable(test_data).float()
        label_test = Variable(test_label[j]).long()
#        input_test = input_test.reshape(input_test.shape[0], 1, input_test.shape[1])
        #  print('label ', int(label_test[0]))
        if (use_gpu):
            model.cpu()
        output_test = model(input_test)
        #     print('000000000' , len(label_test[0]))

        # print('label ', int(output_test[0]))

        pred = output_test.data.max(1, keepdim=True)[1]
        #   print('pred' , pred)
        #    correct += pred.eq(label_test.data.view_as(pred)).cpu().sum()
        #      accracy += np.mean((torch.argmax(output_test, 1) == torch.argmax(label_test, 1)).numpy())

        _, predicted = torch.max(output_test.data, 1)
        total += label_test.size(0)
        for x_pre in predicted.cpu():
            y_pred.append(x_pre)

        for x_real in label_test:
            y_real.append(x_real)

        correct += (predicted.cpu() == label_test).sum()
        #    print(predicted, label_test, correct, total)
        print('predicted', predicted.cpu().float())
        cnt_test += 1
        acc += float(correct) / total
        print('accccccccc', acc)
        print("avg acc: %f" % (100 * correct / total))

        j += 1
    accracy = acc / cnt_test
    print('accracy', accracy)
    print(classification_report(y_real, y_pred,digits=4))
    end_time = time.time()

print("测试集时间", end_time-star_time)

