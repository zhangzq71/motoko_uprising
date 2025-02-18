#include "dataset_line.h"

#include <math.h>
#include <image_save.h>

DatasetLine::DatasetLine(unsigned int sensors_count, unsigned int time_steps)
{
    unsigned int width  = sensors_count;
    unsigned int height = time_steps;
    classes_count = 5;

    input_shape.set(width, height, 1);

    output_shape.set(1, 1, classes_count);

    scale = 4;

    area_width   = 74*scale;
    area_length  = 60*scale;
    line_width   = 15*scale;

    luma_noise_level  = 1.0;
    white_noise_level = 0.2;

    straight_rotation_noise_level = 0.6;

    area.resize(area_length);
    for (unsigned int j = 0; j < area.size(); j++)
        area[j].resize(area_width);

    for (unsigned int j = 0; j < area_length; j++)
        for (unsigned int i = 0; i < area_width; i++)
            area[j][i] = 0.0;

    area_downsampled.resize(height);
    for (unsigned int j = 0; j < area_downsampled.size(); j++)
        area_downsampled[j].resize(width);

    for (unsigned int j = 0; j < height; j++)
        for (unsigned int i = 0; i < width; i++)
            area_downsampled[j][i] = 0.0;

    unsigned int training_count = classes_count*10000;
    unsigned int testing_count  = classes_count*1000;

    training_input.resize(training_count);
    training_output.resize(training_count);

    testing_input.resize(testing_count);
    testing_output.resize(testing_count);


    for (unsigned int i = 0; i < training_count; i++)
    {
        auto item = create_item();

        training_input[i] = item.input;
        training_output[i] = item.output;
    }

    for (unsigned int i = 0; i < testing_count; i++)
    {
        auto item = create_item();

        testing_input[i] = item.input;
        testing_output[i] = item.output;
    }

    export_dataset_image(20, "dataset_examples/examples.png");
    print();
}

DatasetLine::~DatasetLine()
{

}


sLineDatasetItem DatasetLine::create_item()
{
    sLineDatasetItem result;
    unsigned int line_position = rand()%area_width;

    unsigned int line_pos = convert_raw_line_position(line_position);
    float rotation = rnd(-straight_rotation_noise_level, straight_rotation_noise_level);

    area_shifted_line(line_position, rotation);

    downsample(area_downsampled, area);
    add_noise(area_downsampled);


    result.input.resize(input_shape.size());
    result.output.resize(output_shape.size());

    unsigned int ptr = 0;
    for (unsigned int j = 0; j < input_shape.h(); j++)
        for (unsigned int i = 0; i < input_shape.w(); i++)
        {
            result.input[ptr] = area_downsampled[j][i];
            ptr++;
        }

    for (unsigned int j = 0; j < classes_count; j++)
        result.output[j] = 0.0;
    result.output[line_pos] = 1.0;

    return result;
}


unsigned int DatasetLine::area_shifted_line(unsigned int line_position, float rotation)
{
  unsigned int lw_half = line_width/2;

  if (line_position < lw_half)
    line_position = lw_half;

  if ((line_position+lw_half) >= area_width)
    line_position = area_width-lw_half;

  for (unsigned int y = 0; y < area_length; y++)
    for (unsigned int x = 0; x < area_width; x++)
      area[y][x] = 0.0;

  float a = 1.0;
  float b = 0.0;

  b = rotation;

  float c_left  = line_position - lw_half;
  float c_right = line_position + lw_half;

    for (unsigned int y = 0; y < area_length; y++)
      for (unsigned int x = 0; x < area_width; x++)
      {
        if ( ((a*x + b*y - c_left) > 0.0) && ((a*x + b*y - c_right) < 0.0) )
          area[y][x] = 1.0;
      }

/*
  for (unsigned int j = 0; j < area_length; j++)
    for (unsigned int i = (line_position-lw_half); i < (line_position+lw_half); i++)
      area[j][i] = 1.0;
*/
  return line_position;
}

