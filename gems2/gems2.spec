%define	name	gems2
%define	version	2.0
%define	release	1

#%define libname_orig %mklibname %{name}
#%define libname %{libname_orig}0


Name:		%{name}
Summary: 	chemistry geochemistry Gibbs energy minimization
Version:	%{version}
Release:	%{release}
Copyright:	Distributable
#License:	GPL
Group:		Applications/Engineering
Source0: 	%{name}-%{version}.tar.bz2
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
#Target: i586-gnu-linux
Url:		http://les.web.psi.ch/Software/GEMS-PSI/
#BuildRequires: 	libqt3-devel
#Requires:	%{libname} = %{version}-%{release}

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

install -s -m 755 -o 0 -g 0 gems2 /usr/bin/gems2
install -d -m 775 -o 0 -g gems_admin /usr/share/gems2

install -d -m 775 -o 0 -g gems_admin /usr/share/gems2/DB.default
install -m 664 -o 0 -g gems_admin shared/DB.default/* /usr/share/gems2/DB.default 
install -d -m 775 -o 0 /usr/share/gems2/visor.data
install -m 664 -o 0 shared/visor.data/* /usr/share/gems2/visor.data
install -d -m 775 -o 0 /usr/share/gems2/doc/html
install -m 664 -o 0 shared/doc/html/* /usr/share/gems2/doc/html
install -d -m 775 -o 0 /usr/share/gems2/img
install -m 664 -o 0 shared/img/* /usr/share/gems2/img
install -m 664 -o 0 shared/img/gems32.xpm /usr/share/icons/gems.xpm
install -m 664 -o 0 shared/img/gems16.xpm /usr/share/icons/mini/gems.xpm
install -m 664 -o 0 doc/gems2.desktop /usr/share/applications/

%files
/usr/bin/gems2
/usr/share/gems2
/usr/share/icons/gems32.xpm
/usr/share/icons/mini/gems16.xpm
/usr/share/aplications/gems2.desktop

#/usr/local/local/lib/gems/DB.default
#/usr/local/local/lib/gems/profile.default
#/usr/local/local/lib/gems/visor.data
#/usr/local/local/lib/gems/visor.data/visobj.dat
#/usr/local/local/lib/gems/visor.data/visor.dat
#/usr/local/local/lib/gems/doc/html
