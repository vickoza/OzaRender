#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "Loader.h"
#include "point.h"
#include "svector.h"
#include "Misc.h"
#include "matrix.h"

enum class shaderType
{
	none, constantShader, wireframeShader, hiddenlineShader, facetedShader, gouraudShader, textureShader, lightMap, normalMap, normalizationCubeMap, multiTexture, phongShader, texturedPhongShader
};
class shader;

extern std::unordered_map<shaderType, std::unique_ptr<shader>> shaderMap;//HACK need time to look as better design only should change when loading a new DLL or SO
class scene;
class shape;

class shader
{
public:

	virtual ~shader() { }

	virtual void BeginShading(void) = 0;
	virtual void DrawPolygon(int n, point p[], svector vn[], svector pn, svector uv[]) = 0;
	virtual void EndShading(void) = 0;

	// The default loader loads nothing after the name of the shader
	virtual void Load(Loader& input) { }

	virtual void SetScene(scene* newScene) { /*curScene = newScene;*/ }
	virtual void SetShape(shape* newShape) { curShape = newShape; }

	static shaderType LoadShader(Loader& input)
	{
		char shaderClass[256];
		shaderType res;

		input.ReadToken(shaderClass);
		if (strncmp(shaderClass, "shading", 4) == 0) // If we are reading the initial "shading" command
			input.ReadToken(shaderClass);

		if (strcmp(shaderClass, "constant") == 0)
			res = shaderType::constantShader;
		else if (strcmp(shaderClass, "faceted") == 0)
			res = shaderType::facetedShader;
		else if (strcmp(shaderClass, "gouraud") == 0)
			res = shaderType::gouraudShader;
		else if (strcmp(shaderClass, "wireframe") == 0)
			res = shaderType::wireframeShader;
		else if (strcmp(shaderClass, "hiddenline") == 0)
			res = shaderType::hiddenlineShader;
		else if (strcmp(shaderClass, "texture") == 0)
			res = shaderType::textureShader;
		else if (strcmp(shaderClass, "lightmap") == 0)
			res = shaderType::lightMap;
		else if (strcmp(shaderClass, "multitexture") == 0)
			res = shaderType::multiTexture;
		else if (strcmp(shaderClass, "phong") == 0)
			res = shaderType::phongShader;
		else if (strcmp(shaderClass, "texturedphong") == 0)
			res = shaderType::texturedPhongShader;
		else if (strcmp(shaderClass, "normalmap") == 0)
			res = shaderType::normalMap;
		else if (strcmp(shaderClass, "normalizationcubemap") == 0)
			res = shaderType::normalizationCubeMap;
		else
		{
			input.Error(shaderClass, "Unknown shader: %s", shaderClass);
			return shaderType::none;
		}
		shaderMap[res]->Load(input);
		return res;
	}

protected:
	//scene *curScene;		// For reflection mapping
	shape* curShape;		// To retrieve size and other info
};

class textureShader : public shader
{
public:

	textureShader()
	{
		tObject = 0;
		tTextureUnit = 0;
		tEntity = TexEntity::Position;
		tCoordSys = TexCoordSys::Object;
		tShape = TexShape::Planar;
		tAxis = TexAxis::XAxis;
		for (int i = 0; i < 10; i++)
			tName[i][0] = '\0';
		tMinFilter = TexFilter::trilinear;
		tMagFilter = TexFilter::linear;
		tHorzWrap = TexWrapping::Repeat;
		tVertWrap = TexWrapping::Repeat;
		tBlend = TexBlend::modulate;
		tBlendColor[0] = 1.0; tBlendColor[1] = 1.0; tBlendColor[2] = 1.0; tBlendColor[3] = 1.0;
		tConstColor[0] = 0.0; tConstColor[1] = 0.0; tConstColor[2] = 0.0;
		tFilterColor[0] = 1.0; tFilterColor[1] = 1.0; tFilterColor[2] = 1.0; tFilterColor[3] = 1.0;
		tViewTransform = false;
		tLightTransform = -1;
	}
	~textureShader() {}

	virtual void ApplyTexture(point& pOrig, svector& nOrig, svector& uvObj) = 0;
	virtual void CalculateTexCoords(point& pOrig, svector& nOrig, svector& uvObj, double& u, double& v, double& w) = 0;
	virtual void BeginPolygon(int n, point p[], svector vn[], svector pn, svector uv[])
	{
		uBase = -1;
	} // Begin a new polygon (for texture tearing on cylindrical/spherical maps)
	void SetTextureUnit(unsigned int value) { tTextureUnit = value; }

