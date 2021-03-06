#!/bin/bash

#
# Run cola examples (offline-test for cola telegrams and ros services provided by the sick_lidar_localization driver)
# against local test server simulating a localization controller
#

#
# Environment
#

pushd ../../../..
printf "\033c"
source /opt/ros/melodic/setup.bash
source ./devel/setup.bash
# source ./install/setup.bash

#
# Cleanup
#

if [ -d ./src/sick_lidar_localization ]         ; then ./src/sick_lidar_localization/test/scripts/killall.bash         ; fi
if [ -d ./src/sick_lidar_localization_pretest ] ; then ./src/sick_lidar_localization_pretest/test/scripts/killall.bash ; fi
rm -rf ~/.ros/*
rosclean purge -y
if [ ! -d ~/.ros/log/cola_examples ] ; then mkdir -p ~/.ros/log/cola_examples ; fi

#
# Run test server, simulate localization controller for offline tests
#

roslaunch sick_lidar_localization sim_loc_test_server.launch 2>&1 >> ~/.ros/log/cola_examples/sim_loc_test_server.log &
sleep 3 # make sure ros core and sim_loc_test_server are up and running 

#
# Run ros driver, connect to localization controller and receive, convert and publish telegrams
#

# roslaunch sick_lidar_localization sim_loc_driver.launch localization_controller_ip_address:=127.0.0.1 2>&1 | unbuffer -p tee -a ~/.ros/log/cola_examples/sim_loc_driver.log &
roslaunch sick_lidar_localization sim_loc_driver.launch localization_controller_ip_address:=127.0.0.1 2>&1 >> ~/.ros/log/cola_examples/sim_loc_driver.log &
sleep 1 # make sure sim_loc_test_server and sim_loc_driver are up and running 

#
# Send cola examples using ros services provided by the sick_lidar_localization driver
#

popd
./send_cola_examples.bash 2>&1 | unbuffer -p tee -a ~/.ros/log/cola_examples/send_cola_examples.log

#
# Cleanup and exit
#

./killall.bash
sleep 1 ; killall roslaunch ; sleep 1
cat ~/.ros/log/cola_examples/send_cola_examples.log
# sleep 20
mkdir -p ./log/cola_examples
cp -rf ~/.ros/log/cola_examples/*.log ./log/cola_examples
echo -e "\nsick_lidar_localization finished. Warnings and errors:"
grep "WARN" ./log/cola_examples/*.log
grep "ERR" ./log/cola_examples/*.log
echo -e "\nsim_loc_driver check summary:"
grep -i "check messages thread summary" ./log/cola_examples/*.log 
grep -i "verification summary" ./log/cola_examples/*.log 
echo -e "\nsick_lidar_localization summary:"
grep -R "summary" ./log/*

