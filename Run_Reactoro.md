## gems3gui { GEMS-Reactoro } ##

Now we have first worked version of GEMS-Reaktoro project.
We can calculate Systems and Processes on remote server.

Numbers was tested to Kaolinite and Solvus projects
More detailed comparison would be done on next tasks.

For Gem2mt calculation we use internal TNodeArray class, it would be connect for server in next tasks. 


### Building and run GEM-Selektor ###

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


* To switch to another branch (e.g. GEMS-Reactoro), use a git client or open a terminal, cd to ~/gitGEMS3/gems3gui, and type in the command line

```sh

git checkout -b branches/GEMS-Reactoro --track origin/branches/GEMS-Reactoro
git pull origin branches/GEMS-Reactoro
```
or
```sh

git checkout GEMS-Reactoro
```

* Do the same to ~/gitGEMS3/standalone

* The GEMS-Reactoro library uses [ZeroMQ](https://github.com/zeromq/libzmq)  core engine in C++ implements ZMTP/3.1 and header-only C++ binding for [libzmq](https://github.com/zeromq/cppzmq) as thirdparty dependency. To install  in a terminal ~/standalone/gemserver$ execute the following:

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

