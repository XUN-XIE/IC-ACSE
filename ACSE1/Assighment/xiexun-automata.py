"""Implementations on the Abelian Sandpile and Conway's
Game of life on various meshes"""
import numpy as np

def sandpile(initial_state):
    """
    Generate final state for Abelian Sandpile.
    Parameters
    ----------
    initial_state : array_like or list of lists
        Initial 2d state of grid in an array of integers.
    Returns
    -------
    numpy.ndarray
         Final state of grid in array of integers.
    """

    matrix = initial_state
    ilen = matrix.shape[0]
    jlen = matrix.shape[1]
    matrix_tem = np.zeros((ilen+2, jlen+2))

    for i in range(ilen):
        for j in range(jlen):
            matrix_tem[i+1][j+1] = matrix[i][j]

    matrix = matrix_tem

    while matrix[1:-1, 1:-1].max() >= 4:
        boolhigh = matrix >= 4
        matrix[boolhigh] -= 4

        matrix[:-1, :][boolhigh[1:, :]] += 1
        matrix[1:, :][boolhigh[:-1, :]] += 1
        matrix[:, :-1][boolhigh[:, 1:]] += 1
        matrix[:, 1:][boolhigh[:, :-1]] += 1

        matrix[0, :] = 0
        matrix[:, -1] = 0
        matrix[-1, :] = 0
        matrix[:, 0] = 0

    return matrix[1:-1, 1:-1]




def life(initial_state, nsteps, periodic=False):
    """
    Perform iterations of Conway’s Game of Life.
    Parameters
    ----------
    initial_state : array_like or list of lists
        Initial 2d state of grid in an array of booleans.
    nsteps : int
        Number of steps of Life to perform.
    periodic : bool
        If true, then grid is assumed periodic.
    Returns
    -------
    numpy.ndarray
         Final state of grid in array of booleans
    """
    matrix = initial_state.astype(int)
    ilen = matrix.shape[0]
    jlen = matrix.shape[1]

    if periodic is False:
        ilen += 2
        jlen += 2
        initial_state = np.zeros((ilen, jlen))
        for i in range(ilen-2):
            for j in range(jlen-2):
                initial_state[i+1][j+1] = matrix[i][j]

        matrix = initial_state
        nex_mat = matrix.copy()
        step = 0
        while step < nsteps:
            step += 1
            #matrix record the fixed matrix
            #cur_mat record next matrix
            matrix = nex_mat.copy()

            for i in range(1, ilen - 1):
                for j in range(1, jlen - 1):

                    tem_mat = matrix[i-1:i+2, j-1:j+2]
                    summation = tem_mat.sum()

                    if matrix[i][j] == 1 and summation in {1, 2, 5}:
                        nex_mat[i][j] = 0
                    if matrix[i][j] == 0 and summation in {3}:
                        nex_mat[i][j] = 1
        #output = nex_mat == 1
        return (nex_mat == 1)[1:ilen-1, 1:jlen-1]


    if periodic is True:

        matrix_tem = np.zeros((3*ilen, 3*jlen))
        for i in range(ilen):
            for j in range(jlen):
                for row in range(3):
                    for col in range(3):
                        matrix_tem[i+row*ilen][j+col*jlen] = matrix[i][j]
        print(matrix_tem)
        matrix = matrix_tem
        nex_mat = matrix.copy()
        step = 0
        while step < nsteps:
            step += 1
            #matrix = nex_mat.copy()

            for i in range(ilen):
                for j in range(jlen):
                    for row in range(3):
                        for col in range(3):
                            matrix[i + row * ilen][j + col * jlen] = nex_mat[ilen+i][jlen+j]

            for i in range(ilen, 2*ilen):
                for j in range(jlen, 2*jlen):
                    tem_mat = matrix[i - 1:i + 2, j - 1:j + 2]
                    summation = tem_mat.sum()

                    if matrix[i][j] == 1 and summation in {1, 2, 5}:
                        nex_mat[i][j] = 0
                    if matrix[i][j] == 0 and summation in {3}:
                        nex_mat[i][j] = 1
        #output = (nex_mat[ilen:2*ilen, jlen:2*jlen] == 1)

        return (nex_mat[ilen:2*ilen, jlen:2*jlen] == 1)
    return None


