.. GeFiCa documentation master file, created by
   sphinx-quickstart on Tue Jan  1 22:23:05 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

Introduction
============

GeFiCa stands for Germanium detector Field Calculator.  It is used to calculate electrostatic potentials and fields inside high-purity germanium detectors with various geometries. It provides generic numerical calculations based on the successive over-relaxation method as well as analytic ones whenever simplification is possible due to highly symmetric detector geometries. GeFiCa is written in C++, and provided as an extension to the ROOT libraries widely used in the particle physics community. Calculation codes for individual detectors are provided as ROOT macros and python scripts distributed along with the GeFiCa core library, serving as both examples showing the usage of GeFiCa and starting points for customized calculations. They can be run without compilation in a ROOT interactive session or directly from a shell. The numerical results are saved in a ROOT tree, making full use of the I/O optimization and plotting functionalities in ROOT. The speed and precision of the calculation are comparable to other commonly used packages, which qualifies GeFiCa as a scientific research tool. However, the main focus of GeFiCa is to clearly explain and demonstrate the analytic and numeric methods to solve Poisson's equation, practical coding considerations as well as visualization methods, with intensive documentation and example macros. It serves as a one-stop resource for people who want to understand the operating mechanism of such a package under the hood.

Motivation
==========

The calculation of electrostatic potentials and fields in a high-purity germanium (HPGe) detector is the initial step in a full pulse-shape simulation process. It is also used for the optimization of novel detector geometries to avoid unreasonably high depletion voltages, undeleted regions, or to calculate the detector capacitance to optimize electronic noise performance, etc. It is widely used in HPGe detector based neutrinoless double beta decay experiments, such as GERDA and MJD, dark matter experiments, such as CoGeNT, Texono and CDEX, and gamma-ray tracking detectors for studying structure atomic nuclei, such as AGATA and GRETA, etc.

Commonly used field calculation packages include fieldgen (part of siggen) used in GRETITA and MJD, SIMION used in AGATA and GERDA, Maxwell used in most experiments, MaGe used in GERDA and MJD, as well as FEniCS, a popular open-source computing platform for solving partial differential equations. A new package called SolidStateDetectors is under rapid development at the Max-Planck-Institut fuer Physik, Munich, for LEGEND, a new neutrinoless double beta decay experiment as a combined effort of GERDA and MJD.

As suggested by its name, SIMION is a commercial software package primarily used to simulate transportations of charged particles in static or low-frequency RF fields. According to its documentation, it uses finite-difference with much optimized linear-time solving method to calculate 2D and 3D fields with up to almost 20 billion grid points, given enough RAM. Its power in static field calculation is a bit overkilling for HPGe detectors, while it lacks some important features that are required for HPGe detector applications, such as the calculation of depletion voltage, region and detector capacitance, etc. This is understandable given that SIMION's main application is not HPGe detector field calculation.

ANSYS Maxwell is a more popular electromagnetic field simulation software compared to SIMION. It is for the design and analysis of electric motors, actuators, sensors, transformers and other electromagnetic and electromechanical devices. It uses automatic adaptive meshing techniques to achieve user-specified accuracy without detailed instruction from a user. As its main application is not HPGe detector field calculation, it has the same advantages and disadvantages as SIMION, but is more expensive than SIMION.

One has to pay for extra features that are not needed in HPGe field calculation, while still misses out some basic features that are needed in HPGe field calculation, if he or she chooses these commercial software packages.

As sophisticated as SIMION and Maxwell, FEniCS is however freely available, developed as an open-source project by a global community of scientists and software developers.  Using efficient finite element codes, its main purpose is to solve partial differential equations, including Poisson's equation, needed in HPGe field calculations. As versatile as it is, FEniCS demands effort to adapt it to a specific application, such as calculating fields in HPGe detectors. From this point of view, FEniCS has the same drawback as commercial packages, that is, it is overkilling in HPGe field calculation, but lacks basic features that are specific for HPGe application. Nonetheless, there is ongoing effort within MJD collaboration to adapt it for HPGe detectors.

MaGe and siggen, on the other hand, are dedicated software for HPGe signal formation simulation. They are not as versatile and sophisticated as previously mentioned packages, but are just enough for the HPGe application. Initially, MaGe was jointly developed by the Majorana and GERDA collaborations mainly for GEANT4 based MC simulations. It was extended later on to include a full pulse-shape simulation chain using GEANT4 simulation results as input. It can be used for the simulation of both segmented and point-contact detectors. The major drawback of MaGe is that it is only available for GERDA and Majorana collaborations.

