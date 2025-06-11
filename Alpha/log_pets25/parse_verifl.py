import numpy as np

DIR_LOG = "./Alpha/log_pets25/"
DIR_VERIFL = DIR_LOG + "verifl/"

def parse_verifl_file(log_file_name):
    log_file_path = f"{DIR_VERIFL}/{log_file_name}"
    # extract time and data
    log_data = [[] for i in range(6)]
    phase = ""
    side = ""
    time_ms = 0
    with open(log_file_path, "r") as log_file:
        lines = log_file.readlines()
        for line in lines:
            line = line.strip()
            if line == "-------------------- Aggregation phase --------------------":
                phase = "aggr"
            if line == "-------------------- Verification phase --------------------":
                phase = "veri"
            if line == "Client timer:":
                side = "client"
                time_ms = 0
            if line == "Server timer:":
                side = "server"
                time_ms = 0
            if "Average round" in line:
                time_ms += float(line.split()[5])
                if "Unmasking" in line and side == "server":
                    log_data[1].append(time_ms)
            if "Average receive" in line:
                time_ms += float(line.split()[4])
                log_data[0].append(time_ms)
            if "Amortized verification time" in line:
                if side == "client":
                    log_data[2].append(float(line.split()[4]))
                if side == "server":
                    log_data[3].append(float(line.split()[4]))
            if "Total:" in line:
                if phase == "aggr":
                    log_data[4].append(float(line.split()[2]))
                if phase == "veri":
                    log_data[5].append(float(line.split()[2]))
    for i in range(6):
        log_data[i] = np.median(log_data[i])
    return log_data

def parse_verifl(n_clients, n_inputs):
    output_line = f"{n_clients},{n_inputs}"
    # parse log
    log_data = parse_verifl_file(f"VeriFL_{n_inputs}_{n_clients}.log")
    # output
    client_aggr = log_data[0] / 1000
    client_veri = log_data[2] / 1000
    client_time = client_aggr + client_veri
    server_aggr = log_data[1] / 1000
    server_veri = log_data[3] / 1000
    server_time = server_aggr + server_veri
    data_aggr = log_data[4] / 1024
    data_veri = log_data[5] / 1024
    total_time = client_time + server_time
    total_sent = (data_aggr + data_veri) * n_clients
    output_line += f",{client_aggr},{client_veri},{client_time},{server_aggr},{server_veri},{server_time},{data_aggr},{data_veri},{total_time},{total_sent}"
    output_line += "\n"
    return output_line

output_file = open(f"{DIR_LOG}/verifl.csv", "w")
output_file.write("N_Clients,N_Inputs,Client_Aggr,Client_Veri,Client_Time,Server_Aggr,Server_Veri,Server_Time,Data_Aggr,Data_Veri,Total_Time,Total_Sent\n")

output_line = parse_verifl(4, 62006)
output_file.write(output_line)

for nc in range(10, 50, 10):
    output_line = parse_verifl(nc, 100_000)
    output_file.write(output_line)

output_file.close()