	virtual void Load(Loader& input) override
	{
		char token[MAX_INPUT_LENGTH];
		char tmp[MAX_INPUT_LENGTH];

		do
		{
			input.ReadToken(token);

			if (strncmp(token, "texture", 3) == 0)
			{
				int index;

				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete texture command in texture shader");

				if (!input.ReadInt(index))
					strcpy(tName[0], tmp);  // It is a single texture command
				else
					strcpy(tName[index], tmp);
			}
			else if (strncmp(token, "entity", 6) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete entity command in texture shader");

				/*if (isnumeric(tmp))
					tEntity = atoi(tmp);
				else*/ if (strncmp(tmp, "po", 2) == 0)
					tEntity = TexEntity::Position;
				else if (strncmp(tmp, "no", 2) == 0)
					tEntity = TexEntity::Normal;
				else if (strncmp(tmp, "ce", 2) == 0)
					tEntity = TexEntity::Centroid;
				else if (strncmp(tmp, "re", 2) == 0)
					tEntity = TexEntity::Reflection;
				else if (strncmp(tmp, "uv", 2) == 0)
					tEntity = TexEntity::UV;
				else
					input.Error(tmp, "Unrecognized texture entity: %s", tmp);
			}
			else if (strncmp(token, "coordsys", 8) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Coordinate System Command for Texture Shader");

				/*if (isnumeric(tmp))
					tCoordSys = atoi(tmp);
				else*/ if (strncmp(tmp, "object", 6) == 0)
					tCoordSys = TexCoordSys::Object;
				else if (strncmp(tmp, "world", 5) == 0)
					tCoordSys = TexCoordSys::World;
				else
					input.Error(tmp, "Unrecognized Coordinate System for Texture Shader");
			}
			else if (strncmp(token, "shape", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Shape Command for Texture Shader");

				/*if (isnumeric(tmp))
					tShape = atoi(tmp);
				else*/ if (strncmp(tmp, "pl", 2) == 0)
					tShape = TexShape::Planar;
				else if (strncmp(tmp, "cy", 2) == 0)
					tShape = TexShape::Cylindrical;
				else if (strncmp(tmp, "sp", 2) == 0)
					tShape = TexShape::Spherical;
				else if (strncmp(tmp, "bo", 2) == 0)
					tShape = TexShape::Box;
				else
					input.Error(tmp, "Unrecognized texture shape: %s", tmp);
			}
			else if (strncmp(token, "mapaxis", 4) == 0)
			{
				int axis;
				if (!input.ReadInt(axis))
					input.Error("Unrecognized texture map axis: axis %d\n", tAxis);
				tAxis = (TexAxis)axis;
			}
			else if (strncmp(token, "magfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Magnification Filter Command");

				/*if (isnumeric(tmp))
					tMagFilter = atoi(tmp);
				else*/ if (strncmp(tmp, "nearest", 4) == 0)
					tMagFilter = TexFilter::nearest;
				else if (strncmp(tmp, "linear", 4) == 0)
					tMagFilter = TexFilter::linear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "minfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Minification Filter Command");

				/*if (isnumeric(tmp))
					tMinFilter = atoi(tmp);
				else*/ if (strcmp(tmp, "nearest") == 0)
					tMinFilter = TexFilter::nearest;
				else if (strcmp(tmp, "linear") == 0)
					tMinFilter = TexFilter::linear;
				else if (strcmp(tmp, "nearest_mipmaped") == 0)
					tMinFilter = TexFilter::nearest_mipmaped;
				else if (strncmp(tmp, "linear_mipmap", 13) == 0)
					tMinFilter = TexFilter::linear_mipmap;
				else if (strcmp(tmp, "bilinear") == 0)
					tMinFilter = TexFilter::bilinear;
				else if (strcmp(tmp, "nearest_mipmaped_linear") == 0)
					tMinFilter = TexFilter::nearest_mipmaped_linear;
				else if (strcmp(tmp, "trilinear") == 0)
					tMinFilter = TexFilter::trilinear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "hwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Horizontal Wrap Command");

				/*if (isnumeric(tmp))
					tHorzWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tHorzWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tHorzWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tHorzWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized horizontal wrap mode: %s", tmp);
			}
			else if (strncmp(token, "vwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Vertical Wrap Command");

				/*if (isnumeric(tmp))
					tVertWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tVertWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tVertWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tVertWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized vertical wrap mode: %s", tmp);
			}
			else if (strncmp(token, "blend", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Blend Command");

				/*if (isnumeric(tmp))
					tBlend = atoi(tmp);
				else*/ if (strcmp(tmp, "decal") == 0)
					tBlend = TexBlend::decal;
				else if (strcmp(tmp, "replace") == 0)
					tBlend = TexBlend::replace;
				else if (strcmp(tmp, "modulate") == 0)
					tBlend = TexBlend::modulate;
				else if (strcmp(tmp, "blend") == 0)
					tBlend = TexBlend::blend;
				else if (strcmp(tmp, "add") == 0)
					tBlend = TexBlend::add;
				else
					input.Error(tmp, "Unrecognized blend function: %s", tmp);
			}
			else if (strncmp(token, "blendcolor", 9) == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tBlendColor[i]))
						input.Error("Incomplete texture blend color");

