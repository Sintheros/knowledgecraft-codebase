#include "DiamondSquareAlg.h"
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glut.h>
#include <ctime>    // For time()
#include <algorithm>
#include "glfont2.h"
#include "ScriptGen.h"

using namespace std;

//For generating a random number and for seeding the randon number generator
#define random() rand()
#define srandom(x) srand(x)

//Used for turning the points from the Map Generator into 2D array coordinates for node locations
struct Coord {
	float x;
	float y;
	float z;
	string url;//URL associated with this node
	string disName;
	string fileName;//Name of the node

	Coord() : x(0.0f), y(0.0f), z(0.0f) { }
};

//The data structure used by the Map Generator, which we are given when using it
struct Point {
	float _x;
	float _y;
	float _z;
	string _url;
	string _disName;
	string _fileName;

	vector<Coord> children;

	Point(float x, float y, float z, string url, string disName, string fileName) : 
		_x(x), _y(y), _z(z), _url(url), _disName(disName), _fileName(fileName) { }
};

/*
 * randNum - Return a random floating point number such that
 *      (min <= return-value <= max)
 * 32,767 values are possible for any given range.
 */
static float randnum (float min, float max)
{
	int r;
    float	x;
    
	r = random ();
    x = (float)(r & 0x7fff) /
		(float)0x7fff;
    return (x * (max - min) + min);
} 


/*
 * fractRand is a useful interface to randnum.
 */
#define fractRand(v) randnum (-v, v)

#define DEF_HEIGHT_SCALE 1.f
#define PI 3.1415926536

//Level of detail of the map. The map will be a (2^detail) + 1 x (2^detail) + 1 sized grid
static int detail = 10;
//Convert detail into 2^detail + 1
static int size = (1 << detail) + 1;
//Initial distance that the OpenGL preview draws out to
int sight = 300;
//Rotations
static float gamma = 0, beta =15 , alpha = 0;
//Camera positions
static float posX = 0, posZ = 0, posY = -2;

const char filenm[100] = "Ignore.txt";

//Constants for the IDs of certain block types in Minecraft
const char BLOCK_NONE = 0;
const char BLOCK_MOUNTAIN = 1;
const char BLOCK_PLAINS = 2;
const char BLOCK_DIRT = 3;
const char BLOCK_COBBLE = 4;
const char BLOCK_WOOD = 5;
const char BLOCK_WATER = 9;



DiamondSquareAlg::DiamondSquareAlg(void)
{
}


DiamondSquareAlg::~DiamondSquareAlg(void)
{
}

float* fArray; //Primary terrain storage
float* fFractArray; //Same size as fArray, but filled with fractal variance to be applied to fArray
bool* fIsNode; //Whether or not a particular point in fArray is a node or not
bool* fSet; //Whether or not a particular point in fArray has been set already
char* fBlock; //What Minecraft block ID is used for that location

/*
 * avgEndpoints - Given the i location and a stride to the data
 * values, return the average those data values. "i" can be thought of
 * as the data value in the center of two line endpoints. We use
 * "stride" to get the values of the endpoints. Averaging them yields
 * the midpoint of the line.
 *
 * Called by fill1DFractArray.
 */
static float avgEndpoints (int i, int stride, float *fa)
{
    return ((float) (fa[i-stride] +
		     fa[i+stride]) * .5f);
}
/*
 * powerOf2 - Returns 1 if size is a power of 2. Returns 0 if size is
 * not a power of 2, or is zero.
 */
static int powerOf2 (int size)
{
    int i, bitcount = 0;

    /* Note this code assumes that (sizeof(int)*8) will yield the
       number of bits in an int. Should be portable to most
       platforms. */
    for (i=0; i<sizeof(int)*8; i++)
	if (size & (1<<i))
	    bitcount++;
    if (bitcount == 1)
	/* One bit. Must be a power of 2. */
	return (1);
    else
	/* either size==0, or size not a power of 2. Sorry, Charlie. */
	return (0);
}

/*
 * avgDiamondVals - Given the i,j location as the center of a diamond,
 * average the data values at the four corners of the diamond and
 * return it. "Stride" represents the distance from the diamond center
 * to a diamond corner.
 *
 * Called by fill2DFractArray.
 */
static float avgDiamondVals (int i, int j, int stride,
			     int size, int subSize, float *fa)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "X", and the four value we want to average are
       "*"s:
           .   *   .

           *   X   *

           .   *   .
       */

    /* In order to support tiled surfaces which meet seamless at the
       edges (that is, they "wrap"), We need to be careful how we
       calculate averages when the i,j diamond center lies on an edge
       of the array. The first four 'if' clauses handle these
       cases. The final 'else' clause handles the general case (in
       which i,j is not on an edge).
     */
    if (i == 0)
	return ((float) (fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride] +
			 fa[((subSize-stride)*size) + j] +
			 fa[((i+stride)*size) + j]) * .25f);
    else if (i == size-1)
	return ((float) (fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride] +
			 fa[((i-stride)*size) + j] +
			 fa[((0+stride)*size) + j]) * .25f);
    else if (j == 0)
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j+stride] +
			 fa[(i*size) + subSize-stride]) * .25f);
    else if (j == size-1)
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j-stride] +
			 fa[(i*size) + 0+stride]) * .25f);
    else
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride]) * .25f);
}


/*
 * avgSquareVals - Given the i,j location as the center of a square,
 * average the data values at the four corners of the square and return
 * it. "Stride" represents half the length of one side of the square.
 *
 * Called by fill2DFractArray.
 */
static float avgSquareVals (int i, int j, int stride, int size, float *fa)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "*", and the four value we want to average are
       "X"s:
           X   .   X

           .   *   .

           X   .   X
       */
    return ((float) (fa[((i-stride)*size) + j-stride] +
		     fa[((i-stride)*size) + j+stride] +
		     fa[((i+stride)*size) + j-stride] +
		     fa[((i+stride)*size) + j+stride]) * .25f);
}

float *alloc2DArray (int size)
{
    /* For a sizeXsize array, we need (size+1)X(size+1) space. For
       example, a 2x2 mesh needs 3x3=9 data points: 

           *   *   *

           *   *   *

           *   *   *
		*/
	int otherSize = size * size * sizeof(bool);
	int charSize = size * size * sizeof(char);
	fIsNode = ((bool *) malloc (otherSize));
	fSet = ((bool *) malloc (otherSize));
	fBlock = ((char *) malloc (charSize));
	size = size * size * sizeof(float);
	fFractArray = ((float *) malloc (size));
    return ((float *) malloc (size));
}

