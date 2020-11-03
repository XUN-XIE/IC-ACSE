'''
module for test all games in exercise
'''
import os
import numpy as np
import automata

BASE_PATH = os.path.dirname(__file__)


def test_sandpile():
    '''
    function used to test game of sand pile
    '''
    initial64 = np.load(os.sep.join((BASE_PATH, 'pile_64x64_init.npy')))
    final64 = np.load(os.sep.join((BASE_PATH, 'pile_64x64_final.npy')))

    assert (automata.sandpile(initial64) == final64).all()

    initial128 = np.load(os.sep.join((BASE_PATH, 'pile_128x128_init.npy')))
    final128 = np.load(os.sep.join((BASE_PATH, 'pile_128x128_final.npy')))

    assert (automata.sandpile(initial128) == final128).all()


def test_life():
    '''
    function for test for game of life
    '''
    init_mat = np.zeros((5, 5))
    init_mat[2, 1:4] = 1
    init_mat = init_mat == 1

    final_mat = np.zeros((5, 5))
    final_mat[1:4, 2] = 1
    final_mat = final_mat == 1
    # print(final_mat)
    # print(automata.life(init_mat, 1, True))
    assert (automata.life(init_mat, 1, True) == final_mat).all()

    init_mat2 = np.zeros((5, 5))
    init_mat2[1, 1:4] = 1
    init_mat2[2][3] = 1
    init_mat2[3][2] = 1
    init_mat2 = init_mat2 == 1

    final_mat2 = np.zeros((5, 5))
    final_mat2[0, 2:] = 1
    final_mat2[1][-1] = 1
    final_mat2[2][-2] = 1
    final_mat2 = final_mat2 == 1
    # print(final_mat2)
    # print(automata.life(init_mat2, 4, True))
    assert (automata.life(init_mat2, 4) == final_mat2).all()

    final_mat3 = np.zeros((5, 5))
    final_mat3[0][0] = 1
    final_mat3[1][-1] = 1
    final_mat3[-1][0] = 1
    final_mat3[-1][-2:] = 1
    final_mat3 = final_mat3 == 1
    #print(final_mat3)
    #print(automata.life(init_mat2, 8, True))
    assert (automata.life(init_mat2, 8, True) == final_mat3).all()



def test_lifetri():
    '''
    function for test for game of life in condition of triangular
    '''
    init = np.zeros((10, 10))
    init[4:6, 3:7] = 1
    #print(init)

    final = np.zeros((10, 10))
    final[4:6, 5:9] = 1
    final = final == 1
    #print(final)
    #print(automata.lifetri(init, 3))
    assert (automata.lifetri(init, 3, True) == final).all()
    assert (automata.lifetri(init, 3) == final).all()

    #final1 used for test when periodic is True
    #final2 used for test when periodic is False
    final1 = np.zeros((10, 10))
    final2 = np.zeros((10, 10))
    final2[4:6, -3:] = 1
    final1[4:6, -3:] = 1
    final1[4:6, 0] = 1
    assert (automata.lifetri(init, 6, True) == final1).all()
    assert (automata.lifetri(init, 6) == final2).all()


def test_lifegen():
    '''
    function for test for game of life in generic condition
    '''
    # example from life Figure 3
    lis = [0 for i in range(25)]
    lis[11] = lis[12] = lis[13] = 1
    init = np.array(lis)

    lis2 = [0 for i in range(25)]
    lis2[7] = lis2[12] = lis2[17] = 1
    final = np.array(lis2)

    enviroment = {2, 3}
    fertility = {3}

    matrix = np.zeros((25, 25))
    for i in range(25):
        matrix[i][i] = 1

    for i in range(25):
        for j in range(25):
            diff = i - j
            if diff in {-1, 1, -5, 5, -6, 6, -4, 4}:
                matrix[i][j] = 1
    #print(matrix)
    # print(init)
    #print(final)
    #print(automata.life_generic(matrix, init, 1, enviroment, fertility))
    final = final == 1
    assert (automata.life_generic(matrix, init, 1, enviroment, fertility) == final).all()
