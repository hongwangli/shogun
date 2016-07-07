/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2014 - 2016 Soumyajit De
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 */

#ifndef MULTI_KERNEL_MMD_H_
#define MULTI_KERNEL_MMD_H_

#include <vector>
#include <shogun/lib/SGVector.h>
#include <shogun/kernel/Kernel.h>
#include <shogun/statistical_testing/internals/mmd/ComputeMMD.h>
#include <shogun/statistical_testing/internals/KernelManager.h>

namespace shogun
{

namespace internal
{

namespace mmd
{

struct MultiKernelMMD : ComputeMMD
{
	MultiKernelMMD(index_t n_x, index_t n_y, EStatisticType stype)
	{
		m_n_x=n_x;
		m_n_y=n_y;
		m_stype=stype;
	}

	SGVector<float64_t> operator()(const KernelManager& kernel_mgr) const
	{
		SG_SDEBUG("Entering!\n");
		std::vector<terms_t> terms(kernel_mgr.num_kernels());
		const index_t size=m_n_x+m_n_y;
		for (auto j=0; j<size; ++j)
		{
			for (auto i=j; i<size; ++i)
			{
				for (size_t k=0; k<kernel_mgr.num_kernels(); ++k)
				{
					auto kernel=kernel_mgr.kernel_at(k)->kernel(i, j);
					add_term(terms[k], kernel, i, j);
				}
			}
		}

		SGVector<float64_t> result(kernel_mgr.num_kernels());
		for (size_t k=0; k<kernel_mgr.num_kernels(); ++k)
		{
			result[k]=compute(terms[k]);
			SG_SDEBUG("result[%d] = %f!\n", k, result[k]);
		}
		terms.resize(0);

		SG_SDEBUG("Leaving!\n");
		return result;
	}
};

}

}

}

#endif // MULTI_KERNEL_MMD_H_