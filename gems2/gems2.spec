%define	name	gems2
%define	version	2.1
%define	release	1rc9
%define install_dir /usr
%define shared_dir %{install_dir}/share
%define shared_src_dir shared

#%define libname_orig %mklibname %{name}
#%define libname %{libname_orig}0


Name:		%{name}
Summary: 	Chemistry, geochemistry, Gibbs energy minimization
Version:	%{version}
Release:	%{release}
Copyright:	Distributable
#License:	GPL
Group:		Applications/Engineering
Source0: 	%{name}-%{version}.tar.bz2
#BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-buildroot
#Target: i586-gnu-linux
Url:		http://les.web.psi.ch/Software/GEMS-PSI/
#BuildRequires: 	libqt3-devel
#Requires:	%{libname} = %{version}-%{release}

%description
This program calculates complex geochemical equilibria
and helps preparing consistent thermodynamic databases

%prep

%setup

%build
make clean
make RELEASE=YES

%install

#if exists 
/usr/sbin/groupadd gems_admin || /bin/true

install -s -m 755 -o 0 -g 0 gems2 %{install_dir}/bin/gems2

install -d -m 775 -o 0 -g gems_admin %{shared_dir}/gems2
install -d -m 775 -o 0 -g gems_admin %{shared_dir}/gems2/DB.default
install -m 664 -o 0 -g gems_admin %{shared_src_dir}/DB.default/* %{shared_dir}/gems2/DB.default 
install -d -m 775 -o 0 -g gems_admin %{shared_dir}/gems2/projects
install -m 664 -o 0 -g gems_admin %{shared_src_dir}/projects/*.* %{shared_dir}/gems2/projects 
install -d -m 775 -o 0 -g gems_admin %{shared_dir}/gems2/projects/TryNPTDB
install -m 664 -o 0 -g gems_admin %{shared_src_dir}/projects/TryNPTDB/* %{shared_dir}/gems2/projects/TryNPTDB
install -d -m 775 -o 0 %{shared_dir}/gems2/visor.data
install -m 664 -o 0 %{shared_src_dir}/visor.data/* %{shared_dir}/gems2/visor.data
install -d -m 775 -o 0 %{shared_dir}/gems2/doc/html
install -m 664 -o 0 %{shared_src_dir}/doc/html/* %{shared_dir}/gems2/doc/html
install -d -m 775 -o 0 %{shared_dir}/gems2/doc/pdf
install -m 664 -o 0 %{shared_src_dir}/doc/pdf/* %{shared_dir}/gems2/doc/pdf
install -d -m 775 -o 0 %{shared_dir}/gems2/doc/txt
install -m 664 -o 0 %{shared_src_dir}/doc/txt/* %{shared_dir}/gems2/doc/txt

install -d -m 775 -o 0 %{shared_dir}/gems2/img
install -m 664 -o 0 %{shared_src_dir}/img/* %{shared_dir}/gems2/img
install -m 664 -o 0 %{shared_src_dir}/img/gems32.xpm %{shared_dir}/icons/gems2.xpm
install -m 664 -o 0 %{shared_src_dir}/img/gems32.xpm %{shared_dir}/pixmaps/gems2.xpm
#install -m 664 -o 0 img/gems16.xpm %{shared_dir}/icons/mini/gems2.xpm
install -m 664 -o 0 doc/gems2.desktop %{shared_dir}/applications/

%files
%{install_dir}/bin/gems2
%{shared_dir}/gems2
%{shared_dir}/icons/gems2.xpm
%{shared_dir}/pixmaps/gems2.xpm
%{shared_dir}/applications/gems2.desktop
