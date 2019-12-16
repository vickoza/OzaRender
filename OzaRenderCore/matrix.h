#pragma once
#include "point.h"
#include "svector.h"

#define _ALMOST_ZERO 1e-9

class matrix
{
public:

	matrix(int r = 2, int c = 2);
	matrix(int r, int c, double diagVal);
	matrix(const matrix& m1);
	matrix(matrix&& m1);
	~matrix()
	{
		if(m != nullptr)
			delete[] m;
	}

	int Rows(void) { return rows; }
	int Cols(void) { return cols; }

	matrix& operator=(const matrix& m1)
	{
		Copy(m1); return *this;
	}

	double& operator()(int i, int j) { return m[rows * j + i]; }
	double operator()(int i, int j) const { return m[rows * j + i]; }

	operator const double* (void) { return m; }

	void print(void);

	void Copy(const matrix& m1);
	void SwapRows(int i, int j);
	void AddRowMultiple(double d, int i, int j);
	void AddRowMultiple(double d, int i, int j, int startCol);
	void MultiplyRow(double d, int i);
	void Gauss(void);
	void GaussJordan(void);
	void SolveGaussJordan(matrix& RHS);
	matrix Inverse(void);
	double Invert(void);
	matrix Adjoint3x3(void);
	matrix Transpose(void);
	double getElement(int r, int c);

	double Determinant(void);

	void Identity(double d = 1.0);
	void Diagonal(double* v);
	void MakeRotationX(double alpha);
	void MakeRotationY(double alpha);
	void MakeRotationZ(double alpha);
	void MakeRotation(double angle, svector axis);
	void MakeTranslation(double x, double y, double z);
	void MakeScale(double x, double y, double z);

	friend void multiply(const matrix& m1, const matrix& m2, matrix& result);
	friend void add(const matrix& m1, const matrix& m2, matrix& result);
	friend void subtract(const matrix& m1, const matrix& m2, matrix& result);
	friend matrix operator*(const matrix& m1, const matrix& m2);
	friend point operator*(const matrix& m, const point& p);
	friend svector operator*(const matrix& m, const svector& p);
	friend void multiply(const matrix& m, const point& p, point& result);
	friend void multiply(const matrix& m, const svector& p, svector& result);

	matrix& operator+=(const matrix& m2);
	matrix& operator-=(const matrix& m2);
	matrix& operator*=(const matrix m2);

private:

	int rows, cols;
	double* m;
};
