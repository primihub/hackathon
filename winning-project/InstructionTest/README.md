
sudo apt-get install libcapstone3 libcapstone-dev
sudo pip install capstone
make


sudo ./sifter.py --unk --dis --len --sync --tick -- -P1 -t

./summarize.py data/log