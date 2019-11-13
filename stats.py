import re


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
    avg = 0
    mn = 1000
    mx = 0
    for r in pset:
        avg = avg + r.score
        mn = min(mn, r.score)
        mx = max(mx, r.score)
    print(f"P={p} {avg/len(pset)} (from {len(pset)}) min: {mn} max: {mx}")

print(f"{'-'*30} BY DIST AVG {'-'*30}")
for p, pset in byParam.items():
    avg = 0
    groups = {}
    for r in pset:
        d = r.bot_dist()
        if d not in groups:
            groups[d] = (0, 0)
        groups[d] = (groups[d][0] + 1, groups[d][1] + r.score)
    for d, r in groups.items():
        print(f"P={p} Dist={d} {r[1]/r[0]} (from {r[0]})")