				if (!input.ReadFloat(tBlendColor[3]))
					tBlendColor[3] = 1.0;
			}
			else if (strcmp(token, "constcolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tConstColor[i]))
						input.Error("Incomplete texture constant color");
				tConstColor[0] *= 255; // Put it into OpenGL's Range
				tConstColor[1] *= 255;
				tConstColor[2] *= 255;
			}
			else if (strcmp(token, "filtercolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tFilterColor[i]))
						input.Error("Incomplete texture constant color");
			}
			else if (strcmp(token, "transform") == 0)
			{
				matrix m(4, 4, 1.0);

				input.ReadToken(tmp);
				if (strcmp(tmp, "followview") == 0)
				{
					tViewTransform = true;
				}
				else if (strcmp(tmp, "followlight") == 0)
				{
					if (!input.ReadInt(tLightTransform))
						input.Error("Incomplete 'followlight' command");
				}
				else if (!strcmp(tmp, "translate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete translation information for texture");

					m.MakeTranslation(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "scale"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete scale information for texture");

					m.MakeScale(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "rotate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete rotation information for texture");

					m.MakeRotationX(x[0]);
					tTrans.emplace_back(m);
					m.MakeRotationY(x[1]);
					tTrans.emplace_back(m);
					m.MakeRotationZ(x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "matrix")) // matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete matrix information for texture");
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "homogeneous")) // homogeneous matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete homogeneous matrix information for texture");
					tTrans.emplace_back(m);
				}
				else
					input.Error(tmp, "Unrecognized texture transform command: %s", tmp);
			}
			else if (strncmp(token, "end", 3) != 0)
				input.Error(token, "Unrecognized texture shader command: %s", token);
		} while (strncmp(token, "end", 3) != 0);
	}

	enum { MAX_IMAGES = 20, IMAGE_NAME_LENGTH = 256 };

