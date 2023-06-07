# InstructionTest：处理器指令集架构安全中的隐藏指令检测技术

## InstructionTest研究内容：

研究CISC和RISC（x86、ARM、SPARC、RISC-V）等多种架构下基于模糊测试的隐藏指令检测方式以及高效的指令搜索算法。对于检测出的隐藏指令，需要进行分类去冗余与真实性验证的工作。

**隐藏指令检测模块：**首先根据被测试处理器平台信号等信息初始化测试系统，搜索待测试指令，使用反汇编器将被测试指令反汇编，同时将指令放在内存中进行单指令运行。

**隐藏指令搜索算法**：先被检测指令中的已知指令可以通过对其指令格式进行分析，跳过其立即数等部分；对已知指令格式的分析，并且调整指令遍历起始位置以及遍历深度。

**隐藏指令功能分析模块：**在检测出的隐藏指令基础上，首先对基于特定处理器指令集格式下的隐藏指令去冗余与分类算法研究。分类完成后，对指令集的隐藏指令的真实性研究。

**隐藏指令后门检测模块**：采用监控内存与寄存器的变化，以及所经过的执行端口的统计来进行隐藏指令的功能分析。

## InstructionTest依赖环境：

sudo apt-get install libcapstone3 libcapstone-dev
sudo pip install capstone

## InstructionTest项目编译：

make

## InstructionTest项目运行：

sudo ./sifter.py --unk --dis --len --sync --tick -- -P1 -t

## InstructionTest数据收集：

./summarize.py data/log