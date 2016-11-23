
from subprocess import call

num = 1000

for i in range(100):
    num = i * 1000
    args = []
    for n in range(num):
        args.append(str(n))

    args.insert(0, './ex13.bin')
    print args
    call(args)