def lifetri(initial_state, nsteps, periodic=False):
    """
    Perform iterations of Conway’s Game of Life on triangles
    Parameters
    ----------
    initial_state : array_like or list of lists
        Initial state of grid on triangles.
    nsteps : int
        Number of steps of Life to perform.
    Returns
    -------
    numpy.ndarray
         Final state of grid in array of booleans
    """
    matrix = initial_state.astype(int)
    ilen = matrix.shape[0]
    jlen = matrix.shape[1]

    if periodic is False:
        new = np.zeros((ilen + 4, jlen + 4))

        for i in range(ilen):
            for j in range(jlen):
                new[i + 2][j + 2] = matrix[i][j]
        matrix = new
        nex_mat = matrix.copy()

        step = 0
        while step < nsteps:
            step += 1
            # matrix record the fixed matrix
            # cur_mat record next matrix
            #nex_mat[:2, :] = 0
            #nex_mat[:, :2] = 0
            #nex_mat[-2:, :] = 0
            #nex_mat[:, -2:] = 0
            matrix = nex_mat.copy()

            for i in range(2, ilen + 2):
                for j in range(2, jlen + 2):
                    tot = i + j
                    summation = 0
                    # cnetral condition
                    # up triangular
                    if tot % 2 == 0:
                        tem_up3 = matrix[i - 1, j - 1:j + 2]
                        tem_down10 = matrix[i:i + 2, j - 2: j + 3]
                        summation += tem_up3.sum() + tem_down10.sum()
                    # down triangular
                    else:
                        tem_up10 = matrix[i - 1:i + 1, j - 2:j + 3]
                        tem_down3 = matrix[i + 1, j - 1:j + 2]
                        summation += tem_up10.sum() + tem_down3.sum()

                    if matrix[i][j] == 1:
                        summation -= 1

                        if summation < 4 or summation > 6:
                            nex_mat[i][j] = 0
                    else:
                        if summation in {4}:
                            nex_mat[i][j] = 1
        return (nex_mat[2:-2, 2:-2] == 1)

    if periodic is True:
        matrix_tem = np.zeros((3 * ilen, 3 * jlen))
        for i in range(ilen):
            for j in range(jlen):
                for row in range(3):
                    for col in range(3):
                        matrix_tem[i + row * ilen][j + col * jlen] = matrix[i][j]

        matrix = matrix_tem
        nex_mat = matrix.copy()
        step = 0
        while step < nsteps:
            step += 1
            #matrix = nex_mat.copy()

            for i in range(ilen):
                for j in range(jlen):
                    for row in range(3):
                        for col in range(3):
                            matrix[i + row * ilen][j + col * jlen] = nex_mat[ilen+i][jlen+j]

            for i in range(ilen, 2 * ilen):
                for j in range(jlen, 2 * jlen):
                    tot = i + j
                    summation = 0
                    # cnetral condition
                    # up triangular
                    if tot % 2 == 0:
                        tem_up3 = matrix[i - 1, j - 1:j + 2]
                        tem_down10 = matrix[i:i + 2, j - 2: j + 3]
                        summation += tem_up3.sum() + tem_down10.sum()
                    # down triangular
                    else:
                        tem_up10 = matrix[i - 1:i + 1, j - 2:j + 3]
                        tem_down3 = matrix[i + 1, j - 1:j + 2]
                        summation += tem_up10.sum() + tem_down3.sum()

                    if matrix[i][j] == 1:
                        summation -= 1

                        if summation < 4 or summation > 6:
                            nex_mat[i][j] = 0
                    else:
                        if summation in {4}:
                            nex_mat[i][j] = 1

        #output = nex_mat == 1
        return (nex_mat == 1)[ilen:2*ilen, jlen:2*jlen]
    return None

def life_generic(matrix, initial_state, nsteps, environment, fertility):
    """
    Perform iterations of Conway’s Game of Life for an arbitrary
    collection of cells.
    Parameters
    ----------
    matrix : 2d array of bools
        a boolean matrix indicating neighbours for each row
    initial_state : 1d array of bools
        Initial state vectr.
    nsteps : int
        Number of steps of Life to perform.
    environment : set of ints
        neighbour counts for which live cells survive.
    fertility : set of ints
        neighbour counts for which dead cells turn on.
    Returns
    -------
    array
         Final state.
    """
    matrix = matrix.astype(int)
    initial_state = initial_state.astype(int)
    length = matrix.shape[0]
    vector = initial_state
    nex_vec = vector.copy()

    step = 0
    while step < nsteps:
        step += 1
        vector = nex_vec.copy()

        for i in range(length):
            summation = 0

            if vector[i] == 1:
                for j in range(length):
                    if matrix[i][j] == 1 and vector[j] == 1:
                        summation += 1
                summation -= 1

                if summation not in environment:
                    nex_vec[i] = 0

            else:
                for j in range(length):
                    if matrix[i][j] == 1 and vector[j] == 1:
                        summation += 1

                if summation in fertility:
                    nex_vec[i] = 1

    output = nex_vec == 1
    return output
