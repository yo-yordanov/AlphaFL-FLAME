import os.path
import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_PRIO = DIR_LOG + "prio/"

def parse_prio_file(log_file_name):
    log_file_path = f"{DIR_PRIO}/{log_file_name}"
    if not os.path.exists(log_file_path):
        return []
    # keep valid logs
    logs = []
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line.count(", ") == 7:
                logs.append(line)
    # extract time and data
    log_data = [[] for i in range(9)]
    for line in logs:
        fields = line.split(", ")
        if len(fields) < 8: continue
        converted = [float(fields[i]) for i in range(8)]
        for i in range(8):
            log_data[i].append(converted[i])
        log_data[8].append(sum(converted[2:]))
    for i in range(9):
        log_data[i] = np.median(log_data[i])
    return log_data

def parse_prio(n_clients, n_inputs):
    output_line = f"{n_clients},{n_inputs}"
    # parse Alice log
    log_data = parse_prio_file(f"alice_{n_clients}_{n_inputs}.log")
    if len(log_data) == 0:
        output_line += ",NA" * 7 + "\n"
        return output_line
    # output
    data_client = log_data[0] * 2 / 1024 / 1024 / n_clients
    data_server = log_data[1] / 1024 / 1024 * 2
    data_total = data_client * n_clients + data_server
    output_line += f",{log_data[2]},{data_client},{log_data[4]},{log_data[5]},{log_data[8]},{data_server},{data_total}"
    output_line += "\n"
    return output_line

output_file = open(f"{DIR_LOG}/prio.csv", "w")
output_file.write("N_Clients,N_Inputs,Client_Time,Client_Sent,Server_MPC_pre,Server_MPC_veri,Server_Time,Server_Sent,Total_Sent\n")
for nc in range(10, 51, 10):
    for ni in range(100_000, 500_001, 200_000):
        output_line = parse_prio(nc, ni)
        output_file.write(output_line)
output_file.close()