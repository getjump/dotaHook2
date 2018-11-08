#include <math.h>
#include <Windows.h>

#define sqr(x) ((x) * (x))

template<typename T>
struct Vector2;

template<typename T>
struct Vector3
{
	T x, y, z;

	Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
	Vector3(Vector2<T>* vec) : x(vec->x), y(vec->y), z(0) {}

	T dist(const Vector3<T> &to)
	{
		Vector3<T> from = *this;
		return sqrt(sqr(from.x - to.x) + sqr(from.y - to.y) + sqr(from.z - to.z));
	}
};

template<typename T>
struct Vector2
{
	T x, y;

	Vector2(T x, T y) : x(x), y(y) {}
	Vector2() {}

	Vector3<T>* operator &() 
	{
		return new Vector3<T>(this);
	}
};

typedef int(__fastcall *WorldToScreenFn)(Vector3<float> *pWorld, int *pOutX, int *pOutY, OPTIONAL Vector3<float> * pOffset);
int(__fastcall *WorldToScreen)(Vector3<float> *pWorld, int *pOutX, int *pOutY, OPTIONAL Vector3<float> * pOffset);

Vector2<float> WorldToScreenF(Vector3<float> dtWorld) {
	int x, y;
	WorldToScreen(&dtWorld, &x, &y, 0);
	return Vector2<float>(x, y);
}

int WorldToScreenF(Vector3<float> dtWorld, Vector3<float> * dtOut) {
	int x, y;
	int iRet = WorldToScreen(&dtWorld, &x, &y, 0);
	dtOut->x = x;
	dtOut->y = y;
	return iRet;
}