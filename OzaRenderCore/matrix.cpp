#include <algorithm>
#include <iostream>
#include "matrix.h"

using namespace std;

matrix::matrix(int r, int c) : rows(r), cols(c), m(new double[r * c])
{
}

matrix::matrix(int r, int c, double diagVal) : rows(r), cols(c), m(new double[r * c])
{
	memset(m, 0, r * c * sizeof(double));
	int lim = min(rows, cols);
	for (int i = 0; i < lim; i++)
		m[i * rows + i] = diagVal;
}

matrix::matrix(const matrix& m1): rows(m1.rows), cols(m1.cols), m(new double[rows * cols])

{
	memcpy(m, m1.m, rows * cols * sizeof(double));
}

matrix::matrix(matrix&& m1) : rows(m1.rows), cols(m1.cols), m(m1.m)
{
	m1.m = nullptr;
}

void matrix::SwapRows(int i, int j)
{
	double tmp;
	double* d1 = m + i, * d2 = m + j;

	if (i == j)
		return;

	for (int k = 0; k < cols; k++, d1 += cols, d2 += cols)
	{
		tmp = (*d1);
		(*d1) = (*d2);
		(*d2) = tmp;
	}
}
double matrix::getElement(int r, int c)
{
	return m[(r * rows) + c];
}

void matrix::AddRowMultiple(double d, int i, int j)
{
	double* d1 = m + i, * d2 = m + j;

	for (int k = 0; k < cols; k++, d1 += cols, d2 += cols)
		(*d2) += d * (*d1);
}

void matrix::AddRowMultiple(double d, int i, int j, int startCol)
{
	double* d1 = m + i + startCol * cols, * d2 = m + j + startCol * cols;

	for (int k = startCol; k < cols; k++, d1 += cols, d2 += cols)
		(*d2) += d * (*d1);
}

void matrix::MultiplyRow(double d, int i)
{
	double* d1 = m + i;

	for (int k = 0; k < cols; k++, d1 += cols)
		(*d1) *= d;
}

void matrix::Gauss(void)
{
	double pivotVal;
	int pivotIndex;
	double* d1;

	for (int i = 0; i < min(rows, cols); i++)
	{
		d1 = m + i * rows + i;
		pivotVal = *d1;
		pivotIndex = i;
		for (int j = i + 1; j < rows; j++)
		{
			d1++;
			if (fabs(*d1) > fabs(pivotVal))
			{
				pivotVal = *d1;
				pivotIndex = j;
			}
		}
		if (fabs(pivotVal) > 1e-9)
		{
			SwapRows(i, pivotIndex);
			MultiplyRow(1.0 / pivotVal, i);
			for (int j = i + 1; j < rows; j++)
				AddRowMultiple(-m[i * rows + j], i, j, i);
		}
	}
}

void matrix::GaussJordan(void)
{
	double pivotVal;
	int pivotIndex;
	double* d1;

	for (int i = 0; i < rows; i++)
	{
		d1 = m + i * rows + i;
		pivotVal = *d1;
		pivotIndex = i;
		for (int j = i + 1; j < rows; j++)
		{
			d1++;
			if (fabs(*d1) > fabs(pivotVal))
			{
				pivotVal = *d1;
				pivotIndex = j;
			}
		}
		if (fabs(pivotVal) > 1e-9)
		{
			SwapRows(i, pivotIndex);
			MultiplyRow(1.0 / pivotVal, i);
			for (int j = 0; j < rows; j++)
			{
				if (j == i)
					continue;
				AddRowMultiple(-m[i * rows + j], i, j);
			}
		}
	}
}

