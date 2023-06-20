import torch
import torchvision
import torchvision.transforms as transforms


def build_dataset():
    print('==> Preparing data..')
    transform_train = transforms.Compose([
        transforms.RandomCrop(32, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
    ])

    transform_test = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.4914, 0.4822, 0.4465), (0.2023, 0.1994, 0.2010)),
    ])

    trainset = torchvision.datasets.CIFAR10(root='./data', train=True, download=True,
                                            transform=transform_train)
    testset = torchvision.datasets.CIFAR10(root='./data', train=False, download=True,
                                           transform=transform_test)
    # classes = ('plane', 'car', 'bird', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck')

    return trainset, testset


class DataSet:
    def __init__(self, edge_num, client_num_per_edge, batch_size):
        trainset, testset = build_dataset()
        edge_data_list = torch.utils.data.random_split(trainset, [len(trainset) // edge_num for _ in range(edge_num)])
        client_data_list = [torch.utils.data.random_split(edge_data, [len(edge_data) // client_num_per_edge for _ in
                                                                      range(client_num_per_edge)]) for edge_data in
                            edge_data_list]
        self.train = [[torch.utils.data.DataLoader(client_data_list[i][j], batch_size=batch_size, shuffle=True) for j in
                       range(client_num_per_edge)] for i in range(edge_num)]
        self.test = torch.utils.data.DataLoader(testset, batch_size=batch_size, shuffle=False)
        del client_data_list

# dataset = DataSet(10, 10, 64)
# print(len(dataset.train))
# print(len(dataset.train[0]))
