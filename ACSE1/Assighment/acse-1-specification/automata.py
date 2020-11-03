"""Implement several cellular automata, including the Abelian
 sandpile and Conway's game of life."""


# standard imports
import itertools

# numpy and matplotlib
import numpy as np
import matplotlib.pyplot as pyplot


def sandpile(initial_state):
    """
    Generate final state for Abelian Sandpile.
    Parameters
    ----------
    initial_state : array_like or list of lists
         Initial 2d state of grid in an array of integers.
    Returns
    -------
    array_like
         Final state of grid in array of integers.
    """

    state = np.array(initial_state, int)

    # Find the sites which are going to topple.
    unstable = 1*(state >= 4)

    # Loop until nothing more can topple.
    while (unstable).any():

        # Topple everythin at once
        state -= 4*unstable
        state[0:-1, :] += unstable[1:, :]
        state[1:, :] += unstable[0:-1, :]
        state[:, 0:-1] += unstable[:, 1:]
        state[:, 1:] += unstable[:, 0:-1]

        # Update the list of unstable sites for next iteration.
        unstable = 1*(state >= 4)

    return state


def neighbour_count(state, periodic=False):
    """
    Count the number of live neighbours of a regular n-d array.
    Parameters
    ----------

    state : numpy.ndarray of bools
       The Game of Life state.
    periodic : bool
       Whether boundaries are periodic
    Returns
    ------
    ndarray of ints
        Number of living neighbours for each cell.


    The algorithm looks in each direction, and increases the count in a cell
    if that neighbour is alive. This version is generic and will perform
    the count for any system dimension.

    """

    dim = state.shape
    ncount = np.zeros(dim)

    # itertools.product([A,B,C, ...]) with A, B, (e.g.) lists gives
    # an iterator spanning every possible combination of tuples
    # taking the first element from A, the second from B etc.
    #
    # We use it here to automate looking one place back, level, and
    # one place forward in every dimension of the array
    #
    # This also uses two other Python syntax generalizations.
    # For functions f(*[a, b, c]) == f(a, b, c) and
    # 3*[(-1, 1)] = [(-1, 1), (-1, 1), (-1, 1)]
    combos = list(itertools.product(*(state.ndim*[(-1, 0, 1)])))

    # Because it include every combination, it also has the "do nothing"
    # case to count the cell itself. We don't want that, so we remove
    # the all zeros entry from the list.
    combos.remove(state.ndim*(0, ))

    if periodic:
        # In the periodic case, we can use the np.roll function to shift
        # things.
        for combo in combos:
            ncount[...] += 1*np.roll(state, combo, range(state.ndim))
    else:
        # In the non periodic case, we loop over the combinations and
        # deal only with the slices which are actually relevant
        # e.g. in 1D we want to do
        # neighbour_count[0:dim-1] += X[1:dim]
        # neighbour_count[1:dim] += X[0:dim-1]

        _slices = (slice(None, -1), slice(None, None), slice(1, None))

        def lhs_slice(combo):
            """Return the slice of the neighbour_count mesh to increment."""
            return tuple(_slices[c+1] for c in combo)

        def rhs_slice(combo):
            """Return the slice of the X mesh we're testing."""
            return tuple(_slices[1-c] for c in combo)

        for combo in combos:
            ncount[lhs_slice(combo)] += 1*state[rhs_slice(combo)]

    return ncount


def life(initial_state, nt, periodic=False):
    """
    Perform iterations of Conway's Game of Life.

    Parameters
    ----------
    initial_state : array_like or list of lists
         Initial 2d state of grid in an array of booleans.
    nt : int
         Number of steps of Life to perform.
    periodic : bool
         If true, then grid is assumed periodic.

    Returns
    -------
    array_like
         Final state of grid in array of booleans
    """

    state = np.array(initial_state, bool)

    # main loop
    for _ in range(nt):
        ncount = neighbour_count(state,
                                 periodic)
        state = state*(ncount == 2)+(ncount == 3)

    return state