Siggen is mainly developed by David Radford for MJD pulse-shape simulation. It is open-source and freely available. A stand-alone portion of siggen, called fieldgen, is dedicated to the calculation of fields and potentials of point-contact detectors in two dimensional cylindrical coordinates. It cannot be used for segmented detectors. The program is written in c, but the configuration file is plain ASCII with straight forward syntax for a user to specify detailed dimensions of a detector, from the size of the point-contact to the radius of the groove around the point-contact, used to reduce surface leakage current. Fieldgen can also be used to calculated the full depletion voltage, depletion region in case that a detector is not fully depleted, as well as the capacitance of a detector. Those functions are not available in other packages. Fieldgen utilizes the successive over-relaxation method (SOR) to calculate the potential at first in a coarse grid with a typical distance between two grid points of 1 mm. The result of this coarse calculation is then used as the input of a more precise calculation in a finer grid with a typical distance between two grid points of 0.1 mm.   This simple approach, instead of the automatic adaptive meshing techniques used in some of the other packages, makes fieldgen fast and accurate enough for its dedicated application.

SolidStateDetectors.jl is mainly developed by the GeDet group at the Max-Planck-Institut fuer Physik, Munich for LEGEND. It calculates both electric fields and waveforms. In its field calculation part, it contains functions to calculate both point-contact and segmented configurations. It can also handle the situation of partial depletion.  It is written in Julia, a relatively new, high-level general-purpose programming language designed to address the needs of high-performance numerical analysis.

Overall, fieldgen seems to be the best choice at this moment for users interested in HPGe field calculation as long as their detector geometry is similar to that of point-contact ones. (It is not as limiting as it sounds, because the bore hole of a common coaxial detector can be regarded as a very large point-contact, and a planar detector can be regarded as having a large flat point-contact. The main limitation is on segmented or stripped contacts.) However, the lack of detailed documentation makes it  hard for a developer to modify the code of fieldgen for other geometries or to add new features.

This is one of the reasons why many research groups write their own code for HPGe detector field calculation instead of using any of the mentioned major players. An obvious advantage of home brewed code is that it is well understood and easy to tune if needed. The second advantage is that writing their own code instead of using existing ones deepens the understanding of junior researchers on HPGe detector working principles and numerical calculation techniques. Drawbacks of this approach include the limited functionality, the lack of verification and the waste of time in reinventing the wheel.

The purpose of GeFiCa is not to replace any of the existing packages, or even to compete with them. Instead, it is aimed to clearly explain and demonstrate the analytic and numeric methods to solve Poisson's equation, practical coding considerations as well as visualization methods. It does so by providing intensive documentation and example macros, and serves as a one-stop resource for people who want to understand the operating mechanism of such a package under the hood. None of the tools mentioned above fits all applications. Home brewed codes built on top of some existing tools may be the best choice for education and specific applications, as long as the drawbacks mention previously can be effectively overcome through the demonstration provided in GeFiCa.

Basics of numeric solution of Poisson's Equation
================================================

The electric potential \f$\varphi\f$ can be calculated using Poisson's equation \f$ \nabla^2 \varphi(\mathbf{x}) = - \rho(\mathbf{x})/\epsilon_0/\epsilon_R \f$, where \f$\rho\f$ is the ionized impurity concentration, \f$\mathbf{x}\f$ denotes the coordinates, and \f$\epsilon_0,\epsilon_R\f$ are the permittivity of vacuum and relative permittivity of Ge, respectively. The equation can be simplified to \f$ \mathrm{d}^2\varphi/\mathrm{d}x^2 = - \rho/\epsilon_0/\epsilon_R \f$ in 1D. The second-order derivative \f$ \mathrm{d}^2\varphi/\mathrm{d}x^2 \f$ can be calculated numerically as \f$ \mathrm{d}^2 \varphi / \mathrm{d} x^2 = [\varphi(x_{i-1}) - 2\varphi(x_i) + \varphi(x_{i+1})]/\Delta x^2 \f$, where \f$ x_i \f$ is the coordinate of the _i_-th point in a grid, and \f$ \Delta x \f$ is the step length of the grid. The numerical calculation of the second-order derivative with higher orders of accuracy can be done using the [table][] of Finite difference coefficient in Wikipedia.

[table]:https://en.wikipedia.org/wiki/Finite_difference_coefficient

Numerical calculation
=====================

showConvergingSteps.C

Verification
============

Compare to analytic solutions
-----------------------------

Compare to fieldgen
-------------------

Compare to FEniCS
-----------------

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
