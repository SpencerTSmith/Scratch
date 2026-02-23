#define COMMON_IMPLEMENTATION
#include "../common.h"

#include <float.h>
#include <time.h>

// We won't always use all 3 elements, as in the case of 2d data.
#define MAX_INPUT_COUNT 3
typedef struct Point Point;
struct Point
{
  f32 d[MAX_INPUT_COUNT];
};

typedef struct Point_Array Point_Array;
struct Point_Array
{
  Point *v;
  usize count;
};

#define CLASS_COUNT 2
typedef struct Labeled_Data Labeled_Data;
struct Labeled_Data
{
  Point_Array classes[CLASS_COUNT];
  usize input_count;
};

#define HIDDEN_COUNT 32
typedef struct Network Network;
struct Network
{
  usize input_count;

  f32 hidden_weights[HIDDEN_COUNT][MAX_INPUT_COUNT];
  f32 output_weights[HIDDEN_COUNT];
  f32 bias0[HIDDEN_COUNT];
  f32 bias1;

  f32 hidden_output[HIDDEN_COUNT];

  f32 output;
};

static
Labeled_Data load_csv(Arena *arena, String filename)
{
  // 3 By default
  usize input_count = 3;
  if (string_contains_substring(filename, STR("2D")))
  {
    input_count = 2;
  }
  // Assignment says max 500, so we'll go with that.
  Labeled_Data result =
  {
    .classes[0] =
    {
      .v = arena_calloc(arena, 500, Point),
      .count = 0,
    },
    .classes[1] =
    {
      .v = arena_calloc(arena, 500, Point),
      .count = 0,
    },
    .input_count = input_count,
  };
  String csv = read_file_to_arena(arena, filename);

  usize at = 0;

  Point curr_point = {0};
  usize element_idx = 0;

  usize class_idx = 0;

  usize value_start = at;
  while (at < csv.count && value_start < csv.count)
  {
    if (csv.v[at] == ',' || csv.v[at] == '\n')
    {
      String value =
      {
        .v = csv.v + value_start,
        .count = at - value_start,
      };

      f64 number = string_to_f64(value);

      curr_point.d[element_idx] = number;

      // We can now add this point.
      if (element_idx == (result.input_count - 1))
      {
        Point_Array *class = &result.classes[class_idx % STATIC_COUNT(result.classes)];
        ASSERT(class->count < 500, "Too many points.");

        class->v[class->count] = curr_point;

        class->count += 1;
        class_idx += 1;

        element_idx = 0;
        curr_point = (Point){0};
      }
      else
      {
        element_idx += 1;
      }

      value_start = at + 1;
    }

    at += 1;
  }

  return result;
}

Network init_network(i32 seed, usize input_count)
{
  // Biases will just be 0.
  Network result =
  {
    .input_count = input_count
  };

  // Seed each time we init.
  srand(seed);

  for (usize hidden_idx = 0; hidden_idx < STATIC_COUNT(result.hidden_weights); hidden_idx++)
  {
    for (usize input_idx = 0; input_idx < input_count; input_idx++)
    {
      // Float between 0 and 1
      f32 random = (rand() / (f32)RAND_MAX) * 2 - 1;
      result.hidden_weights[hidden_idx][input_idx] = random;
    }
  }

  for (usize hidden_idx = 0; hidden_idx < STATIC_COUNT(result.output_weights); hidden_idx++)
  {
    f32 random = (rand() / (f32)RAND_MAX) * 2 - 1;
    result.output_weights[hidden_idx] = random;
  }

  return result;
}

f32 sigmoid(f32 x)
{
  f32 denominator = 1 + expf(-x);
  return 1 / denominator;
}

f32 forward_pass(Network *network, Point input)
{
  for (usize hidden_idx = 0; hidden_idx < STATIC_COUNT(network->hidden_weights); hidden_idx++)
  {
    f32 sum = network->bias0[hidden_idx];
    for (usize input_idx = 0; input_idx < network->input_count; input_idx++)
    {
      sum += network->hidden_weights[hidden_idx][input_idx] * input.d[input_idx];
    }

    network->hidden_output[hidden_idx] = sigmoid(sum);
  }

  float sum = network->bias1;
  for (usize hidden_idx = 0; hidden_idx < STATIC_COUNT(network->output_weights); hidden_idx++)
  {
    sum += network->output_weights[hidden_idx] * network->hidden_output[hidden_idx];
  }

  return sigmoid(sum);
}

// Now chain rule, going backwards.
void backpropagate(Network *network, Point input, f32 output, f32 target, f32 rate)
{
  // derivative of sigmoid = sigmoid * (1 - sigmoid)
  f32 output_delta = (target - output) * output * (1 - output);;

  f32 hidden_delta[HIDDEN_COUNT] = {0};
  for (usize hidden_idx = 0; hidden_idx < HIDDEN_COUNT; hidden_idx++)
  {
    hidden_delta[hidden_idx] = network->output_weights[hidden_idx] * output_delta * network->hidden_output[hidden_idx] * (1 - network->hidden_output[hidden_idx]);
  }

  // Update everything.
  for (usize hidden_idx = 0; hidden_idx < HIDDEN_COUNT; hidden_idx++)
  {
    network->output_weights[hidden_idx] += rate * output_delta * network->hidden_output[hidden_idx];
    network->bias0[hidden_idx] += rate * hidden_delta[hidden_idx];
    for (usize input_idx = 0; input_idx < network->input_count; input_idx++)
    {
      network->hidden_weights[hidden_idx][input_idx] += rate * hidden_delta[hidden_idx] * input.d[input_idx];
    }
  }
  network->bias1 += rate * output_delta;
}