/*
 * fill2DFractArray - Use the diamond-square algorithm to tessalate a
 * grid of float values into a fractal height map.
 */
void fill2DFractArray (float *fa, int size,
		       int seedValue, float heightScale, float h, float mh)
{
    int	i, j;
    int	stride;
    int	oddline;
    int subSize;
	float ratio, scale, mountratio, mountscale;
	size--;

    if (!powerOf2(size) || (size==1)) {
	/* We can't tesselate the array if it is not a power of 2. */
		cout << "debug";
		return;
    }

    /* subSize is the dimension of the array in terms of connected line
       segments, while size is the dimension in terms of number of
       vertices. */
    subSize = size;
    size++;
    
    /* initialize random number generator */
    srandom (time(NULL));


	/* Set up our roughness constants.
	   Random numbers are always generated in the range 0.0 to 1.0.
	   'scale' is multiplied by the randum number.
	   'ratio' is multiplied by 'scale' after each iteration
	   to effectively reduce the randum number range.
	   */
	ratio = (float)pow(2.f,-h);
	mountratio = (float)pow(2.f,-mh);
	scale = heightScale * ratio;
	mountscale = heightScale * mountratio;

    /* Seed the first four values. For example, in a 4x4 array, we
       would initialize the data points indicated by '*':

           *   .   .   .   *

           .   .   .   .   .

           .   .   .   .   .

           .   .   .   .   .

           *   .   .   .   *

       In terms of the "diamond-square" algorithm, this gives us
       "squares".

       We want the four corners of the array to have the same
       point. This will allow us to tile the arrays next to each other
       such that they join seemlessly. */

    stride = subSize / 2;
    fa[(0*size)+0] =
      fa[(subSize*size)+0] =
        fa[(subSize*size)+subSize] =
          fa[(0*size)+subSize] = 0.f;

    /* Now we add ever-increasing detail based on the "diamond" seeded
       values. We loop over stride, which gets cut in half at the
       bottom of the loop. Since it's an int, eventually division by 2
       will produce a zero result, terminating the loop. */
    while (stride) {
		/* Take the existing "square" data and produce "diamond"
		   data. On the first pass through with a 4x4 matrix, the
		   existing data is shown as "X"s, and we need to generate the
	       "*" now:

               X   .   .   .   X

               .   .   .   .   .

               .   .   *   .   .

               .   .   .   .   .

               X   .   .   .   X

	      It doesn't look like diamonds. What it actually is, for the
	      first pass, is the corners of four diamonds meeting at the
	      center of the array. */
		for (i=stride; i<subSize; i+=stride) {
			for (j=stride; j<subSize; j+=stride) {
				if(i > subSize / 2)
				{
					fa[(i * size) + j] =
						scale * fractRand (.5f) +
						avgSquareVals (i, j, stride, size, fa);
				}
				else
				{
					fa[(i * size) + j] =
						mountscale * fractRand (.5f) +
						avgSquareVals (i, j, stride, size, fa);
				}
				j += stride;
			}
			i += stride;
		}

		/* Take the existing "diamond" data and make it into
	       "squares". Back to our 4X4 example: The first time we
	       encounter this code, the existing values are represented by
	       "X"s, and the values we want to generate here are "*"s:

               X   .   *   .   X

               .   .   .   .   .

               *   .   X   .   *

               .   .   .   .   .

               X   .   *   .   X

	       i and j represent our (x,y) position in the array. The
	       first value we want to generate is at (i=2,j=0), and we use
	       "oddline" and "stride" to increment j to the desired value.
	       */
		oddline = 0;
		for (i=0; i<subSize; i+=stride) {
		    oddline = (oddline == 0);
			for (j=0; j<subSize; j+=stride) {
				if ((oddline) && !j) j+=stride;

				/* i and j are setup. Call avgDiamondVals with the
				   current position. It will return the average of the
				   surrounding diamond data points. */
				if(i > subSize / 2)
				{
					fa[(i * size) + j] =
						scale * fractRand (.5f) +
						avgDiamondVals (i, j, stride, size, subSize, fa);
				}
				else
				{
					fa[(i * size) + j] =
						mountscale * fractRand (.5f) +
						avgDiamondVals (i, j, stride, size, subSize, fa);
				}
				/* To wrap edges seamlessly, copy edge values around
				   to other side of array */
				if (i==0)
					fa[(subSize*size) + j] =
						fa[(i * size) + j];
				if (j==0)
					fa[(i*size) + subSize] =
						fa[(i * size) + j];

				j+=stride;
			}
		}

		/* reduce random number range. */
		scale *= ratio;
		mountscale *= mountratio;
		stride >>= 1;
    }
}

//Set the value of a point if it isn't a node
void fillCircleSetPoint(int px, int pz, int size, float y) {

	if(fIsNode[px * size + pz])
		return;

	float& v = fArray[px * size + pz];

	if(y < v)
		return;
	
	v = y;

	fSet[px * size + pz] = true;
}

//When making a new circle, make sure we define every point in that circle as not yet set
void setCircleBaseVal(float *fa, int size, int cx1, int cx2, int cz1, int cz2, float y, float radius, float drop, float limit)
{

	if(y - (drop * 0.98) < limit) {

		int x = 0;
		int z = radius;
		int h = 1 - radius;

		int px, pz;

		do
		{

			pz = cz2 + x;
			if(pz < size - 1) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					float total = 0.0f;
					int numAdjacent = 0;

					if(i > 0 && i < size - 1) {

						fSet[i * size + pz] = false;
					}
				}
			}

			pz = cz1 - x;
			if(pz > 0) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					float total = 0.0f;
					int numAdjacent = 0;

					if(i > 0 && i < size - 1) {

						fSet[i * size + pz] = false;
					}
				}
			}

			if(h < 0) {

				h += x * 2 + 3;
			} else {

				h += 2 * (x-z) + 5;

				pz = cz2 + z;
				if(pz < size - 1) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {

						float total = 0.0f;
						int numAdjacent = 0;
						
						if(i > 0 && i < size - 1) {

							fSet[i * size + pz] = false;
						}
					}
				}

				pz = cz1 - z;
				if(pz > 0) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {

						float total = 0.0f;
						int numAdjacent = 0;
						
						if(i > 0 && i < size - 1) {

							fSet[i * size + pz] = false;
						}
					}
				}

				--z;
			}
			++x;
		} while (z >= x);
	}
}

