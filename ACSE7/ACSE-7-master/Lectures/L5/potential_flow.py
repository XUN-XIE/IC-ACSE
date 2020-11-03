"""This module contains three classes to solve potential flow problems around an airfoil:

* PotentialFlowFE - a FEM solver where u=grad(phi), it does not permit circulation/lift
* KuttaPotentialFlowFE - a FEM solver that adds circulation and imposes a Kutta condition on the trailing edge
* PotentialFlowPanelMethod - a potential flow solver including circulation and the Kutta condition that uses the panel method."""
import numpy as np
import matplotlib.pyplot as plt
import scipy.linalg as sl
import scipy.sparse as sp
import scipy.sparse.linalg as spl
import triangle
from matplotlib.tri import Triangulation, LinearTriInterpolator
import matplotlib.path

def Laplace_2D_FE(vertices, triangles, vertex_markers, boundary_velocities, sparse=True):
    """Returns a Finite Element discretisation of Laplace's equation

    vertices: a n x 2 array of the positions of the n vertices
    triangles: a m x 3 integer array which for each of the m triangles (elements)
               stores the 3 vertex numbers (from 0 to n-1) that make up this triangle
    boundary_markers: a length n integer array, which for each vertex stores a 0 (for interior
               vertices) or a positive number (for boundary vertices) to mark different parts of the boundary
    boundary_velocities: a dictionary that maps each nonzero value in boundary_markers to a 2d velocity
                       vector, the *normal* component of which is applied as a boundary condition
                       on the corresponding part of the boundary
    sparse: whether to assemble and return a sparse matrix (csr_matrix)"""
    n = len(vertices)
    if sparse:
        A = sp.lil_matrix((n, n))
    else:
        A = np.zeros((n,n))
    tangents = np.zeros((n,2))
    d = [0,0,0]
    for tri in triangles:
        xy = vertices[tri]
        # squared edge lengths:
        d[0] = sum((xy[1]-xy[2])**2)
        d[1] = sum((xy[0]-xy[2])**2)
        d[2] = sum((xy[0]-xy[1])**2)
        dsum = sum(d)
        # triangle area times 8:
        A8 = ((xy[1,0]-xy[0,0])*(xy[2,1]-xy[0,1]) - (xy[2,0]-xy[0,0])*(xy[1,1]-xy[0,1]))*4
        for i, di in zip(tri, d):
            A[i,i] += 2*di/A8
            for j, dj in zip(tri, d):
                if i==j:
                    continue
                A[i,j] += (dsum - 2*(di + dj))/A8
        tangents[tri[0]] += xy[2]-xy[1]
        tangents[tri[1]] += xy[0]-xy[2]
        tangents[tri[2]] += xy[1]-xy[0]

    b = np.zeros(n)
    for i, (xy, vm, t) in enumerate(zip(vertices, vertex_markers.flatten(), tangents)):
        if vm>0:
            n = [-t[1], t[0]]
            un = np.dot(boundary_velocities[vm], n)
            b[i] += un/2.

    i = 0  # choose an arbitrary vertex in which we set phi_i = 0
    A[i,:] = 0.  # zero the row
    A[:,i] = 0.  # also zero the colum
    A[i,i] = 1.  # but put a 1 on the diagonal
    b[i] = 0.  # and zero the rhs entry

    if sparse:
        A = A.tocsr()
    return A, b