#define RUN_COUNT      20
#define EPOCH_COUNT    10000
#define LEARNING_RATE  0.1f
#define NO_IMPROVEMENT 500
int main(int argc, char** argv)
{
  if (argc == 2)
  {
    String filename = string_from_c_string(argv[1]);

    Arena arena = arena_make();
    Labeled_Data data = load_csv(&arena, filename);

    FILE *loss_csv = fopen("loss.csv", "w");
    fprintf(loss_csv, "epoch,loss\n");

    usize training_count = data.classes[0].count * 0.6;
    usize validate_count = data.classes[0].count * 0.2;
    usize test_count     = data.classes[0].count * 0.2;

    // First 2 for the classes, last for the overall.
    f32 *accuracies[3] =
    {
      arena_calloc(&arena, RUN_COUNT, f32),
      arena_calloc(&arena, RUN_COUNT, f32),
      arena_calloc(&arena, RUN_COUNT, f32),
    };

    srand(time(0));
    usize run_loss_report_idx = rand() % RUN_COUNT;

    for (usize run_idx = 0; run_idx < RUN_COUNT; run_idx++)
    {
      Network network = init_network(run_idx, data.input_count);


      f32 best_validate_loss = FLT_MAX;
      usize no_improvement_count = 0;

      for (usize epoch = 0; epoch < EPOCH_COUNT; epoch++)
      {

        // Training.
        for (usize point_idx = 0; point_idx < training_count; point_idx++)
        {
          // Interleave classes, seems like a good idea.
          for (usize class_idx = 0; class_idx < STATIC_COUNT(data.classes); class_idx++)
          {
            Point point = data.classes[class_idx].v[point_idx];
            f32 output = forward_pass(&network, point);
            backpropagate(&network, point, output, (f32)class_idx, LEARNING_RATE);
          }
        }

        // Validate.
        f32 validate_loss = 0.0f;
        for (usize class_idx = 0; class_idx < STATIC_COUNT(data.classes); class_idx++)
        {
          for (usize point_idx = training_count; point_idx < training_count + validate_count; point_idx++)
          {
            Point point = data.classes[class_idx].v[point_idx];
            f32 output = forward_pass(&network, point);
            f32 error = ((f32)class_idx - output);
            validate_loss += error * error; // Square error for loss.
          }
        }
        validate_loss /= (validate_count * CLASS_COUNT);

        if (validate_loss < best_validate_loss)
        {
          best_validate_loss = validate_loss;
          no_improvement_count = 0;
        }
        else
        {
          no_improvement_count += 1;

          if (no_improvement_count > NO_IMPROVEMENT)
          {
            break;
          }
        }

        if (run_idx == run_loss_report_idx && epoch % 10 == 0)
        {
          fprintf(loss_csv, "%lu,%f\n", epoch, validate_loss);
        }
      }

      // Test and collect accuracies.
      f32 total_accuracy = 0.0;
      for (usize class_idx = 0; class_idx < STATIC_COUNT(data.classes); class_idx++)
      {
        usize correct = 0;
        for (usize point_idx = validate_count; point_idx < validate_count + test_count; point_idx++)
        {
          Point point = data.classes[class_idx].v[point_idx];
          f32 output = forward_pass(&network, point);
          if ((output < 0.5f && class_idx == 0) || (output >= 0.5f && class_idx == 1))
          {
            correct += 1;
          }
        }
        f32 accuracy = (f32)correct / test_count;
        accuracies[class_idx][run_idx] = accuracy;
        total_accuracy += accuracy;
      }
      total_accuracy /= CLASS_COUNT;
      accuracies[2][run_idx] = total_accuracy;
    }

    f32 mean0 = 0.0f; // Class 0
    f32 mean1 = 0.0f; // Class 1
    f32 meanA = 0.0f; // Overall
    for (usize run_idx = 0; run_idx < RUN_COUNT; run_idx++)
    {
      mean0 += accuracies[0][run_idx];
      mean1 += accuracies[1][run_idx];
      meanA += accuracies[2][run_idx];
    }
    mean0 /= RUN_COUNT;
    mean1 /= RUN_COUNT;
    meanA /= RUN_COUNT;

    printf("Mean Accuracy:\n");
    printf("  Class 0: %f\n", mean0);
    printf("  Class 1: %f\n", mean1);
    printf("  Overall: %f\n", meanA);

    f32 variance0 = 0.0f;
    f32 variance1 = 0.0f;
    f32 varianceA = 0.0f;
    for (int run_idx = 0; run_idx < RUN_COUNT; run_idx++)
    {
        f32 diff0 = accuracies[0][run_idx] - mean0;
        variance0 += diff0 * diff0;
        f32 diff1 = accuracies[1][run_idx] - mean1;
        variance1 += diff1 * diff1;
        f32 diffA = accuracies[2][run_idx] - meanA;
        varianceA += diffA * diffA;
    }
    variance0 /= RUN_COUNT;
    variance1 /= RUN_COUNT;
    varianceA /= RUN_COUNT;
    f32 std_dev0 = sqrtf(variance0);
    f32 std_dev1 = sqrtf(variance1);
    f32 std_devA = sqrtf(varianceA);

    printf("Std-Dev Accuracy:\n");
    printf("  Class 0: %f\n", std_dev0);
    printf("  Class 1: %f\n", std_dev1);
    printf("  Overall: %f\n", std_devA);

    fclose(loss_csv);
    arena_free(&arena);
  }
  else
  {
    printf("Usage: %s [data.csv]\n", argv[0]);
  }
}