double matrix::Invert(void) // returns the determinant i.e. the product of the pivots
{

	matrix inv(rows, cols, 1.0);
	double pivotVal;
	int pivotIndex;
	double* d1;
	double det = 1.0;

	for (int i = 0; i < rows; i++)
	{
		d1 = m + i * rows + i;
		pivotVal = *d1;
		pivotIndex = i;
		for (int j = i + 1; j < rows; j++)
		{
			d1++;
			if (fabs(*d1) > fabs(pivotVal))
			{
				pivotVal = *d1;
				pivotIndex = j;
			}
		}
		det *= pivotVal;
		if (fabs(pivotVal) > 1e-9)
		{
			SwapRows(i, pivotIndex);
			inv.SwapRows(i, pivotIndex);
			MultiplyRow(1.0 / pivotVal, i);
			inv.MultiplyRow(1.0 / pivotVal, i);
			for (int j = 0; j < rows; j++)
			{
				if (j == i)
					continue;
				double val = -m[i * rows + j];
				AddRowMultiple(val, i, j, i);
				inv.AddRowMultiple(val, i, j);
			}
		}
	}
	memcpy(m, inv.m, rows * cols * sizeof(double));
	return det;
}

matrix matrix::Inverse(void)
{
	matrix inv(rows, cols, 1.0);
	SolveGaussJordan(inv);
	return inv;
}

matrix matrix::Adjoint3x3(void)
{
	matrix tmp(3, 3);
	tmp.m[0] = m[rows + 1] * m[2 * rows + 2] - m[2 * rows + 1] * m[rows + 2];
	tmp.m[3] = m[2 * rows + 1] * m[2] - m[1] * m[2 * rows + 2];
	tmp.m[6] = m[1] * m[rows + 2] - m[rows + 1] * m[2];
	tmp.m[1] = m[rows + 2] * m[2 * rows] - m[2 * rows + 2] * m[rows];
	tmp.m[4] = m[2 * rows + 2] * m[0] - m[2] * m[2 * rows];
	tmp.m[7] = m[2] * m[rows] - m[rows + 2] * m[0];
	tmp.m[2] = m[rows] * m[2 * rows + 1] - m[2 * rows] * m[rows + 1];
	tmp.m[5] = m[2 * rows] * m[1] - m[0] * m[2 * rows + 1];
	tmp.m[8] = m[0] * m[rows + 1] - m[rows] * m[1];

	return tmp;
}

void matrix::SolveGaussJordan(matrix& RHS)
{

	matrix tmp(*this);
	double pivotVal;
	int pivotIndex;
	double* d1;

	for (int i = 0; i < rows; i++)
	{
		d1 = tmp.m + i * rows + i;
		pivotVal = *d1;
		pivotIndex = i;
		for (int j = i + 1; j < rows; j++)
		{
			d1++;
			if (fabs(*d1) > fabs(pivotVal))
			{
				pivotVal = *d1;
				pivotIndex = j;
			}
		}
		if (fabs(pivotVal) > 1e-9)
		{
			tmp.SwapRows(i, pivotIndex);
			RHS.SwapRows(i, pivotIndex);
			tmp.MultiplyRow(1.0 / pivotVal, i);
			RHS.MultiplyRow(1.0 / pivotVal, i);
			for (int j = 0; j < rows; j++)
			{
				if (j == i)
					continue;
				double val = -tmp.m[i * rows + j];
				tmp.AddRowMultiple(val, i, j, i);
				RHS.AddRowMultiple(val, i, j);
			}
		}
	}
}

double matrix::Determinant(void)
{
	matrix tmp(*this);
	double pivotVal;
	int pivotIndex;
	double* d1;
	double det = 1.0;

	for (int i = 0; i < rows; i++)
	{
		d1 = tmp.m + i * rows + i;
		pivotVal = *d1;
		pivotIndex = i;
		for (int j = i + 1; j < rows; j++)
		{
			d1++;
			if (fabs(*d1) > fabs(pivotVal))
			{
				pivotVal = *d1;
				pivotIndex = j;
			}
		}
		det *= pivotVal;
		if (fabs(pivotVal) > 1e-9)
		{
			tmp.SwapRows(i, pivotIndex);
			tmp.MultiplyRow(1.0 / pivotVal, i);
			for (int j = 0; j < rows; j++)
			{
				if (j == i)
					continue;
				double val = -tmp.m[i * rows + j];
				tmp.AddRowMultiple(val, i, j, i);
			}
		}
	}
	return det;
}

matrix matrix::Transpose(void)
{
	matrix t(rows, cols);
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			t(i, j) = (*this)(j, i);
	return t;
}

