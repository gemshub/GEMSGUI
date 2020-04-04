## gems3gui { GEMS-Reactoro project } ##

Now we are at the end of Stage 1 having the first working variant of GEMS-Reaktoro code.
With GEMS-Reaktoro GUI, we can set up a chemical equilibrium problem as usual and compute Systems and Processes using the GEMS3K code as a standalone server.

The accuracy of results and their consistency with the trunk GEM-Selektor version 3.7.0 has been tested on Kaolinite and Solvus projects; 
more detailed comparisons will be done in the next tasks.

For GEM2MT reactive transport simulations, we use so far the internal TNodeArray class, which will be connected to GEMS3K server in the next tasks. 
Main focus at Stage 2 will be to modify Reaktoro as a standalone server using ZeroMQ messaging in a similar way as it has been already done with GEMS3K,
and arrange a compatible I/O file exchange to be able to run Reaktoro server alternatively to GEMS3K server and compare the results of solving the same 
test equilibria and processes. This task may be facilitated by using the new JSON format of GEMS3K I/O files, already readable/writable by GEMS3K server 
and possible to be used by the Reaktoro/Optima server.  

### How to build and run GEMS-Reaktoro ###

For more details see README.md


* If not yet done, copy recursively the "/Docs/build-release" directory from the  folder where this file is located to the folder in which the "/gems-gui" folder is located. Typically, you should have the folders at the same level, as shown below.

```sh
~/gitGEMS3
    /gems3gui
    /standalone
    /build-release
    /build-debug
```
We assume that both subfolders /gems3gui and /standalone are under git control and are checked out to "trunk" branch.

* To switch to GEMS-Reactoro branch, use a git client or open a terminal, cd to ~/gitGEMS3/gems3gui, and type in the command line

```sh

git checkout -b branches/GEMS-Reactoro --track origin/branches/GEMS-Reactoro
git pull origin branches/GEMS-Reactoro
```
or
```sh

git checkout GEMS-Reactoro
```

* Do the same to ~/gitGEMS3/standalone

* The GEMS-Reactoro library uses [ZeroMQ](https://github.com/zeromq/libzmq)  core engine in C++ implements ZMTP/3.1 and header-only 
* C++ binding for [libzmq](https://github.com/zeromq/cppzmq) as thirdparty dependency. 
* To install the ~/standalone/gemserver$, go into a terminal, and execute the following:

```sh

sudo ./install-dependencies.sh

```

* Into Qt Creator open project ~/gitGEMS3/standalone/gemserver/gems3_server.pro, link build directory to ~gitGEMS3/standalone/gemserver-build and build executable.

GEMS-Reaktoro project
* Into Qt Creator open project ~/gitGEMS3/GEMS3gui/gems3gui.pro and build executable.


* Change command line arguments ( add full path to gemserver executable "~gitGEMS3/standalone/gemserver-build"):

```sh

-d -s /home/sveta/devGEMS/gitGEMS3/GEMS3gui -g /home/sveta/devGEMS/gitGEMS3/standalone/gemserver-build
```

* run project and try calculate IPM and/or process


* After crash or stop debugging, if  gems3_server not terminated in terminal execute command ( for Linux)

```sh

killall gems3_server
```


### Next to do ###

1. Compare/Test IPM and processes calculations for more projects, bugfixing

2.  Send json/key-value data without saving files structure

3. Research ZeroMQ patterns, use threads and other and implemented best practices into gems3_server

http://zguide.zeromq.org/page:all#toc26

4. Implement addition server or change "gems3_server" to execute TNodeArray calculations ( need discussion )

5. ...

