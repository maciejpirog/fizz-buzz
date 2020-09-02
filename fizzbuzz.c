// (c) 2016 by Maciej Piróg
// License: MIT
//
// An implementation of the classical FizzBuzz problem inspired by my paper
// "FizzBuzz in Haskell by Embedding a Domain-Specific Language"
// https://themonadreader.files.wordpress.com/2014/04/fizzbuzz.pdf

#include <stdio.h>

typedef void (*cont)(int n);

void skip(int n) {}
void print(int n) { printf("%d", n); }

cont test(int n, int d, char* c, cont k)
{
  if (n % d == 0)
  {
    printf(c); 
    return skip;
  }
  else
  {
    return k;
  }
}

void fizzbuzz(int n)
{
  cont k = print;
  k = test(n, 3, "fizz", k);
  k = test(n, 5, "buzz", k);
  k(n);
}

int main()
{
  int i;
  for (i = 1; i <= 20; i++)
  {
    fizzbuzz(i);
    printf("\n");
  }
  return 0;
}
