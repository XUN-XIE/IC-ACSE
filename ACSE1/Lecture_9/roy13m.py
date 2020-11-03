""" Module to calculate prime numbers."""

import sys

def find_nth_prime(n, primes=None):
    primes = primes or [2]
    test = primes[-1]+1
    while len(primes)<n:
    	for p in primes:
            if p**2>test:
                primes.append(test)
                break
            if test%p==0:
                break
        # special case
        if len(primes)==0:
            primes.append(test)
        test += 1
    return primes

if __name__=="__main__":
    print(find_nth_prime(int(sys.argv[0])))