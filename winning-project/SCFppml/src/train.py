
import json
import copy
import spu
import time
import logging

import secretflow as sf
import pandas as pd
from secretflow.device.driver import reveal
from secretflow.ml.boost.sgb_v import Sgb
from sklearn.metrics import mean_squared_error, roc_auc_score, precision_recall_curve
import matplotlib.pylab as plt

#############   Init     ##################
def run(cluster_config, ip, port):
    with open(cluster_config, "r") as inf:
        config_def = json.loads(inf.read())
    train_alice_path = "./data/a.csv"
    train_bob_path = "./data/b.csv"
    test_alice_path = "./data/test_a.csv"
    test_bob_path = "./data/test_b.csv"
    sf.shutdown()
    tls_config = {
        "ca_cert": "ca root cert of other parties (e.g. bob)",
        "cert": "server cert in pem",
        "key": "server key in pem",
    }

    sf.init(address="{}:{}".format(ip, port), cluster_config=config_def)
    ########################

    alice = sf.PYU("alice")
    bob = sf.PYU("bob")

    # SPU settings
    spu_config = {
        "nodes": [
            # YOUR IP & free port
            {"party": "alice", "id": "local:0", "address": "ip:port"},
            # YOUR IP & free port
            {"party": "bob", "id": "local:1", "address": "ip:port"},
        ],
        "runtime_config": {
            "protocol": spu.spu_pb2.SEMI2K,
            "field": spu.spu_pb2.FM128,
        },
    }
    my_spu = sf.SPU(spu_config)

    # HEU settings
    heu_config = {
        "sk_keeper": {"party": "alice"},
        "evaluators": [{"party": "bob"}],
        "mode": "PHEU",  
        "he_parameters": {
            "schema": "ou",
            "key_pair": {
                "generate": {
                    "bit_size": 2048,
                },
            },
        },
        "encoding": {
            "cleartext_type": "DT_I32",
            "encoder": "IntegerEncoder",
            "encoder_args": {"scale": 1},
        },
    }

    heu_x = sf.HEU(heu_config, spu_config["runtime_config"]["field"])
    heu_config = copy.deepcopy(heu_config)
    sk_keeper = heu_config["sk_keeper"]
    evaluator = heu_config["evaluators"][0]
    heu_config["sk_keeper"] = evaluator
    heu_config["evaluators"][0] = sk_keeper
    heu_y = sf.HEU(heu_config, spu_config["runtime_config"]["field"])

    #############   Loading data    ##################
    start = time.time()
    from secretflow.data.vertical import read_csv as v_read_csv

    train_vdf = v_read_csv(
        {alice: train_alice_path, bob: train_bob_path},
        keys="id",
        drop_keys="id",
        spu=my_spu,
    )
    test_vdf = v_read_csv(
        {alice: test_alice_path, bob: test_bob_path},
        keys="row_num",
        drop_keys="row_num",
        spu=my_spu,
    )

    X_train = train_vdf.drop(columns="label")
    y_train = train_vdf["label"]
    y_train_ = reveal(y_train.partitions[alice].data)

    X_test = test_vdf
    logging.info(f"IO times: {time.time() - start}s")

    #############   Training  ##################
    xgb = Sgb(heu_x)

    start = time.time()
    params = {
        "num_boost_round": 1000,
        "max_depth": 10,
        "learning_rate": 0.15,
        "sketch_eps": 0.05,
        "objective": "logistic",
    }
    model = xgb.train(params, X_train, y_train)
    logging.info(f"main train time: {time.time() - start} s")

    #############   Inference    ##################
    start = time.time()
    spu_yhat = model.predict(X_train)
    yhat = reveal(spu_yhat)
    logging.info(f"main predict time: {time.time() - start} s")
    logging.info(f"main auc: {roc_auc_score(y_train_, yhat)}")

    precision, recall, thresholds = precision_recall_curve(y_train_, yhat)

    plt.plot(recall, precision)
    plt.xlabel("Recall")
    plt.ylabel("Precision")
    plt.title("PR Curve")

    plt.savefig("PR Curve.pdf")

    df_pred['predict_result'] = preds
    df_pred = df_pred[['row_num', 'predict_result']]
    df_pred.to_csv("result.csv", index=False)

    saving_path_dict = {
        device: "./sgb_" + device.party
        for device in test_vdf.partitions.keys()
    }
    model.save_model(saving_path_dict)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(prog='sgb benchmark remote')
    parser.add_argument('config_path')
    parser.add_argument('ip')
    parser.add_argument('port')
    args = parser.parse_args()
    run(args.config_path, args.ip, args.port)