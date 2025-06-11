/*
 * alpha-server.cpp
 *
 * This file implements the server side of AlphaFL.
 *
 */

#include "Alpha/alpha-pre.h"

void input_commit_honest(PlainPlayer &P, int cli_no, vector<Z2<KS>> &x, vector<Z2<K2S>> &m, vector<Z2<S>> &r, Z2<K2S> &v, Z2<K2S> &d)
{
    // Proto_InCom
    int x_len = x.size();
    Z2<KS> xt;
    Z2<K2S> mt;

    // Step 1
    if (P.my_num() == 0)
    {
        get_x0(x);
        get_x0(xt);
        get_m0(m);
        get_m0(mt);
    }

    // Step 4
    if (P.my_num() == 1)
    {
        octetStream os;
        P.receive_player(cli_no, os);
        for (int i = 0; i < x_len; ++i)
        {
            x[i] = os.get<Z2<KS>>();
            m[i] = os.get<Z2<K2S>>();
        }
        xt = os.get<Z2<KS>>();
        mt = os.get<Z2<K2S>>();
    }

    // Consistency check
    // Step 5 & 6
    v += Z2<K2S>(xt);
    for (int i = 0; i < x_len; ++i)
    {
        v += Z2<K2S>(r[i]) * Z2<K2S>(x[i]);
    }
    d += mt;
    for (int i = 0; i < x_len; ++i)
    {
        d += m[i] * Z2<K2S>(r[i]);
    }
}

void input_commit_dishonest(PlainPlayer &P, int cli_no, Z2<S> &alpha, vector<Z2<KS>> &x, vector<Z2<K2S>> &m, vector<Z2<K2S>> &ab0, vector<Z2<K2S>> &ab1, vector<Z2<S>> &r, Z2<K2S> &v, Z2<K2S> &d)
{
    // Proto_InCom_DiHo
    int x_len = x.size();
    Z2<KS> xt;
    Z2<K2S> mt;

    // Pre-Step 1
    if (P.my_num() == 0)
    {
        get_x0(x);
        get_x0(xt);
    }

    // Pre-Step 2
    x.push_back(xt);

    // Pre-Step 3 & 4
    // vOLE
    if (P.my_num() == 0)
    {
        // vole_send_simu(P, 1, b0, xv);
        vole_send_fake(ab0, x);
    }
    if (P.my_num() == 1)
    {
        // vole_recv_simu(P, 0, a0, alpha);
        vole_recv_fake(ab0, alpha);
    }
    x.pop_back();

    // Step 3 & 4
    // vOLE
    if (P.my_num() == 0)
    {
        vole_recv_simu(P, cli_no, ab1, alpha); // a1
    }

    // Step 5
    if (P.my_num() == 1)
    {
        octetStream os;
        P.receive_player(cli_no, os);
        for (int i = 0; i < x_len; ++i)
        {
            x[i] = os.get<Z2<KS>>();
            ab1[i] = os.get<Z2<K2S>>(); // b1
        }
        xt = os.get<Z2<KS>>();
        ab1[x_len] = os.get<Z2<K2S>>(); // b1
    }

    // Step 6
    if (P.my_num() == 0)
    {
        for (int i = 0; i < x_len; ++i)
        {
            m[i] = ab1[i] - ab0[i]; // a1 - b0
        }
        mt = ab1[x_len] - ab0[x_len]; // a1 - b0
    }
    if (P.my_num() == 1)
    {
        for (int i = 0; i < x_len; ++i)
        {
            m[i] = Z2<K2S>(x[i]) * Z2<K2S>(alpha) + ab0[i] - ab1[i]; // a0 - b1
        }
        mt = Z2<K2S>(xt) * Z2<K2S>(alpha) + ab0[x_len] - ab1[x_len]; // a0 - b1
    }

    // Consistency check
    // Step 5 & 6
    v += Z2<K2S>(xt);
    for (int i = 0; i < x_len; ++i)
    {
        v += Z2<K2S>(r[i]) * Z2<K2S>(x[i]);
    }
    d += mt;
    for (int i = 0; i < x_len; ++i)
    {
        d += m[i] * Z2<K2S>(r[i]);
    }
}

void consistency_check_one(PlainPlayer &P, int cli_no, SeededPRNG &RS, Z2<S> &alpha, vector<Z2<KS>> &x, Z2<KS> &xt, vector<Z2<K2S>> &m, Z2<K2S> &mt)
{
    // Consistency check for both honest-majority and dishonest majority
    // Step 5 & 6
    int x_len = x.size();
    vector<Z2<S>> r(x_len);
    Z2<K2S> v, vl(xt), d(mt), z, zl;
    for (int i = 0; i < x_len; ++i)
    {
        r[i] = RS.get<Z2<S>>();
        vl += Z2<K2S>(r[i]) * Z2<K2S>(x[i]);
        d += m[i] * Z2<K2S>(r[i]);
    }

    // Step 7
    octetStream os;
    os.reset_write_head();
    vl.pack(os);
    P.send_to(NUM_SRV - 1 - P.my_num(), os);
    P.receive_player(NUM_SRV - 1 - P.my_num(), os);
    v.unpack(os);
    v += vl;
    zl = d - v * Z2<K2S>(alpha);
    os.reset_write_head();
    zl.pack(os);
    P.send_to(NUM_SRV - 1 - P.my_num(), os);
    P.receive_player(NUM_SRV - 1 - P.my_num(), os);
    z.unpack(os);

    // Step 8
    z += zl;
    if (z != 0)
    {
        cout << "Commitment failed S" << P.my_num() << "C" << cli_no << ": z = " << z << endl;
    }
    else
    {
        // cout << "Commitment succeeded S" << P.my_num() << "C" << cli_no << endl;
    }
}

