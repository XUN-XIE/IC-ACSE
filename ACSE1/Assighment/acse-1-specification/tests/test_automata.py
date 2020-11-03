import os

import numpy as np

import automata

BASE_PATH = os.path.dirname(__file__)

def test_sandpile():

    initial64 = np.load(os.sep.join((BASE_PATH, 'pile_64x64_init.npy')))
    final64 = np.load(os.sep.join((BASE_PATH, 'pile_64x64_final.npy')))

    assert (automata.sandpile(initial64) == final64).all()

    initial128 = np.load(os.sep.join((BASE_PATH, 'pile_128x128_init.npy')))
    final128 = np.load(os.sep.join((BASE_PATH, 'pile_128x128_final.npy')))

    assert (automata.sandpile(initial128) == final128).all()