protected:

	enum class TexEntity { Position, Normal, Centroid, Reflection, UV };
	enum class TexShape { Planar, Cylindrical, Spherical, Box };
	enum class TexAxis { XAxis, YAxis, ZAxis };
	enum class TexCoordSys { Object, World };
	enum class TexWrapping { Repeat, Clamp, Edge };
	enum class TexBlend { decal, replace, modulate, blend, add };
	enum class TexFilter { nearest, linear, nearest_mipmaped, linear_mipmap, bilinear, nearest_mipmaped_linear, trilinear};
				// Needed to change this for derived classes like diffuse map
	virtual void SetupTextureMap(void) = 0;
	void PlanarMap(double x, double y, double z, double& u, double& v)
	{
		switch (tEntity)
		{
		case TexEntity::Position:
			u = x;
			v = y;
			break;
		case TexEntity::Centroid:
		case TexEntity::Normal:
			// The other entities have a range from -1 to 1
			u = x / 2 + .5;
			v = y / 2 + .5;
			break;
		}
	}
	void CylindricalMap(double x, double y, double z, double& u, double& v)
	{
		static double uLast = -1;
		//cout<<uBase<<"   "<<uLast<<endl;
		if (tEntity == TexEntity::Position)
		{
			// Cylindrical map only makes sense with centroid, so convert position to centroid
			x = 2.0 * x - 1.0;
			y = 2.0 * y - 1.0;
			v = z;
		}
		else
			v = (z + 1) / 2.0;

		if (sqrt(x * x + y * y) > TD_SMALL)
		{
			u = atan2(x, y) / (2 * PI);
			if (u < 0)
				u = 1 + u;
		}
		else
		{
			if (uLast != -1)
				u = uLast;
			else
				u = 0.0;
		}

		if (u < 0)
			u = 1 + u; // map goes from 0 to 1


		if (uBase == -1)
			uBase = uLast = u;
		else
		{
			if (uBase > .75 && u < .25)
				u += 1;
			else if (uBase < .25 && u > .75)
				u -= 1;
		}
		uLast = u;
	}
	void SphericalMap(double x, double y, double z, double& u, double& v)
	{
		static double uLast = -1;

		if (tEntity == TexEntity::Position)
		{
			// Spherical map only makes sense with centroid, so convert position to centroid
			x = 2.0 * x - 1;
			y = 2.0 * y - 1;
			z = 2.0 * z - 1;
		}

		double len = sqrt(x * x + y * y + z * z);
		//cout<<len<<"  "<<sqrt(x*x + y*y)<<endl;
		if (sqrt(x * x + y * y) > TD_SMALL)
		{
			x = x / len;
			y = y / len;
			z = z / len;

			u = atan2(x, y) / (2 * PI);
			if (u < 0)
				u = 1 + u;
		}
		else
		{
			if (uLast != -1)
				u = uLast;
			else
				u = 0.0;
		}

		if (len > TD_SMALL)
			v = 1.0 - acos(z / len) / PI;  // Should be zero at bottom 
		else
			v = 0.0;

		// The following piece of code makes sure that there is no seam
		// when the vertices of a polygon span the edge of the texture
		// When the polygon is begun, uBase is reset (see the DrawPolygon method)
		// So the first vertex process set's the uBase.  Then all other 
		// vertices are compared to this uBase value
		if (len > TD_SMALL)
		{
			if (uBase == -1)
				uBase = uLast = u;
			else
			{
				if (uBase > .75 && u < .25)
					u += 1;
				else if (uBase < .25 && u > .75)
					u -= 1;
			}
		}

		double temp = u;
		u = v;
		v = temp;
	}

	void BoxMap(double x, double y, double z, double& u, double& v)
	{
	}

	unsigned int tObject;						// The OpenGL texture object
	unsigned int tTextureUnit;					// The texture unit to use
	char tName[MAX_IMAGES][IMAGE_NAME_LENGTH];	// The texture file names, allows for mipmapping and OpenGL box mapping
	TexEntity	tEntity;			// The texture entity, how the texture coordinates are chosen
	TexCoordSys tCoordSys;			// The texture entity's coordinate system (for reflection mapping, need world)
	TexAxis	tAxis;				// The axis for the texture projection
	TexShape	tShape;				// The shape used to project the texture onto the object
	TexFilter tMinFilter;			// The minimization filter used by the texture
	TexFilter tMagFilter;			// The magnification filter used by the texture
	TexWrapping tHorzWrap;			// The horizontal wrap mode
	TexWrapping tVertWrap;			// The vertical wrap mode
	TexBlend tBlend;				// The blend mode
	float tBlendColor[4];	// The texture constant blend color (only used with "blend" blend mode)
	float tConstColor[3];	// Constant color to add to texture (used to simulate ambient light for diffuse maps)
	float tFilterColor[3];	// Constant color to multiply texture (used in reflection mapping for coef of reflectivity and specular mapping for specular color)
	int glTarget;			// The OpenGL texture target (either TEXTURE_2D or TEXTURE_CUBE_MAP)
	bool tViewTransform;	// Whether to transform the texture according to the view
	int tLightTransform;	// Which light to transform with respect to (-1 = no transform)
	std::vector<matrix> tTrans;	// The list of constant transforms affecting this texture

	double uBase;			// Utility variables to help in cylindrical/spherical mappings to eliminate tearing
};

class lightMap : public textureShader
{
public:

	lightMap() : specSurfColor(1.0, 1.0, 1.0), shininess(1.0){}
	virtual ~lightMap() {}

	virtual void Load(Loader& input)
	{
		char token[MAX_INPUT_LENGTH];

		do
		{
			input.ReadToken(token);

			if (strcmp(token, "speccolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadDouble(specSurfColor[i]))
						input.Error("Incomplete surface specular color information");
			}
			else if (strcmp(token, "shininess") == 0)
			{
				if (!input.ReadDouble(shininess))
					input.Error("Incomplete surface shininess information");
			}
			else if (strcmp(token, "size") == 0)
			{
				int tmp;

				if (!input.ReadInt(tmp))
					input.Error("Incomplete size command for lightmap");

				for (size = 1; size < tmp; size *= 2);
			}
			else if (strncmp(token, "end", 3) != 0)
				input.Error(token, "Unrecognized lightmap shader command: %s", token);
		} while (strncmp(token, "end", 3) != 0);
	}

protected:

	virtual void SetupTextureMap(void) {}

	point specSurfColor;
	double shininess;
	int size;			// The size of the map.  Must be a power of two.
	//BitmapFile cubeMapFaces[6];
};

class normalMap : public textureShader
{
public:

	normalMap() : scale(1.0) {}

	virtual void Load(Loader& input)

