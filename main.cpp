#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>
#include <algorithm>
#include <cassert>
using namespace std;


// 滑动窗法
// 输入：点P，l比特的整数 k
// 输出：Q = [k]P


// 问题：
//（1）F19上椭圆曲线方程y^2=x^3+x+1，E19(1,1)。椭圆曲线的点(15,16),到字节串的转换、利用滑动窗法计算椭圆曲线多倍点运算[100](15,16)

// 解体思路：根据滑动窗法，我们需要预计算 P1 = P, P2 = [2]P

class Point{
public:
    int x;
    int y;
    explicit Point(int a=0, int b=0):x(a), y(b) {};

    bool operator==(const Point &rhs) const {
        return x == rhs.x &&
               y == rhs.y;
    }

    bool operator!=(const Point &rhs) const {
        return !(rhs == *this);
    }
};

void exgcd(int a,int b,int &x,int &y)
{
    if (b == 0) {
        x = 1, y = 0;
        return;
    }
    exgcd(b, a % b, y, x);
    y -= a / b * x;
}

int lambda(Point P1, Point P2, int a) {
    // computer lambda
    int numerator = 0;
    int denominator = 0;

    if (P1.x != P2.x) {
        numerator = P2.y - P1.y;
        denominator = P2.x - P1.x;
    }
    else {
        numerator = 3 * (int)(pow(P1.x, 2)) + a;
        denominator = 2 * P1.y;
    }

    // 约分
    int factor = __gcd(numerator, denominator);
    numerator /= factor;
    denominator /= factor;
    if (0 > numerator*denominator) {    // 分子、分母异号，设置分母为正数
        denominator = abs(denominator);
        numerator = -abs(numerator);
    }

    // 乘法逆元
    int l = 0;
    if (0 != numerator%denominator) {
        int x = 1;
        int y = 0;
        exgcd(denominator, 19, x, y);
        l = numerator * x;
    }
    else {
        l = numerator / denominator;
    }

    while (l < 0) l += 19;
    l %= 19;
    return l;
}

Point add(Point P1, Point P2) {
    int x1 = P1.x;
    int y1 = P1.y;
    int x2 = P2.x;

    // lambda
    int l = lambda(P1, P2, 1);

    // computer x3
    int x3 = (int)(pow(l, 2)) - x1 -x2;
    while (x3 < 0) {
        x3 += 19;
    }
    x3 %= 19;

    // computer y3
    int y3 = l * (x1 - x3) - y1;
    while (y3 < 0) {
        y3 += 19;
    }
    y3 %= 19;

    return Point(x3, y3);
}

int main() {

    // closure test
    vector<Point> F19 {Point( 0,  1), Point( 0, 18), Point( 2,  7), Point( 2, 12),
                       Point( 5,  6), Point( 5, 13), Point( 7,  3), Point( 7, 16),
                       Point( 9,  6), Point( 9, 13), Point(10,  2), Point(10, 17),
                       Point(13,  8), Point(13, 11), Point(14,  2), Point(14, 17),
                       Point(15,  3), Point(15, 16), Point(16,  3), Point(16, 16)};

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            Point b = add(F19[i], F19[j]);
            vector<Point>::iterator it = find(F19.begin(), F19.end(), b);
            if (it == F19.end()) {
                printf("%d: (%2d, %2d) + %d: (%2d, %2d) = (%2d, %2d)\n", i, F19[i].x, F19[i].y, j, F19[j].x, F19[j].y, b.x, b.y);
            }
        }
    }

    // Naive algorithm
    Point ans = add(F19[17], F19[17]);
    for (int i = 0; i < 99; i++) {
        ans = add(ans, F19[17]);
    }
    std::cout << "ans: (" << ans.x << ", " << ans.y << ")" << std::endl;


    const int l = 7;                  // length of k in bits
    bitset<l> kbits(100);        // k = 200, l = 8
    int k[l];              // reverse the order
    for (int i = 0; i < l; i++) {
        k[l-1-i] = kbits[i];
    }

    int r = 5;          // 窗口长度
    // cout << "Please set the window size r: ";
    // cin >> r;

    // Pre computing
    int m = (int)pow(2, r-1)-1;
    int arraySize = pow(2, r);
    vector<Point> points(arraySize);

    Point P (15, 16);
    Point P1(15, 16);
    Point P2 = add(P, P);

    points[0] = P;
    points[1] = P1;
    points[2] = P2;

    for (int i = 1; i <= m; i++) {
        points[2*i+1] = add(points[2*i-1], P2);
    }

    int j = l-1;
    Point O(INT_MAX, INT_MAX);   // 无穷远点
    Point Q = O;
    while (j >= 0) {
        if(0 == k[j]) {
            if (Q.x != INT_MAX) {
                Q = add(Q, Q);
            }
            j = j - 1;
        }
        else {
            int t;
            for (t = j-1; t>0 && t>j-r; t--) {
                if(0==k[t]) break;                    // d.2.1
            }

            int hj = 0;
            for (int i = 0; i < j-t+1; i++) {
                hj = hj + k[t+i]*(int)pow(2, i);               // d.2.2
                if(INT_MAX != Q.x) {
                    Q = add(Q, Q);                                // Q = [2^(j-t+1)]Q
                }
            }

            // Point sum = Q;
            // if(INT_MAX != Q.x) {
            //     for (int i = 1; i < (int)(pow(2, j-t+1)); i++) {
            //         sum = add(sum, Q);
            //     }
            // }
            // Q = [2^(j-t+1)]Q + P_hj
            if(INT_MAX != Q.x) {
                Q = add(Q, points[hj]);
            }
            else {
                Q = points[hj];
            }
            j = t - 1;                  // d.2.4
        }
    }
    std::cout << "Q: (" << Q.x << ", " << Q.y << ")" << std::endl;

    return 0;
}
