import torch
import numpy as np
import math
from model import ResNet34
from util.data import DataSet


class Clients:
    def __init__(self, num_classes, edge_num, client_num, batch_size, client_epochs, device):
        self.model = ResNet34(num_classes).to(device)
        self.bs = batch_size
        self.client_epochs = client_epochs
        self.clients_num = client_num
        self.edge_num = edge_num
        self.client_num_per_edge = client_num // edge_num
        self.dataset = DataSet(edge_num, self.client_num_per_edge, batch_size)
        self.criterion = torch.nn.CrossEntropyLoss()

    def run_test(self, device):
        self.model.eval()
        test_loss = 0
        correct = 0
        total = 0
        with torch.no_grad():
            for batch_idx, (inputs, targets) in enumerate(self.dataset.test):
                inputs, targets = inputs.to(device), targets.to(device)
                if inputs.size(-1) == 28:
                    inputs = inputs.view(inputs.size(0), -1)
                outputs = self.model(inputs)

                _, predicted = outputs.max(1)
                total += targets.size(0)
                correct += predicted.eq(targets).sum().item()

                loss = self.criterion(outputs, targets)
                test_loss += loss.item()

        accuracy = 100. * correct / total
        return test_loss, accuracy

    def train_epoch(self, edge_id, client_id, optimizer, device):
        self.model.train()
        client_train_data_loader = self.dataset.train[edge_id][client_id]
        train_loss = 0
        correct = 0
        total = 0
        for batch_idx, (inputs, targets) in enumerate(client_train_data_loader):
            inputs, targets = inputs.to(device), targets.to(device)
            if inputs.size(-1) == 28:
                inputs = inputs.view(inputs.size(0), -1)
            optimizer.zero_grad()
            outputs = self.model(inputs)
            loss = self.criterion(outputs, targets)
            loss.backward()
            optimizer.step()
            train_loss += loss.item()
            _, predicted = outputs.max(1)
            total += targets.size(0)
            correct += predicted.eq(targets).sum().item()
            accuracy = 100. * correct / total
        return accuracy, train_loss, self.model.state_dict()

    def train_epochs(self, edge_id, client_id, optimizer, device):
        for epoch in range(self.client_epochs):
            train_acc, train_loss, current_client_vars = self.train_epoch(edge_id, client_id, optimizer, device)
        return train_acc, train_loss, current_client_vars

    def get_client_vars(self):
        return self.model.state_dict()

    def set_edge_vars(self, edge_vars):
        self.model.load_state_dict(edge_vars)

    def choose_clients(self, ratio2):
        choose_num = math.floor(self.client_num_per_edge * ratio2)
        return np.random.permutation(self.client_num_per_edge)[:choose_num]
