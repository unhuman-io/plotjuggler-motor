#pragma once

#include <QtPlugin>
#include <thread>
#include "PlotJuggler/datastreamer_base.h"
#include "motor_manager.h"

class PlotJugglerMotor : public PJ::DataStreamer
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "unhuman-io.plotjuggler-motor-plugin")
  Q_INTERFACES(PJ::DataStreamer)

public:
  PlotJugglerMotor();

  virtual bool start(QStringList*) override;

  virtual void shutdown() override;

  virtual bool isRunning() const override;

  virtual ~PlotJugglerMotor() override;

  virtual const char* name() const override
  {
    return "Motor Plot";
  }

  virtual bool isDebugPlugin() override
  {
    return false;
  }

  virtual bool xmlSaveState(QDomDocument& doc,
                            QDomElement& parent_element) const override;

  virtual bool xmlLoadState(const QDomElement& parent_element) override;



private:
  void loop();

  std::thread _thread;

  bool _running;

  void pushSingleCycle();


  MotorManager m_;
  std::vector<std::shared_ptr<Motor>> motors_;
  std::vector<Status> last_statuses_;
  std::vector<uint32_t> cpu_frequency_hz_;
  std::vector<double> t_seconds_;
};
