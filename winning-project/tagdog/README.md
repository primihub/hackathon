# 基于区块链的隐私数据共享和检索方案

隐私保护的加密操作使得检索变得困难，难以在密文中直接获得和敏感词相关的数据。支持数据检索的场景中共享数据采用明文存储，检索时输入关键词明文即可进行匹配，而支持隐私保护的场景中共享数据采用密文存储，检索时无法直接输入关键词明文做匹配，这两者之间的矛盾导致隐私保护和数据检索难以兼顾。因此需要在数据共享的场景中，解决数据的隐私保护和检索功能兼顾的问题，所以我们提出基于区块链的隐私数据共享的检索方案。

在基于区块链的保护隐私的数据共享和检索场景中，主要有四种角色，分别是数据共享者、数据检索者、区块链智能合约和区块链账本。
本方案采用基于属性的可搜索加密方案：对索引采用属性基加密，限制只有属性符合检索策略的检索者才能够检索，只有属性符合访问策略的检索者才能够访问共享数据；基于索引实现支持多共享者的统一授权模式；检索者申请陷门时智能合约嵌入检索关键词自动生成陷门；基于时间属性实现检索权限的撤销和灵活的时间控制。

方案的整体流程如下：

* 数据共享阶段。数据共享者将共享数据和数据访问策略、数据检索策略提交给智能合约进行数据共享。智能合约按照预先设定的规则提取共享数据中的关键词并生成密文索引，将共享数据和密文索引加密后上传到区块链账本中。
* 数据检索阶段。数据检索者向智能合约提交关键词信息并发起检索授权请求，智能合约完成检索陷门的生成，并将陷门返回给检索者。检索者将陷门提交给智能合约请求数据检索，智能合约验证检索者的数据检索权限和数据访问权限，在密文索引中进行匹配，将匹配成功的索引项密文返回数据检索者。
数据检索者在本地解密数据索引项，并向智能合约请求索引项对应的交易数据，即为包含检索关键词的所有交易数据。


## 基础功能部署与使用
### 1 基本环境配置
操作系统：Linux(此处以Ubuntu16.04及以上为例)

相关工具安装：git, wget, curl, go, npm, docker, docker-compose,

#### 1.1 wget安装
    sudo apt-get update
    sudo apt-get install wget

#### 1.2 curl安装
    sudo apt-get update
    sudo apt-get install curl

#### 1.3 go安装及环境配置
下载安装包

    wget https://golang.google.cn/dl/go1.17.7.linux-amd64.tar.gz

解压文件

    tar xfz go1.17.7.linux-amd64.tar.gz -C /usr/local

配置环境变量

需要配置GOPATH, GOPROXY, GO111MODULE, 以及添加系统环境变量
配置GOPATH

    go env -w GOPATH=~/go

配置GOPROXY

    go env -w GOPROXY=https://goproxy.io

配置GO111MODULE

    go env -w GO111MODULE=auto

添加系统环境变量

    sudo gedit ~/.bashrc
    export GOROOT=/usr/local/go #GOROOT是GO安装位置
    export GOPATH=~/go #GOPATH是工作目录的位置
    export PATH=$GOPATH/bin:$GOROOT/bin:$PATH

#### 1.4 npm安装
    sudo apt-get install nodejs
    sudo ln -s /usr/bin/nodejs /usr/bin/node #创建软链接使node命令可用
    sudo apt-get install npm
#### 1.5 docker安装及配置
    curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun
    为使当前用户能够在不sudo的情况下使用docker命令，需进行用户组的配置
    sudo groupadd docker
    sudo gpasswd -a ${USER} docker
    sudo service docker restart
#### 1.6 docker-compose安装及配置
    sudo curl -L https://github.com/docker/compose/releases/download/1.17.0/docker-compose-`uname -s`-`uname -m` -o /usr/local/bin/docker-compose
    sudo chmod +x /usr/local/bin/docker-compose # 授权
### 2 源码及基础功能部署
#### 2.1 demo-api源码下载
demo-api 是项目后端代码 

    tar -zxvf demo-api.tar.gz -C ~/go/src/github.com/privacy-protection
#### 2.2 demo-network源码下载
demo-network 是fabric区块链的链码及启动脚本

    tar -zxvf demo-network.tar.gz -C ~/go/src/github.com/privacy-protection
#### 2.3 common源码下载
Common是fabric-sdk-go的依赖库

    tar -zxvf common.tar.gz -C ~/go/src/github.com/privacy-protection
#### 2.4 fabric-sdk-go源码下载
    tar -zxvf fabric-sdk-go.tar.gz -C ~/go/src/github.com/privacy-protection
#### 2.5 fabric源码下载 
    tar -zxvf fabric.tar.gz -C ~/go/src/github.com/hyperledger

### 3正式部署
#### 3.1 fabric部署
    cd go/src/github.com/hyperledger/fabric
    make release
    cp -r release/linux-amd64/bin ../../privacy-protection/demo-network/
    make docker
#### 3.2 demo-network部署
    cd demo-network
    ./byfn.sh up
#### 3.3 demo-api部署
    cd demo-api
    #第一种方法
    make demo-api
    bin/demo-api
    #第二种方法
    make docker 
    make start

