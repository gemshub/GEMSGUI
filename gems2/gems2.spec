Summary: chemistry geochemistry Gibbs energy minimization
Name: gems2
Version: 2.0b
Release: 1
Copyright: Distributable
Group: Applications/Engineering
Source: gems-2.0b.tar.bz2
#BuildRoot: /tmp/rpmbuild_gems-2.0b
#Target: i586-gnu-linux

%description
This program permits to calculate complex geochemical equilibria
and prepare consistent thermodynamic databases

%prep

%setup

%build
make clean
make RELEASE=YES

%install

#if exists 
/usr/sbin/groupadd -f gems_admin

install -s -m 755 -o 0 -g 0 gems2 /usr/local/bin/gems2
install -d -m 775 -o 0 -g gems_admin /usr/local/share/gems2

install -d -m 775 -o 0 -g gems_admin /usr/local/share/gems2/DB.default
install -m 664 -o 0 -g gems_admin program/DB.default/* /usr/local/share/gems2/DB.default 
install -d -m 775 -o 0 -g gems_admin /usr/local/share/gems2/visor.data
install -m 664 -o 0 -g gems_admin program/visor.data/* /usr/local/share/gems2/visor.data
install -d -m 775 -o 0 -g gems_admin /usr/local/share/gems2/doc/html
install -m 664 -o 0 -g gems_admin program/doc/html/* /usr/local/share/gems2/doc/html
install -d -m 775 -o 0 -g gems_admin /usr/local/share/gems2/img
install -m 664 -o 0 -g gems_admin program/img/* /usr/local/share/gems2/img
install -m 664 -o 0 -g gems_admin program/img/gems16.xpm /usr/local/share/icons/mini/gems.xpm
install -m 664 -o 0 -g gems_admin program/img/gems32.xpm /usr/local/share/icons/gems.xpm
install -m 664 -o 0 -g gems_admin doc/GEMS.kdelnk /usr/local/share/applnk/Applications

%files
/usr/local/bin/gems2
/usr/local/share/gems2
/usr/local/share/icons/gems.xpm
/usr/local/share/icons/mini/gems.xpm
/usr/local/share/applnk/Applications/GEMS.kdelnk

#/usr/local/local/lib/gems/DB.default
#/usr/local/local/lib/gems/profile.default
#/usr/local/local/lib/gems/visor.data
#/usr/local/local/lib/gems/visor.data/visobj.dat
#/usr/local/local/lib/gems/visor.data/visor.dat
#/usr/local/local/lib/gems/doc/html
