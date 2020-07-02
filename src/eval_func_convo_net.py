import torch
import torch.nn as nn
import torch.nn.functional as F

from . import chess

class EvalFuncConvoNet(torch.nn.Module):
    def __init__(self):
        super(EvalFuncConvoNet, self).__init__()
        self.conv1 = nn.Conv2d(13, 16, 3, stride=1)  # 16 shape (3, 3, 1) kernels; output HW is (8 - 3) / 1 + 1 = 6; (16, 6, 6)
        self.bn1 = nn.BatchNorm2d(16)
        self.dropout = nn.Dropout2d(0.3)
        self.fc = nn.Linear(16 * 3 * 3, 1)

    def forward(self, x):
        # shape (n, 8, 8, 13)
        # NHWC to NCHW
        x = x.permute(0, 3, 1, 2)
        # shape (n, 13, 8, 8)
        x = F.relu(self.bn1(self.conv1(x)))
        # shape (n, 16, 6, 6)
        x = F.max_pool2d(x, 2)  # kernel_size=2, stride_size=kernel_size
        # shape (n, 16, 3, 3)
        x = torch.flatten(x, 1)
        # shape (n, 16 * 3 * 3)
        x = self.fc(x)
        # shape (n, 1)
        return x.squeeze(-1)
    
    def loss(self, prediction, label):
        return F.mse_loss(prediction, label)
