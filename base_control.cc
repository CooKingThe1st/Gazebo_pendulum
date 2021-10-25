#include <functional>
#include <boost/bind.hpp>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>

#include <thread>
#include "ros/ros.h"
#include "ros/callback_queue.h"
#include "ros/subscribe_options.h"
#include "std_msgs/Float32.h"
#include "std_msgs/String.h"
using namespace std;
namespace gazebo
{
  class BasePlugin : public ModelPlugin
  {
  public: 

    void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _parent;

      // Store the pointers to the joints
      this->fl_wheel = this->model->GetJoint("left_wheel_hinge");
      this->fl2_wheel = this->model->GetJoint("left_wheel_hinge_2");

      this->fr_wheel = this->model->GetJoint("right_wheel_hinge");
      this->fr2_wheel = this->model->GetJoint("right_wheel_hinge_2");

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&BasePlugin::OnUpdate, this));

      // Initialize ros, if it has not already been initialized.
      if (!ros::isInitialized())
      {
        int argc = 0;
        char **argv = NULL;
        ros::init(argc, argv, "gazebo_client", ros::init_options::NoSigintHandler);
      }

      // Create our ROS node. This acts in a similar manner to
      // the Gazebo node
      this->rosNode.reset(new ros::NodeHandle("gazebo_client"));

      // Create a named topic, and subscribe to it.
      ros::SubscribeOptions so =
        ros::SubscribeOptions::create<std_msgs::Float32>(
            "/" + this->model->GetName() + "/force_command",
            1,
            boost::bind(&BasePlugin::OnRosMsg, this, _1),
            ros::VoidPtr(), &this->rosQueue);
      this->rosSub = this->rosNode->subscribe(so);
      // Spin up the queue helper thread.
      this->rosQueueThread =
        std::thread(std::bind(&BasePlugin::QueueThread, this));
    }

    // Called by the world update start event
    void OnUpdate()
    { // hardcoded before ROS integration:
      // cout << "wow on update";
      // cout << setprecision(2) << fixed << this->fl_wheel->GetVelocity(0) << '\n';

      // Apply a small linear velocity to the model.
      // this->model->SetLinearVel(ignition::math::Vector3d(0, .3, 0));
    }

    // handle incoming ROS messages
    void OnRosMsg(const std_msgs::Float32ConstPtr &_msg)
    {
      float yAxisForce = _msg->data;
      ROS_INFO_STREAM("Received : " << yAxisForce);
      // this->model->GetLink("cart_body")->SetForce(ignition::math::Vector3d(0,yAxisForce,0));
      this->fr_wheel->SetVelocity(0, yAxisForce);
      this->fl_wheel->SetVelocity(0, yAxisForce);
      this->fr2_wheel->SetVelocity(0, yAxisForce);
      this->fl2_wheel->SetVelocity(0, yAxisForce);
    }

  private:

    // ROS helper function to process messages 
    void QueueThread()
    {
      static const double timeout = 0.01;
      while (this->rosNode->ok())
      {
        this->rosQueue.callAvailable(ros::WallDuration(timeout));
      }
    }

    // private members for ROS
    physics::ModelPtr model; // Pointer to the model
    event::ConnectionPtr updateConnection; // Pointer to the update event connection
    std::unique_ptr<ros::NodeHandle> rosNode; // a node used for ROS transport
    ros::Subscriber rosSub; // a ROS subscriber
    ros::CallbackQueue rosQueue; // A ROS callbackqueue that helps process messages
    std::thread rosQueueThread; // A thread the keeps running the rosQueue
    
        // Pointers to joints
    physics::JointPtr fl_wheel;
    physics::JointPtr fr_wheel;
    physics::JointPtr fr2_wheel;
    physics::JointPtr fl2_wheel;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(BasePlugin)
}
