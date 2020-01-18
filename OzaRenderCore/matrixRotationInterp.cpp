#include "matrixRotationInterp.h"

void matrixRotationInterp::Evaluate(double t, double mat[16])
{
	point p[3];

	// Set up the identity
	memset(mat, 0, 16 * sizeof(double));
	mat[15] = 1.0;

	for (int i = 0; i < 3; i++)
	{
		p[i] = columns[i]->Evaluate(t);
		for (int j = 0; j < 3; j++)
			mat[4 * i + j] = p[i][j];
	}
}

void matrixRotationInterp::Load(Loader& input)
{
	char token[256];

	for (int i = 0; i < 3; i++)
	{
		input.ReadToken(token);
		if (strcmp(token, "column") != 0)
			input.Error("A matrixRotationInterp must contain 3 columns.  Only found %d\n", i - 1);

		columns[i] = Interpolator::LoadInterpolator(input);
	}
}