def polys_to_geometry(polys, boundary_markers):
    """Set up geometry dictionary that can be used in triangle mesh generator

        polys: a list of arrays, where each array of shape N_i x 2 contains the
               N_i vertices that describe the i-th polygon. For each polygon the
               first and the last vertex should be the same! (closed polygon).
               All polygons should be counter clock wise, and the first polygon
               is assumed to be the outer boundary.
        boundary_markers: list, for each polygon a marker (nonzero integer)
               to identify it as parts of the boundary
        Returns a dictionary `geom` that contains the following entries:
        geom['vertices'] a list of the initial vertices on the boundaries
        geom['segments'] the edges making up the boundaries: 2-tuples of vertex numbers
        geom['holes']    any holes we want in the interior, specified by a location within each hole
        geom['vertex_markers'] a list of integers, one for each vertex to mark different parts of the boundary
        geom['segment_markers'] a list of integees, one for each edge to mark different parts of the boundary"""

    geom = {'vertices': [], 'segments': []}
    n = 0  # first vertex
    for poly in polys:
        # skip last vertex, as it should be the same as the first:
        geom['vertices'].extend(poly[:-1])
        # use consecutive numbering of vertices to describe the segments
        # the last segment connects back to the first vertex 'n'
        geom['segments'].extend([(n+i, n+i+1) for i in range(len(poly)-2)] + [[n+len(poly)-2, n]])
        n += len(poly)-1

    # add vertex markers:
    geom['vertex_markers'] = np.zeros(n)
    n = 0  # first vertex (start counting again)
    for bm, poly in zip(boundary_markers, polys):
        geom['vertex_markers'][n:n+len(poly)-1] = bm
        n += len(poly)-1

    # each polygon has the same n/o vertices as segments, so the segment_markers ends up being the same:
    geom['segment_markers'] = geom['vertex_markers']

    if len(polys)>1:
        # add holes (skipping first polygon which is assumed outer boundary):
        # we choose these points here half-way the first edge, but slightly inside each of the polygons
        geom['holes'] = []
        for poly in polys[1:]:
            # (dx, dy) is vector from first to second vertex (first edge)
            dx = poly[1][0] - poly[0][0]
            dy = poly[1][1] - poly[0][1]
            # half way first edge plus a 1/100 of the edge length in the inwards normal direction:
            geom['holes'].append((poly[0][0] + dx/2 - dy/100, poly[0][1] + dy/2 + dx/100))

    return geom


def add_point_vortex_and_kutta_condition(A_lapl, b_lapl, tri, boundary_marker, x_c):
    """Add point vortex and Kutta condition to the Laplace FEM system

    A_lapl, b_lapl  - matrix and rhs vector as returned by Laplace_2D_FE
    tri             - mesh generated by Triangle
    boundary_marker - boundary_marker of the wing
    x_c             - x coordinate of vortex centre (x_c, 0) should be inside the wing
    """
    N = A_lapl.shape[0]  # n/o vertices
    A = sp.lil_matrix((N+1, N+1))
    b = np.zeros(N+1)
    # copy Laplace matrix into first N rows and N colums
    i, j = A_lapl.nonzero()
    A[i, j] = A_lapl.data
    b[:N] = b_lapl

    # add contributions of point-vortex to boundary conditions
    vm = tri['vertex_markers'].flatten()
    for triangle in tri['triangles']:
        for ij in [0,1], [1,2], [2,0]:
            if all(vm[triangle[ij]]>0):
                edge = triangle[ij]
                xy0 = tri['vertices'][edge[0]]
                xy1 = tri['vertices'][edge[1]]
                lac = sl.norm(xy0-[x_c,0])
                lbc = sl.norm(xy1-[x_c,0])
                A[edge[0], N] += np.log(lac/lbc)/4./np.pi
                A[edge[1], N] += np.log(lac/lbc)/4./np.pi

    # search for the 2 edges with boundary_marker attached to the vertex with maximum x coordinate:
    x_max = -1e6
    v_max = -1
    max_edges = []
    for triangle in tri['triangles']:
        for ij in [0,1], [1,2], [2,0]:
            if all(vm[triangle[ij]]==boundary_marker):
                edge = triangle[ij]
                xy0 = tri['vertices'][edge[0]]
                xy1 = tri['vertices'][edge[1]]
                if edge[0]==v_max:
                    max_edges.append(edge)
                elif xy0[0] > x_max:
                    max_edges = [edge]
                    x_max = xy0[0]
                    v_max = edge[0]
                if edge[1]==v_max:
                    max_edges.append(edge)
                elif xy1[0] > x_max:
                    max_edges = [edge]
                    x_max = xy1[0]
                    v_max = edge[1]

    # add Kutta condition in last row
    vmx = tri['vertices'][v_max] - [x_c, 0]
    vmx = vmx/(vmx**2).sum()
    lsum = 0.
    for edge in max_edges:
        exy = tri['vertices'][edge]
        t = exy[1]-exy[0]
        l = sl.norm(t)
        A[N, edge[0]] += 1./l
        A[N, edge[1]] += -1./l
        A[N, N] += np.dot([t[1], -t[0]], vmx)/l/2/np.pi
        lsum += l
    A[N,:] *= lsum/2.
    b[N] = 0

    i = 0  # choose an arbitrary vertex in which we set phi_i = 0
    A[i,:] = 0.  # zero the row
    A[:,i] = 0.  # also zero the colum
    A[i,i] = 1.  # but put a 1 on the diagonal
    b[i] = 0.  # and zero the rhs entry

    return A.tocsr(), b


