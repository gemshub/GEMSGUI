Summary: chemistry geochemistry Gibbs energy minimization
Name: gems
Version: 1.93b
Release: 6
Copyright: Distributable
Group: Applications/Engineering
Source: gems-1.93b.tar.bz2
#BuildRoot: /tmp/rpmbuild_gems-1.93b
#Target: i586-gnu-linux

%description
This program permits to calculate complex geochemical equilibria
and prepare consistent thermodynamic databases

%prep

%setup

%build
make clean
make 

%install

#if exists 
/usr/sbin/groupadd -f gems_admin

install -s -m 755 -o 0 -g 0 gems /usr/bin/gems
install -d -m 775 -o 0 -g gems_admin /usr/lib/gems

install -d -m 775 -o 0 -g gems_admin /usr/lib/gems/DB.default
install -m 664 -o 0 -g gems_admin GEMS.sys/DB.default/* /usr/lib/gems/DB.default 
install -d -m 775 -o 0 -g gems_admin /usr/lib/gems/profile/Default
install -m 664 -o 0 -g gems_admin GEMS.sys/profile/Default/* /usr/lib/gems/profile/Default
install -d -m 775 -o 0 -g gems_admin /usr/lib/gems/visor.data
install -m 664 -o 0 -g gems_admin GEMS.sys/visor.data/* /usr/lib/gems/visor.data
install -d -m 775 -o 0 -g gems_admin /usr/lib/gems/doc/html
install -m 664 -o 0 -g gems_admin GEMS.sys/doc/html/* /usr/lib/gems/doc/html
install -d -m 775 -o 0 -g gems_admin /usr/lib/gems/img
install -m 664 -o 0 -g gems_admin GEMS.sys/img/* /usr/lib/gems/img
install -m 664 -o 0 -g gems_admin GEMS.sys/img/gems16.xpm /usr/share/icons/mini/gems.xpm
install -m 664 -o 0 -g gems_admin GEMS.sys/img/gems32.xpm /usr/share/icons/gems.xpm
install -m 664 -o 0 -g gems_admin doc/GEMS.kdelnk /usr/share/applnk/Applications

%files
/usr/bin/gems
/usr/lib/gems
/usr/share/icons/gems.xpm
/usr/share/icons/mini/gems.xpm
/usr/share/applnk/Applications/GEMS.kdelnk

#/usr/local/lib/gems/DB.default
#/usr/local/lib/gems/profile.default
#/usr/local/lib/gems/visor.data
#/usr/local/lib/gems/visor.data/visobj.dat
#/usr/local/lib/gems/visor.data/visor.dat
#/usr/local/lib/gems/doc/html