void matrix::Copy(const matrix& m1)
{
	if (m1.rows * m1.cols != rows * cols)
	{
		delete[] m;
		m = new double[m1.rows * m1.cols];
	}
	rows = m1.rows;
	cols = m1.cols;
	memcpy(m, m1.m, rows * cols * sizeof(double));
}

void matrix::print(void)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			cout << int(1000 * m[rows * j + i] + .5) / 1000.0 << "\t";
		}
		cout << endl;
	}
}

matrix& matrix::operator+=(const matrix& m2)
{
	const double* d1 = m, * d2 = m2.m;
	double* dr = m;
	int lim = rows * cols;


	for (int i = 0; i < lim; i++, dr++, d1++, d2++)
		(*dr) = (*d1) + (*d2);
	return *this;
}

matrix operator+(const matrix& m1, const matrix& m2)
{
	matrix result(m1);
	result += m2;
	return result;
}

void add(const matrix& m1, const matrix& m2, matrix& result)
{
	result = m1;
	result += m2;
}

matrix& matrix::operator-=(const matrix& m2)
{
	const double* d1 = m, * d2 = m2.m;
	double* dr = m;
	int lim = rows * cols;

	for (int i = 0; i < lim; i++, dr++, d1++, d2++)
		(*dr) = (*d1) - (*d2);
	return *this;
}

matrix operator-(const matrix& m1, const matrix& m2)
{
	matrix result(m1);
	result -= m2;
	return result;
}

void subtract(const matrix& m1, const matrix& m2, matrix& result)
{
	result = m1;
	result -= m2;
}

matrix& matrix::operator*=(const matrix m2)
{
	matrix result(*this);
	multiply(*this, m2, result);
	*this = result;
	return *this;
}

matrix operator*(const matrix& m1, const matrix& m2)
{
	matrix result(m1.rows, m2.cols);
	multiply(m1, m2, result);
	return result;
}

