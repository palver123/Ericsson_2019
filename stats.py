import re
import numpy as np

class Res:
    def __init__(self,txt):
        exe_pattern = re.compile('.*Wololo2_(.*).exe')
        ll = txt.split()
        self.seed = int(ll[0])
        self.pc_mul = exe_pattern.search(ll[1]).group(1)
        self.out_pos = int(ll[2])
        self.bot_pos = int(ll[3])
        self.routers = ll[4]
        self.score = float(ll[5])

    def bot_dist(self):
        return min((self.out_pos - self.bot_pos) % 14,(self.bot_pos - self.out_pos ) % 14)

    def short_string(self):
        return f"PM: {self.pc_mul} Score: {self.score} Diff: { self.bot_dist() }"

    def out_router(self):
        return self.routers[self.out_pos * 10: self.out_pos * 10 + 10]


def router_groups(txt, c='1'):
    res = []
    ll = 0
    for t in txt:
        if t == c:
            ll = ll + 1
        else:
            if ll > 0:
                res.append(ll)
            ll = 0
    if ll > 0:
        if txt.startswith(c) and len(res) > 0:
            res[0] = res[0] + ll
        else:
            res.append(ll)
    return tuple(sorted(res, reverse=True))


byParam = {}

with open(r'nightly_stats.txt') as inp:
    for l in inp:
        l = l.strip()
        if len(l) < 1:
            continue
        rr = Res(l)
        if rr.pc_mul not in byParam:
            byParam[rr.pc_mul] = []
        if rr.score == -1: # I think these are network errors
            continue
        byParam[rr.pc_mul].append(rr)

print(f"{'-'*30} WHOLE AVG {'-'*30}")
for p, pset in byParam.items():
    print(f'----{p}----')
    groups = {}
    all = []
    for r in pset:
        key = router_groups(r.out_router())
        if key not in groups:
            groups[key] = []
        groups[key].append(r.score)
        all.append(r.score)

    for k, v in groups.items():
        print(f"Key={k} avg: {np.mean(v)} min: {min(v)} max: {max(v)} count {len(v)}")
    print(f"ALL avg: {np.mean(all)} min: {min(all)} max: {max(all)} count {len(all)}")
