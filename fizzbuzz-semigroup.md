# FizzBuzz on Semigroups

This note is an addendum to my paper

* Maciej Piróg. [FizzBuzz in Haskell by Embedding a DSL](https://themonadreader.files.wordpress.com/2014/04/fizzbuzz.pdf). The Monad.Reader 23:7-16, 2014.

It is self-contained, so don't bother reading the paper, especially that this note serves a different purpose.

## The problem

To quote [Wikipedia](https://en.wikipedia.org/wiki/Fizz_buzz):

> Fizz buzz is a group word game for children to teach them about division. Players take turns to count incrementally, replacing any number divisible by three with the word "fizz", and any number divisible by five with the word "buzz".

The goal is to write a program

``` haskell
fizzbuzz :: Int -> String
```

that solves the FizzBuzz probelm for a given natural number such that:

* **Extensibility:** It is easy to extend the solution with other "sounds" for different divisors (like "hiss" for 7, or "howl" for 11),
* **Modularity:** There is no explosion of the code size when we add the other divisors,
* **Control-flow follows the information flow:** We do not repeat the *modulo* tests, nor store their results in auxiliary variables,
* **The solution is on semigroups:** We do not use the empty string to build the final answer.

One of the solutions discussed in the Monad.Reader paper is the following:

``` haskell
(!>) :: String -> String -> String
d !> "" = d
_ !> ss = ss

fizzbuzz0 :: Int -> String
fizzbuzz0 n =
  show n !>
  ((if n `mod` 3 == 0 then "fizz" else "") ++
   (if n `mod` 5 == 0 then "buzz" else "") ++
   (if n `mod` 7 == 0 then "hiss" else ""))
```

It trivially solves the problem, but the control flow does not follow the information flow, since the `!>` operator performs an additional test in the pattern-matching.

The Monad.Reader paper offers the following solution:

``` haskell
fizzbuzz0a :: Int -> String
fizzbuzz0a n = (test 3 "fizz". test 5 "buzz" . test 7 "hiss") id (show n)
 where
  test x s f | n `mod` x == 0 = const (s ++ f "")
             | otherwise      = f
```

What I don't like about this solution is the fact that we use the empty string `""`. The fact that we at some point concatenate the empty string means that we do not have full control over what is going on. In this note, I derive a solution that does not use the empty string, that is, the codomain type is any semigroup (that is, a monoid without a unit).

## Deriving a solution

We derive the solution starting with the function `fizzbuzz0`. We proceed in a number of steps. In each step we derive an implementation that is extensionally equal to `fizzbuzz0`.

### Step 1: Generalisation

First, we rewrite `fizzbuzz0` to a more general form. This will help us to notice some patterns in the code:

``` haskell
fizzbuzz1 :: Int -> String
fizzbuzz1 n = show n !> concat (map test rules)
 where
  test (x, s) | n `mod` x == 0 = s
              | otherwise      = ""
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

### Step 2: Some similarities

The operators used in the solution are `!>` and `++` hidden in the function `concat`. There are some similarities in how they operate together with `concat`. Check out the following equations, where `ss` stands for a non-empty string:

``` haskell
v !> concat ("" : xs) == v !> concat xs
v !> concat (ss : xs) == (v !> ss) ++ concat xs
v !> concat []        == v

v ++ concat ("" : xs) == v ++ concat xs
v ++ concat (ss : xs) == (v ++ ss) ++ concat xs
v ++ concat []        == v
```

It is easy to see that the sets of equations above for `!>` and `++` are quite similar. Hence, we can say that the following equations hold for `##` standing for `!>` or `++`:

``` haskell
v ## concat ("" : xs) == v ## concat xs
v ## concat (ss : xs) == (v ## ss) ++ concat xs
v ## concat []        == v
```

We notice that when interpreted as term-rewriting rules, the above is a recipe for evaluating expressions of the form `v ## concat xs` (for `##` equal to `!>` or `++`) inductively on the strucutre of `xs`. It is important that it is a mutual induction, since the rule

``` haskell
v !> concat (ss : xs) = (v !> ss) ++ concat xs
```

goes from an expression in which the main symbol is `!>` to an expression in which the main symbol is `++`.

While the set of equations above is a "recipe", it is not a proper Haskell definition. Hence, we introduce a separate function, which implements the rewriting rules:

``` haskell
step :: String -> (String -> String -> String) -> [String] -> String
step v (##) ("" : xs) = step v (##) xs
step v (##) (ss : xs) = step (v ## ss) (++) xs
step v (##) []        = v
```

It implements evaluating of expressions in the sense that

``` haskell
v ## concat xs = step v (##) xs
```

for both appropriate `##`. This gives us the next solution:

``` haskell
fizzbuzz2 :: Int -> String
fizzbuzz2 n = step (show n) (!>) (map test rules)
 where
  test (x, s) | n `mod` x == 0 = s
              | otherwise      = ""
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

### Step 3: The essential observation

Note that the only place in which the operator `##` is applied in the definition of `step` is in the line

``` haskell
step v (##) (ss : xs) = step (v ## ss) (++) xs
```

This means that `##` is applied only to non-empty strings on the right! From this, one easily gets that

``` haskell
step v (!>) = step v (const id)
```

(note that `const id` is synonymous to `\x y -> y`).

Hence, the next solution, which gets rid of the unwanted `!>`'s pattern matching, is as follows:

``` haskell
fizzbuzz3 :: Int -> String
fizzbuzz3 n = step (show n) (const id) (map test rules)
 where
  test (x, s) | n `mod` x == 0 = s
              | otherwise      = ""
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

### Step 4: `step` is a fold

Just by looking at the structure of `step`, we see that it can be given in terms of `foldl`. The following function `step'` is extensionally equal to `step`:

``` haskell
step' :: String -> (String -> String -> String) -> [String] -> String
step' v (##) = fst . foldl aux (v, (##)) 
 where
  aux (v, (##)) "" = (v, (##))
  aux (v, (##)) ss = (v ## ss, (++))
```

We can inline this definition in the `fizzbuzz3` function and obtain the following:

``` haskell
fizzbuzz4 :: Int -> String
fizzbuzz4 n = fst $ foldl aux (show n, const id) (map test rules)
 where
  aux (v, (##)) "" = (v, (##))
  aux (v, (##)) ss = (v ## ss, (++))
  test (x, s) | n `mod` x == 0 = s
              | otherwise      = ""
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

### Step 5: Fold-map fusion

The "fold-map fusion" law states that:

``` haskell
foldl f a . map g == foldl (\a b -> f a (g b)) a
```

Using it to rewrite our program, we obtain the following solution:

``` haskell
fizzbuzz5 :: Int -> String
fizzbuzz5 n = fst $ foldl aux (show n, const id) rules
 where
  aux (v, (##)) (x, "")                  = (v, (##))       -- !!!
  aux (v, (##)) (x, ss) | n `mod` x == 0 = (v ## ss, (++))
                        | otherwise      = (v, (##))
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

Now, we notice that the line marked with `-- !!!` is really not necessary, as all strings in the second components of the pairs come from the `rules` list, hence are never empty. We can safely delete the marked line:

``` haskell
fizzbuzz5a :: Int -> String
fizzbuzz5a n = fst $ foldl aux (show n, const id) rules
 where
  aux (v, (##)) (x, ss) | n `mod` x == 0 = (v ## ss, (++))
                        | otherwise      = (v, (##))
  rules = [(3, "fizz"), (5, "buzz"), (7, "hiss")]
```

### Step 6: Inline the rules

We can obtain an equally elegant solution without the rules given explicitly in the `where` clause. By unfolding the definition of `foldl`, we obtain:

``` haskell
fizzbuzz6 :: Int -> String
fizzbuzz6 n = fst $ (show n, const id) `aux` (3, "fizz") `aux` (5, "buzz") `aux` (7, "hiss")
 where
  aux (v, (##)) (x, ss) | n `mod` x == 0 = (v ## ss, (++))
                        | otherwise      = (v, (##))
```

We can play around a bit, to make this solution more compact and readable:

``` haskell
fizzbuzz6a :: Int -> String
fizzbuzz6a n = fst $ (show n, const id) * (3, "fizz") * (5, "buzz") * (7, "hiss")
 where
  p@(v, f) * (x, s) | n `mod` x == 0 = (f v s, (++))
                    | otherwise      = p
```

## An alternative solution

One more possible solution, which could probably be derived from a `foldr` approach is as follows:

``` haskell
fizzbuzzR :: Int -> String
fizzbuzzR n = fst $ test 3 "fizz" $ test 5 "buzz" (show n, const)
 where
  test x s p@(v, f) | n `mod` x == 0 = (f s v, (++))
                    | otherwise      = p
```

## Acknowledgements

I thank Tom Schrijvers for inspiring the use of pairs.