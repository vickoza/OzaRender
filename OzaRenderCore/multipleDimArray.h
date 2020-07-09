#pragma once
//This is based on Hilbert space-filling curve to map 2d array to a 1d array
template <typename T, size_t rows, size_t cols>
class multipleDimArray
{
public:
    T& operator [] (size_t x, size_t y, size_t res)
    {
        return data[xy2d(rows * cols / res, x * col / res, y * rows / res)];
    }
 
private:
    T data[rows * cols];
   size_t xy2d(size_t n, size_t x, size_t y)
   {
       size_t rx, ry, d = 0;
       for (size_t s = n / 2; s > 0; s /= 2)
       {
           rx = (x & s) > 0;
           ry = (y & s) > 0;
           d += s * s * ((3 * rx) ^ ry);
           rot(n, &x, &y, rx, ry);
       }
       return d;
   }

   //convert d to (x,y)
   void d2xy(size_t n, size_t d, size_t& x, size_t& y)
   {
       size_t rx, ry, t = d;
       x = y = 0;
       for (size_ts = 1; s < n; s *= 2) 
       {
           rx = 1 & (t / 2);
           ry = 1 & (t ^ rx);
           rot(s, x, y, rx, ry);
           x += s * rx;
           y += s * ry;
           t /= 4;
       }
   }

   //rotate/flip a quadrant appropriately
   void rot(size_t n, size_t& x, size_t& y, size_t rx, size_t ry)
   {
       if (ry == 0) 
       {
           if (rx == 1) 
           {
               x = n - 1 - x;
               y = n - 1 - y;
           }
           //Swap x and y
           int t = x;
           x = y;
           y = t;
       }
   }
};

