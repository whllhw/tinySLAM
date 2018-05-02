/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "lds_polar_graph.h"
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

QT_CHARTS_USE_NAMESPACE

LdsPolarGraph::LdsPolarGraph(QWidget *parent) :
  QChartView(parent),
  angular_min_(0),
  angular_max_(360),
  radial_min_(0),
  radial_max(4000),
  chart_(new QPolarChart),
  series_(new QScatterSeries),
  baud_rate_(230400),
  port_("/dev/ttyUSB0"),
  shutting_down_(false)
//  ,serial_(io_, port_)
{
//  port_ = "/dev/ttyUSB0"; // input your portname, ex) Linux: "/dev/ttyUSB0", Windows: "COM1",  Mac: "/dev/tty.SLAB_USBtoUART" or "/dev/tty.usbserial-*"

//  serial_.set_option(boost::asio::serial_port_base::baud_rate(baud_rate_));
//  boost::asio::write(serial_, boost::asio::buffer("b", 1));

            this->usrt_fd = open("uart_data", O_RDONLY);
            if (usrt_fd <= 0)
                printf("can't open file !\n");

  connect(&timer_, SIGNAL(timeout()), this, SLOT(loadData()));
  timer_.start(0);  // msec

  series_->setPen(QColor(Qt::transparent));
  series_->setPen(QColor(255,0,0));
  series_->setMarkerSize(2);

  chart_->legend()->setVisible(false);
  chart_->addSeries(series_);

  QValueAxis *angularAxis = new QValueAxis();
  angularAxis->setTickCount(9);
  angularAxis->setLabelFormat("%.1f");
  angularAxis->setShadesVisible(true);
  angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
  chart_->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

  QValueAxis *radialAxis = new QValueAxis();
  radialAxis->setTickCount(9);
  radialAxis->setLabelFormat("%d");
  chart_->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

  series_->attachAxis(radialAxis);
  series_->attachAxis(angularAxis);

  radialAxis->setRange(radial_min_, radial_max);
  angularAxis->setRange(angular_min_, angular_max_);

  this->setChart(chart_);
  this->setRenderHint(QPainter::Antialiasing);
}

LdsPolarGraph::~LdsPolarGraph()
{
  // Stop motor of LDS
//  boost::asio::write(serial_, boost::asio::buffer("e", 1));
}

void LdsPolarGraph::loadData()
{
  uint16_t *laser_sensor_data;
  laser_sensor_data = poll();

  series_->clear();

  for (int i = angular_min_; i < angular_max_; i++)
  {
    series_->append(i, laser_sensor_data[i]);
  }

  this->setChart(chart_);
}

uint16_t* LdsPolarGraph::poll()
{
  bool      got_scan    = false;
//  int       index       = 0;
  uint8_t   start_count = 0;
  uint32_t  motor_speed = 0;
  uint16_t  rpms        = 0;
  static uint16_t range_data[360]     = {0, };
  static uint16_t intensity_data[360] = {0, };
//  boost::array<uint8_t, 2520> raw_bytes;
  bool flag = false;
  time_t t;
      uint16_t good = 0,ret,count = 0,index = 0, len = 0;
      static uint8_t raw_bytes[2520],last,temp;
      static bool last_read_a0;
      unsigned short good_sets=0;
//      memset(raw_bytes,0,sizeof(raw_bytes));
      while (len < 2520 && !shutting_down_)
      {
          if(last_read_a0){
              temp = 0xfa;
              printf("last read 0xfa!");
          }else{
            ret = read(usrt_fd, &temp, 1);
//              ret = UART0_Recv(usrt_fd,&temp,1);
  //	        printf("%02x ",temp);
              if (ret <= 0) {
                  printf("read error:%d \n", ret);
                  break;
              }
          }
          if (temp == 0xfa)
          {
              flag = false;
              if(last_read_a0){
                  temp = 0xa0;
                  printf("last read 0xa0!");
              }else{
                ret = read(usrt_fd, &temp, 1);
//                  ret = UART0_Recv(usrt_fd,&temp,1);
  //			    printf("%02x ",temp);
                  if (ret <= 0)
                  {
                      printf("read error:%d \n", ret);
                      break;
                  }
              }
              if (temp == 0xa0)
              {
                  last_read_a0 = false;
                  if (index > 2518 || len > 0){
                      last_read_a0 = true;
                      break;
                  }
                  raw_bytes[index] = 0xfa;
                  raw_bytes[index + 1] = 0xa0;
                  len = index + 2;
                  index += 42;
                  last = 0xa0;
                  good = 0;
                  count++;
                  good++;
              }
              else if (temp < 0xdc && 0xa0 < temp)
              {
                  if (index > 2518)
                      break;
                  raw_bytes[index] = 0xfa;
                  raw_bytes[index + 1] = temp;
                  len = index + 2;
                  if (temp > last)
                  {
                      index += 42 * (temp - last);
                      last = temp;
                      good++;
                  }
                  else
                  {
                      flag = true;
                  }
              }
          }
          else if (!flag)
          {
              raw_bytes[len++] = temp;
          }
      }
      for (uint16_t i = 0; i < len; i = i + 42)
      {
          //			printf("%02x,%02x\n", raw_bytes[i], raw_bytes[i + 1]);
          if (raw_bytes[i] == 0xFA && raw_bytes[i + 1] == (0xA0 + i / 42)) //&& CRC check
          {
              good_sets++;
              motor_speed += (raw_bytes[i + 3] << 8) + raw_bytes[i + 2]; //accumulate count for avg. time increment
              rpms = (raw_bytes[i + 3] << 8 | raw_bytes[i + 2]) / 10;
              for (uint16_t j = i + 4; j < i + 40; j = j + 6)
              {
                  index = 6 * (i / 42) + (j - 4 - i) / 6;

                  // Four bytes per reading
                  uint8_t byte0 = raw_bytes[j];
                  uint8_t byte1 = raw_bytes[j + 1];
                  uint8_t byte2 = raw_bytes[j + 2];
                  uint8_t byte3 = raw_bytes[j + 3];

                  // Remaining bits are the range in mm
                  uint16_t intensity = (byte1 << 8) + byte0;

                  // Last two bytes represent the uncertanty or intensity, might also be pixel area of target...
                  // uint16_t intensity = (byte3 << 8) + byte2;
                  uint16_t range = (byte3 << 8) + byte2;
                  range_data[359 - index] = range * 10;
                  intensity_data[359 - index] = intensity;
  //				printf("r[%d]=%f,", 359 - index, range / 1000.0);
  //				printf("%f ",range/1000.0);
              }
          }
      }
      if(good_sets == 0)
          printf("x / 0 error!! in READ_UART.cpp good_set =  %d", good_sets);

//          scan->time_increment = motor_speed / good_sets / 1e8;

      time(&t);
      printf("\ncount:%d,good:%d good_set:%d time:%ld\n---------------------------------------------\n",count,good,good_sets,t);
  return range_data;
}