	{
		char token[MAX_INPUT_LENGTH];
		char tmp[MAX_INPUT_LENGTH];

		do
		{
			input.ReadToken(token);

			if (strncmp(token, "texture", 3) == 0)
			{
				int index;

				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete texture command in texture shader");

				if (!input.ReadInt(index))
					strcpy(tName[0], tmp);  // It is a single texture command
				else
					strcpy(tName[index], tmp);
			}
			else if (strcmp(token, "scale") == 0)
			{
				if (!input.ReadDouble(scale))
					input.Error("Incomplete scale command in normal map");
			}
			else if (strncmp(token, "entity", 6) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete entity command in texture shader");

				/*if (isnumeric(tmp))
					tEntity = atoi(tmp);
				else */if (strncmp(tmp, "po", 2) == 0)
					tEntity = TexEntity::Position;
				else if (strncmp(tmp, "no", 2) == 0)
					tEntity = TexEntity::Normal;
				else if (strncmp(tmp, "ce", 2) == 0)
					tEntity = TexEntity::Centroid;
				else if (strncmp(tmp, "re", 2) == 0)
					tEntity = TexEntity::Reflection;
				else if (strncmp(tmp, "uv", 2) == 0)
					tEntity = TexEntity::UV;
				else
					input.Error(tmp, "Unrecognized texture entity: %s", tmp);
			}
			else if (strncmp(token, "coordsys", 8) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Coordinate System Command for Texture Shader");

				/*if (isnumeric(tmp))
					tCoordSys = atoi(tmp);
				else*/ if (strncmp(tmp, "object", 6) == 0)
					tCoordSys = TexCoordSys::Object;
				else if (strncmp(tmp, "world", 5) == 0)
					tCoordSys = TexCoordSys::World;
				else
					input.Error(tmp, "Unrecognized Coordinate System for Texture Shader");
			}
			else if (strncmp(token, "shape", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Shape Command for Texture Shader");

				/*if (isnumeric(tmp))
					tShape = atoi(tmp);
				else */if (strncmp(tmp, "pl", 2) == 0)
					tShape = TexShape::Planar;
				else if (strncmp(tmp, "cy", 2) == 0)
					tShape = TexShape::Cylindrical;
				else if (strncmp(tmp, "sp", 2) == 0)
					tShape = TexShape::Spherical;
				else if (strncmp(tmp, "bo", 2) == 0)
					tShape = TexShape::Box;
				else
					input.Error(tmp, "Unrecognized texture shape: %s", tmp);
			}
			else if (strncmp(token, "mapaxis", 4) == 0)
			{
				int axis;
				if (!input.ReadInt(axis))
					input.Error("Unrecognized texture map axis: axis %d\n", tAxis);
				tAxis = (TexAxis)axis;
			}
			else if (strncmp(token, "magfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Magnification Filter Command");

				/*if (isnumeric(tmp))
					tMagFilter = atoi(tmp);
				else*/ if (strncmp(tmp, "nearest", 4) == 0)
					tMagFilter = TexFilter::nearest;
				else if (strncmp(tmp, "linear", 4) == 0)
					tMagFilter = TexFilter::linear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "minfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Minification Filter Command");

				/*if (isnumeric(tmp))
					tMinFilter = atoi(tmp);
				else*/ if (strcmp(tmp, "nearest") == 0)
					tMinFilter = TexFilter::nearest;
				else if (strcmp(tmp, "linear") == 0)
					tMinFilter = TexFilter::linear;
				else if (strcmp(tmp, "nearest_mipmaped") == 0)
					tMinFilter = TexFilter::nearest_mipmaped;
				else if (strncmp(tmp, "linear_mipmap", 13) == 0) 
					tMinFilter = TexFilter::linear_mipmap;
				else if (strcmp(tmp, "bilinear") == 0)
					tMinFilter = TexFilter::bilinear;
				else if (strcmp(tmp, "nearest_mipmaped_linear") == 0)
					tMinFilter = TexFilter::nearest_mipmaped_linear;
				else if (strcmp(tmp, "trilinear") == 0)
					tMinFilter = TexFilter::trilinear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "hwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Horizontal Wrap Command");

				/*if (isnumeric(tmp))
					tHorzWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tHorzWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tHorzWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tHorzWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized horizontal wrap mode: %s", tmp);
			}
			else if (strncmp(token, "vwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Vertical Wrap Command");

				/*if (isnumeric(tmp))
					tVertWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tVertWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tVertWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tVertWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized vertical wrap mode: %s", tmp);
			}
			else if (strncmp(token, "blend", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Blend Command");

				/*if (isnumeric(tmp))
					tBlend = atoi(tmp);
				else*/ if (strcmp(tmp, "decal") == 0)
					tBlend = TexBlend::decal;
				else if (strcmp(tmp, "replace") == 0)
					tBlend = TexBlend::replace;
				else if (strcmp(tmp, "modulate") == 0)
					tBlend = TexBlend::modulate;
				else if (strcmp(tmp, "blend") == 0)
					tBlend = TexBlend::blend;
				else if (strcmp(tmp, "add") == 0)
					tBlend = TexBlend::add;
				else
					input.Error(tmp, "Unrecognized blend function: %s", tmp);
			}
			else if (strncmp(token, "blendcolor", 9) == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tBlendColor[i]))
						input.Error("Incomplete texture blend color");