class PotentialFlowFEBase:
    """Base class for Finite Element Potintial Flow problems."""
    def __init__(self, airfoil_boundary, outer_boundary, resolution, u_inf, alpha):
        self.geom = polys_to_geometry([outer_boundary, airfoil_boundary], [1, 2])
        self.tri = triangle.triangulate(self.geom, 'pDqa{:f}'.format(resolution))
        x, y = self.tri['vertices'].T
        self.triang = Triangulation(x, y, self.tri['triangles'])
        self.u_inf = u_inf
        self.alpha = alpha
        self.airfoil = np.array(airfoil_boundary)
        self.chord_length = self.airfoil.max() - self.airfoil.min()

    def plot_mesh(self, ax=None):
        """Plot the mesh."""
        x, y = self.tri['vertices'].T
        ax = ax or plt.gca()
        ax.triplot(x, y, self.tri['triangles'])


class PotentialFlowFE(PotentialFlowFEBase):
    """Potential Flow problem discretized with Finite Element Method."""

    def assemble(self, sparse=True):
        """Assemble the FEM linear system.

        returns matrix and vector A, b."""
        return Laplace_2D_FE(self.tri['vertices'], self.tri['triangles'], self.tri['vertex_markers'],
                                    boundary_velocities={1: [self.u_inf*np.cos(self.alpha), self.u_inf*np.sin(self.alpha)],
                                                         2: [0,0]}, sparse=sparse)

    def plot_solution(self, phi, ax=None, **kwargs):
        """Given the solution vector phi of values at the vertices, plot the solution"""
        intp = LinearTriInterpolator(self.triang, phi)
        x, y = self.tri['vertices'].T
        u, v = intp.gradient(x, y)

        ax = ax or plt.gca()
        ax.tripcolor(self.triang, phi)
        ax.quiver(x, y, u, v, **kwargs)


