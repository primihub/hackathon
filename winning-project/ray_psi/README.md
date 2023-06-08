# **Ray-PSI MacOS版模拟Demo说明**

OS: MacOS
python版本: 3.10.8

## 1. 安装依赖:
step 1:
```commandline
pip install -r requirements.txt
```
step 2:
```commandline
cd curve25519-donna
python setup.py build_ext
python setup.py install
```

## 2. 在两个cmd窗口中分别运行 leader_ray.py 和follower_ray.py

cmd窗口1:
```commandline
python leader_ray.py
```
cmd窗口2:
```commandline
python follower_ray.py
```
