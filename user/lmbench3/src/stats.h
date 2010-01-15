/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _STATS_H
#define _STATS_H

#include "bench.h"
#include "timing.h"

#define ABS(x)	((x) < 0 ? -(x) : (x))

int	int_compare(const void *a, const void *b);
int	uint64_compare(const void *a, const void *b);
int	double_compare(const void *a, const void *b);

typedef	int (*int_stat)(int *values, int size);
typedef	uint64 (*uint64_stat)(uint64 *values, int size);
typedef	double (*double_stat)(double *values, int size);

int	int_median(int *values, int size);
uint64	uint64_median(uint64 *values, int size);
double	double_median(double *values, int size);

int	int_mean(int *values, int size);
uint64	uint64_mean(uint64 *values, int size);
double	double_mean(double *values, int size);

int	int_min(int *values, int size);
uint64	uint64_min(uint64 *values, int size);
double	double_min(double *values, int size);

int	int_max(int *values, int size);
uint64	uint64_max(uint64 *values, int size);
double	double_max(double *values, int size);

double	int_variance(int *values, int size);
double	uint64_variance(uint64 *values, int size);
double	double_variance(double *values, int size);

double	int_moment(int moment, int *values, int size);
double	uint64_moment(int moment, uint64 *values, int size);
double	double_moment(int moment, double *values, int size);

double	int_stderr(int *values, int size);
double	uint64_stderr(uint64 *values, int size);
double	double_stderr(double *values, int size);

double	int_skew(int *values, int size);
double	uint64_skew(uint64 *values, int size);
double	double_skew(double *values, int size);

double	int_kurtosis(int *values, int size);
double	uint64_kurtosis(uint64 *values, int size);
double	double_kurtosis(double *values, int size);

double	int_bootstrap_stderr(int *values, int size, int_stat f);
double	uint64_bootstrap_stderr(uint64 *values, int size, uint64_stat f);
double	double_bootstrap_stderr(double *values, int size, double_stat f);

void	regression(double *x, double *y, double *sig, int n,
		   double *a, double *b, double *sig_a, double *sig_b, 
		   double *chi2);

#endif /* _STATS_H */
