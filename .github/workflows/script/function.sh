#!/bin/sh

# Declare constants for OS Ubuntu, CentOS, openSUSE
readonly UBUNTU=Ubuntu
readonly CENTOS=Centos
readonly OPENSUSE=Opensuse

# Build docker images using Dockerfile for Ubuntu, CentOS, openSUSE
build_docker_image() {
    docker build -t $image_name -f ${{ matrix.docker_file }} .
}

# Check if the file exists with the parameter path passed
check_file_exist() {
    local file_path=$1
    if [ ! -f "$file_path" ]; then
        echo "$file_path not found!"
    fi
}

# Get version in griddb.spec file
get_version() {
    check_file_exist "installer/SPECS/griddb.spec"

    echo $(grep -Eo '[0-9\.]+' installer/SPECS/griddb.spec) > output.txt
    local griddb_version=$(awk '{print $1}' output.txt)
    rm output.txt
    echo $griddb_version
}

# Build C Client for CentOS, openSUSE
build_c_client() {
    local os=$1
    cd c_client
    case $os in
        $CENTOS | $OPENSUSE)
            # Build GridDB server
            cd client/c
            ./bootstrap.sh
            ./configure
            make
            ;;
        $UBUNTU)
            # No need to build before create Ubuntu package
            # C Client will auto build in step build package
            ;;
        *)
            echo "Unknown OS"
            ;;

    esac
}

# Create rpm for CentOS, openSUSE and deb package for Ubuntu
build_package() {
    local os=$1
    case $os in
        $CENTOS | $OPENSUSE)
            # Get griddb version and set source code zip file name,
            #   ex "4.5.2" and "griddb-c-client-4.5.2.zip"
            cd c_client
            local griddb_version=$(get_version)
            cd ..
            local griddb_folder_name="griddb-c-client-${griddb_version}"
            local griddb_zip_file="${griddb_folder_name}.tar.gz"

            # Create rpm file
            rsync -a --exclude=.git c_client/ $griddb_folder_name
            tar -cf $griddb_zip_file $griddb_folder_name
            cp $griddb_zip_file c_client/installer/SOURCES/
            rm -rf $griddb_folder_name
            cd c_client/installer
            check_file_exist "SPECS/griddb.spec"
            rpmbuild --define="_topdir /c_client/installer" -bb --clean SPECS/griddb.spec
            cd ..
            ;;

        $UBUNTU)
            # Create deb file
            cd c_client
            dpkg-buildpackage -b
            ;;

        *)
            echo "Unknown OS"
            ;;

    esac
    # Change package name for OPENSUSE
    if [ $os == $OPENSUSE ]; then
            check_file_exist "installer/RPMS/x86_64/griddb-c-client-$griddb_version-linux.x86_64.rpm"
            mv "installer/RPMS/x86_64/griddb-c-client-$griddb_version-linux.x86_64.rpm" \
              "installer/RPMS/x86_64/griddb-c-client-$griddb_version-opensuse.x86_64.rpm"
    fi
}

# Install rpm and deb package C Client for Ubuntu, CentOS, OpenSUSE
install_c_client() {
    local package_path=$1
    check_file_exist "$package_path"
    local os=$2

    case $os in
        $CENTOS | $OPENSUSE)
            rpm -ivh $package_path
            ;;
        $UBUNTU)
            dpkg -i $package_path
            ;;
        *)
            echo "Unknown OS"
            ;;

    esac
}

# Check information rpm and deb package
check_package() {
    local package_path=$1
    check_file_exist "$package_path"

    local os=$2
    case $os in
        $CENTOS | $OPENSUSE)
            rpm -qip $package_path
            ;;
        $UBUNTU)
            dpkg-deb -I $package_path
            ;;
        *)
            echo "Unknown OS"
            ;;

    esac
}

# Run sample C Client to griddb server
# You can refer to https://github.com/griddb/c_client#execute-a-sample-program-1
run_sample() {
    cd c_client
    local version=$(get_version)
    cd ../
    cp /usr/griddb_c_client-${version}/sample/sample1.c .
    gcc sample1.c -lgridstore
    local notification_host=$1
    local notification_port=$2
    local cluster_name=$3
    local username=$4
    local password=$5
    ./a.out $notification_host $notification_port $cluster_name $username $password
}

# Uninstall package griddb-c-client
uninstall_package() {
    local package_name=$1
    local os=$2
    case $os in
        $CENTOS | $OPENSUSE)
            rpm -e $package_name
            ;;
        $UBUNTU)
            dpkg -r $package_name
            ;;
        *)
            echo "Unknown OS"
            ;;
    esac
}

# Config password and clustername for griddb server
config_griddb() {
    local username=$1
    local password=$2
    local cluster_name=$3
    su -l gsadm -c "gs_passwd $username -p $password"
    su -l gsadm -c "sed -i 's/\"clusterName\":\"\"/\"clusterName\":\"$cluster_name\"/g' /var/lib/gridstore/conf/gs_cluster.json"
}

# Start and run griddb server
start_griddb() {
    local username=$1
    local password=$2
    local cluster_name=$3
    su -l gsadm -c "gs_startnode -w -u $username/$password"
    su -l gsadm -c "gs_joincluster -c $cluster_name -u $username/$password -w"
}

# Copy rpm and deb package from docker container to host
copy_package_to_host() {

    local os=$2
    local container_name=$1
    local griddb_version=$(get_version)
    case $os in
        $CENTOS)
            mkdir -p installer/RPMS/x86_64/
            docker cp $container_name:/c_client/installer/RPMS/x86_64/griddb-c-client-${griddb_version}-linux.x86_64.rpm installer/RPMS/x86_64/
            ;;
        $OPENSUSE)
            mkdir -p installer/RPMS/x86_64/
            docker cp $container_name:/c_client/installer/RPMS/x86_64/griddb-c-client-${griddb_version}-opensuse.x86_64.rpm installer/RPMS/x86_64/
            ;;
        $UBUNTU)
            docker cp $container_name:./griddb-c-client_${griddb_version}_amd64.deb ../
            ;;
        *)
            echo "Unknown OS"
            ;;
    esac
}

