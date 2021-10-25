# Gazebo_pendulum
at first, i tried create the urdf and joint_control_server, but it's quite time_consuming for a semester-end-project
so i shifted to gazebo plugin, which is plug_n_play as its name suggest
quite fun and easy to build model, while still give you the master control of joint
the code is not that hard to read, and the pid number was take from our matlab code
the final version should be pendulum.world, and the lib for it is pendulum_control.cc
all the cart_wheel*.world is for static build and gravity test, as messing with mass DO increase/decrease the stick falling time
2 cpp file is also included, as my reference from other project
final demo is here: https://drive.google.com/drive/folders/1PZatZ1B8R9d7pKhMrPxEzuP-c84UbqlE
