#include "plotjuggler_motor.h"
#include <mutex>
#include <chrono>

using namespace PJ;

PlotJugglerMotor::PlotJugglerMotor()
{
  motors_ = m_.get_connected_motors();

  for (auto motor : motors_) {
    std::make_shared<PJ::PlotGroup>(motor->name());
    dataMap().addNumeric(motor->name() + "/mcu_timestamp");
    dataMap().addNumeric(motor->name() + "/host_timestamp_received");
    dataMap().addNumeric(motor->name() + "/motor_position");
    dataMap().addNumeric(motor->name() + "/joint_position");
    dataMap().addNumeric(motor->name() + "/iq");
    dataMap().addNumeric(motor->name() + "/torque");
    dataMap().addNumeric(motor->name() + "/motor_encoder");
    cpu_frequency_hz_.push_back(170000000);
    std::string text_api_name = motor->name() + "/text_api";
    std::make_shared<PJ::PlotGroup>(text_api_name);
    std::string api_vars = motor->motor_text()->writeread("help");
    //std::cout << api_vars;
    std::istringstream ss(api_vars);
    std::string var;
    while(std::getline(ss, var, '\n')) {
      //std::cout << "api_var " << var << '\n';
      if(var != "shutdown" && var != "help") {
        auto& var_series = dataMap().addNumeric(text_api_name + "/" + var)->second;
        double d = 0;
        try {
          d = std::stod((*motor)[var].get());
        } catch(...) {}
        var_series.pushBack({0, d});
      }
    }
    t_seconds_.push_back(0);
  }

  last_statuses_ = m_.read();
}

bool PlotJugglerMotor::start(QStringList* s)
{
  _running = true;

  std::cout << "************" << s  << std::endl;
  pushSingleCycle();
  _thread = std::thread([this]() { this->loop(); });
  return true;
}

void PlotJugglerMotor::shutdown()
{
  _running = false;
  if (_thread.joinable())
  {
    _thread.join();
  }
}

bool PlotJugglerMotor::isRunning() const
{
  return _running;
}

PlotJugglerMotor::~PlotJugglerMotor()
{
  shutdown();
}

bool PlotJugglerMotor::xmlSaveState(QDomDocument& doc, QDomElement& parent_element) const
{
  return true;
}

bool PlotJugglerMotor::xmlLoadState(const QDomElement& parent_element)
{
  return true;
}

void PlotJugglerMotor::pushSingleCycle()
{
  static int count = 0;
  std::lock_guard<std::mutex> lock(mutex());

  std::vector<Status> statuses = m_.read();
  for(int i=0; i < statuses.size(); i++) {
    uint32_t dt = statuses[i].mcu_timestamp - last_statuses_[i].mcu_timestamp;
    t_seconds_[i] += (double) dt/cpu_frequency_hz_[i];
    dataMap().numeric.find(motors_[i]->name() + "/mcu_timestamp")->second.pushBack({t_seconds_[i], (double) statuses[i].mcu_timestamp});
    dataMap().numeric.find(motors_[i]->name() + "/host_timestamp_received")->second.pushBack({t_seconds_[i], (double) statuses[i].host_timestamp_received});
    dataMap().numeric.find(motors_[i]->name() + "/motor_position")->second.pushBack({t_seconds_[i], statuses[i].motor_position});
    dataMap().numeric.find(motors_[i]->name() + "/joint_position")->second.pushBack({t_seconds_[i], statuses[i].joint_position});
    dataMap().numeric.find(motors_[i]->name() + "/iq")->second.pushBack({t_seconds_[i], statuses[i].iq});
    dataMap().numeric.find(motors_[i]->name() + "/torque")->second.pushBack({t_seconds_[i], statuses[i].torque});
    dataMap().numeric.find(motors_[i]->name() + "/motor_encoder")->second.pushBack({t_seconds_[i], (double) statuses[i].motor_encoder});

    if(count % 300 == 0) {
      std::string api_vars = motors_[i]->motor_text()->writeread("help");
     // std::cout << api_vars;
      std::istringstream ss(api_vars);
      std::string var;
      while(std::getline(ss, var, '\n')) {
        if(var != "shutdown" && var != "help") {
          auto& var_series = dataMap().numeric.find(motors_[i]->name() + "/text_api/" + var)->second;
         //std::cout << var << std::endl;
          double d = 0;
          try {
            d = std::stod((*motors_[i])[var].get());
          } catch(...) {}
          var_series.pushBack({t_seconds_[i], d});
          if(var == "deadtime") {
            var_series.setAttribute(TEXT_COLOR, QColor(Qt::green));
          }
        }
      }
    }
    //std::cout << t_seconds_[i] << std::endl;
  }
  last_statuses_ = statuses;

  count++;
}

void PlotJugglerMotor::loop()
{
  _running = true;
  size_t count = 1;
  auto new_time = std::chrono::steady_clock::now();
  while (_running)
  {
    pushSingleCycle();
    emit dataReceived();
    new_time += std::chrono::milliseconds(1);
    std::this_thread::sleep_until(new_time);
  }
}