				if (!input.ReadFloat(tBlendColor[3]))
					tBlendColor[3] = 1.0;
			}
			else if (strcmp(token, "constcolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tConstColor[i]))
						input.Error("Incomplete texture constant color");
				tConstColor[0] *= 255; // Put it into OpenGL's Range
				tConstColor[1] *= 255;
				tConstColor[2] *= 255;
			}
			else if (strcmp(token, "filtercolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tFilterColor[i]))
						input.Error("Incomplete texture constant color");
			}
			else if (strcmp(token, "transform") == 0)
			{
				matrix m(4, 4, 1.0);

				input.ReadToken(tmp);
				if (strcmp(tmp, "followview") == 0)
				{
					tViewTransform = true;
				}
				else if (strcmp(tmp, "followlight") == 0)
				{
					if (!input.ReadInt(tLightTransform))
						input.Error("Incomplete 'followlight' command");
				}
				else if (!strcmp(tmp, "translate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete translation information for texture");

					m.MakeTranslation(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "scale"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete scale information for texture");

					m.MakeScale(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "rotate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete rotation information for texture");

					m.MakeRotationX(x[0]);
					tTrans.emplace_back(m);
					m.MakeRotationY(x[1]);
					tTrans.emplace_back(m);
					m.MakeRotationZ(x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "matrix")) // matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete matrix information for texture");
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "homogeneous")) // homogeneous matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete homogeneous matrix information for texture");
					tTrans.emplace_back(m);
				}
				else
					input.Error(tmp, "Unrecognized texture transform command: %s", tmp);
			}
			else if (strncmp(token, "end", 3) != 0)
				input.Error(token, "Unrecognized texture shader command: %s", token);
		} while (strncmp(token, "end", 3) != 0);
	}

protected:

	virtual void SetupTextureMap(void) = 0;

	double scale;
};

class normalizationCubeMap : public textureShader
{
public:

	normalizationCubeMap() : size(256) {}

