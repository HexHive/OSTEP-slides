# Introduction to Operating Systems (CS-323, EPFL)

This class is a gently introduction into operating systems concepts at EPFL for undergraduate students in their third year.
The students come with a light background in C programming from a mandatory class on C concepts and an optional C programming project in their second year.
Generally, the C background of the students is rather light and many have not really worked with Linux environments yet.

The class follows the excellent [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/) by Remzi H. Arpaci-Dusseau and Andrea C. Arpaci-Dusseau.
We break the class into four components:

* Virtualization
* Cocurrency
* Persistence
* Security

While recent editions of the book have added classic systems security with a heavy focus on authentication, access control, and cryptography, we focus on software security concepts.
The software security component enables students to reason about bugs, understand vulnerabilities, and gives them tools on how to handle security issues.
We feel that this prepares them better for software development and security aspects finally connect different pieces from the earlier pillars in a different light.


## Building the slides

The slides are written in Markdown and can be translated to PDF through `pandoc`.
We leverage inline TikZ figures in LaTeX environments for visual figures.
If you have the necessary packages installed, you can create the slides through a hearty

```
$ make

```

If your LaTeX and pandoc environments are not on par, you may have to install the following:

```
$ sudo apt install pandoc graphviz dot2tex texlive-full

```

Note that `texlive-full` is overkill but storage is almost free nowadays.


## Changelog

* 2020: Heavily reworked slides in the 2nd year (Mathias Payer)
* 2019: First version of the slides, created from scratch (Mathias Payer)


## References and license

Feel free to reuse these slides but credit [Mathias Payer](https://nebelwelt.net) when reusing the material.