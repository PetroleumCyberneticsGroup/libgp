/**************************************************************
 libgp - Gaussian Process library for Machine Learning
 Copyright (C) 2011 Universität Freiburg
 Author: Manuel Blum
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 ***************************************************************/

#include "gp_sparse.h"
#include <sys/time.h>

namespace libgp {
  
  SparseGaussianProcess::SparseGaussianProcess (size_t input_dim, std::string covf_def) : GaussianProcess(input_dim, covf_def)
  {
  }
  
  SparseGaussianProcess::SparseGaussianProcess (const char * filename) : GaussianProcess(filename)
  {
  }
  
  SparseGaussianProcess::~SparseGaussianProcess ()
  {
  }  
  
  void SparseGaussianProcess::compute()
  {    
    timeval start, end;

    if (covf->get_threshold() == INFINITY) {
      std::cerr << "Warning: no threshold defined, computation will be slow." << std::endl
        << "Use full GP or covariance functions that support thresholding." << std::endl;
    }
    if (sampleset->empty()) return; 
    Eigen::SparseMatrix<double> K(sampleset->size(), sampleset->size());
    //Eigen::DynamicSparseMatrix<double> aux(sampleset->size(), sampleset->size());
    
    alpha.resize(sampleset->size());
    // compute kernel matrix (lower triangle)
    for(size_t i = 0; i < sampleset->size(); ++i) {
      K.startVec(i);
      for(size_t j = i; j < sampleset->size(); ++j) {
        if ((sampleset->x(i)-sampleset->x(j)).norm() <= covf->get_threshold()) {
          K.insertBack(j,i) = covf->get(sampleset->x(i), sampleset->x(j));
        }          
      }
      alpha(i) = sampleset->y(i);
    }
    K.finalize();
    std::cout << 1.0 * K.nonZeros() / (sampleset->size()*sampleset->size()) << std::endl;
    // perform cholesky factorization
    gettimeofday(&start, 0);
    solver.compute(K);
    gettimeofday(&end, 0);
  	std::cout << "time1: " << end.tv_sec - start.tv_sec - ((end.tv_usec - start.tv_usec)<0) 
    << '.' << abs(end.tv_usec - start.tv_usec) << "s" << std::endl;
    gettimeofday(&start, 0);
    solver.solveInPlace(alpha);
    gettimeofday(&end, 0);
  	std::cout << "time2: " << end.tv_sec - start.tv_sec - ((end.tv_usec - start.tv_usec)<0) 
    << '.' << abs(end.tv_usec - start.tv_usec) << "s" << std::endl;
  }
  
  double SparseGaussianProcess::f(const double x[])
  {
    Eigen::VectorXd kstar(sampleset->size());
    // compute covariance between input and training data	
    Eigen::Map<const Eigen::VectorXd> x_vec_map(x, input_dim);
    for(size_t i = 0; i < sampleset->size(); ++i) {
      kstar(i) = covf->get((Eigen::VectorXd &) x_vec_map, sampleset->x(i));
    }
    // compute predicted value
    return kstar.dot(alpha);    
  }
  
  double SparseGaussianProcess::var(const double x[])
  {
    return 0.0;
  }
  
}