	virtual void Load(Loader& input)
	{
		char token[MAX_INPUT_LENGTH];
		char tmp[MAX_INPUT_LENGTH];

		do
		{
			input.ReadToken(token);

			if (strncmp(token, "texture", 3) == 0)
			{
				int index;

				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete texture command in texture shader");

				if (!input.ReadInt(index))
					strcpy(tName[0], tmp);  // It is a single texture command
				else
					strcpy(tName[index], tmp);
			}
			else if (strcmp(token, "size") == 0)
			{
				int tmp;

				if (!input.ReadInt(tmp))
					input.Error("Incomplete size command for lightmap");

				for (size = 1; size < tmp; size *= 2);
			}
			else if (strncmp(token, "entity", 6) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete entity command in texture shader");

				/*if (isnumeric(tmp))
					tEntity = atoi(tmp);
				else*/ if (strncmp(tmp, "po", 2) == 0)
					tEntity = TexEntity::Position;
				else if (strncmp(tmp, "no", 2) == 0)
					tEntity = TexEntity::Normal;
				else if (strncmp(tmp, "ce", 2) == 0)
					tEntity = TexEntity::Centroid;
				else if (strncmp(tmp, "re", 2) == 0)
					tEntity = TexEntity::Reflection;
				else if (strncmp(tmp, "uv", 2) == 0)
					tEntity = TexEntity::UV;
				else
					input.Error(tmp, "Unrecognized texture entity: %s", tmp);
			}
			else if (strncmp(token, "coordsys", 8) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Coordinate System Command for Texture Shader");

				/*if (isnumeric(tmp))
					tCoordSys = atoi(tmp);
				else*/ if (strncmp(tmp, "object", 6) == 0)
					tCoordSys = TexCoordSys::Object;
				else if (strncmp(tmp, "world", 5) == 0)
					tCoordSys = TexCoordSys::World;
				else
					input.Error(tmp, "Unrecognized Coordinate System for Texture Shader");
			}
			else if (strncmp(token, "shape", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Shape Command for Texture Shader");

				/*if (isnumeric(tmp))
					tShape = atoi(tmp);
				else*/ if (strncmp(tmp, "pl", 2) == 0)
					tShape = TexShape::Planar;
				else if (strncmp(tmp, "cy", 2) == 0)
					tShape = TexShape::Cylindrical;
				else if (strncmp(tmp, "sp", 2) == 0)
					tShape = TexShape::Spherical;
				else if (strncmp(tmp, "bo", 2) == 0)
					tShape = TexShape::Box;
				else
					input.Error(tmp, "Unrecognized texture shape: %s", tmp);
			}
			else if (strncmp(token, "mapaxis", 4) == 0)
			{
				int axis;
				if (!input.ReadInt(axis))
					input.Error("Unrecognized texture map axis: axis %d\n", tAxis);
				tAxis = (TexAxis)axis;
			}
			else if (strncmp(token, "magfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Magnification Filter Command");

				/*if (isnumeric(tmp))
					tMagFilter = atoi(tmp);
				else*/ if (strncmp(tmp, "nearest", 4) == 0)
					tMagFilter = TexFilter::nearest;
				else if (strncmp(tmp, "linear", 4) == 0)
					tMagFilter = TexFilter::linear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "minfilter", 9) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Minification Filter Command");

				/*if (isnumeric(tmp))
					tMinFilter = atoi(tmp);
				else*/ if (strcmp(tmp, "nearest") == 0)
					tMinFilter = TexFilter::nearest;
				else if (strcmp(tmp, "linear") == 0)
					tMinFilter = TexFilter::linear;
				else if (strcmp(tmp, "nearest_mipmaped") == 0)
					tMinFilter = TexFilter::nearest_mipmaped;
				else if (strncmp(tmp, "linear_mipmap", 13) == 0)
					tMinFilter = TexFilter::linear_mipmap;
				else if (strcmp(tmp, "bilinear") == 0)
					tMinFilter = TexFilter::bilinear;
				else if (strcmp(tmp, "nearest_mipmaped_linear") == 0)
					tMinFilter = TexFilter::nearest_mipmaped_linear;
				else if (strcmp(tmp, "trilinear") == 0)
					tMinFilter = TexFilter::trilinear;
				else
					input.Error(tmp, "Unrecognized texture magnification filter: %s", tmp);
			}
			else if (strncmp(token, "hwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Horizontal Wrap Command");

				/*if (isnumeric(tmp))
					tHorzWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tHorzWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tHorzWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tHorzWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized horizontal wrap mode: %s", tmp);
			}
			else if (strncmp(token, "vwrap", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Vertical Wrap Command");

				/*if (isnumeric(tmp))
					tVertWrap = atoi(tmp);
				else*/ if (strcmp(tmp, "repeat") == 0)
					tVertWrap = TexWrapping::Repeat;
				else if (strcmp(tmp, "clamp") == 0)
					tVertWrap = TexWrapping::Clamp;
				else if (strcmp(tmp, "edge") == 0 || strcmp(tmp, "clampedge") == 0)
					tVertWrap = TexWrapping::Edge;
				else
					input.Error(tmp, "Unrecognized vertical wrap mode: %s", tmp);
			}
			else if (strncmp(token, "blend", 5) == 0)
			{
				if (!input.ReadToken(tmp))
					input.Error(tmp, "Incomplete Blend Command");

				/*if (isnumeric(tmp))
					tBlend = atoi(tmp);
				else*/ if (strcmp(tmp, "decal") == 0)
					tBlend = TexBlend::decal;
				else if (strcmp(tmp, "replace") == 0)
					tBlend = TexBlend::replace;
				else if (strcmp(tmp, "modulate") == 0)
					tBlend = TexBlend::modulate;
				else if (strcmp(tmp, "blend") == 0)
					tBlend = TexBlend::blend;
				else if (strcmp(tmp, "add") == 0)
					tBlend = TexBlend::add;
				else
					input.Error(tmp, "Unrecognized blend function: %s", tmp);
			}
			else if (strncmp(token, "blendcolor", 9) == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tBlendColor[i]))
						input.Error("Incomplete texture blend color");

				if (!input.ReadFloat(tBlendColor[3]))
					tBlendColor[3] = 1.0;
			}
			else if (strcmp(token, "constcolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tConstColor[i]))
						input.Error("Incomplete texture constant color");
				tConstColor[0] *= 255; // Put it into OpenGL's Range
				tConstColor[1] *= 255;
				tConstColor[2] *= 255;
			}
			else if (strcmp(token, "filtercolor") == 0)
			{
				for (int i = 0; i < 3; i++)
					if (!input.ReadFloat(tFilterColor[i]))
						input.Error("Incomplete texture constant color");
			}
			else if (strcmp(token, "transform") == 0)
			{
				matrix m(4, 4, 1.0);

				input.ReadToken(tmp);
				if (strcmp(tmp, "followview") == 0)
				{
					tViewTransform = true;
				}
				else if (strcmp(tmp, "followlight") == 0)
				{
					if (!input.ReadInt(tLightTransform))
						input.Error("Incomplete 'followlight' command");
				}
				else if (!strcmp(tmp, "translate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete translation information for texture");

					m.MakeTranslation(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "scale"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete scale information for texture");

					m.MakeScale(x[0], x[1], x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "rotate"))
				{
					double x[3];
					for (int i = 0; i < 3; i++)
						if (!input.ReadDouble(x[i]))
							input.Error("Incomplete rotation information for texture");

					m.MakeRotationX(x[0]);
					tTrans.emplace_back(m);
					m.MakeRotationY(x[1]);
					tTrans.emplace_back(m);
					m.MakeRotationZ(x[2]);
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "matrix")) // matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete matrix information for texture");
					tTrans.emplace_back(m);
				}
				else if (!strcmp(tmp, "homogeneous")) // homogeneous matrix
				{
					for (int i = 0; i < 3; i++)
						for (int j = 0; j < 3; j++)
							if (!input.ReadDouble(m(i, j)))
								input.Error("Incomplete homogeneous matrix information for texture");
					tTrans.emplace_back(m);
				}
				else
					input.Error(tmp, "Unrecognized texture transform command: %s", tmp);
			}
			else if (strncmp(token, "end", 3) != 0)
				input.Error(token, "Unrecognized texture shader command: %s", token);
		} while (strncmp(token, "end", 3) != 0);
	}
	/*virtual void ApplyTexture(point& pOrig, svector& nOrig, svector& uvObj);
	virtual void BeginShading(void);*/