//First, we create a circle using a fancy algorithm which uses only ints.
//Then, we will fill in the missing pieces using the same algorithm in a filled circle variety.
void fillCircle (float *fa, int size, int cx1, int cx2, int cz1, int cz2, float y, float radius, float drop, float limit, bool fixNodes)
{

	int x = 0;
	int z = radius;
	int h = 1 - radius;

	int px, pz;

	do
	{
		//Setting circle points requires exactly 8 iterations
			//x, y
			//x, -y
			//-x, y
			//-x, -y
			//y, x
			//y, -x
			//-y, x
			//-y, -x
		px = cx2 + x;
		pz = cz2 + z;
		//This is the x, y iteration
		if(px < size && pz < size) {

			fillCircleSetPoint(px, pz, size, y);
		}

		px = cx1 - x;//-x, y
		if(px >= 0 && pz < size) {

			fillCircleSetPoint(px, pz, size, y);
		}

		pz = cz1 - z;//-x, -y
		if(px >= 0 && pz >= 0) {

			fillCircleSetPoint(px, pz, size, y);
		}

		px = cx2 + x;//x, -y
		if(px < size && pz >= 0) {

			fillCircleSetPoint(px, pz, size, y);
		}

		px = cx2 + z;
		pz = cz2 + x;//y, x
		if(px < size && pz < size) {

			fillCircleSetPoint(px, pz, size, y);
		}

		px = cx1 - z;//-y, x
		if(px >= 0 && pz < size) {

			fillCircleSetPoint(px, pz, size, y);
		}

		pz = cz1 - x;//-y, -x
		if(px >= 0 && pz >= 0) {

			fillCircleSetPoint(px, pz, size, y);
		}

		px = cx2 + z;//y, -x
		if(px < size && pz >= 0) {

			fillCircleSetPoint(px, pz, size, y);
		}

		// midpoint is inside line, choose Upper
		if (h < 0) {
			h += x * 2 + 3;
		// midpoint is on/outside line, choose Lower
		} else {
			h += 2 * (x-z) + 5;
			z--;
		}
		x++;
	} while (z >= x);

	x = 0;
	z = radius;
	h = 1 - radius;

	//Set blocks as node values once we hit the correct radius
	if(radius == 5) {//Nodes have a radius of 5 (the flat wooden plank top)

		do
		{

			pz = cz2 + x;
			if(pz < size - 1) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					if(i > 0 && i < size - 1) {

						fIsNode[i * size + pz] = true;
						if(fixNodes)
							fArray[i * size + pz] = y;
					}
				}
			}

			pz = cz1 - x;
			if(pz > 0) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					if(i > 0 && i < size - 1) {

						fIsNode[i * size + pz] = true;
						if(fixNodes)
							fArray[i * size + pz] = y;
					}
				}
			}

			if(h < 0) {

				h += x * 2 + 3;
			} else {

				h += 2 * (x-z) + 5;

				pz = cz2 + z;
				if(pz < size - 1) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {
						
						if(i > 0 && i < size - 1) {

							fIsNode[i * size + pz] = true;
							if(fixNodes)
								fArray[i * size + pz] = y;
						}
					}
				}

				pz = cz1 - z;
				if(pz > 0) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {
						
						if(i > 0 && i < size - 1) {
							
							fIsNode[i * size + pz] = true;
							if(fixNodes)
								fArray[i * size + pz] = y;
						}
					}
				}

				--z;
			}
			++x;
		} while (z >= x);

		//If we're "repairing" where nodes are, and not filling the first pass, end here
		if(fixNodes)
			return;
	}
	
	x = 0;
	z = radius;
	h = 1 - radius;

	if(y - (drop * 0.98) < limit) {

		if(fixNodes)
			return;

		do
		{

			pz = cz2 + x;
			if(pz < size) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					float total = 0.0f;
					int numAdjacent = 0;

					if(i >= 0 && i < size) {

						if(fIsNode[i * size + pz]) {

							fBlock[i * size + pz] = BLOCK_WOOD;
							continue;
						}
						else {

							fBlock[i * size + pz] = BLOCK_MOUNTAIN;
						}

						float& v = fArray[i * size + pz];

						if(i > 0 && fSet[(i - 1) * size + pz]) {

							total += fArray[(i - 1) * size + pz];
							++numAdjacent;
						}
						if(pz < size - 1 && fSet[i * size + pz + 1]) {

							total += fArray[i * size + pz + 1];
							++numAdjacent;
						}
						if(fSet[i * size + pz - 1]) {

							total += fArray[i * size + pz - 1];
							++numAdjacent;
						}
						if(i < size - 1 && fSet[(i + 1) * size + pz]) {

							total += fArray[(i + 1) * size + pz];
							++numAdjacent;
						}

						if(numAdjacent > 1)
							v = total / (float)(numAdjacent);
					}
				}
			}

			pz = cz1 - x;
			if(pz >= 0) {

				for(int i = cx1 - z; i <= cx2 + z; ++i) {

					float total = 0.0f;
					int numAdjacent = 0;

					if(i >= 0 && i < size) {

						if(fIsNode[i * size + pz]) {

							fBlock[i * size + pz] = BLOCK_WOOD;
							continue;
						}
						else {

							fBlock[i * size + pz] = BLOCK_MOUNTAIN;
						}

						float& v = fArray[i * size + pz];

						if(i > 0 && fSet[(i - 1) * size + pz]) {

							total += fArray[(i - 1) * size + pz];
							++numAdjacent;
						}
						if(fSet[i * size + pz + 1]) {

							total += fArray[i * size + pz + 1];
							++numAdjacent;
						}
						if(pz > 0 && fSet[i * size + pz - 1]) {

							total += fArray[i * size + pz - 1];
							++numAdjacent;
						}
						if(i < size - 1 && fSet[(i + 1) * size + pz]) {

							total += fArray[(i + 1) * size + pz];
							++numAdjacent;
						}

						if(numAdjacent > 1)
							v = total / (float)(numAdjacent);
					}
				}
			}

			if(h < 0) {

				h += x * 2 + 3;
			} else {

				h += 2 * (x-z) + 5;

				pz = cz2 + z;
				if(pz < size) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {

						float total = 0.0f;
						int numAdjacent = 0;
						
						if(i >= 0 && i < size) {

							if(fIsNode[i * size + pz]) {

								fBlock[i * size + pz] = BLOCK_WOOD;
								continue;
							}
							else {

								fBlock[i * size + pz] = BLOCK_MOUNTAIN;
							}

							float& v = fArray[i * size + pz];

							if(i > 0 && fSet[(i - 1) * size + pz]) {

								total += fArray[(i - 1) * size + pz];
								++numAdjacent;
							}
							if(pz < size - 1 && fSet[i * size + pz + 1]) {

								total += fArray[i * size + pz + 1];
								++numAdjacent;
							}
							if(fSet[i * size + pz - 1]) {

								total += fArray[i * size + pz - 1];
								++numAdjacent;
							}
							if(i < size - 1 && fSet[(i + 1) * size + pz]) {

								total += fArray[(i + 1) * size + pz];
								++numAdjacent;
							}

							if(numAdjacent > 1)
								v = total / (float)(numAdjacent);
						}
					}
				}

				pz = cz1 - z;
				if(pz >= 0) {

					for(int i = cx1 - x; i <= cx2 + x; ++i) {

						float total = 0.0f;
						int numAdjacent = 0;
						
						if(i >= 0 && i < size) {

							if(fIsNode[i * size + pz]) {

								fBlock[i * size + pz] = BLOCK_WOOD;
								continue;
							}
							else {

								fBlock[i * size + pz] = BLOCK_MOUNTAIN;
							}

							float& v = fArray[i * size + pz];

							if(i > 0 && fSet[(i - 1) * size + pz]) {

								total += fArray[(i - 1) * size + pz];
								++numAdjacent;
							}
							if(fSet[i * size + pz + 1]) {

								total += fArray[i * size + pz + 1];
								++numAdjacent;
							}
							if(pz > 0 && fSet[i * size + pz - 1]) {

								total += fArray[i * size + pz - 1];
								++numAdjacent;
							}
							if(i < size - 1 && fSet[(i + 1) * size + pz]) {

								total += fArray[(i + 1) * size + pz];
								++numAdjacent;
							}

							if(numAdjacent > 1)
								v = total / (float)(numAdjacent);
						}
					}
				}

				--z;
			}
			++x;
		} while (z >= x);
	}
}

