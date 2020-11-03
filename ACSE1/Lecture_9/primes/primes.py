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
    
def prime_factors(x, primes=None):
    primes = primes or [2]
    factors = []
    primes = primes_up_to(x, primes)
    while x>1:
        for p in primes:
            while x%p==0:
                factors.append(p)
	        x/=p

    return factors

def primes_up_to(x, primes=None):
    primes = primes or [2]
    test = primes[-1]+1
    while primes[-1]<x:
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
    return primes[:-2]