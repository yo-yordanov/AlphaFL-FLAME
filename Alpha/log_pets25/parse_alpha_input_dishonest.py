import os.path
import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_INPUT = DIR_LOG + "alpha_input/"

def parse_dishonest_file(log_file_name):
    # exit if non-exist
    log_file_path = f"{DIR_INPUT}/{log_file_name}"
    if not os.path.exists(log_file_path):
        return [], [], [], []
    # keep valid logs
    time_logs = []
    data_logs = []
    vole_logs = []
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith("Total Time ="):
                time_logs.append(line)
            if line.startswith("Sending directly"):
                data_logs.append(line)
            if line.startswith("vOLE"):
                vole_logs.append(line)
    # exit if error (no valid logs)
    if len(time_logs) == 0:
        return [], [], [], []
    # extract time and data
    time = []
    data = []
    time_vole = []
    data_vole = []
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
    for line in vole_logs:
        fields = line.split()
        if len(fields) < 5: continue
        this_time = float(fields[2])
        time_vole.append(this_time)
        this_data = float(fields[4])
        data_vole.append(this_data)
    return time, data, time_vole, data_vole

def parse_dishonest(n_clients, n_inputs):
    time, data, time_vole, data_vole = {}, {}, {}, {}
    output_line = f"{n_clients},{n_inputs}"
    # parse Alice log
    t, d, tv, dv = parse_dishonest_file(f"server_{n_clients}_{n_inputs}_1_0.log")
    if len(t) == 0:
        output_line += ",NA" * 5 + "\n"
        return output_line
    time["alice"] = t
    data["alice"] = d
    time_vole["alice"] = tv
    data_vole["alice"] = dv
    # parse Bob log
    t, d, tv, dv = parse_dishonest_file(f"server_{n_clients}_{n_inputs}_1_1.log")
    if len(t) == 0:
        output_line += ",NA" * 5 + "\n"
        return output_line
    time["bob"] = t
    data["bob"] = d
    time_vole["bob"] = tv
    data_vole["bob"] = dv
    # parse Client logs
    time["client"] = []
    data["client"] = []
    time_vole["client"] = []
    data_vole["client"] = []
    for c in range(n_clients):
        t, d, tv, dv = parse_dishonest_file(f"client_{n_clients}_{n_inputs}_1_{c}.log")
        if len(t) == 0:
            output_line += ",NA" * 5 + "\n"
            return output_line
        time["client"] += t
        data["client"] += d
        time_vole["client"] += tv
        data_vole["client"] += dv
    # output
    for key in time: time[key] = np.median(time[key])
    for key in data: data[key] = np.median(data[key])
    for key in time_vole: time_vole[key] = np.median(time_vole[key])
    for key in data_vole: data_vole[key] = np.median(data_vole[key])
    data_server = data['alice'] + data['bob']
    data_total = data_server + data['client'] * n_clients
    output_line += f",{time['client']},{data['client']},{time_vole['client']},{data_vole['client']},{time['alice']},{data_server},{time_vole['alice']},{data_total}"
    output_line += "\n"
    return output_line

output_file = open(f"{DIR_LOG}/alpha_input_dishonest.csv", "w")
output_file.write("N_Clients,N_Inputs,Client_Time,Client_Sent,Client_vOLE_Time,Client_vOLE_Sent,Server_Time,Server_Sent,Server_vOLE_time,Total_Sent\n")
for nc in [1, 4]:
    for ni in [62006, 274442, 817872]:
        output_line = parse_dishonest(nc, ni)
        output_file.write(output_line)
for nc in range(10, 51, 10):
    for ni in range(100_000, 500_001, 200_000):
        output_line = parse_dishonest(nc, ni)
        output_file.write(output_line)
output_file.close()