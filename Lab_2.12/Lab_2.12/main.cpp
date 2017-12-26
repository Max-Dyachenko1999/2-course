#include <vector>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <windows.h>
using namespace std;

const char szProgName[] = "Lab_2.12";
const double EPS = 1E-9;
#define det(a,b,c,d)  (a*d-b*c)

//#define min(a,b) (a<b?a:b)
//#define max(a,b) (a<b?b:a)

const int cnt = 100;
const int WindX = 200;
const int WindY = 20;
const int Width = 1000;
const int Height = 700;
const int border = 100;
const int interval = 10;
const int radius = 4;

struct Point
{
	double x, y;
	Point() {}
	Point(double _x, double _y) : x(_x), y(_y) {};
	bool operator< (const Point & p) const
	{
		return x < p.x - EPS || (abs(x - p.x) < EPS && y < p.y - EPS);
	}
	bool operator> (const Point & p) const
	{
		return x > p.x - EPS || (abs(x - p.x) < EPS && y > p.y - EPS);
	}
};
struct Line
{
	Point p1, p2;
	Line(Point _p1, Point _p2) : p1(_p1), p2(_p2) {};
};
struct line_vec
{
	double a, b, c;
	line_vec() {}
	line_vec(Point p, Point q)
	{
		a = p.y - q.y;
		b = q.x - p.x;
		c = -a * p.x - b * p.y;
		norm();
	}
	void norm()
	{
		double z = sqrt(a*a + b*b);
		if (abs(z) > EPS) a /= z, b /= z, c /= z;
	}
	double dist(Point p) const
	{
		return a * p.x + b * p.y + c;
	}
};

bool Compare(Line l1, Line l2)
{
	double s1 = sqrt((l1.p1.x - l1.p2.x)*(l1.p1.x - l1.p2.x) + (l1.p1.y - l1.p2.y)*(l1.p1.y - l1.p2.y));
	double s2 = sqrt((l2.p1.x - l2.p2.x)*(l2.p1.x - l2.p2.x) + (l2.p1.y - l2.p2.y)*(l2.p1.y - l2.p2.y));
	return s1 < s2;
}

inline bool betw(double l, double r, double x)
{
	return min(l, r) <= x + EPS && x <= max(l, r) + EPS;
}
inline bool intersect_1d(double a, double b, double c, double d)
{
	if (a > b)  swap(a, b);
	if (c > d)  swap(c, d);
	return max(a, c) <= min(b, d) + EPS;
}
bool intersect(Point a, Point b, Point c, Point d)
{
	Point left; Point right;
	if (!intersect_1d(a.x, b.x, c.x, d.x) || !intersect_1d(a.y, b.y, c.y, d.y))
		return false;
	line_vec m(a, b);
	line_vec n(c, d);
	double zn = det(m.a, m.b, n.a, n.b);
	if (abs(zn) < EPS)
	{
		if (abs(m.dist(c)) > EPS || abs(n.dist(a)) > EPS)
			return false;
		if (b < a)  swap(a, b);
		if (d < c)  swap(c, d);
		left = max(a, c);
		right = min(b, d);
		return true;
	}
	else
	{
		left.x = right.x = -det(m.c, m.b, n.c, n.b) / zn;
		left.y = right.y = -det(m.a, m.c, n.a, n.c) / zn;
		return betw(a.x, b.x, left.x)
			&& betw(a.y, b.y, left.y)
			&& betw(c.x, d.x, left.x)
			&& betw(c.y, d.y, left.y);
	}
}

vector<Point> CreatePoints(int N)
{
	srand((size_t)time(NULL));
	vector<Point> points;
	for (int i = 0; i < N; i++)
	{
		bool good = true;
		int x = ((border / 2 + (rand() % (Width - border))) / interval) * interval;
		int y = ((border / 2 + (rand() % (Height - border))) / interval) * interval;
		Point p(x, y);
		for (size_t j = 0; j < points.size(); j++)
		{
			if (p.x == points[j].x && p.y == points[j].y)
			{
				i--;
				good = false;
				break;
			}
		}
		if (good) points.push_back(p);
	}
	return points;
}
vector<Line> Triangulation(vector<Point> points)
{
	vector<Line> lines;
	for (size_t i = 0; i < points.size(); i++)
		for (size_t j = 0; j < i; j++)
		{
			Point a = Point(points[i].x + (points[j].x - points[i].x) / 100, points[i].y + (points[j].y - points[i].y) / 100);
			Point b = Point(points[j].x + (points[i].x - points[j].x) / 100, points[j].y + (points[i].y - points[j].y) / 100);
			lines.push_back(Line(a, b));
		}
	sort(lines.begin(), lines.end(), Compare);
	for (size_t i = 0; i < lines.size(); i++)
		for (size_t j = 0; j < i; j++)
			if (intersect(lines[i].p1, lines[i].p2, lines[j].p1, lines[j].p2))
			{
				lines.erase(lines.begin() + i);
				i--;
				break;
			}
	return lines;
}

BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	return LineTo(hdc, x2, y2);
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWnd; MSG lpMsg; WNDCLASS w;

	w.lpszClassName = szProgName;
	w.hInstance = hInstance;
	w.lpfnWndProc = WndProc;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	if (!RegisterClass(&w))	return 0;

	hWnd = CreateWindow(szProgName, "∆адна€ триангул€ци€.", WS_OVERLAPPEDWINDOW, WindX, WindY, Width, Height, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, (HINSTANCE)NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&lpMsg, NULL, 0, 0))
	{
		TranslateMessage(&lpMsg);
		DispatchMessage(&lpMsg);
	}
	return(lpMsg.wParam);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	HPEN hPen;

	vector<Point> points = CreatePoints(cnt);
	vector<Line> lines = Triangulation(points);

	switch (messg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		hPen = CreatePen(1, 2, BLACK_PEN);

		SelectObject(hdc, hBrush); 
		for (size_t i = 0; i < points.size(); i++)
			Ellipse(hdc, (int)points[i].x - radius, (int)points[i].y - radius, (int)points[i].x + radius, (int)points[i].y + radius);

		for (size_t i = 0; i < lines.size(); i++)
			DrawLine(hdc, (int)lines[i].p1.x, (int)lines[i].p1.y, (int)lines[i].p2.x, (int)lines[i].p2.y);

		ValidateRect(hWnd, NULL);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		hBrush = CreateSolidBrush(0);
		hPen = CreatePen(0, 0, 0);
		DeleteObject(hPen);
		DeleteObject(hBrush);
		break;
	default: return(DefWindowProc(hWnd, messg, wParam, lParam));
	}
	return 0;
}