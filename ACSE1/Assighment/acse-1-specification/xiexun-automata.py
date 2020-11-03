"""Implementations on the Abelian Sandpile and Conway's
Game of life on various meshes"""


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

    raise NotImplementedError


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

    raise NotImplementedError


def lifetri(initial_state, nsteps):
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

    raise NotImplementedError


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

    raise NotImplementedError
