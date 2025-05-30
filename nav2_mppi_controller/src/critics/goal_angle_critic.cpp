// Copyright (c) 2022 Samsung Research America, @artofnothingness Alexey Budyakov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "nav2_mppi_controller/critics/goal_angle_critic.hpp"

namespace mppi::critics
{

void GoalAngleCritic::initialize()
{
  auto getParentParam = parameters_handler_->getParamGetter(parent_name_);
  getParentParam(enforce_path_inversion_, "enforce_path_inversion", false);

  auto getParam = parameters_handler_->getParamGetter(name_);
  getParam(power_, "cost_power", 1);
  getParam(weight_, "cost_weight", 3.0f);
  getParam(threshold_to_consider_, "threshold_to_consider", 0.5f);

  RCLCPP_INFO(
    logger_,
    "GoalAngleCritic instantiated with %d power, %f weight, and %f "
    "angular threshold.",
    power_, weight_, threshold_to_consider_);
}

void GoalAngleCritic::score(CriticData & data)
{
  if (!enabled_) {
    return;
  }

  geometry_msgs::msg::Pose goal = utils::getCriticGoal(data, enforce_path_inversion_);

  if (!utils::withinPositionGoalTolerance(
      threshold_to_consider_, data.state.pose.pose, goal))
  {
    return;
  }

  double goal_yaw = tf2::getYaw(goal.orientation);

  if(power_ > 1u) {
    data.costs += (((utils::shortest_angular_distance(data.trajectories.yaws, goal_yaw).abs()).
      rowwise().mean()) * weight_).pow(power_).eval();
  } else {
    data.costs += (((utils::shortest_angular_distance(data.trajectories.yaws, goal_yaw).abs()).
      rowwise().mean()) * weight_).eval();
  }
}

}  // namespace mppi::critics

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(
  mppi::critics::GoalAngleCritic,
  mppi::critics::CriticFunction)
