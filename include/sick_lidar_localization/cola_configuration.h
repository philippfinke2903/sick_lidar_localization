/*
 * @brief cola_configuration sets the initial SIM result output configuration
 * using ros cola services.
 * Configures the following result output settings from launch file:
 * LocSetResultPort, LocSetResultMode, LocSetResultPoseEnabled, LocSetResultEndianness, LocSetResultPoseInterval,
 * LocRequestResultData
 *
 * Copyright (C) 2019 Ing.-Buero Dr. Michael Lehning, Hildesheim
 * Copyright (C) 2019 SICK AG, Waldkirch
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of SICK AG nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission
 *     * Neither the name of Ing.-Buero Dr. Michael Lehning nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *      Authors:
 *         Michael Lehning <michael.lehning@lehning.de>
 *
 *  Copyright 2019 SICK AG
 *  Copyright 2019 Ing.-Buero Dr. Michael Lehning
 *
 */
#ifndef __SIM_LOC_COLA_CONFIGURATION_H_INCLUDED
#define __SIM_LOC_COLA_CONFIGURATION_H_INCLUDED

#include "sick_lidar_localization/cola_services.h"

namespace sick_lidar_localization
{
  /*!
   * Class sick_lidar_localization::ColaConfiguration sets the initial SIM result output configuration
   * using ros cola services.
   * Configures the following result output settings from launch file:
   * LocSetResultPort, LocSetResultMode, LocSetResultPoseEnabled, LocSetResultEndianness, LocSetResultPoseInterval,
   * LocRequestResultData
   */
  class ColaConfiguration
  {
  public:
    
    /*!
     * Constructor
     */
    ColaConfiguration(ros::NodeHandle* nh = 0);
    
    /*!
     * Destructor
     */
    virtual ~ColaConfiguration();
  
    /*!
     * Starts transmitting the initial result output configuration to the localization controller.
     * Configures the following result output settings from launch file:
     * LocSetResultPort, LocSetResultMode, LocSetResultPoseEnabled, LocSetResultEndianness, LocSetResultPoseInterval,
     * LocRequestResultData
     * @return true on success, false in case of errors.
     */
    virtual bool start(void);
  
    /*!
     * Stops transmitting the initial result output configuration to the localization controller.
     */
    virtual void stop(void);

  protected:
  
    /*!
     * Thread callback, transmits the initial SIM result output configuration using ros cola services.
     * Configures the following result output settings from launch file:
     * LocSetResultPort, LocSetResultMode, LocSetResultPoseEnabled, LocSetResultEndianness, LocSetResultPoseInterval,
     * LocRequestResultData
     */
    virtual void runConfigurationThreadCb(void);
  
    /*!
     * Calls a ros service until successfully executed or configuration thread cancelled.
     * @param[in/out] service_telegram service request and response
     * @param[in] service_client ros service client
     * @param[in] retry_delay delay before next retry in case of errors
     * @return true on success or false on failure
     */
    template<typename T> bool callService(T & service_telegram, ros::ServiceClient & service_client, double retry_delay)
    {
      service_telegram.response.success = false;
      while(ros::ok() && m_nh && m_configuration_thread_running && (!service_client.call(service_telegram) || !service_telegram.response.success))
      {
        ROS_WARN_STREAM("## ERROR ColaConfiguration: service call(" << sick_lidar_localization::Utils::flattenToString(service_telegram.request) << ") failed, response: " << sick_lidar_localization::Utils::flattenToString(service_telegram.response) << ", retrying");
        ros::Duration(retry_delay).sleep();
      }
      ROS_INFO_STREAM("ColaConfiguration " << typeid(T).name() << " " << (service_telegram.response.success ? "successfull" : "failed"));
      return service_telegram.response.success;
    }
    
    /*
     * member variables
     */

    ros::NodeHandle* m_nh;                  ///< ros node handle
    boost::thread* m_configuration_thread;  ///< thread to transmit configuration
    bool m_configuration_thread_running;    ///< true: m_configuration_thread is running, otherwise false
    
  }; // class ColaServices
  
} // namespace sick_lidar_localization
#endif // __SIM_LOC_COLA_CONFIGURATION_H_INCLUDED
