#pragma once
#include <memory>
#include <unordered_map>
#include "Loader.h"
#include "point.h"
#include "svector.h"

enum class shaderType
{
	none, constantShader, wireframeShader, hiddenlineShader, facetedShader, gouraudShader, textureShader, lightMap, normalMap, normalizationCubeMap, multiTexture, phongShader, texturedPhongShader
};


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

	static shaderType LoadShader(Loader& input);

protected:
	//scene *curScene;		// For reflection mapping
	shape* curShape;		// To retrieve size and other info
};

class textureShader : public shader
{
public:

	textureShader();
	~textureShader();

	virtual void ApplyTexture(point& pOrig, svector& nOrig, svector& uvObj);
	virtual void CalculateTexCoords(point& pOrig, svector& nOrig, svector& uvObj, double& u, double& v, double& w);
	virtual void BeginPolygon(int n, point p[], svector vn[], svector pn, svector uv[])
	{
		uBase = -1;
	} // Begin a new polygon (for texture tearing on cylindrical/spherical maps)
	void SetTextureUnit(unsigned int value) { tTextureUnit = value; }

	virtual void Load(Loader& input);

	enum { MAX_IMAGES = 20, IMAGE_NAME_LENGTH = 256 };

protected:

	enum TexEntity { Position = 0, Normal, Centroid, Reflection, UV };
	enum TexShape { Planar = 0, Cylindrical, Spherical, Box };
	enum TexAxis { XAxis = 0, YAxis, ZAxis };
	enum TexCoordSys { Object = 0, World };

	// Needed to change this for derived classes like diffuse map
	virtual void SetupTextureMap(void);
	void PlanarMap(double x, double y, double z, double& u, double& v);
	void CylindricalMap(double x, double y, double z, double& u, double& v);
	void SphericalMap(double x, double y, double z, double& u, double& v);
	void BoxMap(double x, double y, double z, double& u, double& v);

	unsigned int tObject;						// The OpenGL texture object
	unsigned int tTextureUnit;					// The texture unit to use
	char tName[MAX_IMAGES][IMAGE_NAME_LENGTH];	// The texture file names, allows for mipmapping and OpenGL box mapping
	int	tEntity;			// The texture entity, how the texture coordinates are chosen
	int tCoordSys;			// The texture entity's coordinate system (for reflection mapping, need world)
	int	tAxis;				// The axis for the texture projection
	int	tShape;				// The shape used to project the texture onto the object
	int tMinFilter;			// The minimization filter used by the texture
	int tMagFilter;			// The magnification filter used by the texture
	int tHorzWrap;			// The horizontal wrap mode
	int tVertWrap;			// The vertical wrap mode
	int tBlend;				// The blend mode
	float tBlendColor[4];	// The texture constant blend color (only used with "blend" blend mode)
	float tConstColor[3];	// Constant color to add to texture (used to simulate ambient light for diffuse maps)
	float tFilterColor[3];	// Constant color to multiply texture (used in reflection mapping for coef of reflectivity and specular mapping for specular color)
	int glTarget;			// The OpenGL texture target (either TEXTURE_2D or TEXTURE_CUBE_MAP)
	bool tViewTransform;	// Whether to transform the texture according to the view
	int tLightTransform;	// Which light to transform with respect to (-1 = no transform)
	//LinkedList<matrix> tTrans;	// The list of constant transforms affecting this texture

	double uBase;			// Utility variables to help in cylindrical/spherical mappings to eliminate tearing
};

class lightMap : public textureShader
{
public:

	lightMap();
	~lightMap();

	virtual void Load(Loader& input);

protected:

	virtual void SetupTextureMap(void);

	point specSurfColor;
	double shininess;
	int size;			// The size of the map.  Must be a power of two.
	//BitmapFile cubeMapFaces[6];
};

class normalMap : public textureShader
{
public:

	normalMap();

	virtual void Load(Loader& input);

protected:

	virtual void SetupTextureMap(void);

	double scale;
};

class normalizationCubeMap : public textureShader
{
public:

	normalizationCubeMap();

	virtual void Load(Loader& input);
	virtual void ApplyTexture(point& pOrig, svector& nOrig, svector& uvObj);
	virtual void BeginShading(void);

protected:

	virtual void SetupTextureMap(void);
	virtual void BeginPolygon(int n, point p[], svector vn[], svector pn, svector uv[]);

	int size;
	svector dPdu;
	//BitmapFile cubeMapFaces[6];
};

class multiTexture : public shader
{
public:

	multiTexture();
	~multiTexture();

	virtual void SetScene(scene* newScene)
	{
		//for (int i = 0; i < nTextures; i++) textures[i]->SetScene(newScene); curScene = newScene;
	}
	virtual void SetShape(shape* newShape)
	{
		for (int i = 0; i < nTextures; i++) textures[i]->SetShape(newShape); curShape = newShape;
	}

	virtual void Load(Loader& input);

private:

	textureShader** textures;  // An array of pointers to textures
	int nTextures;
	int maxTextures;
	bool disableLight;
	bool disableDiffuse;
	bool disableAmbient;
	bool disableSpecular;
};

std::unordered_map<shaderType, std::unique_ptr<shader>> shaderMap;//HACK need time to look as better design only should change when loading a new DLL or SO