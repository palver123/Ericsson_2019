#cmake_minimum_required (VERSION 3.6)
set (CMAKE_CXX_STANDARD 17)
project (Wololo)
add_executable(wololo main.cpp strategy.h turnData.h turnData.cpp strategy.cpp TestingStrategy.cpp TestingStrategy.h basics.h Reader.h Reader.cpp simulation.h simulation.cpp commands.h directions.h graph.h graph.cpp ProbabilityScoreStrategy.h ProbabilityScoreStrategy.cpp Scores.h Scores.cpp)
