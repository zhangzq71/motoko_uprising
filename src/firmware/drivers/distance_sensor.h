#ifndef _DISTANCE_SENSOR_H_
#define _DISTANCE_SENSOR_H_

#include <gpio.h>
#include <thread.h>
#include <timer.h>
#include <embedded_libs/math/filter.h>

#define DISTANCE_SENSOR_COUNT   ((unsigned int)3)

#define DISTANCE_FRONT          ((unsigned int)0)
#define DISTANCE_LEFT           ((unsigned int)1)
#define DISTANCE_RIGHT          ((unsigned int)2)

#define DISTANCE_MAX            ((float)1.0)

struct sDistanceSensor
{
    float left, front, right;
    bool front_obstacle_warning, front_obstacle;
};

class DistanceSensor: public Thread
{
  public:
      sDistanceSensor result;

  private:
      bool m_ready;

  protected:
      Gpio<TGPIOC, 13, GPIO_MODE_OUT> front_ir_led;        //front IR led

      Gpio<TGPIOB, 0, GPIO_MODE_AN> sensor_in_0; //front as ADC input
      Gpio<TGPIOB, 1, GPIO_MODE_AN> sensor_in_1; //left as ADC input
      Gpio<TGPIOC, 0, GPIO_MODE_AN> sensor_in_2; //right as ADC input

      unsigned int state;
      int adc_res_on[DISTANCE_SENSOR_COUNT];
      int adc_res_off[DISTANCE_SENSOR_COUNT];

  public:
      DistanceSensor();
      virtual ~DistanceSensor();

      int init();
      void main();

      bool ready();

  private:
      void filter(int *res_prev, unsigned int sensor_id);

  private:

     Filter front_distance_filter, left_distance_filter, right_distance_filter;
     //CombFilter<float, 3> front_distance_filter, left_distance_filter, right_distance_filter;
};


#endif
