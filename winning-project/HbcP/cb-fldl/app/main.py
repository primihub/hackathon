import torch
import torch.optim as optim
import argparse
import numpy as np
import random
import os
from src.cloud import Cloud
from src.edge import Edges
from src.client import Clients
import matplotlib.pyplot as plt


def setup_seed(seed):
    np.random.seed(seed)
    random.seed(seed)
    torch.manual_seed(seed)
    torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.deterministic = True
    torch.backends.cudnn.benchmark = True


def get_parse():
    # parser for hyperparameter
    parser = argparse.ArgumentParser()
    parser.add_argument('--seed', type=int, default=0)
    parser.add_argument('--fixed_seed', type=bool, default=False)
    parser.add_argument('--edge_num', type=int, default=10)
    parser.add_argument('--client_num', type=int, default=100)
    parser.add_argument('--ratio1', type=float, default=0.2, help='The ratio of chosen edges')
    parser.add_argument('--ratio2', type=float, default=0.05, help='The ratio of chosen client per edge')
    parser.add_argument('--optim', default='adam', type=str, help='optimizer')
    parser.add_argument('--momentum', default=0.9, type=float, help='momentum term')
    parser.add_argument('--beta1', default=0.9, type=float, help='Adam coefficients beta_1')
    parser.add_argument('--beta2', default=0.999, type=float, help='Adam coefficients beta_2')
    parser.add_argument('--weight_decay', default=5e-4, type=float,
                        help='weight decay for optimizers')
    parser.add_argument('--lr', type=float, default=0.001)
    parser.add_argument('--bs', type=int, default=128)
    parser.add_argument('--epochs', type=int, default=500)
    parser.add_argument('--client_epochs', type=int, default=1)
    parser.add_argument('--edge_epochs', type=int, default=1)
    parser.add_argument('--num_classes', type=int, default=10, help='cifar10')
    args = parser.parse_args()
    return args


def create_optimizer(args, model_params):
    if args.optim == 'sgd':
        return optim.SGD(model_params, args.lr, momentum=args.momentum,
                         weight_decay=args.weight_decay)
    elif args.optim == 'adagrad':
        return optim.Adagrad(model_params, args.lr, weight_decay=args.weight_decay)
    elif args.optim == 'adam':
        return optim.Adam(model_params, args.lr, betas=(args.beta1, args.beta2),
                          weight_decay=args.weight_decay)
    elif args.optim == 'amsgrad':
        return optim.Adam(model_params, args.lr, betas=(args.beta1, args.beta2),
                          weight_decay=args.weight_decay, amsgrad=True)
    else:
        raise ValueError('unknown optimizer')


def plot(curve, name):
    if not os.path.isdir('figure'):
        os.mkdir('figure')
    plt.figure()
    plt.xlabel('Epoch')
    plt.ylabel('Training loss')
    plt.plot(curve)
    plt.savefig('figure/{}.png'.format(name))


def reduce_dim(vector):
    # TODO
    pass


if __name__ == '__main__':

    args = get_parse()
    if args.fixed_seed:
        setup_seed(args.seed)
    device = 'cuda' if torch.cuda.is_available() else 'cpu'
    name = 'fed-edge{}-client{}_{}_C{}'.format(args.edge_num, args.client_num, args.ratio1, args.ratio2)

    client = Clients(args.num_classes, args.edge_num, args.client_num, args.bs, args.client_epochs, device)
    edge = Edges(client, args.num_classes, args.edge_num, args.client_num, args.bs, args.edge_epochs, device)
    cloud = Cloud(edge, args.num_classes, args.edge_num, args.client_num, args.bs, device)

    optimizer = create_optimizer(args, cloud.model.parameters())
    scheduler = optim.lr_scheduler.MultiStepLR(optimizer, milestones=[150, 300], gamma=0.1)

    train_losses, train_accuracies = [], []
    test_losses, test_accuracies = [], []

    for epoch in range(0, args.epochs):
        print('\nEpoch: %d' % epoch)
        train_loss, train_acc = cloud.train_epoch(optimizer, args.ratio1,
                                                  args.ratio1, device)
        vector = cloud.get_cloud_and_edges  # [<class 'collections.OrderedDict'>]
        reduce_dim(vector)
        print("Training Acc: {:.4f}, Loss: {:.4f}".format(train_acc, train_loss))
        test_acc, test_loss = cloud.run_test(device=device)
        print("Testing Acc: {:.4f}, Loss: {:.4f}".format(test_acc, test_loss))
        scheduler.step()

        # save loss and acc
        train_losses.append(train_loss)
        train_accuracies.append(train_acc)
        test_losses.append(test_loss)
        test_accuracies.append(test_acc)

        if not os.path.isdir('curve'):
            os.mkdir('curve')

        torch.save({'train_loss': train_loss, 'train_accuracy': train_acc,
                    'test_loss': test_loss, 'test_accuracy': test_acc}, os.path.join('curve', name))

    plot(train_loss, name)
