from src.edge import Edges
from util.fedavg import FedAvg
import numpy as np
import torch
import os


class Cloud:
    def __init__(self, edge, num_classes, edge_num, client_num, batch_size, device):
        self.edge = edge
        self.model = self.edge.model

    def train_epoch(self, optimizer, ratio1, ratio2, device):
        cloud_vars = self.get_cloud_vars()

        edge_vars_sum = []
        random_edges = self.edge.choose_edges(ratio1)

        edge_loss, edge_acc = [], []
        for edge_id in random_edges:
            self.edge.set_cloud_vars(cloud_vars)
            train_acc, train_loss, current_edge_vars = self.edge.train_epochs(edge_id=edge_id, optimizer=optimizer, ratio2=ratio2, device=device)
            
            edge_vars_sum.append(current_edge_vars)

            edge_loss.append(train_loss)
            edge_acc.append(train_acc)
        
        global_vars = FedAvg(edge_vars_sum)
        self._save_cloud_and_edges(global_vars, edge_vars_sum)
        self.set_global_vars(global_vars)
        
        return np.mean(edge_loss), np.mean(edge_acc)
    
    def get_cloud_vars(self):
        return self.model.state_dict()
    
    def set_global_vars(self, global_vars):
        return self.model.load_state_dict(global_vars)

    def run_test(self, device):
        accuracy, test_loss = self.edge.run_test(device)
        return accuracy, test_loss
    
    def _save_cloud_and_edges(self, cloud, edges):
        self.get_cloud_and_edges = [cloud] + edges
    

