# Fantastic Search Engine

This is a search engine created by the team Fantastic Bugs and Where to Find Them in the course EECS398 at the University of Michigan.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.
The project generates a variety of executables, the most important of which are the following:

master - This is the central host for the distributed crawling system.
worker - This is a worker computer for the distributed crawling system. It asks the master for URLs to crawl, and produces files of downloaded pages.
index_builder - This process takes the files of downloaded pages, and produces searchable index files.
query_server - This server hosts index files in the distributed query system.
query_worker - This process receives a query and generates meaningful search results from the index files, including snippets and titles.
server - This server hosts the front end, which takes queries from the user and displays results.

### Prerequisites

meson/ninja - The project is built using meson to generate a build directory, and ninja to compile.
openssl - For https support.

### Installing

From the root directory, type 
<pre lang="no-highlight"><code>meson [FILENAME]
</code></pre>
To create the build directory. Inside the build directory, type
<pre lang="no-highlight"><code>ninja [EXECUTABLE_NAME]
</code></pre>
to compile that executable, or simply
<pre lang="no-highlight"><code>ninja
</code></pre>
to compile all executables.

All executable come with a helpful usage message when running without the proper arguments.

## Deployment

Add additional notes about how to deploy this on a live system

## Authors

* **Aniruddh Agarwal**
* **Jaeyoon Kim**
* **JinSoo Ihm**
* **Grant Reszczyk**
* **Sharon Ye**
* **Chandler Meyers**

## Acknowledgments

* Nicole Hamilton, the instructor for EECS39