unsigned int DatasetLine::convert_raw_line_position(unsigned int line_position)
{
  return (classes_count*line_position)/area_width;
}

void DatasetLine::downsample(std::vector<std::vector<float>> &area_output, std::vector<std::vector<float>> &area_input)
{
  unsigned int kw = area_input[0].size()/area_output[0].size();
  unsigned int kh = area_input.size()/area_output.size();


  for (unsigned int y = 0; y < area_output.size(); y++)
  for (unsigned int x = 0; x < area_output[y].size(); x++)
  {
    float sum = 0.0;
    for (unsigned int j = 0; j < kh; j++)
    for (unsigned int i = 0; i < kw; i++)
      sum+= area_input[y*kh + j][x*kw + i];

    area_output[y][x] = sum/(kw*kh);
  }
}

void DatasetLine::add_noise(std::vector<std::vector<float>> &area)
{
  float luma_noise = luma_noise_level*rnd(0.0, 1.0);


  for (unsigned int y = 0; y < area.size(); y++)
  for (unsigned int x = 0; x < area[y].size(); x++)
  {
    area[y][x] = luma_noise + (1.0 - white_noise_level)*area[y][x] + white_noise_level*(rand()%10000)/10000.0;
  }
}

void DatasetLine::save_image(std::string file_name)
{
  std::vector<float> v(area_length*area_width);

  unsigned int ptr = 0;
  for (unsigned int j = 0; j < area_length; j++)
    for (unsigned int i = 0; i < area_width; i++)
    {
      v[ptr] = area[j][i];
      ptr++;
    }

  ImageSave image(area_width, area_length, true);
  image.save(file_name, v);
}

void DatasetLine::save_image_downsampled(std::string file_name)
{
  std::vector<float> v(input_shape.w()*input_shape.h());

  unsigned int ptr = 0;
  for (unsigned int j = 0; j < input_shape.h(); j++)
    for (unsigned int i = 0; i < input_shape.w(); i++)
    {
      v[ptr] = area_downsampled[j][i];
      ptr++;
    }

  ImageSave image(input_shape.h(), input_shape.w(), true);
  image.save(file_name, v);
}


float DatasetLine::rnd(float min, float max)
{
  float v = (rand()%100000)/100000.0;

  return (max - min)*v + min;
}


void DatasetLine::export_dataset_image(unsigned int size, std::string file_name)
{
  unsigned int spacing = 2;
  unsigned int output_image_height = size*(input_shape.h()+spacing);
  unsigned int output_image_width  = size*(input_shape.w()+spacing);
  unsigned int slice_size = output_image_height*output_image_width;

  ImageSave image(output_image_height, output_image_width, false);
  std::vector<float> v(3*output_image_height*output_image_width);

  for (unsigned int i = 0; i < v.size(); i++)
    v[i] = 1.0;


  for (unsigned int y = 0; y < size; y++)
  for (unsigned int x = 0; x < size; x++)
  {
    unsigned int line_position = rand()%area_width;

    unsigned int line_pos = convert_raw_line_position(line_position);
    float rotation = rnd(-straight_rotation_noise_level, straight_rotation_noise_level);

    area_shifted_line(line_position, rotation);

    downsample(area_downsampled, area);

    add_noise(area_downsampled);


    for (unsigned int j = 0; j < input_shape.h(); j++)
      for (unsigned int i = 0; i < input_shape.w(); i++)
        {
            unsigned int output_idx;
            //output_idx = (j + y*size)*output_image_width + i + x*size;
            output_idx = j*output_image_width + i + (y*(input_shape.h() + spacing)*size + x)*(input_shape.w() + spacing) + (spacing/2)*(1 + output_image_width);

            float value = area_downsampled[j][i];

            float r = 0.0;
            float g = value;
            float b = 0.0;
            v[output_idx + slice_size*0] = r;
            v[output_idx + slice_size*1] = g;
            v[output_idx + slice_size*2] = b;
        }
  }

  image.save(file_name, v);
}