//radius is how far out to go from the central four points. drop is how much the y value decreases for each vertex out. limit is the lowest level to go to.
void fillSquare (float *fa, int size, int cx1, int cx2, int cz1, int cz2, float y, float radius, float drop, float limit)
{
	if(y - (drop * radius) < limit)
		return;

	int i, j;
	j = cz1 - radius;
	if(j >= 0) {
		for(i = cx1 - radius; i <= cx2 + radius; ++i) {

			if(i < 0 || i >= size)
				continue;

			float& v = fArray[i * size + j];

			if(fIsNode[i * size + j])
				continue;
			if(y - (drop * radius) < v)
				continue;
			
			v = y - (drop * radius);
		}
	}

	j = cz2 + radius;
	if(j < size) {
		for(i = cx1 - radius; i <= cx2 + radius; ++i) {

			if(i < 0 || i >= size)
				continue;

			float& v = fArray[i * size + j];

			if(fIsNode[i * size + j])
				continue;
			if(y - (drop * radius) < v)
				continue;
			
			v = y - (drop * radius);
		}
	}

	i = cx1 - radius;
	if(i > 0) {
		for(j = cz1 - radius + 1; j < cz2 + radius; ++j) {

			if(j < 0 || j >= size)
				continue;

			float& v = fArray[i * size + j];

			if(fIsNode[i * size + j])
				continue;
			if(y - (drop * radius) < v)
				continue;
			
			v = y - (drop * radius);
		}
	}

	i = cx2 + radius;
	if(i < size) {
		for(j = cz1 - radius + 1; j < cz2 + radius; ++j) {

			if(j < 0 || j >= size)
				continue;

			float& v = fArray[i * size + j];

			if(fIsNode[i * size + j])
				continue;
			if(y - (drop * radius) < v)
				continue;
			
			v = y - (drop * radius);
		}
	}

	fillSquare(fa, size, cx1, cx2, cz1, cz2, y, radius + 1, drop, limit);
}