void consistency_check_all(PlainPlayer &P, Z2<S> &alpha, vector<Z2<K2S>> &vl, vector<Z2<K2S>> &vr, vector<Z2<K2S>> &d, vector<Z2<K2S>> &zl, vector<Z2<K2S>> &zr)
{
    // Step 7
    int tot_cli = vl.size();
    octetStream os;
    os.reset_write_head();
    for (int i = 0; i < tot_cli; i++)
    {
        vl[i].pack(os);
    }
    P.send_to(NUM_SRV - 1 - P.my_num(), os);
    P.receive_player(NUM_SRV - 1 - P.my_num(), os);
    for (int i = 0; i < tot_cli; i++)
    {
        vr[i].unpack(os);
        vr[i] += vl[i];
        zl[i] = d[i] - vr[i] * Z2<K2S>(alpha);
    }
    os.reset_write_head();
    for (int i = 0; i < tot_cli; i++)
    {
        zl[i].pack(os);
    }
    P.send_to(NUM_SRV - 1 - P.my_num(), os);
    P.receive_player(NUM_SRV - 1 - P.my_num(), os);

    // Step 8
    for (int i = 0; i < tot_cli; i++)
    {
        zr[i].unpack(os);
        zr[i] += zl[i];
        if (zr[i] != 0)
        {
            cout << "Commitment failed S" << P.my_num() << "C" << i << ": z = " << zr[i] << endl;
        }
    }
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
    //   1 dishonest-majority without vOLE pre-processing
    //   2 dishonest-majority with vOLE pre-processing
    int mode = 0;
    if (argc == 5)
    {
        mode = atoi(argv[4]);
        if (mode < 0 || mode > 2)
            exit(1);
    }

    // set up networking on localhost
    int tot_in = atoi(argv[3]);
    int my_no = atoi(argv[1]);
    int tot_cli = atoi(argv[2]);
    int tot_no = tot_cli + NUM_SRV;
    vector<string> nms(tot_no, "localhost");
    Names N(my_no, PORT_BASE, nms);
    PlainPlayer P(N);

    // correlated randomness between servers
    SeededPRNG RS;
    unsigned char seed0[SEED_SIZE] = {1, 1, 1, 1};
    RS.SetSeed(seed0);

    // init all x, m, r, v, d and z for concurrency
    vector<vector<Z2<KS>>> x(tot_cli, vector<Z2<KS>>(tot_in * L_INF));
    vector<vector<Z2<K2S>>> m(tot_cli, vector<Z2<K2S>>(tot_in * L_INF));
    vector<vector<Z2<K2S>>> ab0(tot_cli), ab1(tot_cli);
    vector<vector<Z2<S>>> r(tot_cli, vector<Z2<S>>(tot_in * L_INF));
    for (int i = 0; i < tot_cli; i++)
    {
        for (int j = 0; j < tot_in * L_INF; j++)
        {
            r[i][j] = RS.get<Z2<S>>();
        }
    }
    vector<Z2<K2S>> vl(tot_cli), vr(tot_cli), d(tot_cli), zl(tot_cli), zr(tot_cli);
    if (mode == 1)
    {
        for (int i = 0; i < tot_cli; i++)
        {
            ab0[i].resize(tot_in * L_INF + 1);
            ab1[i].resize(tot_in * L_INF + 1);
        }
    }

    vector<thread> threads(tot_cli);
    auto timer = Timer();
    timer.start();
    Z2<S> alpha = get_alpha(my_no);
    //  input commitment
    for (int i = 0; i < tot_cli; ++i)
    {
        if (mode == 0)
        {
            // honest-majority
            threads[i] = thread(input_commit_honest, ref(P), i + NUM_SRV, ref(x[i]), ref(m[i]), ref(r[i]), ref(vl[i]), ref(d[i]));
        }
        else if (mode == 1)
        {

            // dishonest-majority
            threads[i] = thread(input_commit_dishonest, ref(P), i + NUM_SRV, ref(alpha), ref(x[i]), ref(m[i]), ref(ab0[i]), ref(ab1[i]), ref(r[i]), ref(vl[i]), ref(d[i]));
        }
    }
    for (int i = 0; i < tot_cli; ++i)
    {
        threads[i].join();
    }

    // Centralised consistency check prep (Step 5&6) with OpenMP, slower
    // #pragma omp declare reduction(mysum : Z2<K2S> : omp_out += omp_in) initializer(omp_priv(0))
    // #pragma omp parallel for reduction(mysum : vl[0 : tot_cli], d[0 : tot_cli]) private(j)
    //     for (int i = 0; i < tot_in * L_INF; i++)
    //     {
    //         for (int j = 0; j < tot_cli; j++)
    //         {
    //             vl[j] += Z2<K2S>(r[j][i]) * Z2<K2S>(x[j][i]);
    //             d[j] += m[j][i] * Z2<K2S>(r[j][i]);
    //         }
    //     }

    consistency_check_all(P, alpha, vl, vr, d, zl, zr);
    timer.stop();

    cout << endl;
    if (mode == 0)
    {
        cout << "Server " << my_no << " Input Commitment Honest:" << endl;
    }
    else if (mode == 1)
    {
        cout << "Server " << my_no << " Input Commitment DisHonest:" << endl;
    }
    cout << "Total Time = " << timer.elapsed_then_reset() << " seconds" << endl;
    P.total_comm().print();
}