class TriState():
    """
    Helper class for a hexagonal Game of Life.
    """

    def __init__(self, state, periodic):
        """ Initialise the class."""

        self.periodic = periodic

        self.state = np.array(state, bool)

        # We'll used padding round the periodic case, to avoid
        # needing to write a vast amount of extra logic

        if self.periodic:
            self.int_state = np.empty(np.array(state.shape)+4, int)
        else:
            self.int_state = np.empty(np.array(state.shape), int)
        self.ncount = np.zeros(self.int_state.shape, int)

    def do_neighbour_count(self):
        """Count the neighbours of each cell.

        The logic is divided between odd and even rows, and up
        and downward triangles.
        """
        self.ncount.fill(0)

        if self.periodic:
            self.int_state[2:-2, 2:-2] = 1*self.state
            self.int_state[:2, 2:-2] = 1*self.state[-2:, :]
            self.int_state[-2:, 2:-2] = 1*self.state[:2, :]
            self.int_state[2:-2, :2] = 1*self.state[:, -2:]
            self.int_state[2:-2, -2:] = 1*self.state[:, :2]
            self.int_state[:2, :2] = 1*self.state[-2:, -2:]
            self.int_state[-2:, :2] = 1*self.state[:2, -2:]
            self.int_state[:2, -2:] = 1*self.state[-2:, :2]
            self.int_state[-2:, -2:] = 1*self.state[:2, :2]
        else:
            self.int_state[:, :] = 1*self.state

        # Count neighbours in the same row
        self.ncount[:, :-2] += self.int_state[:, 2:]
        self.ncount[:, :-1] += self.int_state[:, 1:]
        self.ncount[:, 1:] += self.int_state[:, :-1]
        self.ncount[:, 2:] += self.int_state[:, :-2]

        # up and down (3 triangles each)

        self.ncount[:-1, :] += self.int_state[1:, :]
        self.ncount[:-1, :-1] += self.int_state[1:, 1:]
        self.ncount[:-1, 1:] += self.int_state[1:, :-1]

        self.ncount[1:, :] += self.int_state[:-1, :]
        self.ncount[1:, :-1] += self.int_state[:-1, 1:]
        self.ncount[1:, 1:] += self.int_state[:-1, :-1]

        # last two additions make for 8 cases, depending on
        # the row and column

        self.ncount[:-1:2, :-2:2] += self.int_state[1::2, 2::2]
        self.ncount[:-1:2, 2::2] += self.int_state[1::2, :-2:2]
        self.ncount[1:-1:2, 1:-2:2] += self.int_state[2:-1:2, 3::2]
        self.ncount[1:-1:2, 3::2] += self.int_state[2:-1:2, 1:-2:2]
        self.ncount[1::2, :-2:2] += self.int_state[:-1:2, 2::2]
        self.ncount[1::2, 2::2] += self.int_state[:-1:2, :-2:2]
        self.ncount[2:-1:2, 1:-2:2] += self.int_state[1:-2:2, 3::2]
        self.ncount[2:-1:2, 3::2] += self.int_state[1:-2:2, 1:-2:2]

    def update(self):
        """Update the data for the timestep."""

        self.do_neighbour_count()
        if self.periodic:
            ncount = self.ncount[2:-2, 2:-2]
        else:
            ncount = self.ncount

        self.state = (self.state*((ncount >= 4) &
                                  (ncount <= 6)) |
                      ~self.state*(ncount == 4))


def lifetri(initial_state, nt, periodic=False):
    """
    Perform iterations of Conway's Game of Life.
    Parameters

    ----------
    initial_state : list of lists of booleans
         Initial 2d state of grid on triangles.
    nt : int
         Number of steps of Life to perform.
    periodic : bool, optional
         If true, then grid is assumed periodic.
    Returns
    -------
    list of lists
         Final state of grid.
    """

    state = TriState(np.array(initial_state, bool),
                     periodic)

    for _ in range(nt):
        state.update()

    return state.state


def life_generic(matrix, initial_state, nt, environment, fertility):
    """
    Perform iterations of Conway's Game of Life.
    Parameters

    ----------
    matrix : 2d array of bools
        a boolean matrix indicating neighbours for each row
    initial_state : list of lists of booleans
        Initial 2d state of grid on triangles.
    nt : int
        Number of steps of Life to perform.
    Returns
    -------
    list of lists
        Final state of grid.
    """

    #  Make sure input is how we want it
    mat = np.array(matrix, int)
    state = np.array(np.array(initial_state, bool), int)
    environment = list(set(environment))
    fertility = list(set(fertility))

    # initialize once to save time

    ncount = np.empty(state.shape, int)

    # do the loop

    for _ in range(nt):

        mat.dot(state, ncount)
        lives = np.isin(ncount, environment)
        start = np.isin(ncount, fertility)

        state[:] = 1*(state & lives) + 1*((~state) & start)

    return np.array(state, bool)

# The other routines handle plotting


def plot(state, **kwargs):
    """Plot a 2D array.

    Just a naive wrapper around pcolormesh.
    """
    pyplot.pcolormesh(state, **kwargs)


def plot_tri(state, **kwargs):
    """Plot a triangle, optionally filled.

    Parameters
    ----------
    x : array_like
       The 2d coordinates of the centre of the hexagon

    This uses pyplot.fill to plot closed hexagons.
    """

    width = 0.5
    height = np.sqrt(3.0)/2.0

    upward = (state[0]+state[1]+1) % 2

    if upward:
        pyplot.fill([width*(state[1]-1), width*(state[1]+1), width*state[1]],
                    [-height*(state[0]+1), -height*(state[0]+1),
                     -height*(state[0])], **kwargs)
    else:
        pyplot.fill([width*(state[1]-1), width*(state[1]+1), width*(state[1])],
                    [-height*(state[0]), -height*(state[0]),
                     -height*(state[0]+1)], **kwargs)


def plot_tris(state, colours=('w', 'r'), **kwargs):
    """Plot a triangular tesselation.

    Parameters
    ----------
    state : ndarray or list of lists
        The state of the Game of Life system.

    This uses pyplot.fill to plot closed hexagons. A more generic version
    could use the triplot function
    """

    for i, line in enumerate(state):
        for j, val in enumerate(line):
            plot_tri((i, j), edgecolor='k', facecolor=colours[1*val], **kwargs)
