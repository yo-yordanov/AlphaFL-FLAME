/*
 * alpha-client.cpp
 *
 * This file implements the client side of AlphaFL.
 *
 */

#include "Alpha/alpha-pre.h"

void input_commit_honest(PlainPlayer &P, vector<Z2<KS>> &x, vector<Z2<KS>> &x0, vector<Z2<KS>> &x1, vector<Z2<K2S>> &m, vector<Z2<K2S>> &m0, vector<Z2<K2S>> &m1)
{
    // Proto_InCom
    // init
    Z2<S> alpha0 = get_alpha(0);
    Z2<S> alpha1 = get_alpha(1);
    int x_len = x.size();
    Z2<KS> xt0, xt1;
    Z2<K2S> mt0, mt1;

    // Step 1
    get_x0(x0);
    get_x0(xt0);
    get_m0(m0);
    get_m0(mt0);

    // Step 2
    Z2<K2S> alpha = Z2<K2S>(alpha0) + Z2<K2S>(alpha1); // larger ring for alpha
    PRNG local_prng;
    xt1.randomize(local_prng);
    for (int i = 0; i < x_len; ++i)
    {
        x1[i] = x[i] - x0[i];
    }
    Z2<K2S> xt = Z2<K2S>(xt0) + Z2<K2S>(xt1); // larger ring for xt

    // Step 3
    for (int i = 0; i < x_len; ++i)
    {
        m[i] = (Z2<K2S>(x0[i]) + Z2<K2S>(x1[i])) * alpha; // larger ring for x
        m1[i] = m[i] - m0[i];
    }
    Z2<K2S> mt = xt * alpha;
    mt1 = mt - mt0;

    // Step 4
    octetStream os;
    os.reset_write_head();
    for (int i = 0; i < x_len; ++i)
    {
        x1[i].pack(os);
        m1[i].pack(os);
    }
    xt1.pack(os);
    mt1.pack(os);
    P.send_to(1, os);
}

void input_commit_dishonest(PlainPlayer &P, vector<Z2<KS>> &x, vector<Z2<KS>> &x0, vector<Z2<KS>> &x1, vector<Z2<KS>> &xv, vector<Z2<K2S>> &b1)
{
    // Proto_InCom_DiHo
    int x_len = x.size();
    Z2<KS> xt0, xt1, xt;

    // Step 1
    get_x0(x0);
    get_x0(xt0);

    // Step 2
    for (int i = 0; i < x_len; ++i)
    {
        x1[i] = x[i] - x0[i];
        xv[i] = x[i];
    }
    PRNG local_prng;
    xt1.randomize(local_prng);
    xt = xt0 + xt1;
    xv[x_len] = xt;

    // Step 3 & 4
    // vOLE
    vole_send_simu(P, 0, b1, xv);

    // Step 5
    octetStream os;
    os.reset_write_head();
    for (int i = 0; i < x_len; ++i)
    {
        x1[i].pack(os);
        b1[i].pack(os);
    }
    xt1.pack(os);
    b1[x_len].pack(os);
    P.send_to(1, os);
}

int main(int argc, char **argv)
{
    if (argc < 4 || argc > 5)
    {
        cerr << "Usage: " << argv[0] << " <clientno>" << " <clientall>" << " <inputall>" << " [mode]" << endl;
        exit(1);
    }

    // mode:
    //   0 honest-majority
    //   1 dishonest-majority
    int mode = 0;
    if (argc == 5)
    {
        mode = atoi(argv[4]);
        if (mode < 0 || mode > 1)
            exit(1);
    }

    // set up networking on localhost
    int tot_in = atoi(argv[3]);
    int cli_no = atoi(argv[1]);
    int my_no = cli_no + NUM_SRV;
    int tot_cli = atoi(argv[2]);
    int tot_no = tot_cli + NUM_SRV;
    vector<string> nms(tot_no, "localhost");
    Names N(my_no, PORT_BASE, nms);
    PlainPlayer P(N);
    auto timer = Timer();

    // plaintext
    // L-inf is actually enforced here.
    vector<Z2<KS>> x(tot_in * L_INF), x0(tot_in * L_INF), x1(tot_in * L_INF);

    // input commitment
    if (mode == 0)
    {
        // honest-majority
        vector<Z2<K2S>> m(tot_in * L_INF), m0(tot_in * L_INF), m1(tot_in * L_INF);
        timer.start();
        input_commit_honest(P, x, x0, x1, m, m0, m1);
        timer.stop();
        cout << endl
             << "Client Input Commitment Honest:" << endl;
        cout << "Total Time = " << timer.elapsed_then_reset() << " seconds" << endl;
        P.total_comm().print();
    }
    else if (mode == 1)
    {
        // dishonest-majority
        vector<Z2<KS>> xv(tot_in * L_INF + 1);
        vector<Z2<K2S>> b1(tot_in * L_INF + 1);
        timer.start();
        input_commit_dishonest(P, x, x0, x1, xv, b1);
        timer.stop();
        cout << "Client Input Commitment DisHonest:" << endl;
        cout << "Total Time = " << timer.elapsed_then_reset() << " seconds" << endl;
        P.total_comm().print();
    }
}
