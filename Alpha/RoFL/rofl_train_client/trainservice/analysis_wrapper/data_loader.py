
import numpy as np

from src.data.tf_data import Dataset, ImageGeneratorDataset
from src.data.leaf_loader import load_leaf_dataset, process_text_input_indices, process_char_output_indices
# Load dataset from working directory.

def load_dataset(dataset_path, batch_size, augment_data) -> Dataset:
    #return load_shakespeare()
    return load_cifar_dummy()

    # return load_mnist_dummy()

    val = np.load(dataset_path, allow_pickle=True)
    (x_train, y_train), (x_test, y_test) = (val[0], val[1]), (val[2], val[3])
    if augment_data:
        return ImageGeneratorDataset(x_train, y_train, batch_size=batch_size, x_test=x_test, y_test=y_test)
    else:
        return Dataset(x_train, y_train, batch_size=batch_size, x_test=x_test, y_test=y_test)

    # return load_cifar_dummy()

def load_federated_mnist_dummy():
    total_clients = 3383
    (x_train, y_train), (x_test, y_test) = Dataset.get_emnist_dataset(-1, total_clients)
    select = np.random.randint(0, total_clients)
    (x_train, y_train), (x_test, y_test) = (x_train[select], y_train[select]), (x_test[select], y_test[select])
    return Dataset(x_train, y_train, batch_size=32, x_test=x_test, y_test=y_test)

def load_mnist_dummy():
    frac_take = 1.0
    (x_train, y_train), (x_test, y_test) = Dataset.get_mnist_dataset(-1)
    indices_train = np.random.choice(x_train.shape[0], int(x_train.shape[0] * frac_take), replace=False)
    indices_test = np.random.choice(x_test.shape[0], int(x_test.shape[0] * frac_take), replace=False)

    (x_train, y_train), (x_test, y_test) = (x_train[indices_train, :], y_train[indices_train]), \
                                           (x_test[indices_test, :], y_test[indices_test])
    return Dataset(x_train, y_train, batch_size=32, x_test=x_test, y_test=y_test)

def load_cifar_dummy():
    frac_take = 0.1
    (x_train, y_train), (x_test, y_test) = Dataset.get_cifar10_dataset(-1)
    indices_train = np.random.choice(x_train.shape[0], int(x_train.shape[0] * frac_take), replace=False)
    indices_test = np.random.choice(x_test.shape[0], int(x_test.shape[0] * frac_take), replace=False)

    (x_train, y_train), (x_test, y_test) = (x_train[indices_train, :], y_train[indices_train]), \
                                           (x_test[indices_test, :], y_test[indices_test])
    return Dataset(x_train, y_train, batch_size=64, x_test=x_test, y_test=y_test)

def load_shakespeare():
    users, train_data, test_data = load_leaf_dataset("shakespeare")

    x_train = [process_text_input_indices(train_data[user]['x']) for user in users]
    y_train = [process_char_output_indices(train_data[user]['y']) for user in users]

    x_test = np.concatenate([process_text_input_indices(test_data[user]['x']) for user in users])
    y_test = np.concatenate([process_char_output_indices(test_data[user]['y']) for user in users])

    x_train = np.concatenate(x_train)
    y_train = np.concatenate(y_train)

    return Dataset(x_train, y_train, batch_size=64, x_test=x_test, y_test=y_test)