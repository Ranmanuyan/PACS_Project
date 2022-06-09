
#include "bob_array.hpp"


#if 0
/* Test code for array class */
int main(int argc, char **argv) {
  array<int> a(2, 3, 4, 5, 6, 7);
  view<int> v;
  int i, j, k, l, m, n;
  int c = 0;
  for (i = 0; i < 2; i++)
    for (j = 0; j < 3; j++)
      for (k = 0; k < 4; k++)
        for (l = 0; l < 5; l++)
          for (m = 0; m < 6; m++)
            for (n = 0; n < 7; n++)
              a(i, j, k, l, m, n) = c++;

  for (i = 0; i < 2; i++)
    for (j = 0; j < 3; j++)
      for (k = 0; k < 4; k++)
        for (l = 0; l < 5; l++)
          for (m = 0; m < 6; m++)
            for (n = 0; n < 7; n++)
              std::cout << a(i, j, k, l, m, n) << " ";
  std::cout << std::endl;

  v = a.getview(1, 1, 3);
  a.Prt();
  v.Prt();
  for (l = 0; l < 5; l++)
    for (m = 0; m < 6; m++)
      for (n = 0; n < 7; n++)
        std::cout << v(l, m, n) << " ";
  std::cout << std::endl;

  return 0;
}
#endif