class KuttaPotentialFlowFE(PotentialFlowFEBase):
    """Potential Flow problem discretized with FEM including point-vortex and Kutta condition."""
    def __init__(self, airfoil_boundary, outer_boundary, resolution, u_inf, alpha):
        super().__init__(airfoil_boundary, outer_boundary, resolution, u_inf, alpha)
        xy = np.array(airfoil_boundary)
        # choose centre of point-vertex at [x_c, 0]
        # should not be too close to the tip (due to inaccurate integration)
        self.x_c = 0.8 * xy[:,0].min() + 0.2 * xy[:,1].max()

    def assemble(self):
        """Assemble the FEM linear system.

        returns matrix and vector A, b of size (N+1)x(N+1) and (N+1)
        where N is the number of vertices. The last row of the matrix
        is associated with the Kutta condition. The last column of the
        matrix (and thus last entry of the solution vector) is associated
        with the magnitude of the point-vortex solution."""
        A,b = Laplace_2D_FE(self.tri['vertices'], self.tri['triangles'], self.tri['vertex_markers'],
                                    boundary_velocities={1: [self.u_inf*np.cos(self.alpha), self.u_inf*np.sin(self.alpha)],
                                                         2: [0,0]}, sparse=True)
        return add_point_vortex_and_kutta_condition(A, b, self.tri, 2, self.x_c)

    def velocity_from_phi(self, phi, x=None, y=None):
        """Interpolate velocity solution in vertex positions

        Velocity solution is: u = \grad(\phi) + \Gamma u_pf
        where \phi is the solution of phi at the vertices, and u_pf
        the standard point vortex, with magnitude Gamma stored in the
        (N+1)-th entry of the  solution vector."""
        assert (x is None) == (y is None), "Provide either both or neither x and y arguments"
        if x is None:
            x, y = self.tri['vertices'].T

        N = len(phi)-1
        # first normal interpolation phi gradient
        intp = LinearTriInterpolator(self.triang, phi[:N])
        u, v = intp.gradient(x, y)

        # then add point vortex solution
        Gamma = phi[-1]
        r2 = (x-self.x_c)**2 + y**2
        u += -y/r2*Gamma/2/np.pi
        v += (x-self.x_c)/r2*Gamma/2/np.pi
        return u, v

    def plot_solution(self, phi, ax=None, **kwargs):
        """Given the solution vector phi, plot the solution"""
        ax = ax or plt.gca()
        ax.tripcolor(self.triang, phi[:-1])
        u, v = self.velocity_from_phi(phi)
        x, y = self.tri['vertices'].T
        ax.quiver(x, y, u, v, **kwargs);

    def lift_coeffcient(self, phi):
        """Compute C_L, the dimensionless lift coefficient from solution vector phi."""
        return -phi[-1]/(0.5*self.u_inf*self.chord_length)


#
# some auxilary functions used in the panel method:
#

def tangent(xa, xb):
    return (xb-xa)/sl.norm(xb-xa)

def skew(u):
    return np.array([-u[1], u[0]])

def normal(xa, xb):
    return -skew(tangent(xa, xb))

def velocity_panel(xa, xb, xc):
    """Given a source panel between xa and xb, compute the velocity vector in a third location xc."""
    xac = xc - xa
    xbc = xc - xb
    lac = sl.norm(xac)
    lbc = sl.norm(xbc)
    n = normal(xa, xb)
    t = tangent(xa, xb)
    ut = -np.log(lbc/lac)
    c = min(max(np.dot(xac, xbc)/lac/lbc, -1), 1)
    un = np.arccos(c) * np.sign(np.dot(n, xac))
    u = (ut*t + un*n) / (2*np.pi)
    return u


def velocity_matrices(x):
    """Compute the matrices An and At map the solution vector sigma to the normal and tangential components at the boundary panels."""
    n = x.shape[0]-1
    An = np.empty((n, n))
    At = np.empty((n, n))
    for i in range(n):
        ni = normal(x[i], x[i+1])
        ti = tangent(x[i], x[i+1])
        xc = (x[i] + x[i+1])/2.

        for j in range(n):
            if j==i:
                An[i,i] = 0.5
                At[i,i] = 0.0
                continue

            u = velocity_panel(x[j], x[j+1], xc)
            An[i,j] = np.dot(u, ni)
            At[i,j] = np.dot(u, ti)
    return An, At


def freestream_vectors(x, u_inf):
    """Compute the contributions to the normal and tangential components at the boundary points from the background upstream velocity u_inf."""
    n = x.shape[0]-1
    bn = np.empty(n)
    bt = np.empty(n)
    for i in range(n):
        ni = normal(x[i], x[i+1])
        ti = tangent(x[i], x[i+1])
        bn[i] = np.dot(u_inf, ni)
        bt[i] = np.dot(u_inf, ti)
    return bn, bt


