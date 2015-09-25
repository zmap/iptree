import SubnetTree
import json

def populateTrees(file, s):
    with open(file) as f:
        for line in f:
            data = json.loads(line)
            s[str(data["bgp_prefix"])] = str(data["bgp_prefix"])

def lookupAndComp(s, ip):
    #val1 = s[ip]
    val2 = iptree.lookup_best(i, ip)
    #if val1 != val2:
    #    print "Subnet tree says: " + val1 + " but iptree says: " + val2

s = SubnetTree.SubnetTree()
populateTrees("test_data.json", s)
with open("in.txt") as inF:
    for line in inF:
        print s[line.rstrip()]
