Project 2: Armageddon - Atmospheric entry and disruption of asteroids
=====================================================================



Synopsis:
---------

Asteroids entering Earth’s atmosphere are subject to extreme drag forces
that decelerate, heat and disrupt the space rocks. The fate of an
asteroid is a complex function of its initial mass, speed, trajectory
angle and internal strength.

`Asteroids <https://en.wikipedia.org/wiki/Asteroid>`__ 10-100 m in
diameter can penetrate deep into Earth’s atmosphere and disrupt
catastrophically, generating an atmospheric disturbance
(`airburst <https://en.wikipedia.org/wiki/Air_burst>`__) that can cause
`damage on the ground <https://www.youtube.com/watch?v=tq02C_3FvFo>`__.
Such an event occurred over the city of
`Chelyabinsk <https://en.wikipedia.org/wiki/Chelyabinsk_meteor>`__ in
Russia, in 2013, releasing energy equivalent to about 520 `kilotons of
TNT <https://en.wikipedia.org/wiki/TNT_equivalent>`__ (1 kt TNT is
equivalent to :math:`4.184 \times 10^{12}` J), and injuring thousands of
people (`Popova et al.,
2013 <http://doi.org/10.1126/science.1242642>`__; `Brown et al.,
2013 <http://doi.org/10.1038/nature12741>`__). An even larger event
occurred over
`Tunguska <https://en.wikipedia.org/wiki/Tunguska_event>`__, an
unpopulated area in Siberia, in 1908.

The purpose of this exercise is to develop a fast numerical simulator to
predict the fate of asteroids entering Earth’s atmosphere for the
purposes of hazard assessment.

Problem definition
------------------

Equations
~~~~~~~~~

The dynamics of an asteroid in Earth’s atmosphere is
governed by a coupled set of ordinary differential equations:

.. math::
   :nowrap:

   \begin{aligned} 
   \frac{dv}{dt} & = \frac{-C_D\rho_a A v^2}{2 m} + g \sin \theta \\
   \frac{dm}{dt} & = \frac{-C_H\rho_a A v^3}{2 Q} \\
   \frac{d\theta}{dt} & = \frac{g\cos\theta}{v} - \frac{C_L\rho_a A v}{2 m} - \frac{v\cos\theta}{R_P + z} \\
   \frac{dz}{dt} & = -v\sin\theta \\
   \frac{dx}{dt} & = \frac{v\cos\theta}{1 + z/R_P}
   \end{aligned}

In these equations, :math:`v`, :math:`m`, and :math:`A` are the asteroid
speed (along trajectory), mass and cross-sectional area, respectively.
:math:`\theta` is the meteoroid trajectory angle to the horizontal (in radians), :math:`x` is the
downrange distance of the meteoroid from its entry position, :math:`z`
is the altitude and :math:`t` is time; :math:`C_D` is the drag
coefficient, :math:`\rho_a` is the atmospheric density (a function of
altitude ), :math:`C_H` is an ablation efficiency coefficient, :math:`Q`
is the specific heat of ablation; :math:`C_L` is a lift coefficient; and
:math:`R_P` is the planetary radius. All terms use MKS units.

It is common to assume (for simplicity) that, prior to break-up, the
radius (and cross-sectional area) of the asteroid remains constant;
that is, any mass-loss by **ablation** does not change the
cross-sectional area of the asteroid. We will further assume a spherical asteroid. 

A commonly used criterion for the break-up of an asteroid in the
atmosphere is when the ram pressure of the air interacting with the
asteroid :math:`\rho_a v^2` first exceeds the strength of the asteroid
:math:`Y`.

.. math:: \rho_a v^2 = Y

Should break-up occur, the asteroid deforms and spreads laterally as
it continues its passage through the atmosphere. As a result its
radius and cross-sectional area increase, but the asteroid density is
assumed to remain constant (often referred to as 'pancaking').  It is
conventional to define the cross-sectional area of the expanding cloud
of fragments as :math:`A = \pi r^2` (i.e., assuming a circular
cross-section), for use in the above equations.

Several models for the spreading rate :math:`\frac{dr}{dt}` have been proposed. In the
simplest model, the fragmented asteroid’s spreading rate is related to
its along trajectory speed `(Hills and Goda, 1993) <http://doi.org/10.1086/116499>`__:

.. math::  \frac{dr}{dt} = \left[\frac{7}{2}\alpha\frac{\rho_a}{\rho_m}\right]^{1/2} v

Where :math:`r` is the asteroid radius, :math:`\rho_m` is the asteroid
density and :math:`\alpha` is a spreading coefficient, often taken to be
0.3.

Note that fragmentation and spreading **ceases** (:math:`\frac{dr}{dt}
= 0`) when the ram pressure drops back below the strength of the
asteroid :math:`\rho_a v^2 < Y`.

The figure below shows a typical solution to the above set of equations
for an impact airburst scenario. The kinetic energy loss per unit
height, which is a good approximation for the energy transferred to the
atmosphere as heat (and is often referred to as the energy deposited per
km altitude), is shown as a function of altitude.

.. image:: ../images/airburst.png

In this scenario the asteroid experiences breakup at approximately 48-km altitude (denoted by the star), before spreading and decelerating rapidly until an altitude of approximately 15 km at which point the **energy loss per unit height** is **maximum**. This point is often considered to be the **burst altitude**. The total kinetic energy lost by the asteroid at this point is a good estimate of the airburst energy (i.e., the total energy deposited into the atmosphere) for hazard analysis.

Impact scenarios with a burst altitude well (>5 km) above the surface can be considered to be **airburst events** and are unlikely to form a sizable crater on the ground. In such cases, even if a substantial fraction of the original asteroid survives to the ground it will be decelerated to a very small fraction of its initial speed.

Impact scenarios with a burst altitude below the surface (i.e., peak energy deposition per unit height is not reached before the asteroid strikes the ground) will form a sizable impact crater on the ground and can be considered as a **cratering event** (although a sizable proportion of the original kinetic energy of the asteroid may be transferred to the air).

Between these scenarios is a complex regime where a **low altitude airburst combined with a substantial crater-forming event** is likely. This regime is not well understood.

Function API
============

.. automodule:: armageddon
  :members:
  :imported-members:
