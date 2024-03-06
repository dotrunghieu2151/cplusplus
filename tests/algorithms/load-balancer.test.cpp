#include <gtest/gtest.h>
#include <iostream>
#include <load-balancer.hpp>

TEST(RoundRobin, GetServer) { auto lb{load_balancer::RoundRobinLB()}; }