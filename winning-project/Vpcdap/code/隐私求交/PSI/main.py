'''初始化环境。以下三个节点 alice， bob 和 carol 是在一台机器上创建的，以模拟多个参与者。'''
import secretflow as sf
import pandas as pd
A = pd.read_csv('.data1/A.csv')
B = pd.read_csv('.data1/B.csv')
C = pd.read_csv('.data1/C.csv')
A_total=A.shape[0]
# In case you have a running secretflow runtime already.
sf.shutdown()
sf.init(['alice', 'bob', 'carol'], address='local')

'''三方隐私求交
在物理设备上虚拟化三个逻辑设备:
alice, bob: PYU设备，负责参与者的本地明文计算。
spu：SPU设备，由alice和bob组成，负责双方的密文
'''
alice, bob, carol = sf.PYU('alice'), sf.PYU('bob'), sf.PYU('carol')
spu_ABpc = sf.SPU(sf.utils.testing.cluster_def(['alice', 'bob']))
spu_ACpc = sf.SPU(sf.utils.testing.cluster_def(['alice', 'carol']))

'''隐私求交
使用 id 作为求交键来执行三方求交。在实现方面，请确保没有重复的值。
'''
input_path = {alice: '.data1/A.csv', bob: '.data1/B.csv'}
output_path = {alice: '.data1/AB_psi.csv', bob: '.data1/B_psi.csv'}
spu_ABpc.psi_csv(['ID'], input_path, output_path, 'alice')

input_path = {alice: '.data1/A.csv', carol: '.data1/C.csv'}
output_path = {alice: '.data1/AC_psi.csv', carol: '.data1/C_psi.csv'}
spu_ACpc.psi_csv(['ID'], input_path, output_path, 'alice')

AB_psi=pd.read_csv('.data1/AB_psi.csv')
AC_psi=pd.read_csv('.data1/AC_psi.csv')
AB_total=AB_psi.shape[0]
AC_total=AC_psi.shape[0]
threshold = 0.6
print('重合度阈值为%f'%threshold)
AB_rate=AB_total/A_total
AC_rate=AC_total/A_total

n=['B','C']
m=[AB_rate,AC_rate]
for i in range(2):
    print('A和%s的标签重合度为%f'%(n[i],m[i]))
for j in range(2):
    m=[AB_rate,AC_rate]
    if m[j] >= threshold:
        print('A和%s可进行联邦学习'%n[j])
    else:
        print('A和%s不能进行联邦学习'%n[j])