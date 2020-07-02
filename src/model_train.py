import os
import csv
import numpy as np
import torch
import torch.optim as optim

from . import play_games
from . import eval_func_convo_net
from . import chess

class ChessDataset(torch.utils.data.Dataset):
    '''
    Dataloader for chess snapshot datasets
    '''
    def __init__(self, snapshot_file):
        self.boards = []
        self.labels = []
        with open(snapshot_file, 'r') as snapshot_file_:
            reader = csv.reader(snapshot_file_)
            for row in reader:
                # ignore turn counter for now
                self.boards.append(play_games.board_tensor(chess.board_make(row[0]), one_hot=True, free=True))
                self.labels.append(float(row[1]))
        self.boards = torch.stack(self.boards)
        self.labels = torch.as_tensor(self.labels)

    def __len__(self):
        return len(self.labels)
    
    def __getitem__(self, idx):
        if torch.is_tensor(idx):
            idx = idx.tolist()
        return (self.boards[idx], self.labels[idx])

def split_dataset(dataset_file, p_dist=(0.8, 0.2), shuffle=True):
    '''
    Splits a dataset at the specified file by rows (shuffling first if specified) and according
    to the specified probability distribution.
    Returns a list of the filenames of the generated dataset shards, ordered corresponding to p_dist
    by cardinality relative to the original dataset.
    '''
    with open(dataset_file, 'r') as dataset_file_:
        reader = csv.reader(dataset_file_)
        rows = np.array([row for row in reader])
        if shuffle:
            np.random.shuffle(rows)
        p_dist = (len(rows) * np.cumsum([0] + list(p_dist))).astype(int)
        shards = [rows[p_dist[i]:p_dist[i+1],:] for i in range(len(p_dist) - 1)]
        shard_files = ['%s_%d.csv' % (dataset_file[:-4], i) for i in range(len(shards))]
        for i in range(len(shards)):
            with open(shard_files[i], 'w') as shard_file:
                writer = csv.writer(shard_file)
                for row in shards[i]:
                    writer.writerow(row)
        return shard_files

def train(model, train_loader, optimizer):
    model.train()
    for i, (data, label) in enumerate(train_loader):
        optimizer.zero_grad()
        prediction = model(data)
        loss = model.loss(prediction, label)
        loss.backward()
        optimizer.step()
        if i % 50 == 0:
            print('iteration %d loss %.3f' % (i, loss.item()))

if __name__ == '__main__':
    data_dir = os.path.join(os.path.split(os.path.dirname(__file__))[0], 'data')  # ../data
    train_file, test_file = split_dataset(os.path.join(data_dir, 'snapshots560932351177912129.csv'))
    data_train = ChessDataset(train_file)
    train_loader = torch.utils.data.DataLoader(data_train, batch_size=16)
    data_test = ChessDataset(test_file)
    test_loader = torch.utils.data.DataLoader(data_test, batch_size=16)

    model = eval_func_convo_net.EvalFuncConvoNet()
    optimizer = optim.SGD(model.parameters(), lr=1e-3, momentum=0.9, weight_decay=1e-5)

    epochs = 20
    for i in range(epochs):
        print('epoch %d' % i)
        train(model, train_loader, optimizer)
