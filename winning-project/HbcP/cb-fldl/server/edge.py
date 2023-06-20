import numpy as np
import math
from src.client import Clients
from util.fedavg import FedAvg


class Edges:
    def __init__(self, client, num_classes, edge_num, client_num, batch_size, edge_epochs, device):
        self.bs = batch_size
        self.edge_epochs = edge_epochs
        self.client_num = client_num
        self.edge_num = edge_num
        self.client_num_per_edge = client_num // edge_num
        self.client = client
        self.model = self.client.model

    def train_epoch(self, edge_id, optimizer, ratio2, device):

        edge_vars = self.get_edge_vars()
        client_vars_sum = []
        random_clients = self.client.choose_clients(ratio2)
        for client_id in random_clients:
            self.client.set_edge_vars(edge_vars)
            train_acc, train_loss, current_client_vars = self.client.train_epochs(edge_id, client_id,
                                                                                  optimizer=optimizer, device=device)
            client_vars_sum.append(current_client_vars)

        cloud_vars = FedAvg(client_vars_sum)
        self.set_cloud_vars(cloud_vars)
        return train_acc, train_loss, cloud_vars

    def train_epochs(self, edge_id, optimizer, ratio2, device):
        for epoch in range(self.edge_epochs):
            train_acc, train_loss, edge_vars = self.train_epoch(edge_id, optimizer, ratio2, device)
        return train_acc, train_loss, edge_vars

    def set_cloud_vars(self, cloud_vars):
        self.model.load_state_dict(cloud_vars)

    def get_edge_vars(self):
        return self.model.state_dict()

    def choose_edges(self, ratio1):
        choose_num = math.floor(self.edge_num * ratio1)
        return np.random.permutation(self.edge_num)[:choose_num]

    def run_test(self, device):
        accuracy, test_loss = self.client.run_test(device)
        return test_loss, accuracy