def velocity_from_sigma(xi, x, sigma, u_inf):
    """Given the solution vector sigma, compute the velocity in position xi."""
    n = x.shape[0]-1
    gamma = sigma[n]
    u = np.array(u_inf)

    for j in range(n):
        up = velocity_panel(x[j], x[j+1], xi)
        u += sigma[j] * up + gamma * skew(up)
    return u

def sigma_system(x, u_inf):
    """Assemble the entire system A sigma = b using the panel method.

    For n panels, we obtain a (n+1) X (n+1) matrix A, where the first n rows
    enforce no normal flow on the n panels. The last row is associated with the
    Kutta condition on the trailing edge. The n+1 solution vector contains the
    source strengths of the n panels, followed by the circulation Gamma as the
    last entry. 

    x - the n+1 vertex positions of the polygon describing the airfoil (first and last should be the same)
    u_inf - the velocity vector representing the upstream velocity."""
    n = x.shape[0]-1

    An, At = velocity_matrices(x)
    A = np.empty((n+1, n+1))
    # first n rows: normal components at panels 0..n-1 should be zero
    A[:n,:n] = An  # contribution from source sigma's
    A[:n, n] = -At.sum(axis=1)  # contribution from vortex gamma
    # last row: Kutta condition, sum of tangential componets at panel 0 and n-1 should be zero
    A[n,:n] = At[0,:n] + At[n-1,:n]  # contribution from source sigma's
    A[n,n] = np.sum(An[0,:] + An[n-1,:])  # contribution from vortex gamma

    # contributions from free-stream solution
    bn, bt = freestream_vectors(x, u_inf)
    b = np.empty(n+1)
    b[:n] = -bn
    b[n] = -(bt[0] + bt[n-1])

    return A, b

def length(x):
    """The length (perimeter) of the polygon x."""
    n = x.shape[0]-1
    l = 0.
    for i in range(n):
        l += sl.norm(x[i+1]-x[i])
    return l


class PotentialFlowPanelMethod:
    """Class to solve the potential flow around an airfoil using the panel method."""
    def __init__(self, airfoil_boundary, u_inf, alpha):
        """
        airfoil_boundary - a list of n+1 positions (first the same as last) representing a polygon
        u_inf - upstream velocity magnitude
        alpha - angle of attack of upstream velocity with the airfoil."""
        self.u_inf = u_inf
        self.alpha = alpha
        self.u_inf_vec = [u_inf * np.cos(alpha), u_inf * np.sin(alpha)]
        self.airfoil = np.array(airfoil_boundary)
        self.length = length(self.airfoil)
        self.chord_length = self.airfoil.max() - self.airfoil.min()

    def assemble(self):
        """Return the (n+1)x(n+1) linear system A, b """
        return sigma_system(self.airfoil, self.u_inf_vec)

    def lift_coeffcient(self, sigma):
        """The dimensionless lif coefficient C_L"""
        return -sigma[-1]*self.length/(0.5*self.chord_length*self.u_inf)

    def velocity_from_sigma(self, sigma, xy):
        """In the points xy, compute the velocity using the solution vector sigma"""
        uv = []
        xyf = np.array(xy).reshape((-1, 2))
        for xi in xyf:
            uv.append(velocity_from_sigma(xi, self.airfoil, sigma, self.u_inf_vec))
        return np.array(uv).reshape(xy.shape)

    def plot_solution(self, sigma, x, y, ax=None):
        """Using the x, y meshgrid positions, plot streamlines based on the solution vector sigma"""
        ax = ax or plt.gca()
        xy = np.vstack((x.flatten(), y.flatten())).T
        u, v = self.velocity_from_sigma(sigma, xy).T
        pth = matplotlib.path.Path(self.airfoil)
        mask = pth.contains_points(xy)
        u[mask] = 0.
        v[mask] = 0.
        u = u.reshape(x.shape)
        v = v.reshape(x.shape)
        ax.streamplot(x, y, u, v)
        plt.plot(self.airfoil[:,0], self.airfoil[:,1])
