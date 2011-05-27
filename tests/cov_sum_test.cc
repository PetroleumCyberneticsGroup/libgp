//
// libgp - Gaussian Process library for Machine Learning
// Copyright (C) 2010 Universität Freiburg
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "cov_factory.h"

#include <Eigen/Dense>
#include <gtest/gtest.h>

TEST(CovSumTest, get) {

	libgp::CovFactory factory;
	libgp::CovarianceFunction * covf;
  const int input_dim = 5;
  const int param_dim = 7;
  Eigen::VectorXd p(param_dim);
  Eigen::VectorXd a(input_dim);
  Eigen::VectorXd b(input_dim);
  Eigen::VectorXd g(param_dim);
  p << 0.8235,0.6948,0.3171,0.9502,0.0344,1.1000,0.2000;
  a << 0.9143,-0.0292,0.6006,-0.7162,-0.1565;
  b << 0.8315,0.5844,0.9190,0.3115,-0.9286;

	covf = factory.create(input_dim, "CovSum(CovSEard, CovNoise)");

  ASSERT_EQ(input_dim, covf->get_input_dim());
  ASSERT_EQ(param_dim, covf->get_param_dim());
  ASSERT_TRUE(covf->set_loghyper(p));
  ASSERT_NEAR(5.8612, covf->get(a, b), 0.0001);
  ASSERT_NEAR(5.8612, covf->get(b, a), 0.0001);
  ASSERT_NEAR(10.5168, covf->get(a, a), 0.0001);
  ASSERT_NEAR(10.5168, covf->get(b, b), 0.0001);

  covf->grad(a, a, g);
  for(size_t i = 0; i < 5; ++i) ASSERT_NEAR(0.0, g(i), 0.0001);
  ASSERT_NEAR(18.0500, g(5), 0.0001);
  ASSERT_NEAR(2.9836, g(6), 0.0001);
  covf->grad(a, b, g);
  ASSERT_NEAR(0.0078, g(0), 0.0001);
  ASSERT_NEAR(0.5499, g(1), 0.0001);
  ASSERT_NEAR(0.3152, g(2), 0.0001);
  ASSERT_NEAR(0.9255, g(3), 0.0001);
  ASSERT_NEAR(3.2617, g(4), 0.0001);
  ASSERT_NEAR(11.7222, g(5), 0.0001);
  ASSERT_NEAR(0, g(6), 0.0001);
  
}