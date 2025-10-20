import os.path
import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_SA = DIR_LOG + "flame_leaky/"


def parse_file(n_clients, n_inputs):
    output_line = f"{n_clients},{n_inputs}"
    time_logs = []
    log_file_path = f"{DIR_SA}/fl_{n_clients}_{n_inputs}.log"
    if not os.path.exists(log_file_path):
        output_line += ",NA" * 22 + "\n"
        return output_line
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line.startswith("Time"):
                time_logs.append(line)
    if len(time_logs) == 0:
        output_line += ",NA" * 22 + "\n"
        return output_line
    time = {}
    data = {}
    for key in [f"Time{i}" for i in range(1, 12)]:
        time[key] = []
        data[key] = []
    for line in time_logs:
        fields = line.split()
        if len(fields) < 5:
            continue
        this_key = fields[0]
        this_time = float(fields[2])
        this_data = float(fields[4][1:])
        time[this_key].append(this_time)
        data[this_key].append(this_data)
    for key in [f"Time{i}" for i in range(1, 12)]:
        time[key] = np.median(time[key])
        data[key] = np.median(data[key])
        output_line += f",{time[key]},{data[key]}"
    output_line += "\n"
    return output_line


output_file = open(f"{DIR_LOG}/flame_leaky.csv", "w")
output_file.write(
    "N_Clients,N_Inputs,Total_Time,Total_Sent,B2A_Time,B2A_Sent,L2_Time,L2_Sent,Cos_Time,Cos_Sent,DBSCAN_Time,DBSCAN_Sent,Bound_Time,Bound_Sent,Clip_Time,Clip_Sent,Aggr_Time,Aggr_Sent,B2A_Noise_Time,B2A_Noise_Sent,Noise_Time,Noise_Sent,Veri_Time,Veri_Sent\n"
)
for ni in [62006, 274442, 817872]:
    output_line = parse_file(4, ni)
    output_file.write(output_line)
for nc in range(10, 51, 10):
    for ni in range(100_000, 500_001, 200_000):
        output_line = parse_file(nc, ni)
        output_file.write(output_line)
output_file.close()