protected:

	/*virtual void SetupTextureMap(void);
	virtual void BeginPolygon(int n, point p[], svector vn[], svector pn, svector uv[]);*/

	int size;
	svector dPdu;
	//BitmapFile cubeMapFaces[6];
};

class multiTexture : public shader
{
public:

	multiTexture() : disableLight(false), disableDiffuse(false), disableSpecular(false), disableAmbient(false) {}
	~multiTexture() {}

	virtual void SetScene(scene* newScene)
	{
		//for (int i = 0; i < nTextures; i++) textures[i]->SetScene(newScene); curScene = newScene;
	}
	virtual void SetShape(shape* newShape)
	{
		for (auto& texture : textures) texture->SetShape(newShape); curShape = newShape;
	}

	virtual void Load(Loader& input)
	{
		char token[MAX_INPUT_LENGTH];
		int nTextures;

		if (!input.ReadInt(nTextures))
			input.Error("Missing texture count after multitexture command");

		for (int i = 0; i < nTextures; i++)
		{
			if (i < maxTextures)
			{
				auto tmpShader = shader::LoadShader(input);

				if (tmpShader == shaderType::none)
					input.Error("Expected a texture shader in multitexture.");

				// Warning, this will cause an error if the shader loaded is not 
				// a derived class of textureShader.  Need to use RTTI here.
				/*textures.emplace_back(std::make_unique<textureShader>(*shaderMap[shaderType::textureShader]));//HACK
				textures[i]->SetTextureUnit(i);*/
			}
			else
			{
				input.Warning("Graphics card supports %d textures.  Ignoring texture %d", maxTextures, i);
				shaderMap[shaderType::textureShader]->Load(input);
			}
		}

		do
		{
			input.ReadToken(token);
			if (strncmp(token, "disablelight", 12) == 0)
				disableLight = true;
			else if (strncmp(token, "disableambient", 14) == 0)
				disableAmbient = true;
			else if (strncmp(token, "disablediffuse", 14) == 0)
				disableDiffuse = true;
			else if (strncmp(token, "disablespecular", 15) == 0)
				disableSpecular = true;
			else if (strcmp(token, "texture") == 0)
				input.Error(token, "Too many texture commands in multitexture, %d specified at beginning of texture command", nTextures);
			else if (strncmp(token, "end", 3) != 0)
				input.Error(token, "Unrecognized multitexture command: %s", token);
		} while (strncmp(token, "end", 3) != 0);
	}

private:

	std::vector<std::unique_ptr<textureShader>> textures;  // An array of pointers to textures
	int maxTextures;
	bool disableLight;
	bool disableDiffuse;
	bool disableAmbient;
	bool disableSpecular;
};
