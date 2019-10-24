#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <set>
#include <cstdio>
#include <cmath>
#include <deque>
#include <unordered_map>
#include <tuple>
#include <cstring>
#include <functional>
#include <limits>
#include <sstream>
#include <unordered_map>
#define FMT_HEADER_ONLY
#include "fmt/format.h"

using std::vector;
using std::string;
using std::map;
using std::set;
using std::min;
using std::max;
using std::pair;
using std::make_pair;
using std::deque;
using std::pair;
using std::tuple;
using std::function;
using std::numeric_limits;
using std::stringstream;
using std::cin;
using std::cout;
using std::unordered_map;
using std::abs;
using ll = long long;
typedef pair<int,int> point;

#define REP(i, n) for (int i = 0; (i) < (int)(n); ++ (i))
#define REP3(i, m, n) for (int i = (m); (i) < (int)(n); ++ (i))
#define REP_R(i, n) for (int i = int(n) - 1; (i) >= 0; -- (i))
#define REP3R(i, m, n) for (int i = int(n) - 1; (i) >= (int)(m); -- (i))
#define LL_REP(i, n) for (ll i = 0; (i) < (ll)(n); ++ (i))
#define LL_REP3(i, m, n) for (ll i = (m); (i) < (ll)(n); ++ (i))
#define LL_REP_R(i, n) for (ll i =  ll(n) - 1; (i) >= 0; -- (i))
#define LL_REP3R(i, m, n) for (ll i = ll(n) - 1; (i) >= (ll)(m); -- (i))
#define ALL(x) begin(x), end(x)
#define dump(x) cerr << #x " = " << x << endl

constexpr ll magic_mod= 1000000007;
inline ll m_pluss(ll a, ll b)
{
    return (a + b) % magic_mod;
}
inline ll m_mul(ll a, ll b)
{
    return (a * b) % magic_mod;
}
