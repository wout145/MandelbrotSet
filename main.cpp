#include <stdio.h>
#include <math.h>
#include <vector>
#include <raylib.h>
#include <tuple>
#include <array>

constexpr int SCREEN_HEIGHT = 600;
constexpr int SCREEN_WIDTH = 800;

using Grid = std::array<std::tuple<double, double>, SCREEN_HEIGHT * SCREEN_WIDTH>;

struct Calculations {

  std::vector<double> calculateNextIteration(double constantZ, double multiplierIZ, double constantC, double multiplierIC) {
  
    std::vector<double> squareResult = {constantZ*constantZ + multiplierIZ*multiplierIZ, 2*constantZ*multiplierIZ};
    std::vector<double> result = {squareResult[0] + constantC, squareResult[1] + multiplierIC};
  
    return result;
  
  }

  std::tuple<bool, int> calculateInclusion(double constantZ, double multiplierIZ, double constantC, double multiplierIC, int iteration) {

    if (iteration > 20) {
      return std::make_tuple(true, 21);
    }

    std::vector<double> result = calculateNextIteration(constantZ, multiplierIZ, constantC, multiplierIC);

    if (result[0] > 2 || result[0] < -2 || result[1] > 2 || result[1] < -2) {
      return std::make_tuple(false, iteration);
    }

    std::tuple<bool, int> feedback = calculateInclusion(result[0], result[1], constantC, multiplierIC, iteration + 1);

    if (std::get<0>(feedback) == true) {
      return std::make_tuple(true, std::get<1>(feedback));
    }

    return std::make_tuple(false, 0);

  }
};

void initializeStartingGrid(Grid &startingGrid) {

  double row = SCREEN_HEIGHT;
  for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
    double column = i%SCREEN_WIDTH;
    if (i!= 0 && i%SCREEN_WIDTH == 0) {
      row = row - 1;
    }
    // Normalize to 2x2 grid
    double normalizedRow = row / SCREEN_HEIGHT * 4 - 2;
    double normalizedColumn = column / SCREEN_WIDTH * 4 - 2;
    startingGrid[i] = std::make_tuple(normalizedRow, normalizedColumn);
  }
}

  

int main() {

  Grid startingGrid;

  initializeStartingGrid(startingGrid);

  Calculations calculations;

  double constantC = 0.5;
  double multiplierIC = 1;

  std::vector<double> result = calculations.calculateNextIteration(0.5, 1.0, constantC, multiplierIC);
  std::vector<double> result2 = calculations.calculateNextIteration(result[0], result[1], constantC, multiplierIC);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandelbrot Visualization");

  SetTargetFPS(60); 
  
  bool windowShouldClose = false;

  //for (auto it = currentGrid.begin(); it != currentGrid.end(); ++it) {
  //  printf("%f\n", std::get<0>(*it));
  //}

  while (!windowShouldClose) {

    if (IsKeyDown(KEY_ESCAPE)) {
      windowShouldClose = true;
    };

    BeginDrawing();

    ClearBackground(WHITE);

    for (auto it = startingGrid.begin(); it != startingGrid.end(); ++it) {
      double beginningConstantC = std::get<0>(*it);
      double beginningMultiplierIC = std::get<1>(*it);
      if (std::get<0>(calculations.calculateInclusion(0.0, 0.0, beginningConstantC, beginningMultiplierIC, 0)) == true) {
	double pixelX = ((beginningConstantC + 2.0) / 4) * 8;
        double pixelY = ((beginningMultiplierIC + 2.0) / 4) * 8;
	DrawPixel(pixelX, pixelY, BLACK);
      }
    }
    
    EndDrawing();
    
  }

  CloseWindow();
  
  return 0;
} 
