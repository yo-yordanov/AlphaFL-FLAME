import os.path
import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_INPUT = DIR_LOG + "alpha_input/"

def parse_honest_file(log_file_name):
    # exit if non-exist
    log_file_path = f"{DIR_INPUT}/{log_file_name}"
    if not os.path.exists(log_file_path):
        return [], []
    # keep valid logs
    time_logs = []
    data_logs = []
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith("Total Time ="):
                time_logs.append(line)
            if line.startswith("Sending directly"):
                data_logs.append(line)
    # exit if error (no valid logs)
    if len(time_logs) == 0:
        return [], []
    # extract time and data
    time = []
    data = []
    for line in time_logs:
        fields = line.split()
        if len(fields) < 4: continue
        this_time = float(fields[3])
        time.append(this_time)
    for line in data_logs:
        fields = line.split()
        if len(fields) < 3: continue
        this_data = float(fields[2])
        data.append(this_data)
    return time, data

def parse_honest(n_clients, n_inputs):
    time, data = {}, {}
    output_line = f"{n_clients},{n_inputs}"
    # parse Alice log
    t, d = parse_honest_file(f"server_{n_clients}_{n_inputs}_0_0.log")
    if len(t) == 0:
        output_line += ",NA" * 5 + "\n"
        return output_line
    time["alice"] = t
    data["alice"] = d
    # parse Bob log
    t, d = parse_honest_file(f"server_{n_clients}_{n_inputs}_0_1.log")
    if len(t) == 0:
        output_line += ",NA" * 5 + "\n"
        return output_line
    time["bob"] = t
    data["bob"] = d
    # parse Client logs
    time["client"] = []
    data["client"] = []
    for c in range(n_clients):
        t, d = parse_honest_file(f"client_{n_clients}_{n_inputs}_0_{c}.log")
        if len(t) == 0:
            output_line += ",NA" * 5 + "\n"
            return output_line
        time["client"] += t
        data["client"] += d
    # output
    for key in time: time[key] = np.median(time[key])
    for key in data: data[key] = np.median(data[key])
    data_server = data['alice'] + data['bob']
    data_total = data_server + data['client'] * n_clients
    output_line += f",{time['client']},{data['client']},{time['alice']},{data_server},{data_total}"
    output_line += "\n"
    return output_line

output_file = open(f"{DIR_LOG}/alpha_input_honest.csv", "w")
output_file.write("N_Clients,N_Inputs,Client_Time,Client_Sent,Server_Time,Server_Sent,Total_Sent\n")
for nc in [1, 4]:
    for ni in [62006, 274442, 817872]:
        output_line = parse_honest(nc, ni)
        output_file.write(output_line)
for nc in range(10, 51, 10):
    for ni in range(100_000, 500_001, 200_000):
        output_line = parse_honest(nc, ni)
        output_file.write(output_line)
output_file.close()