import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_ESEAFL = DIR_LOG + "eseafl/"

def parse_eseafl_file(log_file_name):
    log_file_path = f"{DIR_ESEAFL}/{log_file_name}"
    # keep valid logs
    logs = []
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith("AN:") or line.startswith("Client:") or line.startswith("Server:"):
                logs.append(line)
    # extract time and data
    log_data = [[] for i in range(9)]
    for line in logs:
        if line.startswith("AN: Malicious Setting (Aggregation Phase):"):
            if line.endswith("B"):
                log_data[0].append(float(line.split()[5]))
            if line.endswith("ms"):
                log_data[1].append(float(line.split()[5]))
        if line.startswith("Server: Malicious Setting (Aggregation Phase):"):
            if line.endswith("B"):
                log_data[2].append(float(line.split()[5]))
            if line.endswith("ms"):
                log_data[3].append(float(line.split()[5]))
        if line.startswith("Server: Commitment Aggregation Time"):
            log_data[4].append(float(line.split()[4]))
        if line.startswith("Client: Malicious Setting (Aggregation Phase):"):
            if line.endswith("B"):
                log_data[5].append(float(line.split()[5]))
            if line.endswith("ms"):
                log_data[6].append(float(line.split()[5]))
        if line.startswith("Client: Decryption Time:"):
            log_data[7].append(float(line.split()[3]))
        if line.startswith("Client: Commitment Time:"):
            log_data[8].append(float(line.split()[3]))
    for i in range(9):
        log_data[i] = np.median(log_data[i])
    return log_data

def parse_eseafl(n_clients, n_inputs):
    output_line = f"{n_clients},{n_inputs}"
    # parse log
    log_data = parse_eseafl_file(f"e-SeaFL_{n_inputs}_{n_clients}.log")
    # output
    an_time = log_data[1] / 1000
    an_sent = log_data[0] / 1024 / 1024
    client_aggr = log_data[6] / 1000
    client_decrypt = log_data[7] / 1000
    client_commit = log_data[8] * 60
    client_time = client_aggr + client_decrypt + client_commit
    client_sent = log_data[5] / 1024 / 1024
    server_aggr = log_data[3] / 1000
    server_commit = log_data[4] / 1000
    server_time = server_aggr + server_commit
    server_sent = log_data[2] / 1024 / 1024
    total_time = an_time + client_time + server_time
    total_sent = an_sent * 3 + client_sent * n_clients + server_sent
    output_line += f",{an_time},{an_sent},{client_aggr},{client_decrypt},{client_commit},{client_time},{client_sent},{server_aggr},{server_commit},{server_time},{server_sent},{total_time},{total_sent}"
    output_line += "\n"
    return output_line

output_file = open(f"{DIR_LOG}/eseafl.csv", "w")
output_file.write("N_Clients,N_Inputs,AN_Time,AN_Sent,Client_Aggr,Client_Decrypt,Client_Commit,Client_Time,Client_Sent,Server_Aggr,Server_Commit,Server_Time,Server_Sent,Total_Time,Total_Sent\n")

output_line = parse_eseafl(4, 62006)
output_file.write(output_line)

for nc in range(10, 50, 10):
    output_line = parse_eseafl(nc, 100_000)
    output_file.write(output_line)

output_file.close()