//nodes holds nodes with an x, y, z. x/z are in the range -100 to 100
//nodes should be sorted with highest points at i=0 and lowest points at i=n
void fill2DArray (float *fa, int size, vector<Point>& nodes, vector<bool>& fixNode, bool fixNodes)
{
    int	i, j;
	int index = -1;
	if(!fixNodes) {

		for(i = 0; i < size; ++i) {
			for(j = 0; j < size; ++j) {
				++index;
				fArray[index] = 0.1f;
				fIsNode[index] = false;
				fSet[index] = false;
				fBlock[index] = BLOCK_PLAINS;
			}
		}
	}

	for(i = 0; i < nodes.size(); ++i) {

		if((i + 1) % 100 == 0 && !fixNodes)
			cout << "Creating mountain " << i + 1 << " of " << nodes.size() << endl;

		float x = nodes[i]._x + 100;
		x = (x / 200.0f) * (float)(size - 1);
		float z = nodes[i]._z + 100;
		z = (z / 200.0f) * (float)(size - 1);
		float y = nodes[i]._y;

		int cx1 = (int)x;
		int cx2 = cx1 + 1;
		int cz1 = (int)z;
		int cz2 = cz1 + 1;

		float radius = 1;
		double drop = 0.01 + (0.02 * y);
		double dropLimit = 0.0001;
		float limit = 0.1f;
		float tempy;

		if(cx2 < size && cz2 < size) {

			float& c11 = fArray[cx1 * size + cz1];
			float& c12 = fArray[cx1 * size + cz2];
			float& c21 = fArray[cx2 * size + cz1];
			float& c22 = fArray[cx2 * size + cz2];
			
			if((fIsNode[cx1 * size + cz1] || fIsNode[cx1 * size + cz2] || fIsNode[cx2 * size + cz1] || fIsNode[cx2 * size + cz2]) && (!fixNodes || !fixNode[i])) {

				fixNode[i] = false;
				continue;
			}

			if(!fixNodes) {

				c11 = y;
				c12 = y;
				c21 = y;
				c22 = y;
			}
			else {

				if(c11 > c12 && c11 > c21 && c11 > c22)
					y = c12 = c21 = c22 = c11;
				else if(c12 > c21 && c12 > c22)
					y = c11 = c21 = c22 = c12;
				else if(c21 > c22)
					y = c11 = c12 = c22 = c21;
				else
					y = c11 = c12 = c21 = c22;
			}

			tempy = y;
			
			while(tempy >= limit) {

				fillCircle(fa, size, cx1, cx2, cz1, cz2, tempy, radius, drop, limit, fixNodes);
				
				if(radius > 5) {

					tempy -= drop;
					drop *= 0.98;
					if(drop < dropLimit)
						drop = dropLimit;
					if(fixNodes)
						tempy = 0;
				}
				++radius;
			}

			tempy = y;
			radius = 1;
			drop = 0.01 + (0.02 * y);

			if(!fixNodes) {

				while(tempy >= limit) {

					setCircleBaseVal(fa, size, cx1, cx2, cz1, cz2, tempy, radius, drop, limit);
				
					if(radius > 5) {

						tempy -= drop;
						drop *= 0.98;
						if(drop < dropLimit)
							drop = dropLimit;
					}
					++radius;
				}
			}
		}
		else if(cx2 < size) {

			float& c11 = fArray[cx1 * size + cz1];
			float& c21 = fArray[cx2 * size + cz1];
			
			if((fIsNode[cx1 * size + cz1] || fIsNode[cx2 * size + cz1]) && (!fixNodes || !fixNode[i])) {

				fixNode[i] = false;
				continue;
			}

			c11 = y;
			c21 = y;

			tempy = y;

			while(tempy >= limit) {

				fillCircle(fa, size, cx1, cx2, cz1, cz1, tempy, radius, drop, limit, fixNodes);
				
				if(radius > 5) {

					tempy -= drop;
					drop *= 0.98;
					if(drop < dropLimit)
						drop = dropLimit;
					if(fixNodes)
						tempy = 0;
				}
				++radius;
			}

			tempy = y;
			radius = 1;
			drop = 0.01 + (0.02 * y);

			if(!fixNodes) {

				while(tempy >= limit) {

					setCircleBaseVal(fa, size, cx1, cx2, cz1, cz2, tempy, radius, drop, limit);
				
					if(radius > 5) {

						tempy -= drop;
						drop *= 0.98;
						if(drop < dropLimit)
							drop = dropLimit;
					}
					++radius;
				}
			}
		}
		else if(cz2 < size) {

			float& c11 = fArray[cx1 * size + cz1];
			float& c12 = fArray[cx1 * size + cz2];
			
			if((fIsNode[cx1 * size + cz1] || fIsNode[cx1 * size + cz2]) && (!fixNodes || !fixNode[i])) {

				fixNode[i] = false;
				continue;
			}

			c11 = y;
			c12 = y;

			tempy = y;
			
			while(tempy >= limit) {

				fillCircle(fa, size, cx1, cx1, cz1, cz2, tempy, radius, drop, limit, fixNodes);
				
				if(radius > 5) {

					tempy -= drop;
					drop *= 0.98;
					if(drop < dropLimit)
						drop = dropLimit;
					if(fixNodes)
						tempy = 0;
				}
				++radius;
			}

			tempy = y;
			radius = 1;
			drop = 0.01 + (0.02 * y);

			if(!fixNodes) {

				while(tempy >= limit) {

					setCircleBaseVal(fa, size, cx1, cx2, cz1, cz2, tempy, radius, drop, limit);
				
					if(radius > 5) {

						tempy -= drop;
						drop *= 0.98;
						if(drop < dropLimit)
							drop = dropLimit;
					}
					++radius;
				}
			}
		}
		else {

			float& c11 = fArray[cx1 * size + cz1];
			
			if((fIsNode[cx1 * size + cz1]) && (!fixNodes || !fixNode[i])) {

				fixNode[i] = false;
				continue;
			}

			c11 = y;

			tempy = y;
			
			while(tempy >= limit) {

				fillCircle(fa, size, cx1, cx1, cz1, cz1, tempy, radius, drop, limit, fixNodes);
				
				if(radius > 5) {

					tempy -= drop;
					drop *= 0.98;
					if(drop < dropLimit)
						drop = dropLimit;
					if(fixNodes)
						tempy = 0;
				}
				++radius;
			}

			tempy = y;
			radius = 1;
			drop = 0.01 + (0.02 * y);

			if(!fixNodes) {

				while(tempy >= limit) {

					setCircleBaseVal(fa, size, cx1, cx2, cz1, cz2, tempy, radius, drop, limit);
				
					if(radius > 5) {

						tempy -= drop;
						drop *= 0.98;
						if(drop < dropLimit)
							drop = dropLimit;
					}
					++radius;
				}
			}
		}
	}
}

bool writeSchematic = true;
double bridgeWidth = 2.0;
bool sortNodes (Point i, Point j) {

	return i._y > j._y;
}

vector<Point> nodes;

