..  Copyright (c) 2024,    

   Distributed under the terms of the BSD 3-Clause License.  

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the xeus-pywrap library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``xeus-pywrap`` build supports the following options:

- ``XEUS_PYWRAP_BUILD_SHARED``: Build the ``xeus-pywrap`` shared library. **Enabled by default**.
- ``XEUS_PYWRAP_BUILD_STATIC``: Build the ``xeus-pywrap`` static library. **Enabled by default**.


- ``XEUS_PYWRAP_USE_SHARED_XEUS``: Link with a `xeus` shared library (instead of the static library). **Enabled by default**.

Building the kernel
~~~~~~~~~~~~~~~~~~~

The package includes two options for producing a kernel: an executable ``xpywrap`` and a Python extension module, which is used to launch a kernel from Python.

- ``XEUS_PYWRAP_BUILD_EXECUTABLE``: Build the ``xpywrap``  executable. **Enabled by default**.


If ``XEUS_PYWRAP_USE_SHARED_XEUS_PYWRAP`` is disabled, xpywrap  will be linked statically with ``xeus-pywrap``.

Building the Tests
~~~~~~~~~~~~~~~~~~

- ``XEUS_PYWRAP_BUILD_TESTS ``: enables the tets  **Disabled by default**.

