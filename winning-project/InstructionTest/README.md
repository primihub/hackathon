# InstructionTest：处理器指令集架构安全中的隐藏指令检测技术

## InstructionTest依赖环境：

sudo apt-get install libcapstone3 libcapstone-dev
sudo pip install capstone

## InstructionTest项目编译：

make

## InstructionTest项目运行：

sudo ./sifter.py --unk --dis --len --sync --tick -- -P1 -t

## InstructionTest数据收集：

./summarize.py data/log