void initTerrain()
{
	cout << "Enter file name for input:\n";
	char filename[100];
	cin >> filename;

	ifstream dFile;
	dFile.open(filename);

	//read in line, seperate by ','
	char line[2000];

	while(true)
	{
		dFile.getline(line, 2000);

		if(dFile.eof()) 
			break;
		string temp(line);

		if(temp.find("DisplayName") != temp.npos) {
			int pos = temp.find_first_of("\"") + 1;
			string toPushName(temp.substr(pos, temp.find_first_of("\"", pos) - pos));

			dFile.getline(line, 2000);
			string fileName(line);

			int filePos = fileName.find_first_of("\"") + 1;
			string toPushFile(fileName.substr(filePos, fileName.find_first_of("\"", filePos) - filePos));

			dFile.getline(line, 2000);
			string earl(line);

			int urlPos = earl.find_first_of("\"");
			urlPos++;
			int urlPos2 = earl.find_first_of("\"", urlPos);

			dFile.getline(line, 2000);
			string coordinates(line);

			int parenPos = coordinates.find_first_of("(") + 1;
			coordinates = coordinates.substr(parenPos, coordinates.find_first_of(")") - parenPos);

			cout << "1" << endl;

			int commaPos = coordinates.find_first_of(",");
			float xp = atof(coordinates.substr(0, commaPos).c_str());
			
			commaPos++;
			int commaPos2 = coordinates.find_first_of(",", commaPos);
			float yp = atof(coordinates.substr(commaPos, commaPos2).c_str());

			float zp = atof(coordinates.substr(commaPos2 + 1, 100).c_str());

			string url(earl.substr(urlPos, urlPos2 - urlPos));
			
			Point p(xp, yp, zp, url, toPushName, toPushFile);

			while(true) 
			{
				dFile.getline(line, 2000);

				string temp(line);

				if(temp.find("Child Coordinates") != temp.npos)
				{
					string childCoord = line;
					Coord c;

					int commaPos3 = childCoord.find_first_of(",");
					float xp = atof(childCoord.substr(0, commaPos3).c_str());

					commaPos3++;
					int commaPos4 = childCoord.find_first_of(",", commaPos3);
					float yp = atof(childCoord.substr(commaPos3, commaPos4).c_str());

					float zp = atof(childCoord.substr(commaPos4 + 1, 100).c_str());

					c.x = xp;
					c.y = yp;
					c.z = zp;

					p.children.push_back(c);
				}
				else
					break;
			}
			
			nodes.push_back(p);
		}
	}

	cout << "Enter Level of Detail (9, 10, or 11):\n";
	int det;
	cin >> det;

	detail = det;
	size = (1 << detail) + 1;

	int randomSeed = time(NULL);
	float surfaceH = 0.6f;
	float mountH = 0.6f;

	fArray = alloc2DArray (size);
	if (fArray==NULL) 
		return;

	int i = 0;
	/*
	ifstream dFile;
	dFile.open("khanXYZ.txt");

	//read in line, seperate by ','
	char line[2000];

	while(true)
	{
		dFile.getline(line, 2000);

		//cout << line << endl;
		if(dFile.eof()) 
			break;

		string temp(line);
		int pos = temp.find_first_of(" ") + 1;
		string pointName = temp.substr(0, pos - 2);
		temp = temp.substr(pos, temp.length() - pos);
		pos = temp.find_first_of(" ") + 1;
		float xp = atof(temp.substr(0, pos - 1).c_str());
		temp = temp.substr(pos, temp.length() - pos);
		pos = temp.find_first_of(" ") + 1;
		float zp = atof(temp.substr(0, pos - 1).c_str());
		float yp = atof(temp.substr(pos, temp.length() - pos).c_str());

		Point p(xp, yp, zp);
		p.name = pointName;
		nodes.push_back(p);
	}
	*/
	vector<bool> fixNode;
	std::sort (nodes.begin(), nodes.end(), sortNodes);
	int j;
	float minx = 256;
	float maxx = -256;
	float minz = 256;
	float maxz = -256;
	float maxy = 0;
	float miny = 3;
	for(i = 0; i < nodes.size(); ++i) {
		fixNode.push_back(true);
		if(nodes[i]._x < minx)
			minx = nodes[i]._x;
		if(nodes[i]._x > maxx)
			maxx = nodes[i]._x;
		if(nodes[i]._z < minz)
			minz = nodes[i]._z;
		if(nodes[i]._z > maxz)
			maxz = nodes[i]._z;
		if(nodes[i]._y > maxy)
			maxy = nodes[i]._y;
		if(nodes[i]._y < miny)
			miny = nodes[i]._y;
	}
	
	float xshift = maxx - ((maxx - minx) / 2.0f);
	float zshift = maxz - ((maxz - minz) / 2.0f);
	maxx -= xshift;
	maxz -= zshift;

	cout << "Enter X Multiplier:\n";
	float xIn;
	cin >> xIn;

	cout << "Enter Z Multiplier:\n";
	float zIn;
	cin >> zIn;

	//float xmult = 80.0f / maxx;
	//float zmult = 80.0f / maxz;
	float xmult = xIn / maxx;
	float zmult = zIn / maxz;
	float ymult = 3.0f / (maxy - miny);

	cout << "Miny = " << miny << endl;
	cout << "Maxy = " << maxy << endl;

	for(i = 0; i < nodes.size(); ++i) {
		nodes[i]._x -= xshift;
		nodes[i]._x *= xmult;
		nodes[i]._y -= miny;
		nodes[i]._y *= ymult;
		nodes[i]._y += 0.4;
		nodes[i]._z -= zshift;
		nodes[i]._z *= zmult;

		for(j = 0; j <  nodes[i].children.size(); ++j) {
			nodes[i].children[j].x -= xshift;
			nodes[i].children[j].x *= xmult;
			nodes[i].children[j].y -= miny;
			nodes[i].children[j].y *= ymult;
			nodes[i].children[j].y += 0.4;
			nodes[i].children[j].z -= zshift;
			nodes[i].children[j].z *= zmult;
		}
	}
	fill2DArray (fArray, size, nodes, fixNode, false);
	fill2DFractArray (fFractArray, size, randomSeed, DEF_HEIGHT_SCALE, surfaceH, mountH);
	for(i = 0; i < size; ++i) {
		for(j = 0; j < size; ++j) {
			fFractArray[i * size + j] = fFractArray[i * size + j] < 0 ? -fFractArray[i * size + j] : fFractArray[i * size + j];
			float val = ((fFractArray[i * size + j] * 3.0f) * (fArray[i * size + j] > 0.1001f ? min(((fArray[i * size + j] - 0.1f) / 6.0f) + 0.1f, 0.3f) : fArray[i * size + j]) * 4.0f);
			fArray[i * size + j] += val;
			fArray[i * size + j] -= 0.1;
		}
	}
	fill2DArray (fArray, size, nodes, fixNode, true);

	for(i = 0; i < nodes.size(); ++i) {

		nodes[i]._x += 100;
		nodes[i]._x = (nodes[i]._x / 200.0f) * (float)(size - 1);
		nodes[i]._z += 100;
		nodes[i]._z = (nodes[i]._z / 200.0f) * (float)(size - 1);
		nodes[i]._y = (int)(fArray[(int)nodes[i]._x * size + (int)nodes[i]._z] * 18.0f);
		
		for(j = 0; j < nodes[i].children.size(); ++j) {

			nodes[i].children[j].x += 100;
			nodes[i].children[j].x = (nodes[i].children[j].x / 200.0f) * (float)(size - 1);
			nodes[i].children[j].z += 100;
			nodes[i].children[j].z = (nodes[i].children[j].z / 200.0f) * (float)(size - 1);
			nodes[i].children[j].y = (int)(fArray[(int)nodes[i].children[j].x * size + (int)nodes[i].children[j].z] * 18.0f);
		}
		
	}

	for(i = 0; i < nodes.size(); ++i)
	{
		cout << "Parent Coord: " << nodes[i]._x << "," << nodes[i]._y << "," << nodes[i]._z << endl;

		for(j = 0; j < nodes[i].children.size(); ++j)
		{
			cout << "Child Coord: " << nodes[i].children[j].x << "," << nodes[i].children[j].y << "," << nodes[i].children[j].z << endl;
		}
	}

	//Now that the array is created, make the output file.
	//Filename = x,z,y
	//Block ID's written in binary, ordered y,z,x
	
	if(writeSchematic) {

		int charSize = size * size * 100 * sizeof(char);
		char* schematic = (char*) malloc(charSize);

		int x, y, z;
		
		for(y = 0; y < 100; ++y) {

			if(y % 10 == 0)
				cout << y << endl;

			for(z = 0; z < size; ++z) {

				for(x = 0; x < size; ++x) {

					int height = (int)(fArray[x * size + z] * 18.0f);
					if(height > y - 11)
						if(height <= 0)
							schematic[y * size * size + z * size + x] = BLOCK_DIRT;
						else
							schematic[y * size * size + z * size + x] = fBlock[x * size + z];
					else if(y > 11)
						schematic[y * size * size + z * size + x] = BLOCK_NONE;
					else
						schematic[y * size * size + z * size + x] = BLOCK_WATER;
				}
			}
		}

		//Now make bridges
		for(i = 0; i < nodes.size(); ++i) {

            for(j = 0; j < nodes[i].children.size(); ++j) {

                double x = nodes[i].children[j].x - nodes[i]._x;
                double y = nodes[i].children[j].y - nodes[i]._y;
                double z = nodes[i].children[j].z - nodes[i]._z;

				if(abs(nodes[i]._x - nodes[i].children[j].x) > 250 || abs(nodes[i]._z - nodes[i].children[j].z) > 250) {

					cout << nodes[i]._fileName.c_str() << " has child " << nodes[i].children[j].fileName.c_str() << endl;
					cout << nodes[i]._x << ", " << nodes[i]._z << " --> " << nodes[i].children[j].x << ", " << nodes[i].children[j].z << endl;
				}

                double startx = nodes[i]._x;
                double endx = nodes[i].children[j].x;
                double starty = nodes[i]._y;
                double endy = nodes[i].children[j].y;
                double startz = nodes[i]._z;

                double xmod = 0.0;
                double zmod = 0.0;
                double dxmod = 0.0;
                double dzmod = 0.0;

                if(abs(x) >= abs(z)) {

                    dxmod = -z / (2.0*abs(x));
                    dzmod = x / (2.0*abs(x));
                    xmod = -dxmod * bridgeWidth * 2.0;
                    zmod = -dzmod * bridgeWidth * 2.0;
                }
                else {

                    dxmod = -z / (2.0*abs(z));
                    dzmod = x / (2.0*abs(z));
                    xmod = -dxmod * bridgeWidth * 2.0;
                    zmod = -dzmod * bridgeWidth * 2.0;
                }

                double loopby;
                if(abs(x) >= abs(y) && abs(x) >= abs(z))
                    loopby = x;
                else if(abs(y) >= abs(z))
                    loopby = y;
                else
                    loopby = z;

                double dx = x / abs(loopby);
                double dy = y / abs(loopby);
                double dz = z / abs(loopby);
                int steps = abs(loopby) + 1;

                x = nodes[i]._x;
                y = nodes[i]._y + 11;
                z = nodes[i]._z;
                double originalx = x;
                double originaly = y;
                double originalz = z;

                for(int l = 0; l < (int)(bridgeWidth * 4.0); ++l) {

                    for(int k = 0; k < steps; ++k) {

                        schematic[(int)y * size * size + (int)(z + zmod) * size + (int)(x + xmod)] = BLOCK_COBBLE;

                        x += dx;
                        y += dy;
                        z += dz;
                    }

                    xmod += dxmod;
                    x = originalx;
                    y = originaly;
                    z = originalz;

                    for(int k = 0; k < steps; ++k) {

                        schematic[(int)y * size * size + (int)(z + zmod) * size + (int)(x + xmod)] = BLOCK_COBBLE;

                        x += dx;
                        y += dy;
                        z += dz;
                    }

                    zmod += dzmod;
                    x = originalx;
                    y = originaly;
                    z = originalz;
                }
            }
        }

		char fileName[50];
		sprintf(fileName, "%d,%d,100.schematic", size, size);

		ofstream mcEditFile(fileName, ios::out | ios::binary);
		for(y = 0; y < 100; ++y) {

			//if(y % 10 == 0)
				//cout << y << endl;

			for(z = 0; z < size; ++z) {

				for(x = 0; x < size; ++x) {

					mcEditFile.write(&(schematic[y * size * size + z * size + x]), 1);
				}
			}
		}

		mcEditFile.close();
	}

	

	ofstream oFile;
	oFile.open(filenm);

	cout << "Writing..." << endl;

	for( int l = 0; l < nodes.size(); l++ )
	{
		oFile << nodes[l]._disName.c_str() <<  ", " << nodes[l]._fileName.c_str() <<  ", " << nodes[l]._x << ", " << nodes[l]._y << ", " << nodes[l]._z << ", " << nodes[l]._url.c_str() << endl;
	}

	cout << "Done writing..." << endl;
	oFile.close();
	
	ScriptGen s;

}

