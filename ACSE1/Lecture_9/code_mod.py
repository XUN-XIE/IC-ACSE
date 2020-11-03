"""Wrapper for rot13 encoding.""" 

import codecs

def rot13(input):
    """Return the rot13 encoding of an input."""
    return codecs.encode(str(input), 'rot13')