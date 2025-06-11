/*
 * To run with 2 parties / SPDZ engines:
 *   ./Scripts/setup-clients.sh to create SSL keys and certificates for clients
 *   ./alpha_input_client.x 0 2 1 32 1
 */

#include "Math/Z2k.hpp"
#include "Networking/sockets.h"
#include "Networking/ssl_sockets.h"
#include "Protocols/fake-stuff.h"
#include "ExternalIO/Client.hpp"

#include <iostream>

template <class T>
void one_run(vector<T> &inputs, Client &client)
{
    // Run the computation
    auto bytes = client.send_private_inputs<T>(inputs);
    cout << "Sent private inputs to each SPDZ engine." << endl;
    cout << "Total data " << 1e-6 * bytes << " MB" << endl;
}

template <class T>
void run(vector<int> &inputs, Client &client)
{
    vector<T> converted(inputs.begin(), inputs.end());
    one_run<T>(converted, client);
}

int main(int argc, char **argv)
{
    int my_client_id;
    int nparties, n_inputs, n_bits;
    vector<int> inputs;
    size_t finish;
    int port_base = 14000;

    if (argc < 5)
    {
        cout << "Usage is alpha_input_client <client identifier> <number of spdz parties> "
             << "<number of inputs> <number of bits per input> <finish (0 false, 1 true)>" << endl;
        exit(0);
    }

    my_client_id = atoi(argv[1]);
    nparties = atoi(argv[2]);
    n_inputs = atoi(argv[3]);
    n_bits = atoi(argv[4]);
    finish = atoi(argv[5]);
    vector<string> hostnames(nparties, "localhost");
    inputs.resize(n_inputs * n_bits, 0);

    bigint::init_thread();

    // Setup connections from this client to each party socket
    Client client(hostnames, port_base, my_client_id);
    auto &specification = client.specification;
    auto &sockets = client.sockets;
    for (int i = 0; i < nparties; i++)
    {
        octetStream os;
        os.store(finish);
        os.Send(sockets[i]);
    }
    cout << "Finish setup socket connections to SPDZ engines." << endl;

    int type = specification.get<int>();
    switch (type)
    {
    case 'R':
    {
        int R = specification.get<int>();
        int R2 = specification.get<int>();
        if (R2 != 64)
        {
            cerr << R2 << "-bit ring not implemented" << endl;
        }

        switch (R)
        {
        case 64:
            run<Z2<64>>(inputs, client);
            break;
        case 104:
            run<Z2<104>>(inputs, client);
            break;
        case 128:
            run<Z2<128>>(inputs, client);
            break;
        default:
            cerr << R << "-bit ring not implemented";
            exit(1);
        }
        break;
    }
    default:
        cerr << "Type " << type << " not implemented";
        exit(1);
    }

    return 0;
}
