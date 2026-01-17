# Robust Image Classifier with OOD Detection 👁️

A robust deep learning model designed for image classification that can effectively identify and reject **Out-Of-Distribution (OOD)** samples. This project addresses the common failure mode of AI models confidently classifying unknown objects into known categories.

## 🎯 Project Goals

*   **Accuracy**: High-performance classification on standard datasets (e.g., CIFAR-10/ImageNet subsets).
*   **Robustness**: Ability to detect when an image does *not* belong to any of the trained classes (OOD Detection).
*   **Reliability**: Using test-time techniques to ensure predictions are stable.

## 🏗️ Technical Architecture

### 1. Gated ResNet Backbone
*   **Base Model**: ResNet18 (Pre-trained options available).
*   **Modification**: The final fully connected layer is modified to support $N+1$ classes (N target classes + 1 Rejection/OOD class).
*   **Mechanism**: The model learns to route unknown features to the extra "catch-all" class.

### 2. Test-Time Augmentation (TTA)
To increase prediction confidence, the system performs inference multiple times on slightly modified versions of the input image (e.g., horizontal flips).
*   **Process**:
    1.  Predict on original image.
    2.  Predict on flipped image.
    3.  Average the logits (Soft Ensemble).
*   **Benefit**: Reduces variance and improves robustness against noise.

### 3. Out-of-Distribution Rejection
The model implements specific logic to handle uncertainty. If the model confidently predicts the $N+1^{th}$ class (the OOD class), it replaces the output with a random noise tensor or a specific rejection signal, preventing false positives.

## 🛠️ Technology Stack

*   **Language**: Python
*   **Library**: PyTorch, Torchvision
*   **Model**: ResNet18 (Residual Neural Network)
*   **Techniques**: Transfer Learning, Data Augmentation, Ensemble Learning

## 📂 Code Structure

*   `A4codes.py`: Main model definition, training loop, and evaluation logic.
    *   **Class `GatedResNet`**: The custom model architecture.
    *   **Function `learn()`**: Handles data loading, augmentation pipelines, and training.
*   `find_device.py`: Utility to select the best available hardware accelerator (CUDA/MPS/CPU).

## 🚀 Usage

The system is designed to be trained on two datasets: an **In-Domain** set (classes 1-10) and an **Out-Domain** set (random images).

```python
from A4codes import learn

# Train the model
model = learn(path_to_in_domain="data/train", path_to_out_domain="data/ood")
```
