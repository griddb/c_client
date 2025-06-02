%define griddb_name griddb-c-client
%define griddb_ver 5.8.0
%define griddb_instdir /usr/griddb_c_client-%{griddb_ver}
# do not strip
%define __spec_install_post /usr/lib/rpm/brp-compress

%define debug_package %{nil}

Name:           %{griddb_name}
Summary:        GridDB Community Edition
Version:        %{griddb_ver}
Release:        linux
Group:          Applications/Databases
Vendor:         Toshiba Digital Solutions Corporation
License:        Apache-2.0
Source:         %{name}-%{version}.tar.gz

%description
GridDB is Database for IoT with both NoSQL interface and SQL Interface.
The GridDB C Client provides NoSQL interface for C language.

%prep
%setup -q

%build

%install
rm -rf %{buildroot}

# Install files
mkdir -p %{buildroot}%{griddb_instdir}/lib
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/ebb
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/picojson
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/purewell
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/sha2
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/yield
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/uuid
mkdir -p %{buildroot}%{griddb_instdir}/3rd_party/omaha
mkdir -p %{buildroot}%{griddb_instdir}/sample
mkdir -p %{buildroot}/usr/lib64
mkdir -p %{buildroot}/usr/include

install -c -m 755 bin/libgridstore.so.0.0.0                     %{buildroot}%{griddb_instdir}/lib
install -c -m 644 client/c/include/gridstore.h                  %{buildroot}%{griddb_instdir}/lib

install -c -m 644 3rd_party/3rd_party.md                        %{buildroot}%{griddb_instdir}/3rd_party
install -c -m 644 3rd_party/Apache_License-2.0.txt              %{buildroot}%{griddb_instdir}/3rd_party
install -c -m 644 3rd_party/BSD_License.txt                     %{buildroot}%{griddb_instdir}/3rd_party
install -c -m 644 3rd_party/MIT_License.txt                     %{buildroot}%{griddb_instdir}/3rd_party
install -c -m 644 3rd_party/ebb/LICENSE                         %{buildroot}%{griddb_instdir}/3rd_party/ebb
install -c -m 644 3rd_party/picojson/org/include/README.mkdn    %{buildroot}%{griddb_instdir}/3rd_party/picojson
install -c -m 644 3rd_party/purewell/purewell.txt               %{buildroot}%{griddb_instdir}/3rd_party/purewell
install -c -m 644 3rd_party/sha2/README                         %{buildroot}%{griddb_instdir}/3rd_party/sha2
install -c -m 644 3rd_party/yield/yield.txt                     %{buildroot}%{griddb_instdir}/3rd_party/yield
install -c -m 644 3rd_party/uuid/uuid/COPYING                   %{buildroot}%{griddb_instdir}/3rd_party/uuid
install -c -m 644 3rd_party/omaha/COPYING                       %{buildroot}%{griddb_instdir}/3rd_party/omaha

install -c -m 644 README.md                                     %{buildroot}%{griddb_instdir}
install -c -m 644 client/c/sample/sample1.c                     %{buildroot}%{griddb_instdir}/sample

# Install symbolic links
ln -sf %{griddb_instdir}/lib/libgridstore.so.0.0.0              %{buildroot}/usr/lib64
ln -sf %{griddb_instdir}/lib/libgridstore.so.0.0.0              %{buildroot}/usr/lib64/libgridstore.so.0
ln -sf %{griddb_instdir}/lib/libgridstore.so.0.0.0              %{buildroot}/usr/lib64/libgridstore.so

ln -sf %{griddb_instdir}/lib/gridstore.h                        %{buildroot}/usr/include

%pre

%post

%files

%defattr(755,root,root)
%dir %{griddb_instdir}
%dir %{griddb_instdir}/lib
%dir %{griddb_instdir}/3rd_party
%dir %{griddb_instdir}/3rd_party/ebb
%dir %{griddb_instdir}/3rd_party/picojson
%dir %{griddb_instdir}/3rd_party/purewell
%dir %{griddb_instdir}/3rd_party/sha2
%dir %{griddb_instdir}/3rd_party/yield
%dir %{griddb_instdir}/3rd_party/uuid
%dir %{griddb_instdir}/3rd_party/omaha
%dir %{griddb_instdir}/sample

%defattr(-,root,root)
%{griddb_instdir}/lib/libgridstore.so.0.0.0
%{griddb_instdir}/lib/gridstore.h
%{griddb_instdir}/3rd_party/3rd_party.md
%{griddb_instdir}/3rd_party/Apache_License-2.0.txt
%{griddb_instdir}/3rd_party/BSD_License.txt
%{griddb_instdir}/3rd_party/MIT_License.txt
%{griddb_instdir}/3rd_party/ebb/LICENSE
%{griddb_instdir}/3rd_party/picojson/README.mkdn
%{griddb_instdir}/3rd_party/purewell/purewell.txt
%{griddb_instdir}/3rd_party/sha2/README
%{griddb_instdir}/3rd_party/yield/yield.txt
%{griddb_instdir}/3rd_party/uuid/COPYING
%{griddb_instdir}/3rd_party/omaha/COPYING
%{griddb_instdir}/README.md
%{griddb_instdir}/sample/sample1.c
/usr/lib64/libgridstore.so
/usr/lib64/libgridstore.so.0
/usr/lib64/libgridstore.so.0.0.0
/usr/include/gridstore.h

%changelog
* Thu May 30 2024 Toshiba Digital Solutions Corporation
- 5.6.0
* Fri Feb 9 2024 Toshiba Digital Solutions Corporation
- 5.5.0
* Mon Jun 12 2023 Toshiba Digital Solutions Corporation
- 5.3.0
* Wed Mar 22 2023 Toshiba Digital Solutions Corporation
- 5.1.0
* Mon May 16 2022 Toshiba Digital Solutions Corporation
- 5.0.0
* Tue Jun 1 2021 Toshiba Digital Solutions Corporation
- 4.6.0
