/*
 * alpha-pre.h
 *
 * This file defines the shared pre-processing for AlphaFL.
 *
 */
#include <chrono>
#include <thread>
#include "Networking/Player.h"
#include "Math/Z2k.hpp"
#include "OT/Row.hpp"
#include "Tools/time-func.h"

#define NUM_SRV 2
#define PORT_BASE 9999
#define L_INF 32
#define K 1
#define S 63
#define KS K + S
#define K2S K + S + S
int MERGED_ROUNDS = 1;

Z2<S> get_alpha(int party_no)
{
    if (party_no == 0)
    {
        // alpha0 should be generated via shared SeededPRNG by Client and Server0 in Proto_InCom
        // alpha0 should be generated via local PRNG by Server0 in Proto_InCom_DiHo
        return 0;
    }
    else
    {
        // alpha1 should be generated via shared SeededPRNG by Client and Server1 in Proto_InCom
        // alpha1 should be generated via local PRNG by Server1 in Proto_InCom_DiHo
        return 1;
    }
}

void get_x0(vector<Z2<KS>> &x0)
{
    // x0 should be generated via shared SeededPRNG by Client and Server0
    fill(x0.begin(), x0.end(), 0);
}

void get_x0(Z2<KS> &x0)
{
    // x0 should be generated via shared SeededPRNG by Client and Server0
    x0 = 0;
}

void get_m0(vector<Z2<K2S>> &m0)
{
    // m0 should be generated via shared SeededPRNG by Client and Server0 in Proto_InCom
    fill(m0.begin(), m0.end(), 0);
}

void get_m0(Z2<K2S> &m0)
{
    // m0 should be generated via shared SeededPRNG by Client and Server0 in Proto_InCom
    m0 = 0;
}

void vole_send_fake(vector<Z2<K2S>> &b, vector<Z2<KS>> &x)
{
    // b should be generated via local PRNG
    fill(b.begin(), b.end(), 0);
    (void)x;
}

void vole_recv_fake(vector<Z2<K2S>> &a, Z2<S> &alpha)
{
    // a0 = b0 + alpha1 * x0
    // here we assume b0 = 0 (vole_send_fake) and x0 = 0 (get_x0)
    fill(a.begin(), a.end(), 0);
    (void)alpha;
}

void vole_prng_fake(vector<pair<SeededPRNG, SeededPRNG>> &vole_prngs)
{
    // the first phase of vOLE: pre-shared SeededPRNGs can be exchanged via OTE
    for (int i = 0; i < S; i++)
    {
        unsigned char seed0[SEED_SIZE] = {0};
        vole_prngs[i].first.SetSeed(seed0);
        unsigned char seed1[SEED_SIZE] = {1};
        vole_prngs[i].second.SetSeed(seed1);
    }
}

SeededPRNG &vole_prng(vector<pair<SeededPRNG, SeededPRNG>> &vole_prngs, int bit_no, int choice)
{
    if (choice == 0)
    {
        return vole_prngs[bit_no].first;
    }
    else
    {
        return vole_prngs[bit_no].second;
    }
}

void vole_send_simu(PlainPlayer &P, int other, vector<Z2<K2S>> &output, vector<Z2<KS>> &x)
{
    vector<pair<SeededPRNG, SeededPRNG>> vole_prngs(S);
    vole_prng_fake(vole_prngs);
    size_t bytes = 0;
    auto timer = Timer();
    timer.start();
    int x_len = x.size();
    octetStream os;
    os.reserve(x_len * 18);
    Row<Z2<K2S>> vx(x_len), res(x_len), t0(x_len), t1(x_len);
    for (int i = 0; i < x_len; ++i)
        vx.rows[i] = x[i];
    for (int i = 0; i < S; ++i)
    {
        t0.randomize(vole_prng(vole_prngs, i, 0), x_len);
        t1.randomize(vole_prng(vole_prngs, i, 1), x_len);
        (vx + t1 + t0).pack(os);
        t1 = t0 << i;
        res += t1;
        if (i % MERGED_ROUNDS == MERGED_ROUNDS - 1 || i == S - 1)
        {
            bytes += os.get_length();
            P.send_to(other, os);
            os.reset_write_head();
        }
    }
    for (int i = 0; i < x_len; ++i)
        output[i] = res.rows[i];
    cout << "vOLE Send: " << timer.elapsed() << " seconds, " << 1e-6 * bytes << " MB" << endl;
    timer.stop();
}

void vole_recv_simu(PlainPlayer &P, int other, vector<Z2<K2S>> &output, Z2<S> &alpha)
{
    vector<pair<SeededPRNG, SeededPRNG>> vole_prngs(S);
    vole_prng_fake(vole_prngs);
    size_t bytes = 0;
    auto timer = Timer();
    timer.start();
    int x_len = output.size();
    octetStream os;
    os.reserve(x_len * 18);
    Row<Z2<K2S>> res(x_len), u(x_len), t(x_len), a(x_len);
    os.reset_write_head();
    P.receive_player(other, os);
    bytes += os.get_length();
    for (int i = 0; i < S; ++i)
    {
        int choice = alpha.get_bit(i);
        t.randomize(vole_prng(vole_prngs, i, choice), x_len);
        if (choice == 1)
        {
            u.unpack(os);
            a = u - t;
        }
        else
        {
            a = t;
            u.unpack(os);
        }
        u = a << i;
        res += u;
        if (i % MERGED_ROUNDS == MERGED_ROUNDS - 1 && i != S - 1)
        {
            os.reset_write_head();
            P.receive_player(other, os);
            bytes += os.get_length();
        }
    }
    for (int i = 0; i < x_len; ++i)
        output[i] = res.rows[i];
    timer.stop();
    cout << "vOLE Recv: " << timer.elapsed() << " seconds, " << 1e-6 * bytes << " MB" << endl;
}