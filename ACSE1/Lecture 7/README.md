# Dask - https://dask.org/

Dask is a flexible library for parallel computing in Python.

Dask is composed of two parts:

* **Dynamic task scheduling** optimized for computation. This is similar to Airflow, Luigi, Celery, or Make, but optimized for interactive computational workloads.
* **“Big Data” collections** like parallel arrays, dataframes, and lists that extend common interfaces like NumPy, Pandas, or Python iterators to larger-than-memory or distributed environments. These parallel collections run on top of dynamic task schedulers.

Dask emphasizes the following virtues:

* **Familiar:** Provides parallelized NumPy array and Pandas DataFrame objects
* **Flexible:** Provides a task scheduling interface for more custom workloads and integration with other projects.
* **Native:** Enables distributed computing in pure Python with access to the PyData stack.
* **Fast:** Operates with low overhead, low latency, and minimal serialization necessary for fast numerical algorithms
* **Scales up:** Runs resiliently on clusters with 1000s of cores
* **Scales down:** Trivial to set up and run on a laptop in a single process
* **Responsive:** Designed with interactive computing in mind, it provides rapid feedback and diagnostics to aid humans


## Dask tutorial

Dask provides a great tutorial on its website that we are going to focus on today:
 
https://tutorial.dask.org/