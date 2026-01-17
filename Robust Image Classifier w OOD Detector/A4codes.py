import os
import sys
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms, models

# ==========================================
#              HYPERPARAMETERS
# ==========================================
BATCH_SIZE = 16
LEARNING_RATE = 0.01
MOMENTUM = 0.9
WEIGHT_DECAY = 0.005
NUM_EPOCHS = 80
NUM_CLASSES = 10
# ==========================================


class LabeledDataset(torch.utils.data.Dataset):
    def __init__(self, original_dataset, override_label=None):
        self.dataset = original_dataset
        self.override_label = override_label

    def __getitem__(self, index):
        data, label = self.dataset[index]
        if self.override_label is not None:
            label = self.override_label
        return data, label

    def __len__(self):
        return len(self.dataset)


class GatedResNet(nn.Module):
    def __init__(self, num_classes=10):
        super(GatedResNet, self).__init__()
        self.backbone = models.resnet18(weights=None)
        num_ftrs = self.backbone.fc.in_features
        self.backbone.fc = nn.Linear(num_ftrs, num_classes + 1)

    def forward(self, x):
        if self.training:
            return self.backbone(x)
        else:
            # --- Test-Time Augmentation (TTA) ---
            # 1. Standard Prediction
            logits_std = self.backbone(x)

            # 2. Flipped Prediction (Horizontal Flip)
            x_flip = torch.flip(x, dims=[3])  # Dim 3 is width
            logits_flip = self.backbone(x_flip)

            # Average the logits (Soft Ensemble)
            logits = (logits_std + logits_flip) / 2.0

            # --- Rejection Logic ---
            probs = F.softmax(logits, dim=1)
            preds = torch.argmax(probs, dim=1)

            final_outputs = logits[:, :10].clone()
            ood_indices = preds == 10

            if ood_indices.sum() > 0:
                random_noise = torch.randn(
                    ood_indices.sum(), 10, device=logits.device
                )
                final_outputs[ood_indices] = random_noise

            return final_outputs


def learn(path_to_in_domain: str, path_to_out_domain: str):
    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    # Standard Augmentation
    train_transforms = transforms.Compose(
        [
            transforms.RandomResizedCrop(224, scale=(0.8, 1.0)),
            transforms.RandomHorizontalFlip(),
            transforms.ColorJitter(brightness=0.2, contrast=0.2),
            transforms.RandomRotation(15),
            transforms.ToTensor(),
            transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225]),
        ]
    )

    if not os.path.exists(path_to_in_domain) or not os.path.exists(
        path_to_out_domain
    ):
        return None

    in_base = datasets.ImageFolder(
        root=path_to_in_domain, transform=train_transforms
    )
    out_base = datasets.ImageFolder(
        root=path_to_out_domain, transform=train_transforms
    )

    # Wrap datasets: ID=Labels, OOD=Label 10
    in_dataset = LabeledDataset(in_base, override_label=None)
    out_dataset = LabeledDataset(out_base, override_label=10)

    count_id = len(in_dataset)
    count_ood = len(out_dataset)

    # We want the total gradient contribution of the OOD class (Class 10)
    # to match the contribution of a SINGLE In-Domain class (e.g., Class 0).
    # Formula: Weight_OOD = (Total_ID / Num_ID_Classes) / Total_OOD
    if count_ood > 0:
        avg_id_samples = count_id / NUM_CLASSES
        ood_weight = avg_id_samples / count_ood
    else:
        ood_weight = 0.0

    full_dataset = torch.utils.data.ConcatDataset([in_dataset, out_dataset])
    train_loader = DataLoader(
        full_dataset,
        batch_size=BATCH_SIZE,
        shuffle=True,
        num_workers=2,
        drop_last=True,
    )

    model = GatedResNet(num_classes=NUM_CLASSES).to(device)

    optimizer = optim.SGD(
        model.parameters(),
        lr=LEARNING_RATE,
        momentum=MOMENTUM,
        weight_decay=WEIGHT_DECAY,
    )
    scheduler = optim.lr_scheduler.CosineAnnealingLR(
        optimizer, T_max=NUM_EPOCHS
    )

    # Apply Dynamic Weights
    class_weights = torch.ones(11).to(device)
    class_weights[10] = ood_weight

    # label_smoothing=0.1 prevents overfitting to specific examples
    criterion = nn.CrossEntropyLoss(weight=class_weights, label_smoothing=0.1)

    model.train()
    for epoch in range(NUM_EPOCHS):
        running_loss = 0.0

        for inputs, labels in train_loader:
            inputs, labels = inputs.to(device), labels.to(device)

            optimizer.zero_grad()
            outputs = model(inputs)
            loss = criterion(outputs, labels)

            loss.backward()
            optimizer.step()

            running_loss += loss.item()

        scheduler.step()

    return model


def compute_accuracy(path_to_eval_folder: str, model):
    if not os.path.exists(path_to_eval_folder):
        return 0.0

    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
    val_transform = transforms.Compose(
        [
            transforms.Resize(256),
            transforms.CenterCrop(224),
            transforms.ToTensor(),
            transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225]),
        ]
    )

    dataset = datasets.ImageFolder(
        root=path_to_eval_folder, transform=val_transform
    )
    dataloader = DataLoader(
        dataset, batch_size=BATCH_SIZE, shuffle=False, num_workers=2
    )

    model.eval()
    model = model.to(device)

    correct = 0
    total = 0

    with torch.no_grad():
        for inputs, labels in dataloader:
            inputs, labels = inputs.to(device), labels.to(device)
            outputs = model(inputs)
            _, preds = torch.max(outputs, 1)
            total += labels.size(0)
            correct += (preds == labels).sum().item()

    accuracy = correct / total if total > 0 else 0.0
    return accuracy


if __name__ == "__main__":
    base_path = os.path.dirname(os.path.abspath(__file__))
    in_domain_path = os.path.join(base_path, "A4data", "in-domain-train")
    out_domain_path = os.path.join(base_path, "A4data", "out-domain-train")
    in_domain_eval = os.path.join(base_path, "A4data", "in-domain-eval")
    out_domain_eval = os.path.join(base_path, "A4data", "out-domain-eval")

    start_event = torch.cuda.Event(enable_timing=True)
    end_event = torch.cuda.Event(enable_timing=True)

    # 1. Train
    start_event.record()
    trained_model = learn(in_domain_path, out_domain_path)
    end_event.record()
    torch.cuda.synchronize()
    elapsed_time = start_event.elapsed_time(end_event)

    # 2. Evaluate
    if trained_model:
        acc = compute_accuracy(in_domain_eval, trained_model)
        print(f"Final In-Domain Accuracy: {acc:.4f}")
        out_acc = compute_accuracy(out_domain_eval, trained_model)
        print(f"Final Out-Domain Accuracy: {out_acc:.4f}")