void init()
{
	initTerrain();
}

/*
 * fill1DFractArray - Tessalate an array of values into an
 * approximation of fractal Brownian motion.
 */
void fill1DFractArray (float *fa, int size,
		       int seedValue, float heightScale, float h)
{
    int	i;
    int	stride;
    int subSize;
	float ratio, scale;

    if (!powerOf2(size) || (size==1)) {
	/* We can't tesselate the array if it is not a power of 2. */
	return;
    }

    /* subSize is the dimension of the array in terms of connected line
       segments, while size is the dimension in terms of number of
       vertices. */
    subSize = size;
    size++;
    
    /* initialize random number generator */
    srandom (time(NULL));


	/* Set up our roughness constants.
	   Random numbers are always generated in the range 0.0 to 1.0.
	   'scale' is multiplied by the randum number.
	   'ratio' is multiplied by 'scale' after each iteration
	   to effectively reduce the randum number range.
	   */
	ratio = (float) pow(2.f,-h);
	scale = heightScale * ratio;

    /* Seed the endpoints of the array. To enable seamless wrapping,
       the endpoints need to be the same point. */
    stride = subSize / 2;
    fa[0] =
      fa[subSize] = 0.f;

    while (stride) {
		for (i=stride; i<subSize; i+=stride) {
			fa[i] = scale * fractRand (.5f) +
				avgEndpoints (i, stride, fa);

			/* reduce random number range */
			scale *= ratio;

			i+=stride;
		}
		stride >>= 1;
    }
}