void multiply(const matrix& m1, const matrix& m2, matrix& result)
{
	const double* d1, * d2;
	double* dr;

	dr = result.m;
	for (int j = 0; j < m2.cols; j++)
	{
		for (int i = 0; i < m1.rows; i++)
		{
			d1 = m1.m + i;
			d2 = m2.m + j * m2.rows;
			*dr = 0.0;
			for (int k = 0; k < m1.cols; k++)
			{
				*dr += (*d1) * (*d2);
				d1 += m1.rows;
				d2++;
			}
			dr++;
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// Matrix Creation
////////////////////////////////////////////////////////////////////////////

void matrix::Identity(double d) // d defaults to 1.0
{
	int lim = min(rows, cols);

	memset(m, 0, rows * cols * sizeof(double));
	for (int i = 0; i < lim; i++)
		m[i * rows + i] = d;
}

void matrix::Diagonal(double* v)
{
	int lim = min(rows, cols);

	memset(m, 0, rows * cols * sizeof(double));
	for (int i = 0; i < lim; i++)
		m[i * rows + i] = v[i];
}

////////////////////////////////////////////////////////////////////////////
// 3D Routines
////////////////////////////////////////////////////////////////////////////

void matrix::MakeRotationX(double alpha)
{
	double a = alpha * PI / 180.0;
	double c = cos(a);
	double s = sin(a);

	memset(m, 0, 16 * sizeof(double));

	m[0] = 1.0;
	m[5] = m[10] = c;
	m[6] = s;
	m[9] = -s;
	m[15] = 1.0;
}

void matrix::MakeRotationY(double alpha)
{
	double a = alpha * PI / 180.0;
	double c = cos(a);
	double s = sin(a);


	memset(m, 0, 16 * sizeof(double));

	m[5] = 1.0;
	m[0] = m[10] = c;
	m[2] = -s;
	m[8] = s;
	m[15] = 1.0;
}

void matrix::MakeRotationZ(double alpha)
{
	double a = alpha * PI / 180.0;
	double c = cos(a);
	double s = sin(a);


	memset(m, 0, 16 * sizeof(double));

	m[10] = 1.0;
	m[0] = m[5] = c;
	m[1] = s;
	m[4] = -s;
	m[15] = 1.0;
}

#define sgn(x) ((x) < 0 ? -1 : 1)

void matrix::MakeRotation(double angle, svector axis)
{
	if ((fabs(axis[0]) < _ALMOST_ZERO) && (fabs(axis[1]) < _ALMOST_ZERO))
	{
		MakeRotationZ(sgn(axis[2]) * angle);
		return;
	}

	axis.normalize();

	double a = angle * PI / 180.0;
	double c = cos(a);
	double s = sin(a);
	double D = sqr(axis[0]) + sqr(axis[1]);
	double u0u1 = axis[0] * axis[1];
	double u0u2 = axis[0] * axis[2];
	double u1u2 = axis[1] * axis[2];
	double N = c * u0u1 * (sqr(axis[2]) - 1) / D;

	m[0] = c * (sqr(axis[1]) + sqr(u0u2)) / D + sqr(axis[0]);
	m[1] = N + s * axis[2] + u0u1;
	m[2] = -axis[1] * s + u0u2 * (1 - c);
	m[3] = 0.0;
	m[4] = N - s * axis[2] + u0u1;
	m[5] = c * (sqr(axis[0]) + sqr(u1u2)) / D + sqr(axis[1]);
	m[6] = axis[0] * s + u1u2 * (1 - c);
	m[7] = 0.0;
	m[8] = axis[1] * s + u0u2 * (1 - c);
	m[9] = -axis[0] * s + u1u2 * (1 - c);
	m[10] = c * D + sqr(axis[2]);
	m[11] = m[12] = m[13] = m[14] = 0.0;
	m[15] = 1.0;
}

void matrix::MakeTranslation(double x, double y, double z)
{
	Identity();
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

void matrix::MakeScale(double x, double y, double z)
{
	Identity();
	m[0] = x;
	m[5] = y;
	m[10] = z;
}


void multiply(const matrix& m, const point& p, point& result)
{

	double X = p[0], Y = p[1], Z = p[2], W = p[3]; // Just in case result == p 
	if (m.rows == 3)
	{
		if (m.cols == 4)
		{
			result[0] = m.m[0] * X + m.m[3] * Y + m.m[6] * Z + m.m[9] * W;
			result[1] = m.m[1] * X + m.m[4] * Y + m.m[7] * Z + m.m[10] * W;
			result[2] = m.m[2] * X + m.m[5] * Y + m.m[8] * Z + m.m[11] * W;
			result[3] = 1.0;
		}
		else
		{
			result[0] = m.m[0] * X + m.m[3] * Y + m.m[6] * Z;
			result[1] = m.m[1] * X + m.m[4] * Y + m.m[7] * Z;
			result[2] = m.m[2] * X + m.m[5] * Y + m.m[8] * Z;
			result[3] = 1.0;
		}
	}
	else if (m.rows == 4)
	{
		result[0] = m.m[0] * X + m.m[4] * Y + m.m[8] * Z + m.m[12] * W;
		result[1] = m.m[1] * X + m.m[5] * Y + m.m[9] * Z + m.m[13] * W;
		result[2] = m.m[2] * X + m.m[6] * Y + m.m[10] * Z + m.m[14] * W;
		result[3] = m.m[3] * X + m.m[7] * Y + m.m[11] * Z + m.m[15] * W;
	}
}

point operator*(const matrix& m, const point& p)
{
	point result;
	multiply(m, p, result);
	return result;
}

void multiply(const matrix& m, const svector& v, svector& result)
{
	double X = v[0], Y = v[1], Z = v[2]; // Just in case result == v
	if (m.rows == 3)
	{
		result[0] = m.m[0] * X + m.m[3] * Y + m.m[6] * Z;
		result[1] = m.m[1] * X + m.m[4] * Y + m.m[7] * Z;
		result[2] = m.m[2] * X + m.m[5] * Y + m.m[8] * Z;
	}
	else if (m.rows == 4)
	{
		result[0] = (m.m[0] * X + m.m[4] * Y + m.m[8] * Z);
		result[1] = (m.m[1] * X + m.m[5] * Y + m.m[9] * Z);
		result[2] = (m.m[2] * X + m.m[6] * Y + m.m[10] * Z);
	}
}

svector operator*(const matrix& m, const svector& v)
{
	svector result;
	multiply(m, v, result);
	return result;
}
