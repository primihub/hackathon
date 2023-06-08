import ray
from src.ecdhpsi import PsiService
import pyarrow as pa
from tools import partition_by_key_value
import time, socket, os
import hashlib

ray.init()
#ray.init(address="auto", runtime_env={"working_dir": "./"})


class PsiUDF(object):

    def hs(self, df, n):
        # return list(map(lambda x: hash(x), df))
        def func(x):
            digest = hashlib.sha256()
            digest.update(bytes(x, encoding="utf8"))
            new_x = int(digest.hexdigest(), 16) % n
            return new_x
        return df.apply(func)

    def __call__(self, df, *args, **kwargs):
        #df = df[df["example_id"] < 100]
        if args[0] == "hash":
            df.insert(0, "hash_id", self.hs(df.loc[:, args[1]], args[2]))
            return df
        else:
            if not kwargs:
                raise Exception("kwargs cannot be empty.")
            app_id = args[3]
            s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
            s.connect(('8.8.8.8', 80))
            ip = s.getsockname()[0]
            local_port = 15000 + df["hash_id"][0]
            file_name = "{}-{}".format(app_id, df["hash_id"][0])
            with open("tmp/l/{}".format(file_name), "w") as f:
                f.write("{}:{}".format(ip, local_port))
            kwargs["iterm_num"] = df.shape[0]
            count = 0
            while True:
                files = os.listdir("tmp/f/")
                if file_name in files:
                    with open(os.path.join("tmp/f/", file_name), "r") as f:
                        peer_address = f.read()
                    break
                else:
                    if count >= 10000:
                        raise Exception('communication failed!')
                    else:
                        time.sleep(0.2)
                        count += 1
            kwargs["local_addr_and_port"] = "0.0.0.0:{}".format(local_port)
            kwargs["peer_addr_and_port"] = peer_address
            df.index = df[args[1]]
            psi_service = PsiService(**kwargs)
            ids = df.index.tolist()

            while True:
                try:
                    psi_res = psi_service.compute_psi(ids, args[2])

                    return pa.Table.from_pandas(df.loc[psi_res])
                except Exception as e:
                    time.sleep(0.2)
                    print(e)
                    continue

act = PsiUDF()
t1 = time.time()
data = ray.data.read_csv("data/raw_data/l1.csv")
t2 = time.time()
data1 = data.map_batches(act, fn_args=("hash", "example_id", 5),
                         batch_size=4096)
psi_service_params = {
    "local_addr_and_port": None,
    "peer_addr_and_port": None,
    "local_rank": 0,
    "peer_rank": 1,
    "version": 1,
    "supported_algos":["ECDH-PSI"],
    "algo_params":None,
    "iterm_num":None,
    "supported_versions":[1],
    "curves":["CURVE25519", "SM2"],
    "hash_methods":["SHA256","SM3"],
    "result_to_rank": -1,
    "bucket_num": 1
}
boundaries1 = [i for i in range(1,5)]
data1 = partition_by_key_value(data1, "hash_id", boundaries1)
t3 = time.time()
data2 = data1.map_batches(act, fn_args=("psi", "example_id", 5000, "psi1"),
                          fn_kwargs=psi_service_params, batch_size=None)
t4 = time.time()
print("time for read_data is {},  split data is {}, psi is {}".format(
                                                t2-t1, t3-t2, t4-t3))
data2.write_csv("data/psi_data/leader")