int mapScale = 20;

void display(void)
{

	/* Adjust camera */
	glLoadIdentity();
	glRotatef (gamma, 0.0, 0.0, 1.0);
	glRotatef (beta, 1.0, 0.0, 0.0);
	glRotatef (alpha, 0.0, 1.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 1.0, 0.0);
	
	GLfloat* tempPt = new GLfloat;

	int index = 0;
	int mapPosX = (size - 1) - ((posX * (size - 1) / (mapScale * 2)) + (size - 1) / 2);
	int mapPosZ = (size - 1) - ((posZ * (size - 1) / (mapScale * 2)) + (size - 1) / 2);
	for (GLfloat i = (mapPosX > sight ? mapPosX - sight : 0); i < (mapPosX < size - sight ? mapPosX + sight : size); i++)
	{
		index = i*size;
		index += (mapPosZ > sight ? mapPosZ - sight : 0);
		glBegin(GL_LINE_STRIP);
		for(GLfloat j = (mapPosZ > sight ? mapPosZ - sight : 0); j < (mapPosZ < size - sight ? mapPosZ + sight : size); j++)
		{
			tempPt[0] = (-mapScale) + ((float)(mapScale * 2)*i)/size;
			tempPt[1] = fArray[index];
			tempPt[2] = (-mapScale) + ((float)(mapScale * 2)*j)/size;

			if(fIsNode[index])
				glColor3f(1.0, 0.0, 0.0);
			else if(tempPt[1]>.3)
				glColor3f(tempPt[1], tempPt[1], tempPt[1]);
			else if(tempPt[1]<0.1)
				glColor3f(0.2, 0.2, 0.8 + tempPt[1]*2);
			else
				glColor3f(0.0, (tempPt[1]) + 0.5, 0.0);

			tempPt[0] += posX;
			tempPt[1] += posY;
			tempPt[2] += posZ;

			glVertex3fv(tempPt);
			index++;
		}
		glEnd();
	}	

	index = 0;
	for (GLfloat j = (mapPosZ > sight ? mapPosZ - sight : 0); j < (mapPosZ < size - sight ? mapPosZ + sight : size); j++)
	{
		index = j;
		index += (mapPosX > sight ? (mapPosX) - sight : 0) * size;
		glBegin(GL_LINE_STRIP);
		for(GLfloat i = (mapPosX > sight ? (mapPosX) - sight : 0); i < (mapPosX < size - sight ? mapPosX + sight : size); i++)
		{
			tempPt[0] = (-mapScale) + ((float)(mapScale * 2)*i)/size;
			tempPt[1] = fArray[index];
			tempPt[2] = (-mapScale) + ((float)(mapScale * 2)*j)/size;

			if(fIsNode[index])
				glColor3f(1.0, 0.0, 0.0);
			else if(tempPt[1]>.3)
				glColor3f(tempPt[1], tempPt[1], tempPt[1]);
			else if(tempPt[1]<0.1)
				glColor3f(0.2, 0.2, 0.8 + tempPt[1]*2);
			else
				glColor3f(0.0, (tempPt[1]) + 0.5, 0.0);

			tempPt[0] += posX;
			tempPt[1] += posY;
			tempPt[2] += posZ;

			glVertex3fv(tempPt);
			index+=(size);
		}
		glEnd();
	}
	
	glFlush();   
}

void reshape(int w, int h)
{
	//glClearColor(0.0, 0.0, 0.0, 1.0);	
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-3.0, 3.0, -3.0*(GLfloat)h/(GLfloat)w, 
				3.0*(GLfloat)h/(GLfloat)w, -60.0, 60.0);
	else
		glOrtho(-3.0*(GLfloat)w/(GLfloat)h, 
				3.0*(GLfloat)w/(GLfloat)h, -3.0, 3.0, -60.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
   	/* Adjust camera */
	glRotatef (gamma, 0.0, 0.0, 1.0);
	glRotatef (beta, 1.0, 0.0, 0.0);
	glRotatef (alpha, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'a':
		alpha-=1;
		glutPostRedisplay();
		break;
	case 'd':
		alpha+=1;
		glutPostRedisplay();
		break;
	case 'w':
		posZ+=(cos(alpha * PI / 180.0f) * 0.2f);
		posX-=(sin(alpha * PI / 180.0f) * 0.2f);
		glutPostRedisplay();
		break;
	case 's':
		posZ-=(cos(alpha * PI / 180.0f) * 0.2f);
		posX+=(sin(alpha * PI / 180.0f) * 0.2f);
		glutPostRedisplay();
		break;
	case 'm':
		sight += 20;
		glutPostRedisplay();
		break;
	case 'l':
		sight -= 20;
		glutPostRedisplay();
		break;
	}
}

int main(int argc, char** argv)
{
	cout << "Test 1" << endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize (500, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	//glClearColor(0.0, 0.0, 0.0, 1.0);	
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cout << "Test 2" << endl;
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutMainLoop();
	return 0;
}
